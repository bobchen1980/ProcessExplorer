// SortableHeaderCtrl.cpp : implementation file
//

#include "stdafx.h" //MUST add this header
#include "SortableHeaderCtrl.h"
#include "MultiColumnSortListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortableHeaderCtrl

CSortableHeaderCtrl::CSortableHeaderCtrl()
{
	m_bSortAsc = FALSE;
	m_nSortCol = -1;
	m_iLastColumn = -1;
	if(AfxGetApp()->GetProfileInt(_T("Settings"),
		_T("DisplayImage"),YES)==YES)
	{
	    m_bmpArrowUp.LoadBitmap( IDB_ARROWUP );
	    m_bmpArrowDown.LoadBitmap( IDB_ARROWDOWN );
	}
	else
	{
		m_bmpArrowUp.LoadBitmap(IDB_BLANK);
		m_bmpArrowDown.LoadBitmap(IDB_BLANK);
	}
}

CSortableHeaderCtrl::~CSortableHeaderCtrl()
{
	m_bmpArrowUp.DeleteObject();
	m_bmpArrowDown.DeleteObject();
}


BEGIN_MESSAGE_MAP(CSortableHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortableHeaderCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortableHeaderCtrl message handlers
/////////////////////////////////////////////////////////////////////////////

int CSortableHeaderCtrl::SetSortImage( int nCol, BOOL bAsc )
{
    CBitmap *pTempBmp = NULL;
	if( bAsc )
		pTempBmp = &m_bmpArrowUp;
	else
		pTempBmp = &m_bmpArrowDown;
	
	HD_ITEM Item;	
	Item.mask = HDI_FORMAT;
	GetItem( nCol, &Item );
	Item.mask = HDI_BITMAP | HDI_FORMAT;	
	Item.fmt |= HDF_BITMAP;
	Item.hbm = (HBITMAP)pTempBmp->GetSafeHandle();
	SetItem( nCol, &Item );
	m_iLastColumn = nCol;
    return 1;
}

void CSortableHeaderCtrl::OnEndtrack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMultiColumnSortListView *pView = (CMultiColumnSortListView *)GetParent();
	pView->SaveColumnWidths();
	*pResult = 0;
}

const int CSortableHeaderCtrl::GetLastColumn() const
{
	return m_iLastColumn;
}

void CSortableHeaderCtrl::RemoveSortImage( int iItem )
{
	if( iItem != -1 )
	{
		HD_ITEM hditem;	
		hditem.mask = HDI_FORMAT;
		GetItem( iItem, &hditem );
		hditem.mask = HDI_FORMAT;	
		hditem.fmt &= ~HDF_BITMAP;
		SetItem( iItem, &hditem );
	}
}

void CSortableHeaderCtrl::RemoveAllSortImages()
{
	int iCount = GetItemCount();
	for( int i = 0; i < iCount; i++ )
		RemoveSortImage( i );
}
