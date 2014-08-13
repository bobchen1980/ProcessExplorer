#include "stdafx.h"
#include "Pro2Port.h"

HINSTANCE g_hNtDLL;

BOOL InitNTDLL()
{
	g_hNtDLL = LoadLibrary( "ntdll.dll" );
	if ( !g_hNtDLL )
	{
		return FALSE;
	} 
	return TRUE;
}



PCHAR ProcessPidToName(HANDLE hProcessSnap, DWORD ProcessId, PCHAR ProcessPath,PCHAR ProcessName)
{
	PROCESSENTRY32 processEntry = { 0 };
	processEntry.dwSize = sizeof(PROCESSENTRY32); 
	lstrcpy(ProcessPath, "???");
	lstrcpy(ProcessName, "???");
	
	if (!Process32First(hProcessSnap, &processEntry)) 
	{
		//AfxMessageBox("hProcessSnap Error");
		return ProcessName;
	}
	
	do 
	{
		if (processEntry.th32ProcessID == ProcessId)
		{
			MODULEENTRY32 me32       = {0}; 
			GetProcessModule(processEntry.th32ProcessID, 
					1, &me32, sizeof(MODULEENTRY32)); 
				
			if (lstrlen(me32.szExePath) != 0)
			{
				lstrcpy(ProcessPath, me32.szExePath);
			}
			else
			{
				lstrcpy(ProcessPath, processEntry.szExeFile);
			}		
			lstrcpy(ProcessName, processEntry.szExeFile);
			TRACE("processEntry:%d,%s,%s\n",ProcessId,processEntry.szExeFile,me32.szExePath);
			return ProcessName;
		}
		
	} while(Process32Next(hProcessSnap, &processEntry));
	
	return ProcessName;
}

HANDLE OpenPhysicalMemory(HANDLE& hSection)
{
    NTSTATUS          status;
    UNICODE_STRING    physmemString;
    OBJECT_ATTRIBUTES attributes;
	HANDLE            hMemory;
	RTLINITUNICODESTRING RtlInitUnicodeString;
	ZWOPENSECTION ZwOpenSection;

    
	RtlInitUnicodeString = (RTLINITUNICODESTRING)GetProcAddress( g_hNtDLL, "RtlInitUnicodeString"); 
	ZwOpenSection = 	(ZWOPENSECTION)GetProcAddress( g_hNtDLL, "ZwOpenSection"); 


	RtlInitUnicodeString(&physmemString, L"\\Device\\PhysicalMemory"); 

    InitializeObjectAttributes(&attributes, &physmemString,
		OBJ_CASE_INSENSITIVE, NULL, NULL); 

    status = ZwOpenSection(&hSection, SECTION_MAP_READ, &attributes ); 
   

	if (!NT_SUCCESS(status))	return NULL;

    hMemory = MapViewOfFile(hSection, FILE_MAP_READ,
		0, 0x30000, 0x1000);
 
    if (GetLastError() != 0)	return NULL;     
	
    return hMemory;
}

void AdjustDacl(HANDLE hProcess)
{
	SID world = { SID_REVISION,1, SECURITY_WORLD_SID_AUTHORITY, 0 }; 

	LPTSTR ptstrName   = (LPTSTR)&world;
	EXPLICIT_ACCESS ea = { STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL, SET_ACCESS, NO_INHERITANCE,
		{ 0, NO_MULTIPLE_TRUSTEE, TRUSTEE_IS_SID, TRUSTEE_IS_USER, ptstrName}};
	
	ACL * pdacl = 0;
	if (SetEntriesInAcl(1, &ea, 0, &pdacl) != ERROR_SUCCESS)
	{
		printf( "SetEntriesInAcl Error:%d", GetLastError());
	}

	if (SetSecurityInfo(hProcess, SE_KERNEL_OBJECT,
			DACL_SECURITY_INFORMATION, 0, 0, pdacl, 0) != ERROR_SUCCESS)
	{
		printf( "SetSecurityInfo Error:%d", GetLastError());
	}

	LocalFree(pdacl);
}

HANDLE OpenDeviceTcpUdp(WCHAR * deviceName)
{
    UNICODE_STRING    physmemString;
    OBJECT_ATTRIBUTES attributes;
    IO_STATUS_BLOCK   iosb;
    HANDLE            hDeviceHandle;
	RTLINITUNICODESTRING RtlInitUnicodeString;
	ZWOPENFILE ZwOpenFile;

    
	RtlInitUnicodeString = (RTLINITUNICODESTRING)GetProcAddress( g_hNtDLL, "RtlInitUnicodeString"); 
	ZwOpenFile = 	(ZWOPENFILE)GetProcAddress( g_hNtDLL, "ZwOpenFile"); 
 
	RtlInitUnicodeString(&physmemString, deviceName);    

    if (GetLastError() != 0)	return NULL;

    InitializeObjectAttributes(&attributes, &physmemString,
		OBJ_CASE_INSENSITIVE, 0, NULL);

    NTSTATUS status = ZwOpenFile(&hDeviceHandle, 0x100000, &attributes, &iosb, 3, 0);

    if (!NT_SUCCESS(status))	return NULL;

	return hDeviceHandle;
}

