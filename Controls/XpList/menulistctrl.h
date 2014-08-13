#if !defined(AFX_MENULISTCTRL_H__CEF602E7_3FF2_4350_BBB5_623114E5B98C__INCLUDED_)
#define AFX_MENULISTCTRL_H__CEF602E7_3FF2_4350_BBB5_623114E5B98C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// menulistctrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMenuListCtrl window

class CMenuListCtrl : public CListCtrl
{
// Construction
public:
	CMenuListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMenuListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMenuListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMenuListCtrl)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPopmlOpen();
	afx_msg void OnPopmlCopy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MENULISTCTRL_H__CEF602E7_3FF2_4350_BBB5_623114E5B98C__INCLUDED_)
