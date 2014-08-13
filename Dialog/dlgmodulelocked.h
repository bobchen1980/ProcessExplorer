 #if !defined(AFX_DLGMODULELOCKED_H__BC51BDD6_993D_429A_904F_23B619138DBB__INCLUDED_)
#define AFX_DLGMODULELOCKED_H__BC51BDD6_993D_429A_904F_23B619138DBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgModuleLocked.h : header file
//
#include "../Controls/SortListCtrl/SortListCtrl.H"
#include <tchar.h>
/////////////////////////////////////////////////////////////////////////////
// CDlgModuleLocked dialog

class CDlgModuleLocked : public CDialog
{
// Construction
public:
	CString m_strMLPath;
	CDlgModuleLocked(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgModuleLocked)
	enum { IDD = IDD_DLGMODULELK };
	CSortListCtrl	m_List;
	CString	m_strMLName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgModuleLocked)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_IsAllSafe;
	LPCTSTR GetFileNamePosition( LPCTSTR lpPath );
	void WhoUsesModule( LPCTSTR lpFileName, BOOL bFullPathCheck );
	// Generated message map functions
	//{{AFX_MSG(CDlgModuleLocked)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDlgmoduleDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODULELOCKED_H__BC51BDD6_993D_429A_904F_23B619138DBB__INCLUDED_)
