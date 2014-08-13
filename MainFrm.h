                        // MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__D896ED7B_CF7E_4991_B6B8_F93054BE1587__INCLUDED_)
#define AFX_MAINFRM_H__D896ED7B_CF7E_4991_B6B8_F93054BE1587__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CMutex m_Mutex;
	DWORD  m_dwPID;
	UINT   m_NowViewListType;
	CWinThread* m_pThreadModule;

// Operations
public:
	CListCtrl            m_wndInfoList;
	CListCtrl            m_wndDLLList;
	CMenuListCtrl			 m_wndModuleList;
	CMenuListCtrl			 m_wndSuspiconList;
	CEditEx              m_wndPEList;
	CStatusBarXP  m_wndXPStatusBar;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	ULONG ulHiddenProcType;
	void ExeBar_HIDDEN_EXE(ULONG ulOp);
	BOOL SetModuleContent(CString lParam[]);
	BOOL SetInfoContent(CString cType);
	LRESULT HandleThreadMes(WPARAM wParam, CString lParam[]);
	friend UINT Thread_GetModuleList(LPVOID param);
	BOOL StartModuleThread(DWORD dwPID);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBarXP      m_wndXPToolBar;
	CButton m_Database;
	UINT    m_uTimer;
	CImageList	    m_ilToolBar;	// image list for toolbar
	BCMenu	m_menu;					 //加入XP风格菜单变量
	CCoolBar        m_wndModuleBar;
	CCoolBar        m_wndInfoBar;
	CCoolTabCtrl    m_TabCtrlInfo;
	CCoolTabCtrl    m_TabCtrlModule;
	CMyTreeCtrl	    m_treePopup;
// Generated message map functions
protected:
	CWnd m_wndParent;
	BOOL SetSystemTray();
	CString DetectPeInfo(char *lpFileName,CStringArray &cRelatedDll);
	UINT GetListItemFromClm(CListCtrl& listView,CString szText);
	BOOL SetMenuStyles();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMainmenuTop();
	afx_msg void OnUpdateMainmenuTop(CCmdUI* pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
    afx_msg LRESULT SetToolBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT SetStatusBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT SetModeleBar(WPARAM, LPARAM);
	afx_msg LRESULT SetInfoBar(WPARAM, LPARAM);
	afx_msg void OnViewInfoBar();
	afx_msg void OnViewModuleBar();
	afx_msg void ExeBar_REFRESHE();
	afx_msg void ExeBar_KILL();
	afx_msg void ExeBar_NORMAL ();
	afx_msg void ExeBar_NETWORK ();
	afx_msg void ExeBar_PERFORMON ();
	afx_msg void ExeBar_SERVICE ();
	afx_msg void ExeBar_BOOT ();
	afx_msg void ExeBar_COMPUTEE ();
	afx_msg void ExeBar_CONSOLE();
	afx_msg void ExeBar_REGEDIT();
	afx_msg void ExeBar_NOTPAD ();
	afx_msg void ExeBar_HELP ();
	afx_msg void ExeBar_HOME ();
	afx_msg void ExeBar_ABOUT ();
	afx_msg void ExeBar_HIDPROC_SEL();
	afx_msg void OnUpdateViewModuleBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewInfoBar(CCmdUI* pCmdUI);	
	afx_msg void ExeBar_HIDDENPROC();
	afx_msg void OnUpdateToolHidden(CCmdUI* pCmdUI);
	afx_msg void ExeBar_DISABLEDAUTO();
	afx_msg void OnUpdateMenuDisabledauto(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__D896ED7B_CF7E_4991_B6B8_F93054BE1587__INCLUDED_)
