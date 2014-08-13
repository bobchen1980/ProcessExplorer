// ProcessFunction.cpp: implementation of the ProcessFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcessFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Global Process Functions
// Because Use psapi.dll so Only can use in Win2000
//////////////////////////////////////////////////////////////////////

//提升进程权限
BOOL UpdateProcessPrivilege( HANDLE hProcess, LPCTSTR lpPrivilegeName = SE_DEBUG_NAME )
// hProcess [in] : 要提升的进程，目标进程
// lpPrivilegeName [in] : 要提升到的特权，目标特权
// 返回值 : TRUE : 成功; FALSE : 失败
{
	HANDLE hToken;
	if ( ::OpenProcessToken( hProcess, TOKEN_ALL_ACCESS, &hToken ) ) {
		
		LUID destLuid;
		if ( ::LookupPrivilegeValue( NULL, lpPrivilegeName, &destLuid ) ) {
			
			TOKEN_PRIVILEGES TokenPrivileges;
			TokenPrivileges.PrivilegeCount = 1;
			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			TokenPrivileges.Privileges[0].Luid = destLuid;
			int iResult;
			if ( iResult = ::AdjustTokenPrivileges( hToken, FALSE, &TokenPrivileges, 
									0, NULL, NULL ) ) {
				
				return TRUE;				
			}
		}
	}

	return FALSE;
}

BOOL KillProcess( DWORD dwPID )
	// 杀掉指定的进程
	// 要杀掉的进程的ID
{
	HANDLE hProcess = ::OpenProcess( PROCESS_TERMINATE, FALSE, dwPID );

	if( hProcess != NULL ) {

		if ( ::TerminateProcess( hProcess, 0 ) ) {

			::CloseHandle( hProcess );
			return TRUE;
		}
		else {
			::CloseHandle( hProcess );
		}
	}
	return FALSE;
}

