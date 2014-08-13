/*
	
	Usage:
		You generally should not use this class directly, though it
		is possible. You need to do two things to use it directly.
		Set m_strUniqueName to someting, and set m_strColumnWidthSection
		to where you want the column widths to be saved.

		The purpose of m_strUniqueName is to allow for saving of
		multiple instances of listview objects. So obviously you would
		need to set this differently for each instance. SetUniqueName must be called
		before calling InsertColumn() or LoadColumnWidths().

		If you are deriving from this class, you need to do the following:
		Add a class to your project derived from CListView, then go into the
		header file and include MultiColumnSortListView.h and change all
		references to CListView to CMultiColumnSortListView. Then in the .cpp
		file of your class, change all the message maps to CMultiColumnSortListView
		instead of CListView. That should do it.

  Compiling:
		One problem you will have is finding IDB_ARROWUP and IDB_ARROWDOWN.
		Those bitmaps will be included with this set of classes, You should
		add them to your project or add your own bitmaps named correctly.
		These are the bitmaps that show the sort order on the header control.

        I hope this is simple enough, kind of a pain to get started but after
		that it should be cake and hopefully it will be useful.
  
  Things to be aware of:
		
		Multithreading:
		     If you delete all the items from another thread
			 in the middle of a sort, it will crash. This is the only
			 bug i have found.
		
		Column Widths:
			 
			MINCOLWIDTH - Minimum width a column can be.
			MAXCOLWIDTH - Maximum width a column can be.
			These are hard coded in the header file. Be aware.
			
			MAXCOLUMNS  - The most columns you can have right
			now is 64, that is only because im use __int64 to
			hold the column sort states. Who needs more than
			64 columns anyway? If you do, you can change it to
			CUIntArray, i just didnt see the need for a whole int
			when all i needed was a bit. 
		        

  Credits:
		
		Iuri Apollonio -- Sorting Class ( great class )
		Zafir Anjum    -- CMultiColumnSortListView::GetColumnCount
		Roger Onslow   -- CMultiColumnSortListView::AutoSizeColumns
		Zafir Anjum    -- CSortableHeaderCtrl::SetSortImage
		Me             -- The Rest, I think.

*/
// MultiColumnSortListView.cpp : implementation file
//

#include "stdafx.h"  //must add this header
#include "MultiColumnSortListView.h"
#include "SortClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnSortListView

IMPLEMENT_DYNCREATE(CMultiColumnSortListView, CListView)
/*
When deriving from this class you must set m_strUniqueName to something
this name is used to save each instances column widths to the registry
*/
CMultiColumnSortListView::CMultiColumnSortListView()
{	
	m_strUniqueName.Empty();
	m_strColumnWidthSection = "ColumnWidths";
	m_bSorting = false;
	m_lColumnSortStates = 0;
}

CMultiColumnSortListView::~CMultiColumnSortListView()
{
}


BEGIN_MESSAGE_MAP(CMultiColumnSortListView, CListView)
	//{{AFX_MSG_MAP(CMultiColumnSortListView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked) 
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiColumnSortListView drawing

void CMultiColumnSortListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}
/////////////////////////////////////////////////////////////////////////////
// CMultiColumnSortListView diagnostics
#ifdef _DEBUG
void CMultiColumnSortListView::AssertValid() const
{
	CListView::AssertValid();
}
void CMultiColumnSortListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CMultiColumnSortListView message handlers

/*
This function saves the columns widths of the listctrl to the registry.
This is called in two places, OnDestroy, and OnEndTrack in the headerCtrl class.
*/
void CMultiColumnSortListView::SaveColumnWidths()
{	//You must set a unique name for every listctrl
	//ASSERT( m_strUniqueName.GetLength() );
	CString strEntry( m_strUniqueName );
	CString strValue;
	CListCtrl &rListCtrl = GetListCtrl();
	int iNumColumns = GetColumnCount();
	for( int i = 0; i < iNumColumns; i++ )
	{
		CString strTemp;
		strTemp.Format("%d,", rListCtrl.GetColumnWidth( i ) );
		strValue += strTemp;
	}
	AfxGetApp()->WriteProfileString( m_strColumnWidthSection, strEntry, strValue );
}

