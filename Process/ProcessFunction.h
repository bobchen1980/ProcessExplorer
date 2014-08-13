// ProcessFunction.h: interface for the ProcessFunction class.
//
#include <TLHELP32.H>

//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//进程描述信息
typedef struct _PROCESSINFO32
{
	DWORD		dwPID;
	TCHAR		strPath[_MAX_PATH];
	TCHAR		strName[_MAX_FNAME];
	FILETIME    ftCreation;
	FILETIME	ftExit;
	FILETIME	ftKernel;
	FILETIME	ftUser;
	DWORD       dwPriority;
	DWORD       dwFlag;
} PROCESSINFO32, *LPPROCESSINFO32;


//进程描述信息
typedef struct _MODULEINFO32
{
	TCHAR		strPath[_MAX_PATH];
	TCHAR		strName[_MAX_FNAME];
	
} MODULEINFO32, *LPMODULEINFO32;

typedef DWORD WINAPI GModuleExApi(HANDLE hProcess,HMODULE hModule,LPSTR lpFilename,DWORD nSize);
typedef BOOL WINAPI Emodules(HANDLE hProcess,HMODULE *lphModule,DWORD cb,LPDWORD lpcbNeeded);
typedef BOOL WINAPI EProcesses(DWORD * lpidProcess,DWORD   cb,DWORD * cbNeeded);

BOOL EnumProcessesInfo_NT( PROCESSINFO32* lpPsInfo, ULONG ulSize, ULONG* pulNeeded );
BOOL EnumProcessesInfo_98( PROCESSINFO32* lpPsInfo, ULONG ulSize, ULONG* pulNeeded );
BOOL EnumProcessModulesInfo_NT( DWORD dwPID, MODULEINFO32 *lpModInfo, DWORD dwSize, DWORD *lpdwNeeded );
BOOL EnumProcessModulesInfo_98( DWORD dwPID, MODULEINFO32 *lpModInfo, DWORD dwSize, DWORD *lpdwNeeded );

BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, 
     LPMODULEENTRY32 lpMe32, DWORD cbMe32);

BOOL GetHiddenProcMoreInfo(LPPROCESSINFO32 lpProcInfo, ULONG ulIndex);

BOOL KillProcess( DWORD dwPID );
BOOL UpdateProcessPrivilege( HANDLE hProcess, LPCTSTR lpPrivilegeName );
///进程完整的结束，释放所有资源
#define TA_FAILED 0
#define TA_SUCCESS_CLEAN 1
#define TA_SUCCESS_KILL 2
#define TA_SUCCESS_16 3

DWORD TerminateApp(DWORD dwPID, DWORD dwTimeout );
BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam );
