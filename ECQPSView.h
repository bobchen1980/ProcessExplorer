// ECQPSView.h : interface of the CECQPSView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECQPSVIEW_H__EABFC38F_14AA_42D8_B62E_D3B84F438DD5__INCLUDED_)
#define AFX_ECQPSVIEW_H__EABFC38F_14AA_42D8_B62E_D3B84F438DD5__INCLUDED_

#include "PROCESS\perfmon.h"	// Added by ClassView
#include "CONTROLS\TRAYBOOL\TaskbarNotifier.h"	// Added by ClassView
#include "CONTROLS\TRAYBOOL\XInfoTip.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CECQPSView : public CMultiColumnSortListView
{
protected: // create from serialization only
	CECQPSView();
	DECLARE_DYNCREATE(CECQPSView)

// Attributes
public:
	CECQPSDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CECQPSView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL
// Implementation
public:
	void UpdateProcessList_HiddenDriver();
	void UpdateProcessList_HiddenProc(ULONG ulOp);
	void UpdateProcessList_NetworkXp(CString cType);
	void UpdateProcessList_Network2k(CString cType);
	void UpdateProcessList_Performon(CString cType);
	void UpdateProcessList_Normal(CString cType);
	virtual ~CECQPSView();
	afx_msg void OnPopmenuAutokill();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	LRESULT SetListExternStyle(WPARAM wParam, LPARAM lParam);
	LRESULT SetListBackFore(WPARAM wParam, LPARAM lParam);
// Generated message map functions
protected:
	CXInfoTip m_Tip;
	CTaskbarNotifier m_ctrlTaskBar;
	void UpdateProcessList_GetCounterData(UINT nItemInserted);
	CPerfMon m_PerfMon;
	CString GetProcessWarn(CString sName, CString sPath ,CString& sInfo);
	DWORD GetTotalThreadList(DWORD dwOwnerPID);
	void SetListProfile();
	int GetProcessIcon(CString strAppName, CString strPathAppName);
	Ico2Index	m_mProcName2IcoIndex;
	Process2Index m_ProcessIdIndex;
	Process2Index m_CurrentPIdIndex;
	CImageList m_cImageList;
	//{{AFX_MSG(CECQPSView)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPopmenuOpen();
	afx_msg void OnPopmenuKill();
	afx_msg void OnPopmenuDelete();
	afx_msg void OnPopmenuCopy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void UpdateNomalListCtrl(CString cType,LPPROCESSINFO32 lparrPsInfo, ULONG ulNeeded);
	DWORD GetAllProcNumFromKernel(ULONG ulNeeded,ULONG ulOp);
	DWORD CheckHiddenProc(LPPROCESSINFO32 lpProcInfo, ULONG  ulNeeded,ULONG ulOp);
};

#ifndef _DEBUG  // debug version in ECQPSView.cpp
inline CECQPSDoc* CECQPSView::GetDocument()
   { return (CECQPSDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECQPSVIEW_H__EABFC38F_14AA_42D8_B62E_D3B84F438DD5__INCLUDED_)
