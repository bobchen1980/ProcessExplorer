#if !defined(AFX_DIRECTROYTREECTRL_H__E76343F3_246F_4293_A15E_B5329A4BAA0E__INCLUDED_)
#define AFX_DIRECTROYTREECTRL_H__E76343F3_246F_4293_A15E_B5329A4BAA0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectroyTreeCtrl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDirectroyTreeCtrl window

class CDirectroyTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CDirectroyTreeCtrl();
// Attributes
public:
    HTREEITEM      m_hitSel;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectroyTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL FindSubDir( LPCTSTR strPath);
	virtual ~CDirectroyTreeCtrl();
    HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR strPath);
	BOOL GetSysImgList();
    void DisplayPath(HTREEITEM hParent, LPCTSTR strPath);
    CString GetFullPath(HTREEITEM hItem);
    BOOL IsValidPath(LPCTSTR strPath);
    void ExpandItem(HTREEITEM hItem, UINT nCode);
    BOOL DisplayTree(LPCTSTR strRoot);
    BOOL SetSelPath(LPCTSTR strPath);
    HTREEITEM SearchSiblingItem( HTREEITEM hItem, LPCTSTR strText);
	// Generated message map functions
protected:
	void ShowPopupMenu( CPoint& point );
	//{{AFX_MSG(CDirectroyTreeCtrl)
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTreeDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	CImageList     m_imgList;
	CString        m_strRoot;
	CString        m_strError;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTROYTREECTRL_H__E76343F3_246F_4293_A15E_B5329A4BAA0E__INCLUDED_)
