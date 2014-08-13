// ECQPS.h : main header file for the ECQPS application
//

#if !defined(AFX_ECQPS_H__A24F198C_6765_4CFD_B0CB_92614F7C37F7__INCLUDED_)
#define AFX_ECQPS_H__A24F198C_6765_4CFD_B0CB_92614F7C37F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "CONTROLS\SYSTRAY\SystemTray.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CECQPSApp:
// See ECQPS.cpp for the implementation of this class
//

class CECQPSApp : public CWinApp
{
public:
	CSystemTray m_TrayIcon;
	CECQPSApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CECQPSApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CECQPSApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECQPS_H__A24F198C_6765_4CFD_B0CB_92614F7C37F7__INCLUDED_)