//获取进程信息列表
BOOL EnumProcessesInfo_NT( PROCESSINFO32* lpPsInfo, ULONG ulSize, ULONG* pulNeeded )
// lpPsInfo [out] : 指向PROCESSINFO结构数组的指针
// nSize [in] : lpPsInfo中的元素个数
// nNeeded [out] : 实际的元素个数
// 返回值 : TRUE : 成功; FALSE : 失败
{
	ASSERT( pulNeeded );

	GModuleExApi* _GetModuleFileNameEx =NULL;
	Emodules* _EnumProcessModules =NULL;
	EProcesses* _EnumProcesses =NULL;
	HINSTANCE hDLL= LoadLibrary("psapi.dll");
	if (hDLL != NULL)
	{
#ifdef UNICODE
		_GetModuleFileNameEx = (GModuleExApi *)GetProcAddress(hDLL,"GetModuleFileNameExW");
		_EnumProcessModules = (Emodules *)GetProcAddress(hDLL,"EnumProcessModules");
		_EnumProcesses =  (EProcesses *)GetProcAddress(hDLL,"EnumProcesses");
#else
		_GetModuleFileNameEx = (GModuleExApi *)GetProcAddress(hDLL,"GetModuleFileNameExA");
		_EnumProcessModules = (Emodules *)GetProcAddress(hDLL,"EnumProcessModules");
		_EnumProcesses =  (EProcesses *)GetProcAddress(hDLL,"EnumProcesses");
#endif // !UNICODE
		
		if (_GetModuleFileNameEx == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
		if (_EnumProcessModules == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
		if (_EnumProcesses == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
	}
	
	LPDWORD        lpdwPIDs ;			//存储进程ID数组
	DWORD          dwbSize, dwbSize2;
	
	dwbSize2 = 256 * sizeof( DWORD );
	lpdwPIDs = NULL;
	
	do {
		
		if( lpdwPIDs ) {
			
			HeapFree( GetProcessHeap(), 0, lpdwPIDs );
			dwbSize2 *= 2;
		}
		
		lpdwPIDs = (LPDWORD)HeapAlloc( GetProcessHeap(), 0, dwbSize2 );
		if( lpdwPIDs == NULL ) {
			return FALSE ;
		}
		
		if( ! _EnumProcesses( lpdwPIDs, dwbSize2, &dwbSize ) ) {
			
			HeapFree( GetProcessHeap(), 0, lpdwPIDs ) ;
			return FALSE ;
		}
		
	}while( dwbSize == dwbSize2 ) ;
	
	ULONG ulCount  = dwbSize / sizeof( DWORD );
	
	//如果为询问数量，则返回实际数量
	if ( NULL == lpPsInfo && 0 == ulSize ) {
		
		*pulNeeded = ulCount;
		return TRUE;
	}
	
	ASSERT( lpPsInfo );
	if ( NULL == lpPsInfo ) {
		return FALSE;
	}
	
	if ( ulSize <= ulCount ) {
		*pulNeeded = ulSize;
	}
	else {
		*pulNeeded = ulCount;
	}
	
	//获得进程信息
	HANDLE	hProcess;
	HMODULE	hModule;
	DWORD		dwSize;
	
	
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	// Loop through each ProcID.
	for( ULONG ulIndex = 0 ; ulIndex < (*pulNeeded) ; ulIndex++ )
	{
		// Open the process (if we can... security does not
		// permit every process in the system).
		//		TRACE("PID To Open:%d\r\n", lpdwPIDs[ulIndex] );
		
		lpPsInfo[ulIndex].dwPID = lpdwPIDs[ulIndex];
		lpPsInfo[ulIndex].strPath[0] = 0;
		lpPsInfo[ulIndex].strName[0] = 0;
		
		// Because Can't Open 0 And 8 Process,
		// Mark Them At There 
		if ( 0 == lpdwPIDs[ulIndex] ) {
			
			strcpy( lpPsInfo[ulIndex].strName, "System Idle Process" );
			continue;
		}
		else if ( 8 == lpdwPIDs[ulIndex] || 4 == lpdwPIDs[ulIndex] ) {
			
			strcpy( lpPsInfo[ulIndex].strName, "System" );
			continue;
		}
		
		// Open Process And Get Process Infomation
		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE, lpPsInfo[ulIndex].dwPID );
		if( hProcess != NULL )
		{
			// Here we call EnumProcessModules to get only the
			// first module in the process this is important,
			// because this will be the .EXE module for which we
			// will retrieve the full path name in a second.
            if (!GetProcessTimes(hProcess, &lpPsInfo[ulIndex].ftCreation, 
				&lpPsInfo[ulIndex].ftExit, &lpPsInfo[ulIndex].ftKernel, 
				&lpPsInfo[ulIndex].ftUser))
			{
				TRACE("GetProcessTimes() failed: %d\n", GetLastError());
			}

			lpPsInfo[ulIndex].dwPriority=GetPriorityClass (hProcess);;


			if( _EnumProcessModules( hProcess, &hModule,
				sizeof(hModule), &dwSize ) ) {
				
				// Get Full pathname:
				if( _GetModuleFileNameEx( hProcess, hModule, 
					path_buffer, sizeof(path_buffer) ) ) {
					
					_tsplitpath( path_buffer, drive, dir, fname, ext );               
					strcpy( lpPsInfo[ulIndex].strPath, path_buffer );
					sprintf( lpPsInfo[ulIndex].strName, "%s%s", fname, ext );
					//         TRACE( "ModuleFileName:%s\r\n", path_buffer );
				}
			}
			CloseHandle( hProcess ) ;
		}
	}	
	FreeLibrary(hDLL);
	return TRUE;
}

//- 获取指定进程的模块列表
BOOL EnumProcessModulesInfo_NT( DWORD dwPID, MODULEINFO32 *lpModInfo, 
									  DWORD dwSize, DWORD *lpdwNeeded )
	//- lpModInfo [out] :	指向MODULEINFO结构数组的指针
	//- dwSize [in] :			lpPsInfo中的元素个数
	//- lpdwNeeded [out] :	实际的元素个数
	//- 返回值 : TRUE : 成功; FALSE : 失败
{
	//	ASSERT( dwPID );
	ASSERT( lpdwNeeded );
	GModuleExApi* _GetModuleFileNameEx =NULL;
	Emodules* _EnumProcessModules =NULL;
	EProcesses* _EnumProcesses =NULL;
	HINSTANCE hDLL= LoadLibrary("psapi.dll");
	if (hDLL != NULL)
	{
#ifdef UNICODE
		_GetModuleFileNameEx = (GModuleExApi *)GetProcAddress(hDLL,"GetModuleFileNameExW");
		_EnumProcessModules = (Emodules *)GetProcAddress(hDLL,"EnumProcessModules");
		_EnumProcesses =  (EProcesses *)GetProcAddress(hDLL,"EnumProcesses");
#else
		_GetModuleFileNameEx = (GModuleExApi *)GetProcAddress(hDLL,"GetModuleFileNameExA");
		_EnumProcessModules = (Emodules *)GetProcAddress(hDLL,"EnumProcessModules");
		_EnumProcesses =  (EProcesses *)GetProcAddress(hDLL,"EnumProcesses");
#endif // !UNICODE
		
		if (_GetModuleFileNameEx == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
		if (_EnumProcessModules == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
		if (_EnumProcesses == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
	}	
	// Because Can't Open 0 And 8 Process, Mark Them At There 
	if ( 0 == dwPID || 8 == dwPID ) {
		*lpdwNeeded = 0;
		return FALSE;
	}
	
	// It's Only Two Way To Call This Function,
	// Query Module Count Or Get Module List.
	BOOL IsQueryCount;
	if ( NULL != lpdwNeeded && NULL == lpModInfo && 0 == dwSize ) {
		IsQueryCount = TRUE;
	} 
	else if ( NULL != lpdwNeeded && NULL != lpModInfo && 0 != dwSize ) {
		IsQueryCount = FALSE;
	}
	else {
		return FALSE;
	}
	
	// Open Process And Get Process Infomation
	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE, dwPID );
	
	if( hProcess == NULL ) {
		return FALSE;
	}
	
	DWORD dwCountByte;
	if( IsQueryCount ) { //if Query Modules Count
		
		BOOL bResult = _EnumProcessModules( hProcess, NULL, 0, &dwCountByte );
		*lpdwNeeded = dwCountByte / sizeof(DWORD);
		CloseHandle( hProcess );
		return bResult;
	}
	
	
	//Get Modules Info 
	if ( _EnumProcessModules( hProcess, NULL, 0, &dwCountByte ) ) {
		
		(*lpdwNeeded) = dwCountByte / sizeof(DWORD);
		HMODULE *lphMddule = new HMODULE[*lpdwNeeded];
		if ( NULL == lphMddule ) {
			CloseHandle( hProcess );
			return FALSE;
		}
		
		if ( ! _EnumProcessModules( hProcess, lphMddule, 
			(*lpdwNeeded)*sizeof(DWORD), &dwCountByte ) ) {
			
			delete[] lphMddule;
			CloseHandle( hProcess );
			return FALSE;
		}
		
		char path_buffer[_MAX_PATH];
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		
		for ( ULONG IndexMod = 0; IndexMod < (*lpdwNeeded); IndexMod++ ) {
			
			// Get Full pathname:
			if( _GetModuleFileNameEx( hProcess, lphMddule[IndexMod], 
				path_buffer, sizeof(path_buffer) ) ) {
				
				_tsplitpath( path_buffer, drive, dir, fname, ext );               
				strcpy( lpModInfo[IndexMod].strPath, path_buffer );
				sprintf( lpModInfo[IndexMod].strName, "%s%s", fname, ext );
			}
			else {
				
				lpModInfo[IndexMod].strPath[0] = 0;
				lpModInfo[IndexMod].strName[0] = 0;
			}
		}
		
		delete[] lphMddule;
	}
	FreeLibrary(hDLL);
	CloseHandle( hProcess );
	return TRUE;
}

BOOL EnumProcessesInfo_98( PROCESSINFO32* lpPsInfo, ULONG ulSize, ULONG* pulNeeded )
// lpPsInfo [out] : 指向PROCESSINFO结构数组的指针
// nSize [in] : lpPsInfo中的元素个数
// nNeeded [out] : 实际的元素个数
// 返回值 : TRUE : 成功; FALSE : 失败
{
	ASSERT( pulNeeded );
	
    HANDLE         hProcessSnap = NULL; 
    BOOL           bRet      = FALSE; 
    PROCESSENTRY32 pe32      = {0}; 
	ULONG ulIndex = 0 ;
 
    //  Take a snapshot of all processes in the system. 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    //  Fill in the size of the structure before using it. 

    pe32.dwSize = sizeof(PROCESSENTRY32); 
 
    //  Walk the snapshot of the processes, and for each process, 
    //  display information. 

    if (Process32First(hProcessSnap, &pe32)) 
    { 
        DWORD         dwPriorityClass; 
        BOOL          bGotModule = FALSE; 
        MODULEENTRY32 me32       = {0}; 
 
        do 
        { 
            bGotModule = GetProcessModule(pe32.th32ProcessID, 
                pe32.th32ModuleID, &me32, sizeof(MODULEENTRY32)); 

            if (bGotModule) 
            { 
                HANDLE hProcess; 
 
               // Get the actual priority class. 
                hProcess = OpenProcess (PROCESS_ALL_ACCESS, 
                    FALSE, pe32.th32ProcessID); 

                dwPriorityClass = GetPriorityClass (hProcess); 

                CloseHandle (hProcess); 

				//如果为询问数量，则返回实际数量
				if ( NULL == lpPsInfo && 0 == ulSize ) {
					
					ulIndex++;
					continue;
				}

                // Print the process's information. 
 				lpPsInfo[ulIndex].dwPID = pe32.th32ProcessID;
				lpPsInfo[ulIndex].strPath[0] = 0;
				lpPsInfo[ulIndex].strName[0] = 0;
				lpPsInfo[ulIndex].dwPriority=dwPriorityClass;

 				if ( 0 == pe32.th32ProcessID ) {
					
					strcpy( lpPsInfo[ulIndex].strName, "System Idle Process" );
					continue;
				}
				else if ( 8 == pe32.th32ProcessID ) {
					
					strcpy( lpPsInfo[ulIndex].strName, "System" );
					continue;
				}
				strcpy( lpPsInfo[ulIndex].strPath, me32.szExePath );
				sprintf( lpPsInfo[ulIndex].strName, "%s", me32.szModule );
				ulIndex++;
				//printf( "\nPriority Class Base\t%d\n", 
                //    pe32.pcPriClassBase); 
                //printf( "PID\t\t\t%d\n", pe32.th32ProcessID);
                //printf( "Thread Count\t\t%d\n", pe32.cntThreads);
                //printf( "Module Name\t\t%s\n", me32.szModule);
                //printf( "Full Path\t\t%s\n\n", me32.szExePath);
				
            } 
        } 
        while (Process32Next(hProcessSnap, &pe32)); 
        bRet = TRUE; 
    } 
    else 
        bRet = FALSE;    // could not walk the list of processes 
 
    // Do not forget to clean up the snapshot object. 
	*pulNeeded=ulIndex;
	
    CloseHandle (hProcessSnap); 
    return (bRet); 

}

//- 获取指定进程的模块列表
BOOL EnumProcessModulesInfo_98( DWORD dwPID, MODULEINFO32 *lpModInfo, 
									  DWORD dwSize, DWORD *lpdwNeeded )
{
	return TRUE;
}

BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, 
     LPMODULEENTRY32 lpMe32, DWORD cbMe32) 
{ 
    BOOL          bRet        = FALSE; 
    BOOL          bFound      = FALSE; 
    HANDLE        hModuleSnap = NULL; 
    MODULEENTRY32 me32        = {0}; 
 
    // Take a snapshot of all modules in the specified process. 

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
    if (hModuleSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    // Fill the size of the structure before using it. 

    me32.dwSize = sizeof(MODULEENTRY32); 
 
    // Walk the module list of the process, and find the module of 
    // interest. Then copy the information to the buffer pointed 
    // to by lpMe32 so that it can be returned to the caller. 

    if (Module32First(hModuleSnap, &me32)) 
    { 
        do 
        { 
            if (me32.th32ModuleID == dwModuleID) 
            { 
                CopyMemory (lpMe32, &me32, cbMe32); 
                bFound = TRUE; 
            } 
        } 
        while (!bFound && Module32Next(hModuleSnap, &me32)); 
 
        bRet = bFound;   // if this sets bRet to FALSE, dwModuleID 
                         // no longer exists in specified process 
    } 
    else 
        bRet = FALSE;           // could not walk module list 
 
    // Do not forget to clean up the snapshot object. 

    CloseHandle (hModuleSnap); 
 
    return (bRet); 
}

DWORD TerminateApp(DWORD dwPID, DWORD dwTimeout )
{
      HANDLE   hProc ;
      DWORD   dwRet ;

      // If we can't open the process with PROCESS_TERMINATE rights,
      // then we give up immediately.
      hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,
         dwPID);

      if(hProc == NULL)
      {
         return TA_FAILED ;
      }

      // TerminateAppEnum() posts WM_CLOSE to all windows whose PID
      // matches your process's.
      EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM) dwPID) ;

      // Wait on the handle. If it signals, great. If it times out,
      // then you kill it.
      if(WaitForSingleObject(hProc, dwTimeout)!=WAIT_OBJECT_0)
         dwRet=(TerminateProcess(hProc,0)?TA_SUCCESS_KILL:TA_FAILED);
      else
         dwRet = TA_SUCCESS_CLEAN ;

      CloseHandle(hProc) ;

      return dwRet ;
}

BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam )
{
  DWORD dwID ;

  GetWindowThreadProcessId(hwnd, &dwID) ;

  if(dwID == (DWORD)lParam)
  {
     PostMessage(hwnd, WM_CLOSE, 0, 0) ;
  }

  return TRUE ;
}

