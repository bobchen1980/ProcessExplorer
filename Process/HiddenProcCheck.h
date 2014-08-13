#include "StdAfx.h"
#include "HiddenProcLinkedList.h"

#define VERSION							"6.0"
#define PAGE_SIZE						0x1000
#define PAGE_SIZE_MASK					0xFFFFF000

#define PHYMEM_OBJNAME					"\\device\\physicalmemory"
#define SYMLINK_OBJNAME					"\\??\\CheckHiddenProc"

#define DRV_NAME						"CheckHiddenProc"
#define DRV_FILENAME					"CheckHiddenProc.sys"
#define IOCTL_CODE1						0x22E018
#define IOCTL_CODE2						0x22E01C
#define IOCTL_GETVERSION				0x22E020


#define NT_PROCNAMELEN					16

#define STATUS_SUCCESS					((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH		((NTSTATUS)0xC0000004L)
#define OBJ_CASE_INSENSITIVE			0x00000040L
#define OBJ_KERNEL_HANDLE				0x00000200L
#define PAGE_READONLY					0x02
#define PAGE_READWRITE					0x04
#define SYMBOLIC_LINK_ALL_ACCESS		(STANDARD_RIGHTS_REQUIRED | 0x1)
#define DEF_KERNEL_BASE					0x80400000L
#define	SystemModuleInformation			11
#define PROT_MEMBASE					0x80000000

typedef LONG	NTSTATUS;
typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

typedef struct _STRING {
  USHORT  Length;
  USHORT  MaximumLength;
  PCHAR  Buffer;
} ANSI_STRING, *PANSI_STRING;
/*
typedef struct _UNICODE_STRING {
  USHORT  Length;
  USHORT  MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
*/
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Reserved[2];
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION;

typedef struct _MODULE_ENTRY {
	LIST_ENTRY link;		// Flink, Blink
	BYTE unknown1[16];
	DWORD imageBase;
	DWORD entryPoint;
	DWORD imageSize;
	UNICODE_STRING drvPath;
	UNICODE_STRING drvName;
	//...
} MODULE_ENTRY, *PMODULE_ENTRY;


typedef struct _PROCINFO
{
    DWORD pId;
	DWORD pFlag;
    char pName[NT_PROCNAMELEN];
} PROCINFO;

typedef struct _DRIVERINFO
{
	DWORD loadAddr;
	DWORD imageSize;
	char drvName[MAX_PATH];
} DRIVERINFO;

typedef struct ServiceDescriptorEntry {
        PDWORD KiServiceTable;
        PDWORD CounterTableBase;
        DWORD  ServiceLimit;
        PBYTE  ArgumentTable;
} SDE;


//*******************************************************************************************************
// Definition of Native APIs
//
//*******************************************************************************************************

NTSTATUS (WINAPI * _RtlAnsiStringToUnicodeString)
	(PUNICODE_STRING  DestinationString,
	 IN PANSI_STRING  SourceString,
	 IN BOOLEAN);

VOID (WINAPI *_RtlInitAnsiString)
	(IN OUT PANSI_STRING  DestinationString,
	 IN PCHAR  SourceString);

VOID (WINAPI *_RtlFreeUnicodeString)
	(IN PUNICODE_STRING  UnicodeString);

NTSTATUS (WINAPI *_RtlUnicodeStringToAnsiString)
	(IN OUT PANSI_STRING  DestinationString  OPTIONAL,
	IN PUNICODE_STRING  SourceString,
	IN BOOLEAN  AllocateDestinationString);

VOID (WINAPI *_RtlFreeAnsiString)
	(IN PANSI_STRING AnsiString);

NTSTATUS (WINAPI *_NtOpenSection)
	(OUT PHANDLE  SectionHandle,
	 IN ACCESS_MASK  DesiredAccess,
	 IN POBJECT_ATTRIBUTES  ObjectAttributes);

NTSTATUS (WINAPI *_NtMapViewOfSection)
	(IN HANDLE  SectionHandle,
	 IN HANDLE  ProcessHandle,
	 IN OUT PVOID  *BaseAddress,
	 IN ULONG  ZeroBits,
	 IN ULONG  CommitSize,
	 IN OUT PLARGE_INTEGER  SectionOffset,
	 IN OUT PULONG  ViewSize,
	 IN SECTION_INHERIT  InheritDisposition,
	 IN ULONG  AllocationType,
	 IN ULONG  Protect);

NTSTATUS (WINAPI *_NtUnmapViewOfSection)
	(IN HANDLE ProcessHandle,
	 IN PVOID BaseAddress);

NTSTATUS (WINAPI * _NtQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);

NTSTATUS (WINAPI * _NtCreateSymbolicLinkObject)(PHANDLE SymLinkHandle,
                                    ACCESS_MASK DesiredAccess,
                                    POBJECT_ATTRIBUTES ObAttributes,
                                    PUNICODE_STRING ObName);


//*********************************************************************************************
//*********************************************************************************************
// START PE File Handling Functions
//*********************************************************************************************
//*********************************************************************************************

//*******************************************************************************************************
// PE File structure declarations
//
//*******************************************************************************************************

struct PE_Header 
{
	unsigned long signature;
	unsigned short machine;
	unsigned short numSections;
	unsigned long timeDateStamp;
	unsigned long pointerToSymbolTable;
	unsigned long numOfSymbols;
	unsigned short sizeOfOptionHeader;
	unsigned short characteristics;
};

struct PE_ExtHeader
{
	unsigned short magic;
	unsigned char majorLinkerVersion;
	unsigned char minorLinkerVersion;
	unsigned long sizeOfCode;
	unsigned long sizeOfInitializedData;
	unsigned long sizeOfUninitializedData;
	unsigned long addressOfEntryPoint;
	unsigned long baseOfCode;
	unsigned long baseOfData;
	unsigned long imageBase;
	unsigned long sectionAlignment;
	unsigned long fileAlignment;
	unsigned short majorOSVersion;
	unsigned short minorOSVersion;
	unsigned short majorImageVersion;
	unsigned short minorImageVersion;
	unsigned short majorSubsystemVersion;
	unsigned short minorSubsystemVersion;
	unsigned long reserved1;
	unsigned long sizeOfImage;
	unsigned long sizeOfHeaders;
	unsigned long checksum;
	unsigned short subsystem;
	unsigned short DLLCharacteristics;
	unsigned long sizeOfStackReserve;
	unsigned long sizeOfStackCommit;
	unsigned long sizeOfHeapReserve;
	unsigned long sizeOfHeapCommit;
	unsigned long loaderFlags;
	unsigned long numberOfRVAAndSizes;
	unsigned long exportTableAddress;
	unsigned long exportTableSize;
	unsigned long importTableAddress;
	unsigned long importTableSize;
	unsigned long resourceTableAddress;
	unsigned long resourceTableSize;
	unsigned long exceptionTableAddress;
	unsigned long exceptionTableSize;
	unsigned long certFilePointer;
	unsigned long certTableSize;
	unsigned long relocationTableAddress;
	unsigned long relocationTableSize;
	unsigned long debugDataAddress;
	unsigned long debugDataSize;
	unsigned long archDataAddress;
	unsigned long archDataSize;
	unsigned long globalPtrAddress;
	unsigned long globalPtrSize;
	unsigned long TLSTableAddress;
	unsigned long TLSTableSize;
	unsigned long loadConfigTableAddress;
	unsigned long loadConfigTableSize;
	unsigned long boundImportTableAddress;
	unsigned long boundImportTableSize;
	unsigned long importAddressTableAddress;
	unsigned long importAddressTableSize;
	unsigned long delayImportDescAddress;
	unsigned long delayImportDescSize;
	unsigned long COMHeaderAddress;
	unsigned long COMHeaderSize;
	unsigned long reserved2;
	unsigned long reserved3;
};


struct SectionHeader
{
	unsigned char sectionName[8];
	unsigned long virtualSize;
	unsigned long virtualAddress;
	unsigned long sizeOfRawData;
	unsigned long pointerToRawData;
	unsigned long pointerToRelocations;
	unsigned long pointerToLineNumbers;
	unsigned short numberOfRelocations;
	unsigned short numberOfLineNumbers;
	unsigned long characteristics;
};

struct MZHeader
{
	unsigned short signature;
	unsigned short partPag;
	unsigned short pageCnt;
	unsigned short reloCnt;
	unsigned short hdrSize;
	unsigned short minMem;
	unsigned short maxMem;
	unsigned short reloSS;
	unsigned short exeSP;
	unsigned short chksum;
	unsigned short exeIP;
	unsigned short reloCS;
	unsigned short tablOff;
	unsigned short overlay;
	unsigned char reserved[32];
	unsigned long offsetToPE;
};


struct ImportDirEntry
{
	DWORD importLookupTable;
	DWORD timeDateStamp;
	DWORD fowarderChain;
	DWORD nameRVA;
	DWORD importAddressTable;
};


struct FixupBlock
{
	unsigned long pageRVA;
	unsigned long blockSize;
};


DWORD myStrlenA(char *ptr)
{
	DWORD len = 0;
	while(*ptr)
	{
		len++;
		ptr++;
	}

	return len;
}


BOOL myStrcmpA(char *str1, char *str2)
{
	while(*str1 && *str2)
	{
		if(*str1 == *str2)
		{
			str1++;
			str2++;
		}
		else
		{
			return FALSE;
		}
	}

	if(*str1 && !*str2)
	{
		return FALSE;
	}
	else if(*str2 && !*str1)
	{
		return FALSE;
	}

	return TRUE;	
}


//*******************************************************************************************************
// Fills the various structures with info of a PE image.  The PE image is located at modulePos.
//
//*******************************************************************************************************

bool readPEInfo(char *modulePos, MZHeader *outMZ, PE_Header *outPE, PE_ExtHeader *outpeXH,
				SectionHeader **outSecHdr)
{
	// read MZ Header
	MZHeader *mzH;
	mzH = (MZHeader *)modulePos;

	if(mzH->signature != 0x5a4d)		// MZ
	{
		TRACE("File does not have MZ header\n");
		return false;
	}

	// read PE Header
	PE_Header *peH;
	peH = (PE_Header *)(modulePos + mzH->offsetToPE);

	if(peH->sizeOfOptionHeader != sizeof(PE_ExtHeader))
	{
		TRACE("Unexpected option header size.\n");
		
		return false;
	}

	// read PE Ext Header
	PE_ExtHeader *peXH;
	peXH = (PE_ExtHeader *)((char *)peH + sizeof(PE_Header));

	// read the sections
	SectionHeader *secHdr = (SectionHeader *)((char *)peXH + sizeof(PE_ExtHeader));

	*outMZ = *mzH;
	*outPE = *peH;
	*outpeXH = *peXH;
	*outSecHdr = secHdr;

	return true;
}


