#if !defined(AFX_DLGSERVICE_H__490132CD_D745_464D_82BE_E7070085CDDE__INCLUDED_)
#define AFX_DLGSERVICE_H__490132CD_D745_464D_82BE_E7070085CDDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgservice.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgService dialog
#include "../Controls/SortListCtrl/SortListCtrl.H"

class CDlgService : public CDialog
{
// Construction
public:
	CDlgService(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgService)
	enum { IDD = IDD_DLGSERVICE };
	CButton	m_btnViewDriver;
	CSortListCtrl	m_List;
	BOOL	m_IsAll;
	BOOL    m_IsService;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgService)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadNTServices(DWORD dwType,DWORD dwState);

	// Generated message map functions
	//{{AFX_MSG(CDlgService)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnServiceRerfrsh();
	afx_msg void OnServiceDelete();
	afx_msg void OnServiceDriver();
	afx_msg void OnServiceAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSERVICE_H__490132CD_D745_464D_82BE_E7070085CDDE__INCLUDED_)
