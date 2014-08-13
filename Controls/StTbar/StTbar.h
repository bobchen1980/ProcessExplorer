#if !defined(AFX_STTBAR_H__ACA9297E_8F62_11D2_822A_00600815B1D4__INCLUDED_)
#define AFX_STTBAR_H__ACA9297E_8F62_11D2_822A_00600815B1D4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StTbar.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CStTbar window
class CMyMiniFrm : public CMiniFrameWnd
{
//	DECLARE_DYNCREATE(CMyMiniFrm)
public:
	CMyMiniFrm();           // protected constructor used by dynamic creation
protected:


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyMiniFrm)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~CMyMiniFrm();

	// Generated message map functions
	//{{AFX_MSG(CMyMiniFrm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CStTbar : public CStatic
{
// Construction
public:
	CStTbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStTbar)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStTbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStTbar)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
	CToolBar		m_wndToolBar;
	CMyMiniFrm*		m_minifrm;
public:
	void	PreTranslate(MSG* pMsg);
	CToolBar&	GetTBar(){return m_wndToolBar;};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STTBAR_H__ACA9297E_8F62_11D2_822A_00600815B1D4__INCLUDED_)