/*
This function loads all the column widths for each column that was saved and applies the width
to the column. This function should be called when you are done inserting data. Or you 
can call SetColumnWidth to set the column width right after you InsertColumn(), If you call 
my InsertColumn it will do this for you.
*/
int CMultiColumnSortListView::LoadColumnWidths()
{	//This function will load all the column widths and apply the widths to each respective column
	int iNumColumns = GetColumnCount();
	CListCtrl &rListCtrl = GetListCtrl();
	for( int i = 0; i < iNumColumns; i++ )
	{
		SetColumnWidth( i );
	}
	return 1;
}

void CMultiColumnSortListView::SortColumn( int iSubItem, bool bSortingMultipleColumns )
{	
	CListCtrl &rListCtrl = GetListCtrl();
	rListCtrl.LockWindowUpdate();
	rListCtrl.SetRedraw(FALSE);
	int iNumCombinedSortedCols = GetNumCombinedSortedColumns();
	m_bSorting = true;
	if( bSortingMultipleColumns )
	{
		if( NotInCombinedSortedColumnList( iSubItem ) )
			m_aCombinedSortedColumns[ iNumCombinedSortedCols++ ] = iSubItem;
		else
			MoveItemInCombinedSortedListToEnd( iSubItem );

		for( int i = iNumCombinedSortedCols - 1; i >= 0 ; i-- )
		{
			SORT_STATE ssEachItem = GetItemSortState( m_aCombinedSortedColumns[i] );
			if( iNumCombinedSortedCols - 1 != i )
				ssEachItem = (SORT_STATE)!ssEachItem;
			
			CSortClass csc(&rListCtrl, m_aCombinedSortedColumns[i], IsColumnNumeric( m_aCombinedSortedColumns[i] ) );	
			csc.Sort( ssEachItem);
			if( i == iNumCombinedSortedCols - 1 )
			{	//Only swap the last column's sort order.
				m_ctlHeaderCtrl.SetSortImage( m_aCombinedSortedColumns[i], ssEachItem );
				SetItemSortState( m_aCombinedSortedColumns[i] , (SORT_STATE)!ssEachItem );			
			}
		}
	}
	else
	{
		m_ctlHeaderCtrl.RemoveAllSortImages();
		EmptyArray(m_aCombinedSortedColumns);
		m_aCombinedSortedColumns[ 0 ] = iSubItem;
		SORT_STATE ssEachItem = GetItemSortState( iSubItem );
		
		CSortClass csc(&rListCtrl, iSubItem, IsColumnNumeric( iSubItem ) );	
		csc.Sort(ssEachItem );
		m_ctlHeaderCtrl.SetSortImage( iSubItem, ssEachItem );
		SetItemSortState( iSubItem , (SORT_STATE)!ssEachItem );
	}
	rListCtrl.UnlockWindowUpdate();
	rListCtrl.SetRedraw(TRUE);
	m_bSorting = false;
}

int CMultiColumnSortListView::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	CListCtrl &rListCtrl = GetListCtrl();
	GetListCtrl().InsertColumn( nCol, lpszColumnHeading, nFormat, nWidth, nSubItem );
	SetColumnWidth( nCol );
	return 1;
}

//Utility function to size columns based on its data.
void CMultiColumnSortListView::AutoSizeColumn( int iColumn )
{
	CListCtrl &rListCtrl = GetListCtrl();
	rListCtrl.SetRedraw(false);
	
	rListCtrl.SetColumnWidth(iColumn,LVSCW_AUTOSIZE);
	int wc1 = rListCtrl.GetColumnWidth( iColumn );
	rListCtrl.SetColumnWidth(iColumn,LVSCW_AUTOSIZE_USEHEADER);
	int wc2 = rListCtrl.GetColumnWidth( iColumn );
	int wc = max(MINCOLWIDTH,max( wc1,wc2 ));
	
	if( wc > MAXCOLWIDTH )
		wc = MAXCOLWIDTH;
	
	rListCtrl.SetColumnWidth( iColumn,wc );  
	rListCtrl.SetRedraw(true);
}


