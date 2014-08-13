#if !defined(AFX_DLGREGBOOT_H__075B4185_CCD6_47C0_88EA_55A2FB7F5552__INCLUDED_)
#define AFX_DLGREGBOOT_H__075B4185_CCD6_47C0_88EA_55A2FB7F5552__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgregboot.h : header file
//
#include "../Registry/Registry.h"
#include "../Controls/SortListCtrl/SortListCtrl.H"

/////////////////////////////////////////////////////////////////////////////
// CDlgRegBoot dialog
typedef struct _RegItem {
	HKEY mKey;
	CString mSub;
} RegItem;

#define RegCount 5   //查找的这册表项数

class CDlgRegBoot : public CDialog
{
// Construction
public:
	CDlgRegBoot(CWnd* pParent = NULL);   // standard constructor
	RegItem cBootReg[10];

// Dialog Data
	//{{AFX_DATA(CDlgRegBoot)
	enum { IDD = IDD_DLGREGBOOT };
	CSortListCtrl	m_List;
	CButton	m_btnDelete;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRegBoot)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadRegBoot(HKEY hmKey,CString cSubKey);

	// Generated message map functions
	//{{AFX_MSG(CDlgRegBoot)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRegbootDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREGBOOT_H__075B4185_CCD6_47C0_88EA_55A2FB7F5552__INCLUDED_)