//*******************************************************************************************************
// Returns the total size required to load a PE image into memory
//
//*******************************************************************************************************

int calcTotalImageSize(MZHeader *inMZ, PE_Header *inPE, PE_ExtHeader *inpeXH,
				       SectionHeader *inSecHdr)
{
	int result = 0;
	int alignment = inpeXH->sectionAlignment;

	if(inpeXH->sizeOfHeaders % alignment == 0)
		result += inpeXH->sizeOfHeaders;
	else
	{
		int val = inpeXH->sizeOfHeaders / alignment;
		val++;
		result += (val * alignment);
	}
	for(int i = 0; i < inPE->numSections; i++)
	{
		if(inSecHdr[i].virtualSize)
		{
			if(inSecHdr[i].virtualSize % alignment == 0)
				result += inSecHdr[i].virtualSize;
			else
			{
				int val = inSecHdr[i].virtualSize / alignment;
				val++;
				result += (val * alignment);
			}
		}
	}

	return result;
}


//*******************************************************************************************************
// Returns the aligned size of a section
//
//*******************************************************************************************************

unsigned long getAlignedSize(unsigned long curSize, unsigned long alignment)
{	
	if(curSize % alignment == 0)
		return curSize;
	else
	{
		int val = curSize / alignment;
		val++;
		return (val * alignment);
	}
}

//*******************************************************************************************************
// Copy a PE image from exePtr to ptrLoc with proper memory alignment of all sections
//
//*******************************************************************************************************

bool loadPE(char *exePtr, MZHeader *inMZ, PE_Header *inPE, PE_ExtHeader *inpeXH,
			SectionHeader *inSecHdr, LPVOID ptrLoc)
{
	char *outPtr = (char *)ptrLoc;
	
	memcpy(outPtr, exePtr, inpeXH->sizeOfHeaders);
	outPtr += getAlignedSize(inpeXH->sizeOfHeaders, inpeXH->sectionAlignment);

	for(int i = 0; i < inPE->numSections; i++)
	{
		if(inSecHdr[i].sizeOfRawData > 0)
		{
			unsigned long toRead = inSecHdr[i].sizeOfRawData;
			if(toRead > inSecHdr[i].virtualSize)
				toRead = inSecHdr[i].virtualSize;

			memcpy(outPtr, exePtr + inSecHdr[i].pointerToRawData, toRead);

			outPtr += getAlignedSize(inSecHdr[i].virtualSize, inpeXH->sectionAlignment);
		}
	}

	return true;
}


//*******************************************************************************************************
// Loads the DLL into memory and align it
//
//*******************************************************************************************************