//Utility function to get rid of all the columns
void CMultiColumnSortListView::DeleteAllColumns()
{
	CListCtrl &rListCtrl = GetListCtrl();
	int iNumCols = GetColumnCount();
	for ( int i = 0; i < iNumCols; i++ )
		rListCtrl.DeleteColumn(0);
}


//Utility function to get rid of all items.
void CMultiColumnSortListView::DeleteAllItems()
{
	CListCtrl &rListCtrl = GetListCtrl();
	if( rListCtrl.GetItemCount() > 0 )
		rListCtrl.DeleteAllItems();
}


//Utility function to get the number of columns
UINT CMultiColumnSortListView::GetColumnCount()
{
	CHeaderCtrl *pHeaderCtrl = (CHeaderCtrl*)GetListCtrl().GetDlgItem(0);
	return pHeaderCtrl->GetItemCount();
}

int CMultiColumnSortListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// set list control's style to hilight the entire row
	DWORD dwStyle = SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE);
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP;
	SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)dwStyle);
	m_ctlHeaderCtrl.SubclassWindow( GetDlgItem(0)->m_hWnd );		

	return 0;
}

//only sorting in report view so far.
BOOL CMultiColumnSortListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_REPORT;
	return CListView::PreCreateWindow(cs);
}

//Utility function to tell you if a sort is taking place
const bool CMultiColumnSortListView::IsSorting() const
{
	return CMultiColumnSortListView::m_bSorting;
}


//Utility function to tell you if the control key is being pressed
const int CMultiColumnSortListView::IsControlPressed() const
{
	return (::GetKeyState( VK_CONTROL ) < 0 );
}


//Message handler for when a header is clicked.
void CMultiColumnSortListView::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if(AfxGetApp()->GetProfileInt(_T("Settings"),
		_T("AllowSort"),YES)==YES)
   {
	  CListCtrl &list=GetListCtrl();
	  HD_NOTIFY *pHDN = (HD_NOTIFY *) pNMHDR;
	  if( pHDN->iButton == 0 )
	  {
		if( IsControlPressed() )
			SortColumn( pHDN->iItem, MULTI_COLUMN_SORT );
		else
			SortColumn( pHDN->iItem, SINGLE_COLUMN_SORT );
	  }
	  *pResult = 0;
   }
}

/*
Message handler for when control is about to be destroyed.
This is where the column widths are saved.
*/
void CMultiColumnSortListView::OnDestroy() 
{
	SaveColumnWidths();	
	CListView::OnDestroy();
}

/*
Utility function to tell you if a column is in the combined sorted list.
*/
bool CMultiColumnSortListView::NotInCombinedSortedColumnList(int iItem) const
{
	int iNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	for( int i = 0; i < iNumCombinedSortedColumns; i++ )
	{
		if( m_aCombinedSortedColumns[i] == iItem )
			return false;
	}
	return true;
}

/*
Utility function to get you the sort state of a column
*/
const SORT_STATE CMultiColumnSortListView::GetItemSortState( int iItem ) const
{
	return (SORT_STATE)((m_lColumnSortStates) & ( 1 << iItem ));
}

/*
Utility function to set the sort state of a column
*/
void CMultiColumnSortListView::SetItemSortState(int iItem, SORT_STATE bSortState)
{
	if( bSortState != GetItemSortState( iItem ) )
		m_lColumnSortStates ^= (1 << iItem);
}

/*
Utility function to get you the number of combined sorted columns
*/
const int CMultiColumnSortListView::GetNumCombinedSortedColumns() const
{
	for( int i = 0; i < MAX_COLUMNS; i++ )
		if( m_aCombinedSortedColumns[i] == -1 )
			return i;
	return MAX_COLUMNS;
}

/*
Utility function clear some internal arrays
*/
void CMultiColumnSortListView::EmptyArray( int *pArray )
{
	memset( pArray, -1, MAX_COLUMNS );
}

