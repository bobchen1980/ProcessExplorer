
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
///静态连接
//#pragma comment(lib, "ntdll.lib")

#include <windows.h>
#include <ntsecapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <malloc.h>
#include <Aclapi.h>
#include <Tlhelp32.h>
#include <iprtrmib.h>
#include "..\Library\IPhelp\IPHlpApi.h"
#include <afxsock.h> // Take afxsock of this directory with winsock2.h instead of #include <afxsock.h>		// MFC socket extensions

#pragma warning(disable: 4786)
#pragma warning(disable: 4530)
#include <string>
#include <map>
using namespace std;

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define NTAPI	__stdcall

typedef LONG	NTSTATUS;
typedef LONG	KPRIORITY;

#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_VALID_ATTRIBUTES    0x000003F2L

#define SystemHandleInformation 16

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080
#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_FOR_RECOVERY                  0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800
#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000
#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000
#define FILE_COPY_STRUCTURED_STORAGE            0x00000041
#define FILE_STRUCTURED_STORAGE                 0x00000441
#define FILE_VALID_OPTION_FLAGS                 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS            0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS        0x00000032
#define FILE_VALID_SET_FLAGS                    0x00000036
/////////////////////////////////////////////////////
typedef struct {
	ULONG Present;
	ULONG WriteTable;
	ULONG User;
	ULONG WriteThru;
	ULONG NoCache;
	ULONG Accessed;
	ULONG Dirty;
	ULONG PageSize;
	ULONG Global;
	ULONG Available;
	ULONG Pfn;
} PTE, *PPTE;

typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID Pointer;
	};
	
	unsigned long *Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#if defined(_WIN64)
typedef struct _IO_STATUS_BLOCK32 {
	NTSTATUS Status;
	ULONG Information;
} IO_STATUS_BLOCK32, *PIO_STATUS_BLOCK32;
#endif

typedef struct _SYSTEM_HANDLE_INFORMATION {//Information Class 16
	ULONG ProcessId;
	UCHAR ObjectTypeNumber;
	UCHAR Flags;                     //0x01 =PROTECT_FROM_CLOSE,0x02 =INHERIT
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
}SYSTEM_HANDLE_INFORMATION,*PSYSTEM_HANDLE_INFORMATION;

typedef struct _TDI_CONNECTION_INFO { 
	ULONG          State; 
	ULONG          Event; 
	ULONG          TransmittedTsdus; 
	ULONG          ReceivedTsdus; 
	ULONG          TransmissionErrors; 
	ULONG          ReceiveErrors; 
	LARGE_INTEGER  Throughput; 
	LARGE_INTEGER  Delay; 
	ULONG          SendBufferSize; 
	ULONG          ReceiveBufferSize; 
	BOOLEAN        Unreliable; 
} TDI_CONNECTION_INFO, *PTDI_CONNECTION_INFO; 

typedef struct _TDI_CONNECTION_INFORMATION { 
	LONG   UserDataLength; 
	PVOID  UserData; 
	LONG   OptionsLength; 
	PVOID  Options; 
	LONG   RemoteAddressLength; 
	PVOID  RemoteAddress; 
} TDI_CONNECTION_INFORMATION, *PTDI_CONNECTION_INFORMATION; 

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
	SECTION_MAP_WRITE |      \
	SECTION_MAP_READ |       \
	SECTION_MAP_EXECUTE |    \
SECTION_EXTEND_SIZE)

typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
	PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;

typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

/////////////////////////////////////////////////////
#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
	(p)->RootDirectory = r;                             \
	(p)->Attributes = a;                                \
	(p)->ObjectName = n;                                \
	(p)->SecurityDescriptor = s;                        \
	(p)->SecurityQualityOfService = NULL;               \
}

typedef  NTSTATUS (CALLBACK* ZWOPENSECTION)(
  OUT PHANDLE  SectionHandle,
  IN  ACCESS_MASK  DesiredAccess,
  IN  POBJECT_ATTRIBUTES  ObjectAttributes
  ); 

typedef  VOID  (CALLBACK* RTLINITUNICODESTRING)(
     IN OUT PUNICODE_STRING  DestinationString,
     IN PCWSTR  SourceString
     ); 
typedef NTSTATUS  (CALLBACK* ZWQUERYSYSTEMINFORMATION)(
	IN UINT SystemInformationClass, 
	IN OUT PVOID SystemInformation, 
	IN ULONG SystemInformationLength, 
	OUT PULONG ReturnLength OPTIONAL
	);
typedef NTSTATUS  (CALLBACK* ZWOPENFILE)(
   OUT PHANDLE FileHandle, 
   IN ACCESS_MASK DesiredAccess, 
   IN POBJECT_ATTRIBUTES ObjectAttributes, 
   OUT PIO_STATUS_BLOCK IoStatusBlock,
   IN ULONG ShareAccess,
   IN ULONG OpenOptions
   );
//动态加载库定义
BOOL InitNTDLL();

PCHAR ProcessPidToName(HANDLE hProcessSnap, DWORD ProcessId, PCHAR ProcessPath,PCHAR ProcessName);

HANDLE OpenPhysicalMemory(HANDLE& hSection);

void AdjustDacl(HANDLE hProcess);

HANDLE OpenDeviceTcpUdp(WCHAR * deviceName);

PULONG GetHandleList();

PVOID GetTcpUdpObject(PULONG pBuffer, HANDLE hHandle, DWORD ProcessId);

BOOL GetPTE(PVOID objAddress, HANDLE hMapPhysicalMemory, HANDLE hSection, PTE& pte);

CString	IpConvert2State(DWORD dwState);
///////////////////////////////////////////
//////////XP下获取进程端口的定义
///////////////////////////////////////////
#define HOSTNAMELEN		256
#define PORTNAMELEN		256
#define ADDRESSLEN		HOSTNAMELEN+PORTNAMELEN
#define FLAG_ALL_ENDPOINTS	1

typedef struct {
  DWORD   dwState;        // state of the connection
  DWORD   dwLocalAddr;    // address on local computer
  DWORD   dwLocalPort;    // port number on local computer
  DWORD   dwRemoteAddr;   // address on remote computer
  DWORD   dwRemotePort;   // port number on remote computer
  DWORD	  dwProcessId;
} MIB_TCPEXROW, *PMIB_TCPEXROW;


typedef struct {
	DWORD			dwNumEntries;
	MIB_TCPEXROW	table[ANY_SIZE];
} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;



typedef struct {
  DWORD   dwLocalAddr;    // address on local computer
  DWORD   dwLocalPort;    // port number on local computer
  DWORD	  dwProcessId;
} MIB_UDPEXROW, *PMIB_UDPEXROW;


typedef struct {
	DWORD			dwNumEntries;
	MIB_UDPEXROW	table[ANY_SIZE];
} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;						   

typedef DWORD (WINAPI *pAllocateAndGetTcpExTableFromStack_Def)(
  PMIB_TCPEXTABLE *pTcpTable,  // buffer for the connection table
  BOOL bOrder,               // sort the table?
  HANDLE heap,
  DWORD zero,
  DWORD flags
);
typedef DWORD (WINAPI *pAllocateAndGetUdpExTableFromStack_Def)(
  PMIB_UDPEXTABLE *pTcpTable,  // buffer for the connection table
  BOOL bOrder,               // sort the table?
  HANDLE heap,
  DWORD zero,
  DWORD flags
);