BOOL GetHiddenProcMoreInfo(LPPROCESSINFO32 lpProcInfo, ULONG ulIndex)
{
	GModuleExApi* _GetModuleFileNameEx =NULL;
	Emodules* _EnumProcessModules =NULL;
	EProcesses* _EnumProcesses =NULL;
	HINSTANCE hDLL= LoadLibrary("psapi.dll");
	if (hDLL != NULL)
	{
#ifdef UNICODE
		_GetModuleFileNameEx = (GModuleExApi *)GetProcAddress(hDLL,"GetModuleFileNameExW");
		_EnumProcessModules = (Emodules *)GetProcAddress(hDLL,"EnumProcessModules");
		_EnumProcesses =  (EProcesses *)GetProcAddress(hDLL,"EnumProcesses");
#else
		_GetModuleFileNameEx = (GModuleExApi *)GetProcAddress(hDLL,"GetModuleFileNameExA");
		_EnumProcessModules = (Emodules *)GetProcAddress(hDLL,"EnumProcessModules");
		_EnumProcesses =  (EProcesses *)GetProcAddress(hDLL,"EnumProcesses");
#endif // !UNICODE
		
		if (_GetModuleFileNameEx == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
		if (_EnumProcessModules == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
		if (_EnumProcesses == NULL )	{
			FreeLibrary(hDLL);  
			return NULL;
		}
	}
	
	HANDLE	hProcess;
	HMODULE	hModule;
	DWORD		dwSize;
	
	
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	// Open Process And Get Process Infomation
	hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE, lpProcInfo[ulIndex].dwPID );
	if( hProcess != NULL )
	{
		// Here we call EnumProcessModules to get only the
		// first module in the process this is important,
		// because this will be the .EXE module for which we
		// will retrieve the full path name in a second.
		if (!GetProcessTimes(hProcess, &lpProcInfo[ulIndex].ftCreation, 
			&lpProcInfo[ulIndex].ftExit, &lpProcInfo[ulIndex].ftKernel, 
			&lpProcInfo[ulIndex].ftUser))
		{
			TRACE("GetProcessTimes() failed: %d\n", GetLastError());
			return FALSE;

		}
		
		lpProcInfo[ulIndex].dwPriority=GetPriorityClass (hProcess);;
		
		
		if( _EnumProcessModules( hProcess, &hModule,
			sizeof(hModule), &dwSize ) ) {
			
			// Get Full pathname:
			if( _GetModuleFileNameEx( hProcess, hModule, 
				path_buffer, sizeof(path_buffer) ) ) {
				
				_tsplitpath( path_buffer, drive, dir, fname, ext );               
				strcpy( lpProcInfo[ulIndex].strPath, path_buffer );
				sprintf( lpProcInfo[ulIndex].strName, "%s%s", fname, ext );
				//         TRACE( "ModuleFileName:%s\r\n", path_buffer );
			}
		}
		CloseHandle( hProcess ) ;
	}

	FreeLibrary(hDLL);
	return TRUE;
}
