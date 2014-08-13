/*#############################################################################
# ENUMNTSRV.CPP
#
# SCA Software International S.A.
# http://www.scasoftware.com
# scaadmin@scasoftware.com
#
# Copyright (c) 1999 SCA Software International S.A.
#
# Date: 05.12.1999.
# Author: Zoran M.Todorovic
#
# This software is provided "AS IS", without a warranty of any kind.
# You are free to use/modify this code but leave this header intact.
#
#############################################################################*/

#include "stdafx.h"
#include <WinSvc.h>
#include "EnumNtSrv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//=============================================================================
// class TNtServiceInfo
//
//=============================================================================

TNtServiceInfo::TNtServiceInfo()
{
  m_strServiceName.Empty();
  m_strDisplayName.Empty();
  m_strBinaryPath.Empty();
  m_dwServiceType = 0;
  m_dwStartType = 0;
  m_dwErrorControl = 0;
  m_dwCurrentState = 0;
}

TNtServiceInfo::TNtServiceInfo(const TNtServiceInfo& source)
{
  *this = source;
}

TNtServiceInfo::~TNtServiceInfo()
{
}

TNtServiceInfo& TNtServiceInfo::operator=(const TNtServiceInfo& source)
{
  m_strServiceName = source.m_strServiceName;
  m_strDisplayName = source.m_strDisplayName;
  m_strBinaryPath = source.m_strBinaryPath;
  m_dwServiceType = source.m_dwServiceType;
  m_dwStartType = source.m_dwStartType;
  m_dwErrorControl = source.m_dwErrorControl;
  m_dwCurrentState = source.m_dwCurrentState;
  return *this;
}

// Return a service type as a string
CString TNtServiceInfo::GetServiceType(void)
{
  // Winnt.h
  CString str = "未知";
  if (m_dwServiceType & SERVICE_WIN32) {
      if (m_dwServiceType & SERVICE_WIN32_OWN_PROCESS)
          str = "系统";
      else if (m_dwServiceType & SERVICE_WIN32_SHARE_PROCESS)
          str = "共享";
      //if (m_dwServiceType & SERVICE_INTERACTIVE_PROCESS)
      //    str += " (交互)";
  }
  switch (m_dwServiceType) {
  case SERVICE_KERNEL_DRIVER: str = "内核"; break;
  case SERVICE_FILE_SYSTEM_DRIVER: str = "系统"; break;
  };
  return str;
}

// Return a service start type as a string
CString TNtServiceInfo::GetStartType(void)
{
  // Winnt.h
  TCHAR *types[] = {
      "机前", // 0
      "系统", // 1
      "自动", // 2
      "手工", // 3
      "禁止" // 4
  };
  return CString(types[m_dwStartType]);
}

// Return this service error control as a string
CString TNtServiceInfo::GetErrorControl(void)
{
  // Winnt.h
  TCHAR *types[] = {
      "ERROR_IGNORE", // 0
      "ERROR_NORMAL", // 1
      "ERROR_SEVERE", // 2
      "ERROR_CRITICAL" // 3
  };
  return CString(types[m_dwErrorControl]);
}

// Return this service current state as a string
CString TNtServiceInfo::GetCurrentState(void)
{
  // Winsvc.h
  TCHAR *types[] = {
      "未知",
      "停止", // 1
      "正在启动", // 2
      "正在停止", // 3
      "运行", // 4
      "正在继续", // 5
      "正在暂停", // 6
      "暂停" // 7
  };
  return CString(types[m_dwCurrentState]);
}

// Enumerate services on this machine and return a pointer to an array of objects.
// Caller is responsible to delete this pointer using delete [] ...
// dwType = bit OR of SERVICE_WIN32, SERVICE_DRIVER
// dwState = bit OR of SERVICE_ACTIVE, SERVICE_INACTIVE
TNtServiceInfo *TNtServiceInfo::EnumServices(DWORD dwType, DWORD dwState,
                                             DWORD *pdwCount)
{
  ASSERT(pdwCount != NULL);
  // Maybe check if dwType and dwState have at least one constant specified
  *pdwCount = 0;
  TNtServiceInfo *info = NULL;
  SC_HANDLE scman = ::OpenSCManager(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE);
  if (scman) {
    ENUM_SERVICE_STATUS service, *lpservice;
    BOOL rc;
    DWORD bytesNeeded,servicesReturned,resumeHandle = 0;
    rc = ::EnumServicesStatus(scman,dwType,dwState,&service,sizeof(service),
                              &bytesNeeded,&servicesReturned,&resumeHandle);
    if ((rc == FALSE) && (::GetLastError() == ERROR_MORE_DATA)) {
      DWORD bytes = bytesNeeded + sizeof(ENUM_SERVICE_STATUS);
      lpservice = new ENUM_SERVICE_STATUS [bytes];
      ::EnumServicesStatus(scman,dwType,dwState,lpservice,bytes,
                                &bytesNeeded,&servicesReturned,&resumeHandle);
      *pdwCount = servicesReturned;    // Not a chance that 0 services is returned
      info = new TNtServiceInfo [servicesReturned];
      TCHAR Buffer[1024];         // Should be enough for service info
      QUERY_SERVICE_CONFIG *lpqch = (QUERY_SERVICE_CONFIG*)Buffer;
      for (DWORD ndx = 0; ndx < servicesReturned; ndx++) {
        info[ndx].m_strServiceName = lpservice[ndx].lpServiceName;
        info[ndx].m_strDisplayName = lpservice[ndx].lpDisplayName;
        info[ndx].m_dwServiceType = lpservice[ndx].ServiceStatus.dwServiceType;
        info[ndx].m_dwCurrentState = lpservice[ndx].ServiceStatus.dwCurrentState;
        SC_HANDLE sh = ::OpenService(scman,lpservice[ndx].lpServiceName,SERVICE_QUERY_CONFIG);
        if (::QueryServiceConfig(sh,lpqch,sizeof(Buffer),&bytesNeeded)) {
            info[ndx].m_strBinaryPath = lpqch->lpBinaryPathName;
            info[ndx].m_dwStartType = lpqch->dwStartType;
            info[ndx].m_dwErrorControl = lpqch->dwErrorControl;
        }
        ::CloseServiceHandle(sh);
      }
      delete [] lpservice;
    }
    ::CloseServiceHandle(scman);
  }
  return info;
}

// Enumerate services on this machine and return an STL list of service objects 
// dwType = bit OR of SERVICE_WIN32, SERVICE_DRIVER
// dwState = bit OR of SERVICE_ACTIVE, SERVICE_INACTIVE
void TNtServiceInfo::EnumServices(DWORD dwType, DWORD dwState, 
                                  TNtServiceInfoList *pList)
{
  ASSERT(pList != NULL);

  TNtServiceInfo *pSrvList = NULL;
  DWORD dwCount = 0;
  pSrvList = TNtServiceInfo::EnumServices(dwType, dwState, &dwCount);
  for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex ++) {
    pList->insert(pList->end(), pSrvList[dwIndex]);
  }
  delete [] pSrvList;
}

/*#############################################################################
# End of file ENUMNTSRV.CPP
#############################################################################*/
