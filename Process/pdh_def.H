/*++

Copyright (C) 1996-1997 Microsoft Corporation

Module Name:

    PDH.H

Abstract:

    Header file for the Performance Data Helper (PDH) DLL functions.

--*/
#ifndef _PDH_H_
#define _PDH_H_

// system include files required for datatype and constant definitions
#include <windows.h>    // necessary for data types used in this file
#include <winperf.h>    // necessary for the Detail Level definitions

#ifdef __cplusplus
extern "C" {
#endif

typedef LONG            PDH_STATUS;

#define PDH_FUNCTION    PDH_STATUS __stdcall

// version info
#define PDH_CVERSION_WIN40  ((DWORD)(0x0400))
// v1.1 revision of PDH -- basic log functions
// v1.2 of the PDH -- adds variable instance counters
// v1.3 of the PDH -- adds log service control & stubs for NT5/PDH v2 fn's
#define PDH_VERSION         ((DWORD)((PDH_CVERSION_WIN40) + 0x0003))

// define severity masks
#define IsSuccessSeverity(ErrorCode)    \
    ((((DWORD)(ErrorCode) & (0xC0000000L)) == 0x00000000L) ? TRUE : FALSE)
#define IsInformationalSeverity(ErrorCode)    \
    ((((DWORD)(ErrorCode) & (0xC0000000L)) == 0x40000000L) ? TRUE : FALSE)
#define IsWarningSeverity(ErrorCode)    \
    ((((DWORD)(ErrorCode) & (0xC0000000L)) == 0x80000000L) ? TRUE : FALSE)
#define IsErrorSeverity(ErrorCode)      \
    ((((DWORD)(ErrorCode) & (0xC0000000L)) == 0xC0000000L) ? TRUE : FALSE)

// data type definitions

typedef HANDLE  HCOUNTER;
typedef HANDLE  HQUERY;
typedef HANDLE  HLOG;

typedef struct _PDH_RAW_COUNTER {
    DWORD       CStatus;
    FILETIME    TimeStamp;
    LONGLONG    FirstValue;
    LONGLONG    SecondValue;
    DWORD       MultiCount;
} PDH_RAW_COUNTER, *PPDH_RAW_COUNTER;

typedef struct _PDH_RAW_COUNTER_ITEM_A {
    LPSTR           szName;
    PDH_RAW_COUNTER RawValue;
} PDH_RAW_COUNTER_ITEM_A, *PPDH_RAW_COUNTER_ITEM_A;

typedef struct _PDH_RAW_COUNTER_ITEM_W {
    LPWSTR          szName;
    PDH_RAW_COUNTER RawValue;
} PDH_RAW_COUNTER_ITEM_W, *PPDH_RAW_COUNTER_ITEM_W;

typedef struct _PDH_FMT_COUNTERVALUE {
    DWORD    CStatus;
    union {
        LONG        longValue;
        double      doubleValue;
        LONGLONG    largeValue;
        LPSTR       AnsiStringValue;
        LPWSTR      WideStringValue;
    };
} PDH_FMT_COUNTERVALUE, *PPDH_FMT_COUNTERVALUE;

typedef struct _PDH_FMT_COUNTERVALUE_ITEM_A {
    LPSTR                   szName;
    PDH_FMT_COUNTERVALUE    FmtValue;
} PDH_FMT_COUNTERVALUE_ITEM_A, *PPDH_FMT_COUNTERVALUE_ITEM_A;

typedef struct _PDH_FMT_COUNTERVALUE_ITEM_W {
    LPWSTR                  szName;
    PDH_FMT_COUNTERVALUE    FmtValue;
} PDH_FMT_COUNTERVALUE_ITEM_W, *PPDH_FMT_COUNTERVALUE_ITEM_W;

typedef struct _PDH_STATISTICS {
    DWORD                   dwFormat;
    DWORD                   count;
    PDH_FMT_COUNTERVALUE    min;
    PDH_FMT_COUNTERVALUE    max;
    PDH_FMT_COUNTERVALUE    mean;
} PDH_STATISTICS, *PPDH_STATISTICS;

typedef struct _PDH_COUNTER_PATH_ELEMENTS_A {
    LPSTR   szMachineName;
    LPSTR   szObjectName;
    LPSTR   szInstanceName;
    LPSTR   szParentInstance;
    DWORD   dwInstanceIndex;
    LPSTR   szCounterName;
} PDH_COUNTER_PATH_ELEMENTS_A, *PPDH_COUNTER_PATH_ELEMENTS_A;

typedef struct _PDH_COUNTER_PATH_ELEMENTS_W {
    LPWSTR  szMachineName;
    LPWSTR  szObjectName;
    LPWSTR  szInstanceName;
    LPWSTR  szParentInstance;
    DWORD   dwInstanceIndex;
    LPWSTR  szCounterName;
} PDH_COUNTER_PATH_ELEMENTS_W, *PPDH_COUNTER_PATH_ELEMENTS_W;

typedef struct _PDH_DATA_ITEM_PATH_ELEMENTS_A {
    LPSTR   szMachineName;
    GUID    ObjectGUID;
    DWORD   dwItemId;
    LPSTR   szInstanceName;
} PDH_DATA_ITEM_PATH_ELEMENTS_A, * PPDH_DATA_ITEM_PATH_ELEMENTS_A;

typedef struct _PDH_DATA_ITEM_PATH_ELEMENTS_W {
    LPWSTR  szMachineName;
    GUID    ObjectGUID;
    DWORD   dwItemId;
    LPWSTR  szInstanceName;
} PDH_DATA_ITEM_PATH_ELEMENTS_W, * PPDH_DATA_ITEM_PATH_ELEMENTS_W;

typedef struct _PDH_COUNTER_INFO_A {
    DWORD   dwLength;
    DWORD   dwType;
    DWORD   CVersion;
    DWORD   CStatus;
    LONG    lScale;
    LONG    lDefaultScale;
    DWORD   dwUserData;
    DWORD   dwQueryUserData;
    LPSTR   szFullPath;
    union   {
        PDH_DATA_ITEM_PATH_ELEMENTS_A DataItemPath;
        PDH_COUNTER_PATH_ELEMENTS_A CounterPath;
        struct {
            LPSTR   szMachineName;
            LPSTR   szObjectName;
            LPSTR   szInstanceName;
            LPSTR   szParentInstance;
            DWORD   dwInstanceIndex;
            LPSTR   szCounterName;
        };
    };
    LPSTR   szExplainText;
    DWORD   DataBuffer[1];
} PDH_COUNTER_INFO_A, *PPDH_COUNTER_INFO_A;

typedef struct _PDH_COUNTER_INFO_W {
    DWORD   dwLength;
    DWORD   dwType;
    DWORD   CVersion;
    DWORD   CStatus;
    LONG    lScale;
    LONG    lDefaultScale;
    DWORD   dwUserData;
    DWORD   dwQueryUserData;
    LPWSTR  szFullPath;
    union   {
        PDH_DATA_ITEM_PATH_ELEMENTS_W DataItemPath;
        PDH_COUNTER_PATH_ELEMENTS_W CounterPath;
        struct {
            LPWSTR   szMachineName;
            LPWSTR   szObjectName;
            LPWSTR   szInstanceName;
            LPWSTR   szParentInstance;
            DWORD    dwInstanceIndex;
            LPWSTR   szCounterName;
        };
    };
    LPWSTR  szExplainText;
    DWORD   DataBuffer[1];
} PDH_COUNTER_INFO_W, *PPDH_COUNTER_INFO_W;

typedef struct _PDH_TIME_INFO {
    LONGLONG    StartTime;
    LONGLONG    EndTime;
    DWORD       SampleCount;
} PDH_TIME_INFO, *PPDH_TIME_INFO;

typedef struct _PDH_RAW_LOG_RECORD {
    DWORD       dwStructureSize;
    DWORD       dwRecordType;
    DWORD       dwItems;
    UCHAR       RawBytes[1];
} PDH_RAW_LOG_RECORD, *PPDH_RAW_LOG_RECORD;

typedef struct _PDH_LOG_SERVICE_QUERY_INFO_A {
    DWORD   dwSize;
    DWORD   dwFlags;
    DWORD   dwLogQuota;
    LPSTR   szLogFileCaption;
    LPSTR   szDefaultDir;
    LPSTR   szBaseFileName;
    DWORD   dwFileType;
    DWORD   dwReserved;
    union {
        struct {
            DWORD   PdlAutoNameInterval;
            DWORD   PdlAutoNameUnits;
            LPSTR   PdlCommandFilename;
            LPSTR   PdlCounterList;
            DWORD   PdlAutoNameFormat;
            DWORD   PdlSampleInterval;
            FILETIME    PdlLogStartTime;
            FILETIME    PdlLogEndTime;
        };
        struct {
            DWORD   TlNumberOfBuffers;
            DWORD   TlMinimumBuffers;
            DWORD   TlMaximumBuffers;
            DWORD   TlFreeBuffers;
            DWORD   TlBufferSize;
            DWORD   TlEventsLost;
            DWORD   TlLoggerThreadId;
            DWORD   TlBuffersWritten;
            DWORD   TlLogHandle;
            LPSTR   TlLogFileName;
        };
    };
} PDH_LOG_SERVICE_QUERY_INFO_A, *PPDH_LOG_SERVICE_QUERY_INFO_A;

typedef struct _PDH_LOG_SERVICE_QUERY_INFO_W {
    DWORD   dwSize;
    DWORD   dwFlags;
    DWORD   dwLogQuota;
    LPWSTR  szLogFileCaption;
    LPWSTR  szDefaultDir;
    LPWSTR  szBaseFileName;
    DWORD   dwFileType;
    DWORD   dwReserved;
    union {
        struct {
            DWORD   PdlAutoNameInterval;
            DWORD   PdlAutoNameUnits;
            LPWSTR  PdlCommandFilename;
            LPWSTR  PdlCounterList;
            DWORD   PdlAutoNameFormat;
            DWORD   PdlSampleInterval;
            FILETIME    PdlLogStartTime;
            FILETIME    PdlLogEndTime;
        };
        struct {
            DWORD   TlNumberOfBuffers;
            DWORD   TlMinimumBuffers;
            DWORD   TlMaximumBuffers;
            DWORD   TlFreeBuffers;
            DWORD   TlBufferSize;
            DWORD   TlEventsLost;
            DWORD   TlLoggerThreadId;
            DWORD   TlBuffersWritten;
            DWORD   TlLogHandle;
            LPWSTR  TlLogFileName;
        };
    };
} PDH_LOG_SERVICE_QUERY_INFO_W, *PPDH_LOG_SERVICE_QUERY_INFO_W;

//
//  Time value constants
//
#define MAX_TIME_VALUE      ((LONGLONG)0x7FFFFFFFFFFFFFFF)
#define MIN_TIME_VALUE      ((LONGLONG)0)
// dwFormat flag values
//
#define PDH_FMT_RAW     ((DWORD)0x00000010)
#define PDH_FMT_ANSI    ((DWORD)0x00000020)
#define PDH_FMT_UNICODE ((DWORD)0x00000040)
#define PDH_FMT_LONG    ((DWORD)0x00000100)
#define PDH_FMT_DOUBLE  ((DWORD)0x00000200)
#define PDH_FMT_LARGE   ((DWORD)0x00000400)
#define PDH_FMT_NOSCALE ((DWORD)0x00001000)
#define PDH_FMT_1000    ((DWORD)0x00002000)
#define PDH_FMT_NODATA  ((DWORD)0x00004000)

#define PERF_DETAIL_COSTLY   ((DWORD)0x00010000)
#define PERF_DETAIL_STANDARD ((DWORD)0x0000FFFF)


#ifdef __cplusplus
}
#endif
#endif //_PDH_H_