/*
This function will move a clicked column to the end of the combined
column list. This is useful when you move backwards through column clicks.
Like click columns: 0, 1, 2, 1. The array will hold [0,1,2] after the first 3
clicks, this function will change it to [0,2,1] after the 4th click.
*/
void CMultiColumnSortListView::MoveItemInCombinedSortedListToEnd(int iItem)
{
	int iNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	int aCombinedSortedColumns[MAX_COLUMNS];
	memset( aCombinedSortedColumns, -1, MAX_COLUMNS );
	int iItemIndex = FindItemInCombedSortedList( iItem );
	if( iItemIndex != -1 )
	{
		if( iItemIndex > 0 )
		{
			memcpy( aCombinedSortedColumns, m_aCombinedSortedColumns, iItemIndex * sizeof( int ) );
			memcpy( &aCombinedSortedColumns[iItemIndex], &m_aCombinedSortedColumns[iItemIndex + 1], (iNumCombinedSortedColumns - iItemIndex - 1) * sizeof(int) );
		}
	}
	aCombinedSortedColumns[ iNumCombinedSortedColumns - 1 ] = iItem;
	memcpy( m_aCombinedSortedColumns, aCombinedSortedColumns, MAX_COLUMNS * sizeof(int) );
	for( int i = 0; i < MAX_COLUMNS ; i++ )
	{
		if( aCombinedSortedColumns[i] == -1 )
			break;
	}
}

/*
Utility function to find an item in the combined sorted list.
*/
int CMultiColumnSortListView::FindItemInCombedSortedList( int iItem )
{
	int iNumCombinedSortedColumns = GetNumCombinedSortedColumns();
	for( int i = 0; i < iNumCombinedSortedColumns; i++ )
	{
		if(m_aCombinedSortedColumns[i] == iItem )
			return i;
	}
	return -1;
}

/*
Utility function to look up a columns width in the registry.
*/
const int CMultiColumnSortListView::GetRegColumnWidth( int iColumn ) const
{	//You must set a unique name for each 
	ASSERT( m_strUniqueName.GetLength() );
	
	CString strEntry( m_strUniqueName );
	CString strValue, strSubString;
	CString strSection( m_strColumnWidthSection );
	strValue = AfxGetApp()->GetProfileString( strSection, strEntry, "" );
	AfxExtractSubString(strSubString, strValue, iColumn, ',');
	return atoi( (LPCTSTR )strSubString );
}

/*
Utility function to Autosize all columns in the case there is no registry entry.
*/
void CMultiColumnSortListView::AutoSizeAllColumns()
{
	int iNumCols = GetColumnCount();
	for( int i = 0; i < iNumCols; i++ )
	{
		AutoSizeColumn( i );
	}
}

/*
Utility function to set the width on the column based on the registry
value and a set minimum column width.
*/
void CMultiColumnSortListView::SetColumnWidth( int nCol )
{
	CListCtrl &rListCtrl = GetListCtrl();
	int iWidth = GetRegColumnWidth( nCol );
	if( iWidth < MINCOLWIDTH )
		AutoSizeColumn( nCol );
	else
		rListCtrl.SetColumnWidth( nCol, iWidth );
}

/*
Utility function to set a column that will contain only numeric values.
Speeds up the sorting if this is set on the right columns.
*/
void CMultiColumnSortListView::SetColumnNumeric( int iCol )
{
	m_aNumericColumns.Add( iCol );
}

/*
Utility function to tell you if the given column is set as numeric.
*/
const bool CMultiColumnSortListView::IsColumnNumeric( int iCol ) const
{
	for( int i = 0; i < m_aNumericColumns.GetSize(); i++ )
	{	
		if( m_aNumericColumns.GetAt( i ) == (UINT)iCol )
			return true;
	}
	return false;
}

/*
Utility function to remove the numeric status of a column
*/
void CMultiColumnSortListView::UnsetColumnNumeric(int iCol)
{
	int iIndex = FindNumericColumnIndex( iCol );
	if( iIndex >= 0 )
		m_aNumericColumns.RemoveAt( iIndex );
}

/*
Utility function to find a numeric column in the array.
*/
int CMultiColumnSortListView::FindNumericColumnIndex( int iCol )
{
	for( int i = 0; i < m_aNumericColumns.GetSize(); i++ )
	{	
		if( m_aNumericColumns.GetAt( i ) == (UINT)iCol )
			return i;
	}
	return -1;
}

void CMultiColumnSortListView::SetUniqueName(LPCTSTR lpszUniqueName)
{
	m_strUniqueName = lpszUniqueName;
}
