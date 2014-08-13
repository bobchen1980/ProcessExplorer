#if !defined(AFX_MULTICOLUMNSORTLISTVIEW_H__E58FDCA1_0449_11D2_9AED_2F8C0624A000__INCLUDED_)
#define AFX_MULTICOLUMNSORTLISTVIEW_H__E58FDCA1_0449_11D2_9AED_2F8C0624A000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MultiColumnSortListView.h : header file
//
#include <afxcview.h>
#include "SortableHeaderCtrl.h"

#define MINCOLWIDTH	       70
#define MAXCOLWIDTH	       70
#define MAX_COLUMNS	       64
#define MULTI_COLUMN_SORT  1
#define SINGLE_COLUMN_SORT 0


enum SORT_STATE{ DESCENDING, ASCENDING };
/////////////////////////////////////////////////////////////////////////////
// CMultiColumnSortListView view

class CMultiColumnSortListView : public CListView
{
protected:
	CMultiColumnSortListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMultiColumnSortListView)
	
// Attributes
public:

// Operations
public:
	void SetUniqueName( LPCTSTR );
	int FindNumericColumnIndex( int );
	void UnsetColumnNumeric( int iCol );
	const bool IsColumnNumeric( int ) const;
	void SetColumnNumeric( int );
	void SetColumnWidth( int );
	void AutoSizeColumn( int iColumn );
	void AutoSizeAllColumns();
	const int GetRegColumnWidth( int ) const;
	int FindItemInCombedSortedList( int );
	
	void MoveItemInCombinedSortedListToEnd( int );
	
	void EmptyArray( int * );
	const int GetNumCombinedSortedColumns() const;
	
	void SetItemSortState( int, SORT_STATE );
	const SORT_STATE GetItemSortState( int ) const;

	bool NotInCombinedSortedColumnList( int iItem ) const;
	
	const int IsControlPressed() const;
	
	int  InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = 1, int nSubItem = 1 );
		
	void SortColumn( int, bool = false );

	int  LoadColumnWidths();
	void SaveColumnWidths();
	
	UINT GetColumnCount();
	void DeleteAllColumns();
	void DeleteAllItems();
	const bool IsSorting() const;
	
	__int64       m_lColumnSortStates;
	int           m_aCombinedSortedColumns[MAX_COLUMNS];
	CUIntArray    m_aNumericColumns;
	CImageList	  m_ilImageList;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiColumnSortListView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMultiColumnSortListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	CString				m_strColumnWidthSection;
	CSortableHeaderCtrl m_ctlHeaderCtrl;	
	CString			    m_strUniqueName;
	bool				m_bAutoSizeOnInsert;
	bool				m_bSorting;
	//{{AFX_MSG(CMultiColumnSortListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult) ;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTICOLUMNSORTLISTVIEW_H__E58FDCA1_0449_11D2_9AED_2F8C0624A000__INCLUDED_)
