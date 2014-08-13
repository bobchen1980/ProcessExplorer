#if !defined(AFX_SORTABLEHEADERCTRL_H__0A345281_0458_11D2_9AED_2F8C0624A000__INCLUDED_)
#define AFX_SORTABLEHEADERCTRL_H__0A345281_0458_11D2_9AED_2F8C0624A000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SortableHeaderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSortableHeaderCtrl window

class CSortableHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CSortableHeaderCtrl();
	int  SetSortImage( int nCol, BOOL bAsc );
	int     m_nSortCol;
    BOOL    m_bSortAsc;
	CBitmap m_bmpArrowUp;
	CBitmap m_bmpArrowDown;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortableHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void RemoveAllSortImages();
	void RemoveSortImage( int iItem );
	const int GetLastColumn() const;
	int m_iLastColumn;
	virtual ~CSortableHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortableHeaderCtrl)
		afx_msg void OnEndtrack(NMHDR* pNMHDR, LRESULT* pResult);
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTABLEHEADERCTRL_H__0A345281_0458_11D2_9AED_2F8C0624A000__INCLUDED_)
