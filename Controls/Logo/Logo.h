#if !defined(AFX_LOGO_H__8D31EEF5_6478_11D4_890F_0000E8E0DC35__INCLUDED_)
#define AFX_LOGO_H__8D31EEF5_6478_11D4_890F_0000E8E0DC35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Logo.h : header file
//

const UINT    ON_LOGO_CLICK = RegisterWindowMessage(_T("OnLogoClick"));

/////////////////////////////////////////////////////////////////////////////
// CLogo window

class CLogo : public CBitmapButton
{
// Construction
public:
	CLogo();

// Attributes
public:

// Operations
public:
	virtual void SetPosition();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogo)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual ~CLogo();

	// Generated message map functions
protected:
	virtual CDockBar* GetParentDockBar();
	//{{AFX_MSG(CLogo)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGO_H__8D31EEF5_6478_11D4_890F_0000E8E0DC35__INCLUDED_)