PULONG GetHandleList()
{
    ULONG  cbBuffer = 0x1000;               
    PULONG pBuffer  = new ULONG[cbBuffer]; 
    NTSTATUS Status;
	ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation;

    
	ZwQuerySystemInformation = 	(ZWQUERYSYSTEMINFORMATION)GetProcAddress( g_hNtDLL, "ZwQuerySystemInformation"); 
	
    do
	{
        Status = ZwQuerySystemInformation(
			SystemHandleInformation,
			pBuffer,
			cbBuffer * sizeof(ULONG), 
			NULL
			);
		
        if (Status == STATUS_INFO_LENGTH_MISMATCH)
        {
            delete [] pBuffer;
            pBuffer = new ULONG[cbBuffer *= 2];
        }
        else if (!NT_SUCCESS(Status))
        {
            delete [] pBuffer;
            return NULL;
        }
	}while (Status == STATUS_INFO_LENGTH_MISMATCH);

    return pBuffer;
}

PVOID GetTcpUdpObject(PULONG pBuffer, HANDLE hHandle, DWORD ProcessId)
{
	int nCount = *pBuffer;
    PSYSTEM_HANDLE_INFORMATION pProcesses = (PSYSTEM_HANDLE_INFORMATION)(pBuffer + 1);
	
    for (int i = 0; i < nCount; i++)
    {
        if (pProcesses->ProcessId == ProcessId && pProcesses->Handle == (int)hHandle)
        {
           return (PVOID)pProcesses;
		}
		pProcesses++;
	}
	return NULL;
}

BOOL GetPTE(PVOID objAddress, HANDLE hMapPhysicalMemory, HANDLE hSection, PTE& pte)
{
	DWORD dwPhysMemBuf = (DWORD)hMapPhysicalMemory, dwAddress = (DWORD)objAddress;
    LPVOID pNewMapPhy  = NULL;
	DWORD dwNewAddress = *((LPDWORD)(dwPhysMemBuf + (dwAddress >> 0x16) * 4));
	
	if ((dwNewAddress & 0x000000ff) < 0x01)
	{
		return FALSE;
	}
	if ((dwNewAddress & 0x000000ff) < 0x80)
	{
		pNewMapPhy = MapViewOfFile(hSection, 4, 0, dwNewAddress & 0xFFFFF000, 0x1000);
		dwNewAddress = (dwAddress >> 0x0c) & 0x3ff; 
		dwNewAddress = *((LPDWORD)((DWORD)pNewMapPhy + 4 * dwNewAddress)) & 0xFFFFF000;
		UnmapViewOfFile(pNewMapPhy);
		pNewMapPhy = NULL;
	}
	else
	{
		dwNewAddress = (dwNewAddress & 0xFFFFF000) + (dwAddress & 0x003ff000);
	}

	pNewMapPhy = MapViewOfFile(hSection, FILE_MAP_READ,
		0, dwNewAddress, 0x1000);

	if (pNewMapPhy == NULL)	
	{
		long lError = GetLastError();
		return FALSE;
	}
	else
	{
		memcpy(&pte, (char *)pNewMapPhy + (dwAddress & 0x00000FFF), sizeof(PTE));
	}

	UnmapViewOfFile(pNewMapPhy);
	return TRUE;
}

CString	IpConvert2State(DWORD dwState)
{
	switch(dwState)
	{
		case MIB_TCP_STATE_CLOSED:
			return "CLOSED";
		
		case MIB_TCP_STATE_LISTEN:
			return "LISTENING";

		case MIB_TCP_STATE_SYN_SENT:
			return "SYN_SENT";

		case MIB_TCP_STATE_SYN_RCVD:
			return "SYN_RECEIVED";

		case MIB_TCP_STATE_ESTAB:
			return "ESTABLISHED";

		case MIB_TCP_STATE_FIN_WAIT1:
			return "FIN_WAIT1";

		case MIB_TCP_STATE_FIN_WAIT2:
			return "FIN_WAIT2";

		case MIB_TCP_STATE_CLOSE_WAIT:
			return "CLOSE_WAIT";

		case MIB_TCP_STATE_CLOSING:
			return "CLOSING";

		case MIB_TCP_STATE_LAST_ACK:
			return "LAST_ACK";

		case MIB_TCP_STATE_TIME_WAIT:
			return "TIME_WAIT";

		case MIB_TCP_STATE_DELETE_TCB:
			return "DELETE_TCB";

		default:
			return "UNKNOWN";
	}
}