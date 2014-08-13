#if !defined(AFX_DLGWARNING_H__62AB7176_0487_463A_BA7B_4831101187C3__INCLUDED_)
#define AFX_DLGWARNING_H__62AB7176_0487_463A_BA7B_4831101187C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgwarning.h : header file
//
#include "Controls/StaticEdit/ColorStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgWarning dialog

class CDlgWarning : public CDialog
{
// Construction
public:
	CDlgWarning(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWarning)
	enum { IDD = IDD_DLGWARNING };
	CColorStatic	m_WarnInfo;
	CButton	m_Cancel;
	CButton	m_OK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWarning)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBitmap m_bmpBackground;
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CDlgWarning)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWARNING_H__62AB7176_0487_463A_BA7B_4831101187C3__INCLUDED_)
