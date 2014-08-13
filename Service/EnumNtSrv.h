/*#############################################################################
# ENUMNTSRV.H
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

#ifndef __ENUMNTSRV_H__
#define __ENUMNTSRV_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

using namespace std;

class TNtServiceInfo;

typedef list<TNtServiceInfo> TNtServiceInfoList;

//=============================================================================
// class TNtServiceInfo
//
//=============================================================================

class TNtServiceInfo {
public:
  CString m_strServiceName;
  CString m_strDisplayName;
  CString m_strBinaryPath;
  DWORD m_dwServiceType;
  DWORD m_dwStartType;
  DWORD m_dwErrorControl;
  DWORD m_dwCurrentState;

public:
  TNtServiceInfo();
  TNtServiceInfo(const TNtServiceInfo& source);
  TNtServiceInfo& operator=(const TNtServiceInfo& source);
  virtual ~TNtServiceInfo();

  CString GetServiceType(void);
  CString GetStartType(void);
  CString GetErrorControl(void);
  CString GetCurrentState(void);
  
  static TNtServiceInfo *EnumServices(DWORD dwType, DWORD dwState, DWORD *pdwCount);
  static void EnumServices(DWORD dwType, DWORD dwState, TNtServiceInfoList *pList);
};

#endif

/*#############################################################################
# End of file ENUMNTSRV.H
#############################################################################*/