LPVOID loadDLL(char *dllName)
{
	char moduleFilename[MAX_PATH + 1];
	LPVOID ptrLoc = NULL;
	MZHeader mzH2;
	PE_Header peH2;
	PE_ExtHeader peXH2;
	SectionHeader *secHdr2;

	GetSystemDirectory(moduleFilename, MAX_PATH);
	if((myStrlenA(moduleFilename) + myStrlenA(dllName)) >= MAX_PATH)
		return NULL;

	strcat(moduleFilename, dllName);

	// load this EXE into memory because we need its original Import Hint Table

	HANDLE fp;
	fp = CreateFile(moduleFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	
	if(fp != INVALID_HANDLE_VALUE)
	{
		BY_HANDLE_FILE_INFORMATION fileInfo;
		GetFileInformationByHandle(fp, &fileInfo);

		DWORD fileSize = fileInfo.nFileSizeLow;
		//TRACE("Size = %d\n", fileSize);
		if(fileSize)
		{
			LPVOID exePtr = HeapAlloc(GetProcessHeap(), 0, fileSize);
			if(exePtr)
			{
				DWORD read;

				if(ReadFile(fp, exePtr, fileSize, &read, NULL) && read == fileSize)
				{					
					if(readPEInfo((char *)exePtr, &mzH2, &peH2, &peXH2, &secHdr2))
					{
						int imageSize = calcTotalImageSize(&mzH2, &peH2, &peXH2, secHdr2);						

						//ptrLoc = VirtualAlloc(NULL, imageSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
						ptrLoc = HeapAlloc(GetProcessHeap(), 0, imageSize);
						if(ptrLoc)
						{							
							loadPE((char *)exePtr, &mzH2, &peH2, &peXH2, secHdr2, ptrLoc);
						}
					}

				}
				HeapFree(GetProcessHeap(), 0, exePtr);
			}
		}
		CloseHandle(fp);
	}

	return ptrLoc;
}


DWORD procAPIExportAddr(DWORD hModule, char *apiName)
{	
	if(!hModule || !apiName)
		return 0;

	char *ptr = (char *)hModule;
	ptr += 0x3c;		// offset 0x3c contains offset to PE header
	
	ptr = (char *)(*(DWORD *)ptr) + hModule + 0x78;		// offset 78h into PE header contains addr of export table

	ptr = (char *)(*(DWORD *)ptr) + hModule;			// ptr now points to export directory table

	// offset 24 into the export directory table == number of entries in the Export Name Pointer Table
	// table
	DWORD numEntries = *(DWORD *)(ptr + 24);
	//TRACE("NumEntries = %d\n", numEntries);

	DWORD *ExportNamePointerTable = (DWORD *)(*(DWORD *)(ptr + 32) + hModule);  // offset 32 into export directory contains offset to Export Name Pointer Table	
	
	DWORD ordinalBase = *((DWORD *)(ptr + 16));
	//TRACE("OrdinalBase is %d\n", ordinalBase);


	WORD *ExportOrdinalTable = (WORD *)((*(DWORD *)(ptr + 36)) + hModule);	// offset 36 into export directory contains offset to Ordinal Table
	DWORD *ExportAddrTable = (DWORD *)((*(DWORD *)(ptr + 28)) + hModule); // offset 28 into export directory contains offset to Export Addr Table

	for(DWORD i = 0; i < numEntries; i++)
	{		
		char *exportName = (char *)(ExportNamePointerTable[i] + hModule);

		if(myStrcmpA(exportName, apiName) == TRUE)
		{			
			WORD ordinal = ExportOrdinalTable[i];
			//TRACE("%s (i = %d) Ordinal = %d at %X\n", exportName, i, ordinal, ExportAddrTable[ordinal]);

			return (DWORD)(ExportAddrTable[ordinal]);
		}		
	}

	return 0;
}

//*********************************************************************************************
//*********************************************************************************************
// END PE File Handling Functions
//*********************************************************************************************
//*********************************************************************************************


//*******************************************************************************************************
// Gets address of native API's that we'll be using
//
//*******************************************************************************************************

BOOL getNativeAPIs(void)
{
	HMODULE hntdll;

	hntdll = GetModuleHandle("ntdll.dll");
			
	*(FARPROC *)&_RtlAnsiStringToUnicodeString = 
			GetProcAddress(hntdll, "RtlAnsiStringToUnicodeString");

	*(FARPROC *)&_RtlInitAnsiString = 
			GetProcAddress(hntdll, "RtlInitAnsiString");

	*(FARPROC *)&_RtlFreeUnicodeString = 
			GetProcAddress(hntdll, "RtlFreeUnicodeString");

	*(FARPROC *)&_RtlFreeAnsiString = 
			GetProcAddress(hntdll, "RtlFreeAnsiString");

	*(FARPROC *)&_RtlUnicodeStringToAnsiString = 
			GetProcAddress(hntdll, "RtlUnicodeStringToAnsiString");

	*(FARPROC *)&_NtOpenSection =
			GetProcAddress(hntdll, "NtOpenSection");

	*(FARPROC *)&_NtMapViewOfSection =
			GetProcAddress(hntdll, "NtMapViewOfSection");

	*(FARPROC *)&_NtUnmapViewOfSection =
			GetProcAddress(hntdll, "NtUnmapViewOfSection");

	*(FARPROC *)&_NtCreateSymbolicLinkObject =
			GetProcAddress(hntdll, "NtCreateSymbolicLinkObject");

	*(FARPROC *)&_NtQuerySystemInformation =
		GetProcAddress(hntdll, "ZwQuerySystemInformation");

	if(_RtlAnsiStringToUnicodeString && _RtlInitAnsiString && _RtlFreeUnicodeString &&
	   _RtlUnicodeStringToAnsiString && _RtlFreeAnsiString &&		
	   _NtOpenSection && _NtMapViewOfSection && _NtUnmapViewOfSection && _NtQuerySystemInformation)
	{
		return TRUE;
	}
	return FALSE;
}



BOOL createSymLink(char *inObjName, char *inLinkName)
{
	OBJECT_ATTRIBUTES oAttr;
	BOOL result = TRUE;
	HANDLE hSymLink;

	if(!_NtCreateSymbolicLinkObject)
		return FALSE;

	ANSI_STRING aObjName;
		
	_RtlInitAnsiString(&aObjName, inObjName);
						
	UNICODE_STRING uObjName;

	if(_RtlAnsiStringToUnicodeString(&uObjName, &aObjName, TRUE) != STATUS_SUCCESS)
	{		
		return FALSE;
	}

	ANSI_STRING aLinkName;

	_RtlInitAnsiString(&aLinkName, inLinkName);
						
	UNICODE_STRING uLinkName;

	if(_RtlAnsiStringToUnicodeString(&uLinkName, &aLinkName, TRUE) != STATUS_SUCCESS)
	{	
		_RtlFreeUnicodeString(&uObjName);
		return FALSE;
	}

    oAttr.Length = sizeof(OBJECT_ATTRIBUTES);
    oAttr.RootDirectory = NULL;
    oAttr.Attributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
    oAttr.ObjectName = &uLinkName;
    oAttr.SecurityDescriptor = NULL;
    oAttr.SecurityQualityOfService = NULL;

	if (_NtCreateSymbolicLinkObject(&hSymLink, SYMBOLIC_LINK_ALL_ACCESS,
                                    &oAttr, &uObjName) != STATUS_SUCCESS)
	{
		result = FALSE;
		goto done;
	}

done:
	_RtlFreeUnicodeString(&uObjName);
	_RtlFreeUnicodeString(&uLinkName);
	return result;	
}


//*******************************************************************************************************
// Obtain a handle to \device\physicalmemory
//
//*******************************************************************************************************

HANDLE openPhyMem(char *name)
{
	HANDLE hPhyMem;
	OBJECT_ATTRIBUTES oAttr;

	ANSI_STRING aStr;
		
	_RtlInitAnsiString(&aStr, name);
						
	UNICODE_STRING uStr;

	if(_RtlAnsiStringToUnicodeString(&uStr, &aStr, TRUE) != STATUS_SUCCESS)
	{		
		return INVALID_HANDLE_VALUE;	
	}

    oAttr.Length = sizeof(OBJECT_ATTRIBUTES);
    oAttr.RootDirectory = NULL;
    oAttr.Attributes = OBJ_CASE_INSENSITIVE;
    oAttr.ObjectName = &uStr;
    oAttr.SecurityDescriptor = NULL;
    oAttr.SecurityQualityOfService = NULL;

	if(_NtOpenSection(&hPhyMem, SECTION_MAP_READ, &oAttr ) != STATUS_SUCCESS)
	{
		_RtlFreeUnicodeString(&uStr);
		return INVALID_HANDLE_VALUE;
	}

	_RtlFreeUnicodeString(&uStr);
	return hPhyMem;
}


//*******************************************************************************************************
// Map in a section of physical memory into this process's virtual address space.
//
//*******************************************************************************************************

BOOL mapPhyMem(HANDLE hPhyMem, DWORD *phyAddr, DWORD *length, PVOID *virtualAddr)
{
	NTSTATUS			ntStatus;
	PHYSICAL_ADDRESS	viewBase;

	*virtualAddr = 0;
	viewBase.QuadPart = (ULONGLONG) (*phyAddr);

	ntStatus = _NtMapViewOfSection(hPhyMem, (HANDLE)-1, virtualAddr, 0,
								*length, &viewBase, length,
                                ViewShare, 0, PAGE_READONLY );

	if(ntStatus != STATUS_SUCCESS)
	{
		TRACE("Failed to map physical memory view of length %X at %X!\n", *length, *phyAddr);
		return FALSE;					
	}

	*phyAddr = viewBase.LowPart;
	return TRUE;
}


//*******************************************************************************************************
// Unmap section of physical memory
//
//*******************************************************************************************************

void unmapPhyMem(DWORD virtualAddr)
{
	NTSTATUS status;

	status = _NtUnmapViewOfSection((HANDLE)-1, (PVOID)virtualAddr);
	if(status != STATUS_SUCCESS)
	{
		TRACE("Unmapping view failed!\n");
	}
}


//*******************************************************************************************************
// Assign SECTION_MAP_READ assess of \device\physicalmemory to current user.
//
//*******************************************************************************************************

BOOL assignACL(char *inObjName)
{
	HANDLE hPhyMem;
	OBJECT_ATTRIBUTES oAttr;
	BOOL result = FALSE;

	ANSI_STRING aStr;
		
	_RtlInitAnsiString(&aStr, inObjName);
						
	UNICODE_STRING uStr;

	if(_RtlAnsiStringToUnicodeString(&uStr, &aStr, TRUE) != STATUS_SUCCESS)
	{		
		return FALSE;
	}

    oAttr.Length = sizeof(OBJECT_ATTRIBUTES);
    oAttr.RootDirectory = NULL;
    oAttr.Attributes = OBJ_CASE_INSENSITIVE;
    oAttr.ObjectName = &uStr;
    oAttr.SecurityDescriptor = NULL;
    oAttr.SecurityQualityOfService = NULL;

	if(_NtOpenSection(&hPhyMem, READ_CONTROL | WRITE_DAC, &oAttr ) != STATUS_SUCCESS)
	{
		_RtlFreeUnicodeString(&uStr);
		return FALSE;
	}
	else
	{
		PACL dacl;
		PSECURITY_DESCRIPTOR sd;
		
		if(GetSecurityInfo(hPhyMem, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL,
						&dacl, NULL, &sd) == ERROR_SUCCESS)
		{
			EXPLICIT_ACCESS ea;
			char userName[MAX_PATH];
			DWORD userNameSize = MAX_PATH-1;

			GetUserName(userName, &userNameSize);
			ea.grfAccessPermissions = SECTION_MAP_READ;
			ea.grfAccessMode = GRANT_ACCESS;
			ea.grfInheritance = NO_INHERITANCE;
			ea.Trustee.pMultipleTrustee = NULL;
			ea.Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
			ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
			ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
			ea.Trustee.ptstrName = userName;

			PACL newDacl;
			if(SetEntriesInAcl(1, &ea, dacl, &newDacl) == ERROR_SUCCESS)
			{
				if(SetSecurityInfo(hPhyMem, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL,
								newDacl, NULL) == ERROR_SUCCESS)
				{		
					result = TRUE;
				}

				LocalFree(newDacl);
			}
		}
	}

	_RtlFreeUnicodeString(&uStr);
	return result;	
}


//*********************************************************************************************
// Assign loaddriver priviledge to our process, so we can load our support driver.
//
//*********************************************************************************************

BOOL getLoadDriverPriv()
{
	HANDLE hToken;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		LUID huid;
		if(LookupPrivilegeValue(NULL, "SeLoadDriverPrivilege", &huid))
		{
			LUID_AND_ATTRIBUTES priv;
			priv.Attributes = SE_PRIVILEGE_ENABLED;
			priv.Luid = huid;

			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			tp.Privileges[0] = priv;

			if(AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

//*********************************************************************************************
// Sets up the necessary registry settings to load the support driver
//
//*********************************************************************************************

BOOL setupRegistry()
{
	HKEY hkey;
	if(RegCreateKey(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\"DRV_NAME, &hkey) != ERROR_SUCCESS)
		return FALSE;

	DWORD val;
	val = 1;
	if(RegSetValueEx(hkey, "Type", 0, REG_DWORD, (PBYTE)&val, sizeof(val)) != ERROR_SUCCESS)
		return FALSE;

	if(RegSetValueEx(hkey, "ErrorControl", 0, REG_DWORD, (PBYTE)&val, sizeof(val)) != ERROR_SUCCESS)
		return FALSE;
	
	val = 3;
	if(RegSetValueEx(hkey, "Start", 0, REG_DWORD, (PBYTE)&val, sizeof(val)) != ERROR_SUCCESS)
		return FALSE;

	char *imgName = "System32\\DRIVERS\\"DRV_FILENAME;
	
	if(RegSetValueEx(hkey, "ImagePath", 0, REG_EXPAND_SZ, (PBYTE)imgName, strlen(imgName)) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

//*********************************************************************************************
// Actual code to load our driver into memory
//
//*********************************************************************************************

BOOL loadDriver()
{
	// call ntdll APIs
	HMODULE hntdll;
	NTSTATUS (WINAPI * _RtlAnsiStringToUnicodeString)
		(PUNICODE_STRING  DestinationString,
		 IN PANSI_STRING  SourceString,
		 IN BOOLEAN);

	VOID (WINAPI *_RtlInitAnsiString)
		(IN OUT PANSI_STRING  DestinationString,
		 IN PCHAR  SourceString);

	NTSTATUS (WINAPI * _ZwLoadDriver)
		(IN PUNICODE_STRING DriverServiceName);

	NTSTATUS (WINAPI * _ZwUnloadDriver)
		(IN PUNICODE_STRING DriverServiceName);

	VOID (WINAPI * _RtlFreeUnicodeString)
		(IN PUNICODE_STRING  UnicodeString);


	hntdll = GetModuleHandle("ntdll.dll");
			
	*(FARPROC *)&_ZwLoadDriver = GetProcAddress(hntdll, "NtLoadDriver");

	*(FARPROC *)&_ZwUnloadDriver = GetProcAddress(hntdll, "NtUnloadDriver");
				
	*(FARPROC *)&_RtlAnsiStringToUnicodeString = 
			GetProcAddress(hntdll, "RtlAnsiStringToUnicodeString");

	*(FARPROC *)&_RtlInitAnsiString = 
			GetProcAddress(hntdll, "RtlInitAnsiString");

	*(FARPROC *)&_RtlFreeUnicodeString = 
			GetProcAddress(hntdll, "RtlFreeUnicodeString");

	if(_ZwLoadDriver && _ZwUnloadDriver && _RtlAnsiStringToUnicodeString &&
	   _RtlInitAnsiString && _RtlFreeUnicodeString)
	{
		ANSI_STRING aStr;
		
		_RtlInitAnsiString(&aStr, 
		"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\"DRV_NAME);
						
		UNICODE_STRING uStr;

		if(_RtlAnsiStringToUnicodeString(&uStr, &aStr, TRUE) != STATUS_SUCCESS)
			return FALSE;
		else
		{
			if(_ZwLoadDriver(&uStr) == STATUS_SUCCESS)
			{
				_RtlFreeUnicodeString(&uStr);
				return TRUE;
			}
			_RtlFreeUnicodeString(&uStr);
		}
	}

	return FALSE;
}


//*********************************************************************************************
// Actual code to remove our driver from memory
//
//*********************************************************************************************

BOOL unloadDriver()
{
	// call ntdll APIs
	HMODULE hntdll;
	NTSTATUS (WINAPI * _RtlAnsiStringToUnicodeString)
		(PUNICODE_STRING  DestinationString,
		 IN PANSI_STRING  SourceString,
		 IN BOOLEAN);

	VOID (WINAPI *_RtlInitAnsiString)
		(IN OUT PANSI_STRING  DestinationString,
		 IN PCHAR  SourceString);

	NTSTATUS (WINAPI * _ZwLoadDriver)
		(IN PUNICODE_STRING DriverServiceName);

	NTSTATUS (WINAPI * _ZwUnloadDriver)
		(IN PUNICODE_STRING DriverServiceName);

	VOID (WINAPI * _RtlFreeUnicodeString)
		(IN PUNICODE_STRING  UnicodeString);


	hntdll = GetModuleHandle("ntdll.dll");
			
	*(FARPROC *)&_ZwLoadDriver = GetProcAddress(hntdll, "NtLoadDriver");

	*(FARPROC *)&_ZwUnloadDriver = GetProcAddress(hntdll, "NtUnloadDriver");
				
	*(FARPROC *)&_RtlAnsiStringToUnicodeString = 
			GetProcAddress(hntdll, "RtlAnsiStringToUnicodeString");

	*(FARPROC *)&_RtlInitAnsiString = 
			GetProcAddress(hntdll, "RtlInitAnsiString");

	*(FARPROC *)&_RtlFreeUnicodeString = 
			GetProcAddress(hntdll, "RtlFreeUnicodeString");

	if(_ZwLoadDriver && _ZwUnloadDriver && _RtlAnsiStringToUnicodeString &&
	   _RtlInitAnsiString && _RtlFreeUnicodeString)
	{
		ANSI_STRING aStr;
		
		_RtlInitAnsiString(&aStr, 
		"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\"DRV_NAME);
						
		UNICODE_STRING uStr;

		if(_RtlAnsiStringToUnicodeString(&uStr, &aStr, TRUE) != STATUS_SUCCESS)
			return FALSE;
		else
		{
			if(_ZwUnloadDriver(&uStr) == STATUS_SUCCESS)
			{
				_RtlFreeUnicodeString(&uStr);
				return TRUE;
			}
			_RtlFreeUnicodeString(&uStr);
		}
	}

	return FALSE;
}


//*********************************************************************************************
// Removes our driver file and registry settings
//
//*********************************************************************************************

void cleanupDriver(void)
{
	char sysDir[MAX_PATH + 1];
	GetSystemDirectory(sysDir, MAX_PATH);
	strncat(sysDir, "\\drivers\\"DRV_FILENAME, MAX_PATH);
	DeleteFile(sysDir);

	RegDeleteKey(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\"DRV_NAME"\\Enum");
	RegDeleteKey(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\"DRV_NAME);
}


//*********************************************************************************************
// Attempts to get a handle to our kernel driver.  If fails, try to install the driver.
//
//*********************************************************************************************

HANDLE openDriver(void)
{
	HANDLE hDevice;
	
	hDevice = CreateFile("\\\\.\\"DRV_NAME, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		   				 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hDevice == INVALID_HANDLE_VALUE)
	{		
		char drvFullPath[MAX_PATH+1];
		char *filePart;

		ZeroMemory(drvFullPath, MAX_PATH);		
		GetFullPathName(DRV_FILENAME, MAX_PATH, drvFullPath, &filePart);
		
		//TRACE("%s\n", drvFullPath);
		HANDLE hFile = CreateFile(drvFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL, 0);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			TRACE("Cannot find required driver file %s\n", drvFullPath);
			return INVALID_HANDLE_VALUE;
		}
		else
		{
			CloseHandle(hFile);

			char sysDir[MAX_PATH + 1];
			GetSystemDirectory(sysDir, MAX_PATH);
			strncat(sysDir, "\\drivers\\"DRV_FILENAME, MAX_PATH);
			CopyFile(drvFullPath, sysDir, TRUE);

			if(!getLoadDriverPriv())
			{
				TRACE("Error getting load driver privilege!\n");
			}
			else
			{
				if(!setupRegistry())
				{
					TRACE("Error setting driver registry keys!\nMake sure you are running this as Administrator.\n");
				}
				else
				{
					loadDriver();
					hDevice = CreateFile("\\\\.\\"DRV_NAME, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		   				 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if(hDevice == INVALID_HANDLE_VALUE)
					{
						TRACE("Error loading kernel support driver!\nMake sure you are running this as Administrator.\n");
					}
				}
			}
			cleanupDriver();
		}
	}
	
	return hDevice;
}


//*********************************************************************************************
// Remove our kernel driver from memory
//
//*********************************************************************************************

void uninstallDriver(void)
{
	char drvFullPath[MAX_PATH+1];
	char *filePart;

	ZeroMemory(drvFullPath, MAX_PATH);		
	GetFullPathName(DRV_FILENAME, MAX_PATH, drvFullPath, &filePart);

	HANDLE hFile = CreateFile(drvFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("Cannot find required driver file %s\n", drvFullPath);
		return;
	}
	else
	{
		CloseHandle(hFile);

		char sysDir[MAX_PATH + 1];
		GetSystemDirectory(sysDir, MAX_PATH);
		strncat(sysDir, "\\drivers\\"DRV_FILENAME, MAX_PATH);
		CopyFile(drvFullPath, sysDir, TRUE);

		if(!getLoadDriverPriv())
		{
			TRACE("Error getting load driver privilege!\n");
		}
		else
		{
			if(!setupRegistry())
			{
				TRACE("Error setting driver registry keys!\nMake sure you are running this as Administrator.\n");
			}
			else
			{
				if(unloadDriver())
					TRACE("Support driver successfully unloaded.\n");
				else
					TRACE("Unload support driver failed.  It is probably not loaded.\n");
			}
		}
		cleanupDriver();
	}
}


HANDLE openMemory(void)
{
	HANDLE hPhyMem = INVALID_HANDLE_VALUE;

	hPhyMem = openPhyMem(PHYMEM_OBJNAME);		// open \device\physicalmemory

	if(hPhyMem == INVALID_HANDLE_VALUE)
	{
		assignACL(PHYMEM_OBJNAME);
		hPhyMem = openPhyMem(PHYMEM_OBJNAME);
	}
	
	if(hPhyMem == INVALID_HANDLE_VALUE)
	{
		if(createSymLink(PHYMEM_OBJNAME, SYMLINK_OBJNAME))	// access via symlink
		{		
			hPhyMem = openPhyMem(SYMLINK_OBJNAME);

			if(hPhyMem == INVALID_HANDLE_VALUE)
			{
				assignACL(SYMLINK_OBJNAME);
				hPhyMem = openPhyMem(SYMLINK_OBJNAME);
			}
		}
		if(hPhyMem == INVALID_HANDLE_VALUE)
		{
			TRACE("Could not open physical memory device!\nMake sure you are running as Administrator.\n");			
		}
		else
			TRACE("Accessing %s via NtCreateSymbolicLinkObject\n\n", PHYMEM_OBJNAME);	
	}

	return hPhyMem;
}


DWORD getRequiredSize(DWORD wantedAddr, DWORD len)
{	
	DWORD endAddr = wantedAddr + len;
	DWORD alignedAddr = wantedAddr & PAGE_SIZE_MASK;
	DWORD alignedEndAddr = (endAddr & PAGE_SIZE_MASK) + PAGE_SIZE;
	
	return alignedEndAddr - alignedAddr;
}


//*********************************************************************************************
// Map in physical pages.  This handles the case where the size of the page to be mapped
// takes up more than 1 non-consecutive physical pages. 
//
// len parameter must be page aligned
//
//*********************************************************************************************

BOOL mapPhyPages(HANDLE hPhyMem, HANDLE hDevice, DWORD *kernelVirtualAddr, DWORD *len, LPVOID *outPtr)
{
	BOOL result = FALSE;
	DWORD phyAddr, virtualAddr, mapLen, ptr, byteRet, i;
	unsigned char *resultPtr, *curPtr;

	virtualAddr = *kernelVirtualAddr & PAGE_SIZE_MASK;		// page align	

	resultPtr = (unsigned char *)malloc(*len);
	if(!resultPtr)
		goto done;

	curPtr = resultPtr;

	for(i = 0; i < *len / PAGE_SIZE; i++)
	{
		phyAddr = virtualAddr;
		mapLen = PAGE_SIZE;

		if(!DeviceIoControl(hDevice, IOCTL_CODE2, &phyAddr, 4, &phyAddr, 4, &byteRet, NULL))
			goto done;								

		if(!mapPhyMem(hPhyMem, &phyAddr, &mapLen, (LPVOID *)&ptr))
			goto done;

		memcpy(curPtr, (LPVOID)ptr, PAGE_SIZE);

		unmapPhyMem((DWORD)ptr);
		ptr = NULL;	

		virtualAddr += PAGE_SIZE;
		curPtr += PAGE_SIZE;
	}
	
	result = TRUE;
	*kernelVirtualAddr = *kernelVirtualAddr & PAGE_SIZE_MASK;		// page align	
	*outPtr = (LPVOID *)resultPtr;

done:
	if(!result)
		free(resultPtr);

	return result;
}

//*********************************************************************************************
//*********************************************************************************************
// START Scheduler List Traversal Functions
//*********************************************************************************************
//*********************************************************************************************

#define WIN2K_EPROC_OFFSETFROM_WAITLISTENTRY		(0x1D0)
#define WIN2K_ACTIVELINKS_OFFSET					(0xA0)

#define WINXP_EPROC_OFFSETFROM_WAITLISTENTRY		(0x1C0)
#define WINXP_ACTIVELINKS_OFFSET					(0x88)

#define WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK		(0x1c0)


#define NOT_SUPPORTED	0
#define WIN2K			1
#define WINXP			2

DWORD gKernelVersion					= NOT_SUPPORTED;


//*********************************************************************************************
// Adds the address of a process's EPROCESS structure to list only if it don't already
// exists.
//
//*********************************************************************************************

int addProcessToList(LINKED_LIST *procList, DWORD eprocAddr)
{
	LINKED_LIST_NODE *curNode = procList->head;

	while(curNode)
	{
		PROCINFO *pi = (PROCINFO *)curNode->data;

		if(pi->pId == eprocAddr)
		{
			return 1;		// found, so don't add
		}
		curNode = curNode->next;
	}	

	PROCINFO *pNew = (PROCINFO *)malloc(sizeof(PROCINFO));
	if(pNew)
	{
		ZeroMemory(pNew, sizeof(PROCINFO));
		
		pNew->pId = eprocAddr;
		insertItem(procList, pNew);
		return 2;		// added
	}
	return -1;	// malloc error
}


//*********************************************************************************************
// getSchProcessList - retrieve list of processes by traversing EPROCESS.KTHREAD.WaitList Entry 
// via \device\physicalmemory
//
// On Win2K, it traverses KiWaitInListHead, KiWaitOutListHead and KiDispatcherReadyListHead
//
// On WinXP it traverses KiWaitListHead and KiDispatcherReadyListHead.
// Support for WinXP is incomplete and doesn't lists all process since traversing the two lists
// doesn't give me all the processes.
//
// caller must initialize drvList and free the inserted items
// 
//*********************************************************************************************

DWORD getSchProcessList(LINKED_LIST *procList)
{
	HANDLE hDevice = openDriver();
	HANDLE hPhyMem = INVALID_HANDLE_VALUE;
	unsigned char *ptr = NULL;	
	DWORD count = 0;
	BOOL hasError = TRUE;
	DWORD aEPROC_OFFSETFROM_WAITLISTENTRY	= 0;
	DWORD aACTIVELINKS_OFFSET				= 0;
	DWORD endIndex;
	DWORD byteRet;
	DWORD wantedAddr;
	DWORD procNameOffset;
	DWORD len, start;

	if(gKernelVersion == WIN2K)
	{
		aEPROC_OFFSETFROM_WAITLISTENTRY = WIN2K_EPROC_OFFSETFROM_WAITLISTENTRY;
		aACTIVELINKS_OFFSET = WIN2K_ACTIVELINKS_OFFSET;		
	}
	else if(gKernelVersion == WINXP)
	{
		aEPROC_OFFSETFROM_WAITLISTENTRY = WINXP_EPROC_OFFSETFROM_WAITLISTENTRY;
		aACTIVELINKS_OFFSET = WINXP_ACTIVELINKS_OFFSET;		
	}

	if(hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD addresses[8];		
		hPhyMem = openMemory();

		if(hPhyMem != INVALID_HANDLE_VALUE)
		{
			if(!DeviceIoControl(hDevice, IOCTL_CODE1, NULL, 0, addresses, sizeof(addresses), &byteRet, NULL))
				goto done;
			
			procNameOffset = addresses[4];
			if(addresses[5] == 0 || addresses[6] == 0 || procNameOffset == 0)
				goto done;

			DWORD listHeadAddress[34];
			endIndex = 1;
			listHeadAddress[0] = addresses[5];
			listHeadAddress[1] = addresses[6];

			if(addresses[7])
			{
				endIndex += 32;
				DWORD curHeadAddr = addresses[7];

				for(int i = 0, j = 2; i < 32; i++, j++)
				{
					listHeadAddress[j] = curHeadAddr;
					curHeadAddr += 8;
				}
			}			
			
			for(DWORD listIndex = 0; listIndex <= endIndex; listIndex ++)
			{
				DWORD kiWaitInListHead = listHeadAddress[listIndex];
				DWORD limit = kiWaitInListHead;
				
				//TRACE("%.8X ", kiWaitInListHead);
				
				wantedAddr = kiWaitInListHead;
				len = getRequiredSize(wantedAddr, sizeof(DWORD));

				// map in page containing KiWaitInListHead
				if(!mapPhyPages(hPhyMem, hDevice, &kiWaitInListHead, &len, (LPVOID *)&ptr) || len < 4)
					goto done;

				start = wantedAddr - kiWaitInListHead;	
				DWORD curThread = *(DWORD *)&ptr[start];

				//TRACE(" %.8X %.8X\n", curThread, *(DWORD *)&ptr[start+4]);

				free(ptr); ptr = NULL;	
			
				while(curThread != limit)
				{					
					DWORD prevThread;					

					wantedAddr = curThread;
					len = getRequiredSize(wantedAddr, aEPROC_OFFSETFROM_WAITLISTENTRY + sizeof(DWORD));

					// map in page containing ETHREAD structure for current thread, staring at WaitListEntry
					if(!mapPhyPages(hPhyMem, hDevice, &curThread, &len, (LPVOID *)&ptr) || 
						len < aEPROC_OFFSETFROM_WAITLISTENTRY + sizeof(DWORD))
					{
						goto done;
					}

					start = wantedAddr - curThread;	
					prevThread = curThread;
					curThread = *(DWORD *)&ptr[start];		// Flink

					DWORD curProcess = *(DWORD *)&ptr[start + aEPROC_OFFSETFROM_WAITLISTENTRY];

					free(ptr); ptr = NULL;	

					DWORD addResult = addProcessToList(procList, curProcess);
					if(addResult == 2)
						count++;		// added
					else if(addResult == -1)
						goto done;		// malloc error

					if(curThread == prevThread)		// loop check
						goto done;
				}
			}
		}
		else
			goto done;
	}

	hasError = FALSE;
done:
	if(ptr)
		free(ptr);

	LINKED_LIST_NODE *curNode = procList->head;

	// retrieve the PIDs and Process names
	while(curNode)
	{
		PROCINFO *pi = (PROCINFO *)curNode->data;
		DWORD curProcess = pi->pId;

		// map in page containing EPROCESS

		wantedAddr = curProcess;
		DWORD reqSize = aACTIVELINKS_OFFSET + procNameOffset + NT_PROCNAMELEN;
		len = getRequiredSize(wantedAddr, reqSize);

		// map in page contain ETHREAD structure for current thread
		if(!mapPhyPages(hPhyMem, hDevice, &curProcess, &len, (LPVOID *)&ptr) || len < reqSize)
		{
			curNode = curNode->next;
			continue;
		}

		start = wantedAddr - curProcess;	
		DWORD curPID = *(DWORD *)&ptr[start + aACTIVELINKS_OFFSET - 4];
		char *procName = (char *)&ptr[start + aACTIVELINKS_OFFSET + procNameOffset];

		pi->pId = curPID;		
		memcpy(pi->pName, procName, NT_PROCNAMELEN);
		pi->pName[NT_PROCNAMELEN-1] = 0;

		//TRACE("%d %s\n", curPID, procName);

		free(ptr); ptr = NULL;	

		curNode = curNode->next;
	}	

	if(hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
	if(hPhyMem != INVALID_HANDLE_VALUE)
		CloseHandle(hPhyMem);

	if(hasError)
		TRACE("ENUMERATION OF PROCESS LIST TERMINATED ABNORMALLY.\nRESULTS MAY BE INACCURATE!\n\n");

	return count;
}

//*********************************************************************************************
//*********************************************************************************************
// END Scheduler List Traversal Functions
//*********************************************************************************************
//*********************************************************************************************



//*********************************************************************************************
//*********************************************************************************************
// START Process List Traversal Functions
//*********************************************************************************************
//*********************************************************************************************

//*********************************************************************************************
// getProcessList - retrieve list of processes by traversing EPROCESS.ActiveProcessLinks via
// \device\physicalmemory
//
// caller must initialize drvList and free the inserted items
// 
//*********************************************************************************************

DWORD getProcessList(LINKED_LIST *procList)
{
	HANDLE hDevice = openDriver();
	HANDLE hPhyMem = INVALID_HANDLE_VALUE;
	unsigned char *ptr = NULL;	
	DWORD count = 0;
	BOOL hasError = TRUE;

	if(hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD addresses[8];
		DWORD byteRet;
		hPhyMem = openMemory();

		if(hPhyMem != INVALID_HANDLE_VALUE)
		{
			if(!DeviceIoControl(hDevice, IOCTL_CODE1, NULL, 0, addresses, sizeof(addresses), &byteRet, NULL))
				goto done;

			DWORD psInitialSystemProcessLink = addresses[3];				// PsInitialSystemProcess.ActiveLinks
			DWORD procNameOffset = addresses[4];		// Offset to imageName
			if(psInitialSystemProcessLink == 0 || procNameOffset == 0)
				goto done;

			DWORD curAddr = psInitialSystemProcessLink;	
			DWORD head = curAddr;

			do
			{
				curAddr -= 4;		// include the pid

				DWORD wantedAddr = curAddr;
				DWORD reqLen;
				
				if(gKernelVersion == WIN2K)
					reqLen = procNameOffset + NT_PROCNAMELEN + 4;
				else 
					reqLen = WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK + 8;

				DWORD len = getRequiredSize(wantedAddr, reqLen);
				
				// map in page containing EPROCESS
				if(!mapPhyPages(hPhyMem, hDevice, &curAddr, &len, (LPVOID *)&ptr) || len < reqLen)
					goto done;

				DWORD start = wantedAddr - curAddr;		
				DWORD pid = *(DWORD *)&ptr[start];
				char *procName = (char *)&ptr[start + procNameOffset + 4];
								
				//TRACE("%5d %.8X - %s\n", pid, *(DWORD *)&ptr[start + WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK + 4], procName);

				PROCINFO *pInfo = (PROCINFO *)malloc(sizeof(PROCINFO));
				if(pInfo)
				{
					memcpy(pInfo->pName, procName, NT_PROCNAMELEN);
					pInfo->pName[NT_PROCNAMELEN-1] = 0;
					pInfo->pId = pid;
					pInfo->pFlag = 0;
					
					if(gKernelVersion == WINXP)
					{
						pInfo->pFlag = *(DWORD *)&ptr[start + WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK + 4];						
					}

					insertItem(procList, pInfo);
				}
				else
					goto done;
				count++;

				if(strcmpi(procName, "system") == 0)
					head = *(DWORD *)&ptr[start + 8]; 

				// next module
				curAddr = *(DWORD *)&ptr[start + 4];
				// unmap
				free(ptr); ptr = NULL;
			}			
			while(curAddr != head);
		}
		else
		{
			goto done;
		}
	}

	hasError = FALSE;
done:
	if(ptr)
		free(ptr);
	if(hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
	if(hPhyMem != INVALID_HANDLE_VALUE)
		CloseHandle(hPhyMem);

	if(hasError)
		TRACE("ENUMERATION OF PROCESS LIST TERMINATED ABNORMALLY.\nRESULTS MAY BE INACCURATE!\n\n");

	return count;
}


//*********************************************************************************************
// Sorts processes array by PID
//
//*********************************************************************************************

void sortProcInfoByPID(PROCINFO *pi, DWORD numRec)
{
	if(!pi || !numRec)
		return;

	DWORD minPID = 0;

	for(DWORD i = 0; i < numRec; i++)
	{
		minPID = i;

		for(DWORD j = i + 1; j < numRec; j++)
		{
			if(pi[j].pId < pi[minPID].pId)
				minPID = j;
		}
		if(minPID != i)
		{
			PROCINFO temp;
			temp = pi[i];
			pi[i] = pi[minPID];
			pi[minPID] = temp;
		}
	}
}


//*********************************************************************************************
//*********************************************************************************************
// START HANDLE_TABLE List Traversal Functions
//*********************************************************************************************
//*********************************************************************************************

#define WIN2K_OBJECTTABLE_OFFSETFROM_ACTIVELINKS	(0x128-WIN2K_ACTIVELINKS_OFFSET)
#define WIN2K_HANDLETABLELIST_OFFSET				(0x54)
#define WIN2K_EPROC_OFFSETFROM_HANDLETABLE			(0x0c)

#define WINXP_OBJECTTABLE_OFFSETFROM_ACTIVELINKS	(0xC4-WINXP_ACTIVELINKS_OFFSET)
#define WINXP_HANDLETABLELIST_OFFSET				(0x1C)
#define WINXP_EPROC_OFFSETFROM_HANDLETABLE			(0x04)


//*********************************************************************************************
// getHandleProcessList - retrieve list of processes by traversing HANDLE_TABLE links via
// \device\physicalmemory
// http://www.rootkit.com/newsread.php?newsid=307
//
// caller must initialize procList and free the inserted items
// 
//*********************************************************************************************

DWORD getHandleProcessList(LINKED_LIST *procList)
{
	HANDLE hDevice = openDriver();
	HANDLE hPhyMem = INVALID_HANDLE_VALUE;
	unsigned char *ptr = NULL;	
	DWORD count = 0;
	BOOL hasError = TRUE;

	if(hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD addresses[8];
		DWORD byteRet;
		PROCINFO *pInfo = NULL;
		hPhyMem = openMemory();

		if(hPhyMem != INVALID_HANDLE_VALUE)
		{
			if(!DeviceIoControl(hDevice, IOCTL_CODE1, NULL, 0, addresses, sizeof(addresses), &byteRet, NULL))
				goto done;

			DWORD psInitialSystemProcessLink = addresses[3];				// PsInitialSystemProcess.ActiveLinks
			DWORD procNameOffset = addresses[4];		// Offset to imageName
			if(psInitialSystemProcessLink == 0 || procNameOffset == 0)
				goto done;

			DWORD curAddr = psInitialSystemProcessLink;	

			// map in the EPROCESS pointed to by PsInitialSystemProcess
			DWORD reqLen;
			curAddr -= 4;
			
			if(gKernelVersion == WIN2K)
				reqLen = procNameOffset + NT_PROCNAMELEN + 4;
			else
				reqLen = WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK + 8;

			DWORD wantedAddr = curAddr;
			DWORD len = getRequiredSize(wantedAddr, reqLen);

			// map in page containing EPROCESS of "System" process
			if(!mapPhyPages(hPhyMem, hDevice, &curAddr, &len, (LPVOID *)&ptr) || len < reqLen)
				goto done;

			DWORD start = wantedAddr - curAddr;		
			DWORD handleTablePtr;
			DWORD pid = *(DWORD *)&ptr[start];
			char *procName = (char *)&ptr[start + procNameOffset + 4];

			// save this process info
			pInfo = (PROCINFO *)malloc(sizeof(PROCINFO));
			if(pInfo)
			{
				memcpy(pInfo->pName, procName, NT_PROCNAMELEN);
				pInfo->pName[NT_PROCNAMELEN-1] = 0;
				pInfo->pId = pid;
				pInfo->pFlag = 0;
						
				if(gKernelVersion == WINXP)
					pInfo->pFlag = *(DWORD *)&ptr[start + WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK + 4];

				insertItem(procList, pInfo);
				count = 1;
			}
			else
				goto done;
			
			if(gKernelVersion == WIN2K)
				handleTablePtr = *(DWORD *)&ptr[start + 4 + WIN2K_OBJECTTABLE_OFFSETFROM_ACTIVELINKS];	// Get address of its ObjectTable
			else
				handleTablePtr = *(DWORD *)&ptr[start + 4 + WINXP_OBJECTTABLE_OFFSETFROM_ACTIVELINKS];	// Get address of its ObjectTable

			// unmap
			free(ptr); ptr = NULL;
		
			// map in the Handle Table of "System" process
			DWORD head = handleTablePtr;
			DWORD loopCount = 0;

			do
			{
				loopCount++;
				wantedAddr = curAddr = handleTablePtr;

				if(gKernelVersion == WIN2K)
					reqLen = WIN2K_HANDLETABLELIST_OFFSET + 8;		// win2K
				else
					reqLen = WINXP_HANDLETABLELIST_OFFSET + 8;		// winXP

				len = getRequiredSize(wantedAddr, reqLen);

				// map in page containing the HANDLE TABLE
				if(!mapPhyPages(hPhyMem, hDevice, &curAddr, &len, (LPVOID *)&ptr) || len < reqLen)
					goto done;

				start = wantedAddr - curAddr;

				
				/*TRACE1("PID = %d\n", *(DWORD *)&ptr[start+0x8]);
				TRACE1("Next Link = %.8X\n", *(DWORD *)&ptr[start+WINXP_HANDLETABLELIST_OFFSET]);
				TRACE1("EPROC Addr = %.8X\n", *(DWORD *)&ptr[start+WINXP_EPROC_OFFSETFROM_HANDLETABLE]);
				TRACE1("Handle Count = %.8X\n", *(DWORD *)&ptr[start+0x3c]);
				*/

				DWORD eProcPtr;

				if(gKernelVersion == WIN2K)
				{
					handleTablePtr = *(DWORD *)&ptr[start+WIN2K_HANDLETABLELIST_OFFSET] - WIN2K_HANDLETABLELIST_OFFSET;
					eProcPtr = *(DWORD *)&ptr[start+WIN2K_EPROC_OFFSETFROM_HANDLETABLE];		// win2k
				}
				else
				{
					handleTablePtr = *(DWORD *)&ptr[start+WINXP_HANDLETABLELIST_OFFSET] - WINXP_HANDLETABLELIST_OFFSET;
					eProcPtr = *(DWORD *)&ptr[start+WINXP_EPROC_OFFSETFROM_HANDLETABLE];		// winxp
				}

				free(ptr); ptr = NULL;

				// skip the first EPROCESS since, we've already added seen it before the loop
				if(loopCount > 1 && eProcPtr && ((eProcPtr & 0xFF000000) >> 24) >= 0x80)
				{
					//===== map in EPROCESS to copy the process name =====//
					if(gKernelVersion == WIN2K)
					{
						eProcPtr += WIN2K_ACTIVELINKS_OFFSET - 4;
						reqLen = procNameOffset + NT_PROCNAMELEN + 4;
					}
					else
					{
						eProcPtr += WINXP_ACTIVELINKS_OFFSET - 4;
						reqLen = WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK + 8;
					}

					wantedAddr = curAddr = eProcPtr;
					len = getRequiredSize(wantedAddr, reqLen);

					// map in page containing EPROCESS
					if(!mapPhyPages(hPhyMem, hDevice, &curAddr, &len, (LPVOID *)&ptr) || len < reqLen)
						continue;

					start = wantedAddr - curAddr;
					
					pid = *(DWORD *)&ptr[start];
					procName = (char *)&ptr[start + procNameOffset + 4];
					
					pInfo = (PROCINFO *)malloc(sizeof(PROCINFO));
					if(pInfo)
					{
						memcpy(pInfo->pName, procName, NT_PROCNAMELEN);
						pInfo->pName[NT_PROCNAMELEN-1] = 0;
						pInfo->pId = pid;
						pInfo->pFlag = 0;
						
						if(gKernelVersion == WINXP)
							pInfo->pFlag = *(DWORD *)&ptr[start + WINXP_PROCFLAG_OFFSETFROM_ACTIVELINK + 4];

						insertItem(procList, pInfo);
						//TRACE1("getHandleProcessList:%d.",count);
						//TRACE1("getHandleProcessList:%d\n",pInfo->pId);
					}
					else
						goto done;
					
					count++;
					free(ptr); ptr = NULL;
				}				
			} while(handleTablePtr != head);
		}			
	}

	hasError = FALSE;
done:
	if(ptr)
		free(ptr);
	if(hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
	if(hPhyMem != INVALID_HANDLE_VALUE)
		CloseHandle(hPhyMem);

	if(hasError)
		count = 0;

	return count;
}


//*********************************************************************************************
//*********************************************************************************************
// END Handle Table List Traversal Functions
//*********************************************************************************************
//*********************************************************************************************




//*********************************************************************************************
//*********************************************************************************************
// START Driver List Traversal Functions
//*********************************************************************************************
//*********************************************************************************************

//*********************************************************************************************
// getDriverList - retrieve list of drivers by traversing PsLoadedModuleList via
// \device\physicalmemory
//
// caller must initialize drvList and free the inserted items
// 
//*********************************************************************************************

DWORD getDriverList(LINKED_LIST *drvList)
{
	HANDLE hDevice = openDriver();
	HANDLE hPhyMem = INVALID_HANDLE_VALUE;
	unsigned char *ptr = NULL;	
	DWORD count = 0;
	BOOL hasError = TRUE;

	if(hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD addresses[8];
		DWORD byteRet;
		hPhyMem = openMemory();

		if(hPhyMem != INVALID_HANDLE_VALUE)
		{
			if(!DeviceIoControl(hDevice, IOCTL_CODE1, NULL, 0, addresses, sizeof(addresses), &byteRet, NULL))
				goto done;

			DWORD phyAddr = addresses[0];		// PsLoadedModuleList
			if(phyAddr == 0)
				goto done;

			DWORD psLoadedModuleListHead = phyAddr;
			DWORD wantedAddr = phyAddr;
			DWORD len = getRequiredSize(wantedAddr, sizeof(MODULE_ENTRY));

			// map in page containing PsLoadedModuleList
			if(!mapPhyPages(hPhyMem, hDevice, &phyAddr, &len, (LPVOID *)&ptr) || len < 4)
				goto done;

			DWORD start = wantedAddr - phyAddr;		
			phyAddr = *(DWORD *)&ptr[start];

			free(ptr); ptr = NULL;

			while(phyAddr != psLoadedModuleListHead)
			{
				wantedAddr = phyAddr;
				len = getRequiredSize(wantedAddr, sizeof(MODULE_ENTRY));

				// map in page containing MODULE_ENTRY
				if(!mapPhyPages(hPhyMem, hDevice, &phyAddr, &len, (LPVOID *)&ptr) || len < sizeof(MODULE_ENTRY))
					goto done;

				start = wantedAddr - phyAddr;		
				PMODULE_ENTRY pMod = (PMODULE_ENTRY)&ptr[start];
				DWORD imageBase = pMod->imageBase;
				DWORD imageSize = pMod->imageSize;
				
				UNICODE_STRING ustr;
				DWORD ustrAddr = (DWORD)pMod->drvPath.Buffer;
				ustr.Length = pMod->drvPath.Length;
				ustr.MaximumLength = pMod->drvPath.Length;
				
				// next module
				phyAddr = (DWORD)pMod->link.Flink;
				// unmap
				free(ptr); ptr = NULL;

				len = getRequiredSize(ustrAddr, ustr.Length);

				// map in page containing UNICODE_STRING buffer of the driver's name
				wantedAddr = ustrAddr;
				if(!mapPhyPages(hPhyMem, hDevice, &ustrAddr, &len, (LPVOID *)&ptr) || len < ustr.Length)
					goto done;

				start = wantedAddr - ustrAddr;
				ustr.Buffer = (PWSTR)&ptr[start];

				DRIVERINFO *dInfo = (DRIVERINFO *)malloc(sizeof(DRIVERINFO));
				if(dInfo)
				{
					ANSI_STRING astr;
					dInfo->loadAddr = imageBase;
					dInfo->imageSize = imageSize;

					if(_RtlUnicodeStringToAnsiString(&astr, &ustr, TRUE) == STATUS_SUCCESS)
					{
						TRACE("%.8X, %.8X -> %s\n", imageBase, imageSize, astr.Buffer);
						ZeroMemory(dInfo->drvName, MAX_PATH);
						strncpy(dInfo->drvName, astr.Buffer, MAX_PATH - 1);

						_RtlFreeAnsiString(&astr);
					}
					else
					{
						TRACE("%.8X, %.8X -> unknown\n", imageBase, imageSize);
						strncpy(dInfo->drvName, "unknown", MAX_PATH - 1);
					}
	
					insertItem(drvList, dInfo);
				}
				else
					goto done;
				count++;

				// unmap
				free(ptr); ptr = NULL;
			}			
		}
		else
		{
			goto done;
		}
	}

	hasError = FALSE;
done:
	if(ptr)
		free(ptr);
	if(hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);
	if(hPhyMem != INVALID_HANDLE_VALUE)
		CloseHandle(hPhyMem);

	if(hasError)
		return 0;

	return count;
}


//*********************************************************************************************
// Callback function of the LINKED_LIST structure to check whether every driver entry
// obtained by traversing PsLoadedModuleList can be found in the copy obtained by calling
// ZwQuerySystemInformation.
//
// procData contains driver list return by ZwQuerySystemInformation
//
//*********************************************************************************************

BOOL compareDriverList(LPVOID nodeData, LPVOID procData)
{
	DWORD numEntries = *((DWORD *)procData);
	DRIVERINFO *dInfo = (DRIVERINFO *)nodeData;
		
	SYSTEM_MODULE_INFORMATION *smi = (SYSTEM_MODULE_INFORMATION *)((char *)procData + sizeof(DWORD));
	BOOL found = FALSE;

	for(DWORD j = 0; j < numEntries; j++)
	{
		if(dInfo->loadAddr == (DWORD)smi->Base && strcmpi(dInfo->drvName, smi->ImageName) == 0)
		{
			found = TRUE;
			break;
		}
		smi++;
	}
			
	if(!found)
		TRACE("%8X * %s --[Hidden]--\n", dInfo->loadAddr, dInfo->drvName);
	else
		TRACE("%8X - %s\n", dInfo->loadAddr, dInfo->drvName);			

	return TRUE;
}

//*********************************************************************************************
//*********************************************************************************************
// START ntoskrnl KiServiceTable Checking Functions
//*********************************************************************************************
//*********************************************************************************************

#define NTOSKRNL_SSDT	1
#define WIN32K_SSDT		2

DWORD *getKiServiceTableEntries(DWORD *outNumEntries, DWORD outKiAddr[2], DWORD tableType)
{
	HANDLE hDevice = openDriver();
	HANDLE hPhyMem = INVALID_HANDLE_VALUE;
	unsigned char *ptr = NULL;	
	DWORD count = 0;
	BOOL hasError = TRUE;
	DWORD *retVal = NULL;	
	*outNumEntries = 0;

	if(hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD addresses[8];
		DWORD byteRet;
		hPhyMem = openMemory();

		if(hPhyMem != INVALID_HANDLE_VALUE)
		{
			if(!DeviceIoControl(hDevice, IOCTL_CODE1, NULL, 0, addresses, sizeof(addresses), &byteRet, NULL))
				goto done;

			DWORD KeServiceDescriptorTable;
			
			if(tableType == NTOSKRNL_SSDT)
				KeServiceDescriptorTable = addresses[1];		// KeServiceDescriptorTable
			else
				KeServiceDescriptorTable = addresses[2];		// KeServiceDescriptorTableShadow

			if(KeServiceDescriptorTable == 0)
				goto done;

			DWORD wantedAddr = KeServiceDescriptorTable;
			DWORD len = getRequiredSize(wantedAddr, sizeof(SDE) << 2);

			// map in page containing KeServiceDescriptorTable
			if(!mapPhyPages(hPhyMem, hDevice, &KeServiceDescriptorTable, &len, (LPVOID *)&ptr) || len < (sizeof(SDE) << 2))
				goto done;

			DWORD start = wantedAddr - KeServiceDescriptorTable;		
			DWORD KiServiceTable;
			DWORD numEntries, maxEntries; 
			
			if(tableType == NTOSKRNL_SSDT)
			{
				KiServiceTable = *(DWORD *)&ptr[start];
				numEntries = *(DWORD *)&ptr[start + 8];
				maxEntries = 300;
				outKiAddr[0] = addresses[1];		// KeServiceDescriptorTable
			}
			else
			{
				KiServiceTable = *(DWORD *)&ptr[start + 16];
				numEntries = *(DWORD *)&ptr[start + 24];
				maxEntries = 700;
				outKiAddr[0] = addresses[2];		// KeServiceDescriptorTableShadow
			}
			
			outKiAddr[1] = KiServiceTable;
			
			free(ptr); ptr = NULL;

			if(numEntries > maxEntries)
			{
				TRACE("Number of KiServiceTable(%d) entries > %d???\n", numEntries, maxEntries);
				goto done;
			}
			
			if(KiServiceTable == 0)
				goto done;
			
			wantedAddr = KiServiceTable;
			len = getRequiredSize(wantedAddr, sizeof(DWORD) * numEntries);
			DWORD reqLen = len;

			// map in page containing KeServiceDescriptorTable
			if(!mapPhyPages(hPhyMem, hDevice, &KiServiceTable, &len, (LPVOID *)&ptr) || len < reqLen)
				goto done;

			retVal = (DWORD *)malloc(sizeof(DWORD) * numEntries);

			if(retVal)
			{
				*outNumEntries = numEntries;
				start = wantedAddr - KiServiceTable;
				DWORD *SSDT = (DWORD *)&ptr[start];
				for(DWORD i = 0; i < numEntries; i++)
				{
					retVal[i] = SSDT[i];
				}
			}

			free(ptr); ptr = NULL;
		}
	}
done:
	if(ptr)
		free(ptr);

	return retVal;
}


//*********************************************************************************************
// Checks that the address of KiServiceTable found in the disk image of ntoskrnl has an
// entry in the relocation table.  This makes sure that it's not an arbitrary constant..
//
//*********************************************************************************************

BOOL checkKiServiceTableAddr(PVOID exeAddr, DWORD chkAddr, PE_ExtHeader *peXH2)
{
	if(peXH2->relocationTableAddress && peXH2->relocationTableSize)
	{
		FixupBlock *fixBlk = (FixupBlock *)((char *)exeAddr + peXH2->relocationTableAddress);		

		while(fixBlk->blockSize)
		{
			int numEntries = (fixBlk->blockSize - sizeof(FixupBlock)) >> 1;
	
			unsigned short *offsetPtr = (unsigned short *)(fixBlk + 1);

			for(int i = 0; i < numEntries; i++)
			{				
				int relocType = (*offsetPtr & 0xF000) >> 12;
				
				if(relocType == 3)
				{
					DWORD *codeLoc = (DWORD *)((char *)exeAddr + fixBlk->pageRVA + (*offsetPtr & 0x0FFF));
					
					if(fixBlk->pageRVA + (*offsetPtr & 0x0FFF) + peXH2->imageBase == chkAddr)
					{
						return TRUE;
					}
				}
				offsetPtr++;
			}
			fixBlk = (FixupBlock *)offsetPtr;
		}
	}
	return FALSE;
}


//*********************************************************************************************
// Thanks to 90210 for this excellent way of getting the KiServiceTable address from the disk image of
// ntoskrnl.exe
// http://www.rootkit.com/newsread.php?newsid=176
//
//*********************************************************************************************

DWORD getKiServiceTableAddr(PVOID exeAddr, DWORD sdtAddr, PE_ExtHeader *peXH2)
{
	if(peXH2->relocationTableAddress && peXH2->relocationTableSize)
	{
		FixupBlock *fixBlk = (FixupBlock *)((char *)exeAddr + peXH2->relocationTableAddress);		

		while(fixBlk->blockSize)
		{
			//TRACE("Addr = %X\n", fixBlk->pageRVA);
			//TRACE("Size = %X\n", fixBlk->blockSize);

			int numEntries = (fixBlk->blockSize - sizeof(FixupBlock)) >> 1;
			//TRACE("Num Entries = %d\n", numEntries);

			unsigned short *offsetPtr = (unsigned short *)(fixBlk + 1);

			for(int i = 0; i < numEntries; i++)
			{				
				int relocType = (*offsetPtr & 0xF000) >> 12;
				
				//TRACE("Val = %X\n", *offsetPtr);
				//TRACE("Type = %X\n", relocType);

				if(relocType == 3)
				{
					DWORD *codeLoc = (DWORD *)((char *)exeAddr + fixBlk->pageRVA + (*offsetPtr & 0x0FFF));					

					if(*codeLoc == sdtAddr + peXH2->imageBase &&
						*(WORD *)((DWORD)codeLoc - 2) == 0x05c7)
					{
						DWORD kiServiceTableAddr = *(DWORD *)((DWORD)codeLoc + 4);
						
						// checks for presence of found address in the relocation table
						if(checkKiServiceTableAddr(exeAddr, kiServiceTableAddr, peXH2))
						{
							return kiServiceTableAddr - peXH2->imageBase;
						}
					}						
				}

				offsetPtr++;
			}

			fixBlk = (FixupBlock *)offsetPtr;
		}
	}
	return 0;
}


//*********************************************************************************************
// Builds a table of native API names using the export table of ntdll.dll
//
//*********************************************************************************************

BOOL buildNativeAPITable(DWORD hModule, char *nativeAPINames[], DWORD numNames)
{
	if(!hModule)
		return FALSE;

	char *ptr = (char *)hModule;
	ptr += 0x3c;		// offset 0x3c contains offset to PE header
	
	ptr = (char *)(*(DWORD *)ptr) + hModule + 0x78;		// offset 78h into PE header contains addr of export table

	ptr = (char *)(*(DWORD *)ptr) + hModule;			// ptr now points to export directory table

	
	// offset 24 into the export directory table == number of entries in the Name Pointer Table
	// table
	DWORD numEntries = *(DWORD *)(ptr + 24);	
	
	DWORD *ExportNamePointerTable = (DWORD *)(*(DWORD *)(ptr + 32) + hModule);  // offset 32 into export directory contains offset to Export Name Pointer Table	

	DWORD ordinalBase = *((DWORD *)(ptr + 16));

	WORD *ExportOrdinalTable = (WORD *)((*(DWORD *)(ptr + 36)) + hModule);	// offset 36 into export directory contains offset to Ordinal Table
	DWORD *ExportAddrTable = (DWORD *)((*(DWORD *)(ptr + 28)) + hModule); // offset 28 into export directory contains offset to Export Addr Table


	for(DWORD i = 0; i < numEntries; i++)
	{		
		// i now contains the index of the API in the Ordinal Table
		// ptr points to Export directory table

		WORD ordinalValue = ExportOrdinalTable[i];		
		DWORD apiAddr = (DWORD)ExportAddrTable[ordinalValue] + hModule;
		char *exportName = (char *)(ExportNamePointerTable[i] + hModule);
		
		// Win2K
		if(gKernelVersion == WIN2K &&
		   *((unsigned char *)apiAddr) == 0xB8 && 
		   *((unsigned char *)apiAddr + 9) == 0xCD && 
		   *((unsigned char *)apiAddr + 10) == 0x2E)
		{
			DWORD serviceNum = *(DWORD *)((char *)apiAddr + 1);
			if(serviceNum < numNames)
			{
				nativeAPINames[serviceNum] = exportName;
			}
			//TRACE("%X - %s\n", serviceNum, exportName);
		}

		// WinXP
		else if(gKernelVersion == WINXP &&
				*((unsigned char *)apiAddr) == 0xB8 && 
				*((unsigned char *)apiAddr + 5) == 0xBA && 
				*((unsigned char *)apiAddr + 6) == 0x00 &&
				*((unsigned char *)apiAddr + 7) == 0x03 &&
				*((unsigned char *)apiAddr + 8) == 0xFE &&
				*((unsigned char *)apiAddr + 9) == 0x7F)
		{
			DWORD serviceNum = *(DWORD *)((char *)apiAddr + 1);
			if(serviceNum < numNames)
			{
				nativeAPINames[serviceNum] = exportName;
			}
			//TRACE("%X - %s\n", serviceNum, exportName);
		}
	}

	return TRUE;
}


//*********************************************************************************************
// Find hooked ntoskrnl SSDT entries by compare it with the copy in the disk image.
//
//*********************************************************************************************

void findHookedSSDT(void)
{
	MZHeader mzH2;
	PE_Header peH2;
	PE_ExtHeader peXH2;
	SectionHeader *secHdr2;
	LPVOID ntdll = NULL;
	char **nativeApiNames = NULL;
	LINKED_LIST drvList;	
	PVOID exeAddr = NULL;
	DWORD drvCount = 0;
	LINKED_LIST_NODE *curNode;
	DRIVERINFO ntoskrnlInfo;
	DWORD fileKiServiceTableOffset;
	DWORD *fileKiServiceTable = NULL;
	DWORD numEntries;
	DWORD *SSDT = NULL;
	DWORD sdtAddr;
	BOOL ntoskrnlAddrFound = FALSE;
	DWORD KiAddr[2];
	char *tableName[2] = { "KeServiceDescriptorTable", "KiServiceTable" };

	initList(&drvList);

	TRACE("Checks SDT for Hooked Native APIs\n\n");

	exeAddr = loadDLL("\\ntoskrnl.exe");
	if(!exeAddr)
	{
		TRACE("Failed to load ntoskrnl.exe!\n");
		goto done;
	}

	if(!readPEInfo((char *)exeAddr, &mzH2, &peH2, &peXH2, &secHdr2))
	{
		TRACE("Failed to get PE header of ntoskrnl.exe!\n");
		goto done;
	}

	sdtAddr = procAPIExportAddr((DWORD)exeAddr, "KeServiceDescriptorTable");
	if(!sdtAddr)
	{
		TRACE("Failed to get address of KeServiceDescriptorTable!\n");
		goto done;
	}

	drvCount = getDriverList(&drvList);
	if(drvCount <= 0)
	{
		TRACE("Error getting Module/Driver list!\n");
		goto done;
	}

	curNode = drvList.head;
	
	while(curNode)
	{
		DRIVERINFO *dInfo = (DRIVERINFO *)curNode->data;
		
		strlwr(dInfo->drvName);
		if(strstr(dInfo->drvName, "ntoskrnl.exe"))
		{
			ntoskrnlInfo = *dInfo;
			ntoskrnlAddrFound = TRUE;
			break;
		}

		curNode = curNode->next;
	}	

	if(!ntoskrnlAddrFound)
	{
		TRACE("Error getting Kernel base address!\n");
		goto done;
	}

	fileKiServiceTableOffset = getKiServiceTableAddr(exeAddr, sdtAddr, &peXH2);
	fileKiServiceTable = (DWORD *)((DWORD)exeAddr + fileKiServiceTableOffset);	
	SSDT = getKiServiceTableEntries(&numEntries, KiAddr, NTOSKRNL_SSDT);

	if(SSDT)
	{
		TRACE("KeServiceDescriptorTable\t\t%X\n", KiAddr[0]);
		TRACE("KeServiceDescriptorTable.ServiceTable\t%X\n", KiAddr[1]);
		TRACE("KeServiceDescriptorTable.ServiceLimit\t%d\n\n", numEntries);

		nativeApiNames = (char **)malloc(sizeof(char *) * numEntries);
		if(!nativeApiNames)
		{
			TRACE("Error : Failed to allocate memory.\n");
			goto done;
		}
		ZeroMemory(nativeApiNames, sizeof(char *) * numEntries);

		ntdll = loadDLL("\\ntdll.dll");
		if(!ntdll)
		{
			TRACE("Error : Failed to load ntdll.dll\n");
			goto done;
		}

		buildNativeAPITable((DWORD)ntdll, nativeApiNames, numEntries);		
		DWORD hookCount = 0;

		for(DWORD i = 0; i < numEntries; i++)
		{
			if(SSDT[i] - ntoskrnlInfo.loadAddr != fileKiServiceTable[i] - peXH2.imageBase)	// hooked
			{
				BOOL found = FALSE;
				curNode = drvList.head;
				while(curNode)
				{
					DRIVERINFO *dInfo = (DRIVERINFO *)curNode->data;
					
					if(SSDT[i] >= dInfo->loadAddr && SSDT[i] < dInfo->loadAddr + dInfo->imageSize)
					{
						TRACE("%-25s %3X %s [%.8X]\n", 
								(nativeApiNames[i] ? nativeApiNames[i] : "Unknown API"), i,
								dInfo->drvName, SSDT[i]);

						found = TRUE;
						break;
					}
					curNode = curNode->next;
				}	
				if(!found)
				{
					TRACE("Entry %3X - [hooked by unknown at %.8X]\n", i, SSDT[i]);
				}
				hookCount++;
			}
		}
		if(hookCount)
			TRACE("\n");
		
		for(int p = 0; p < 2; p++)
		{
			if(KiAddr[p] < ntoskrnlInfo.loadAddr || 
			   KiAddr[p] >= ntoskrnlInfo.loadAddr + ntoskrnlInfo.imageSize)
			{
				TRACE("WARNING: %s is located outside ntoskrnl.exe\n\n", tableName[p]);

				curNode = drvList.head;
				while(curNode)
				{
					DRIVERINFO *dInfo = (DRIVERINFO *)curNode->data;
						
					if(KiAddr[p] >= dInfo->loadAddr && KiAddr[p] < dInfo->loadAddr + dInfo->imageSize)
					{
						TRACE("It is hooked by [%s]\n\n", dInfo->drvName);
						break;					
					}	
					curNode = curNode->next;
				}
			}
		}

		TRACE("Number of Service Table entries hooked = %u\n", hookCount);
	}
	else
	{
		TRACE("Error getting ServiceTable entries!\n");
	}

done:
	freeList(&drvList);
	if(exeAddr)
		HeapFree(GetProcessHeap(), 0, exeAddr);
	if(ntdll)
		HeapFree(GetProcessHeap(), 0, ntdll);
	if(SSDT)
		free(SSDT);
	if(nativeApiNames)
		free(nativeApiNames);
}


//*********************************************************************************************
// Find hooked win32k.sys SSDT entries by finding entries with addresses that are outside
// the memory image of win32k.sys.
//
//*********************************************************************************************

void findHookedGDI_SSDT(void)
{
	LINKED_LIST drvList;	
	DWORD drvCount = 0;
	LINKED_LIST_NODE *curNode;
	DRIVERINFO win32kInfo;

	DWORD numEntries;
	DWORD *SSDT = NULL;	
	BOOL win32kAddrFound = FALSE;
	DWORD KiAddr[2];	

	initList(&drvList);

	TRACE("Checks Shadow SDT for Hooked Native GDI APIs\n\n");

	drvCount = getDriverList(&drvList);
	if(drvCount <= 0)
	{
		TRACE("Error getting Module/Driver list!\n");
		goto done;
	}

	curNode = drvList.head;
	
	while(curNode)
	{
		DRIVERINFO *dInfo = (DRIVERINFO *)curNode->data;
		
		strlwr(dInfo->drvName);
		if(strstr(dInfo->drvName, "win32k.sys"))
		{
			win32kInfo = *dInfo;
			win32kAddrFound = TRUE;
			break;
		}

		curNode = curNode->next;
	}	

	SSDT = getKiServiceTableEntries(&numEntries, KiAddr, WIN32K_SSDT);

	if(SSDT)
	{
		TRACE("KeServiceDescriptorTableShadow\t\t\t\t%X\n", KiAddr[0]);
		TRACE("KeServiceDescriptorTableShadow.SDE[1].ServiceTable\t%X\n", KiAddr[1]);
		TRACE("KeServiceDescriptorTableShadow.SDE[1].ServiceLimit\t%d\n\n", numEntries);

		DWORD hookCount = 0;

		for(DWORD i = 0; i < numEntries; i++)
		{
			BOOL found = FALSE;
			curNode = drvList.head;

			if(SSDT[i] < win32kInfo.loadAddr || SSDT[i] >= win32kInfo.loadAddr + win32kInfo.imageSize)
			{
				while(curNode)
				{
					DRIVERINFO *dInfo = (DRIVERINFO *)curNode->data;
						
					if(SSDT[i] >= dInfo->loadAddr && SSDT[i] < dInfo->loadAddr + dInfo->imageSize)
					{
						TRACE("Entry %3X Hooked - %s [%.8X]\n", i, dInfo->drvName, SSDT[i]);
						found = TRUE;
						break;
					}
					curNode = curNode->next;
				}	
				if(!found)
				{
					TRACE("Entry %3X - [hooked by unknown at %.8X]\n", i, SSDT[i]);
				}
				hookCount++;
			}
		}
		if(hookCount)
			TRACE("\n");		

		if(KiAddr[1] < win32kInfo.loadAddr || 
		   KiAddr[1] >= win32kInfo.loadAddr + win32kInfo.imageSize)
		{
			BOOL found = FALSE;
			curNode = drvList.head;
			TRACE("WARNING: KeServiceDescriptorTableShadow.SDE[1].ServiceTable is\nlocated outside win32k.sys\n\n");
			
			while(curNode)
			{
				DRIVERINFO *dInfo = (DRIVERINFO *)curNode->data;
						
				if(KiAddr[1] >= dInfo->loadAddr && KiAddr[1] < dInfo->loadAddr + dInfo->imageSize)
				{
					TRACE("It is hooked by [%s]\n\n", dInfo->drvName);
					found = TRUE;
					break;					
				}	
				curNode = curNode->next;
			}
		}	

		TRACE("Number of GDI Service Table entries hooked = %u\n", hookCount);
	}
	else
	{
		TRACE("Error getting GDI ServiceTable entries!\n");
	}

done:
	freeList(&drvList);
	if(SSDT)
		free(SSDT);
}

