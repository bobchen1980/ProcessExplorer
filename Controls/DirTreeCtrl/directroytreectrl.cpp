// DirectroyTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DirectroyTreeCtrl.h"
#include "SortStringArray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static WORD ToolId[] = 
{                       IDB_BITMAP_ROOT_FILE,
                        16, 16,
                        IDM_ROOT_FILE_OPEN,
                        IDM_ROOT_FILE_EDIT,
                        IDM_ROOT_FILE_COPY,
                        IDM_ROOT_FILE_CUT,
						IDM_ROOT_FILE_PASTE,
						IDM_ROOT_FILE_EXPAND,
                        IDM_ROOT_FILE_COLLAPSE,
						IDM_ROOT_FILE_RENAME,
                        IDM_ROOT_FILE_DELETE,
						IDM_ROOT_FILE_PROPERTY,NULL
};

/////////////////////////////////////////////////////////////////////////////
// CDirectroyTreeCtrl

CDirectroyTreeCtrl::CDirectroyTreeCtrl()
{
	m_hitSel=NULL;
}

CDirectroyTreeCtrl::~CDirectroyTreeCtrl()
{
	m_imgList.Detach();
}


BEGIN_MESSAGE_MAP(CDirectroyTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDirectroyTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnTreeDblclk)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectroyTreeCtrl message handlers

//get the system image list
BOOL CDirectroyTreeCtrl::GetSysImgList()
{
	SHFILEINFO shFinfo;
	HIMAGELIST hImgList = NULL;

	if ( GetImageList( TVSIL_NORMAL ) )
		m_imgList.Detach();
	
	hImgList = (HIMAGELIST)SHGetFileInfo( "C:\\",
							  0,
							  &shFinfo,
							  sizeof( shFinfo ),
							  SHGFI_SYSICONINDEX | 
							  SHGFI_SMALLICON );
	if ( !hImgList )
	{
		m_strError = "Cannot retrieve the Handle of SystemImageList!";
		return FALSE;
	}
	m_imgList.m_hImageList = hImgList;    
	SetImageList( &m_imgList, TVSIL_NORMAL );
	return TRUE;   // OK
}

//see if it is a directory
BOOL CDirectroyTreeCtrl::FindSubDir( LPCTSTR strPath)
{
	CFileFind find;
	CString   strTemp = strPath;
	BOOL      bFind;
	if ( strTemp[strTemp.GetLength()-1] == '\\' )
		strTemp += "*.*";
	else
		strTemp += "\\*.*";
	bFind = find.FindFile( strTemp );
	while ( bFind )
	{
		bFind = find.FindNextFile();

		if ( find.IsDirectory() && !find.IsDots() )
			return TRUE;
		if ( !find.IsDirectory() && !find.IsHidden() )
			return TRUE;
	}
	return FALSE;
}



// Displaying the Path in the TreeCtrl
void CDirectroyTreeCtrl::DisplayPath(HTREEITEM hParent, LPCTSTR strPath)
{
	CFileFind find;
	CString   strPathFiles = strPath;
	BOOL      bFind;
	CSortStringArray strDirArray;
	CSortStringArray strFileArray;
	
	if ( strPathFiles.Right(1) != "\\" )
		strPathFiles += "\\";
	strPathFiles += "*.*";

	bFind = find.FindFile( strPathFiles );

	while ( bFind )
	{
		bFind = find.FindNextFile();
		if ( find.IsDirectory() && !find.IsDots() )
			strDirArray.Add( find.GetFilePath() );
		if ( !find.IsDirectory())
			strFileArray.Add( find.GetFilePath() );

	}
    
	strDirArray.Sort();
	SetRedraw( FALSE );
	CWaitCursor wait;
    
	for ( int i = 0; i < strDirArray.GetSize(); i++ )
	{
			HTREEITEM hItem = AddItem( hParent, strDirArray.GetAt(i) );
			if ( FindSubDir( strDirArray.GetAt(i) ) )
				InsertItem( "", 0, 0, hItem );
	}

	strFileArray.Sort();
	for ( i = 0; i < strFileArray.GetSize(); i++ )
		HTREEITEM hItem = AddItem( hParent, strFileArray.GetAt(i) );
	SetRedraw( TRUE );
}

//Add item include icon
HTREEITEM CDirectroyTreeCtrl::AddItem(HTREEITEM hParent, LPCTSTR strPath)
{
	// Adding the Item to the TreeCtrl with the current Icons
	SHFILEINFO shFinfo;
	int iIcon, iIconSel;
    CString  strTemp = strPath;
    
	if ( strTemp.Right(1) != '\\' )
		 strTemp += "\\";
	if ( !SHGetFileInfo( strTemp,
						0,
						&shFinfo,
						sizeof( shFinfo ),
						SHGFI_ICON | 
					    SHGFI_SMALLICON ) )
	{
		m_strError = "Error Gettting SystemFileInfo!";
		return NULL;
	}

	iIcon = shFinfo.iIcon;

	// we only need the index from the system image list
	DestroyIcon( shFinfo.hIcon );

	if ( !SHGetFileInfo( strTemp,
						0,
						&shFinfo,
						sizeof( shFinfo ),
						SHGFI_ICON | SHGFI_OPENICON |
					    SHGFI_SMALLICON ) )
	{
		m_strError = "Error Gettting SystemFileInfo!";
		return NULL;
	}

	iIconSel = shFinfo.iIcon;

	// we only need the index of the system image list
	DestroyIcon( shFinfo.hIcon );

	if ( strTemp.Right(1) == "\\" )
		strTemp.SetAt( strTemp.GetLength() - 1, '\0' );
	
	if ( hParent == TVI_ROOT )
		return InsertItem( strTemp, iIcon, iIconSel, hParent );

	return InsertItem( GetFileNameFromPath( strTemp ), iIcon, iIconSel, hParent );
}


// get the Full Path of the item
CString CDirectroyTreeCtrl::GetFullPath(HTREEITEM hItem)
{
	CString strReturn= "";
	CString strTemp;
	HTREEITEM hParent = hItem;

	while ( hParent )
	{
		strTemp  = GetItemText( hParent );
		strTemp += "\\";
		strReturn = strTemp + strReturn;
		hParent = GetParentItem( hParent );
	}
	strReturn.TrimRight( '\\' );
    return strReturn;
}


void CDirectroyTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CString strPath;
	 
	if ( pNMTreeView->itemNew.state & TVIS_EXPANDED )
	{
		ExpandItem( pNMTreeView->itemNew.hItem, TVE_EXPAND );
	}
	else
	{
		HTREEITEM hChild = GetChildItem( pNMTreeView->itemNew.hItem );
				
		while ( hChild ) 
		{
			DeleteItem( hChild );
			hChild = GetChildItem( pNMTreeView->itemNew.hItem );
		}
		InsertItem( "", pNMTreeView->itemNew.hItem );
	}
	*pResult = 0;
}


void CDirectroyTreeCtrl::ExpandItem(HTREEITEM hItem, UINT nCode)
{	
	CString strPath;
	
	if ( nCode == TVE_EXPAND )
	{
		HTREEITEM hChild = GetChildItem( hItem );
		while ( hChild )
		{
			DeleteItem( hChild );
			hChild = GetChildItem( hItem );
		}
        
		strPath = GetFullPath( hItem );
		DisplayPath( hItem, strPath );
	}
}

// Setting the Selection in the Tree
BOOL CDirectroyTreeCtrl::SetSelPath(LPCTSTR strPath)
{
	HTREEITEM hParent  = TVI_ROOT;
	int       iLen    = strlen(strPath) + 2;
	char*     pszPath = new char[iLen];
	char*     pPath   = pszPath;
	BOOL      bRet    = FALSE;
    
	if ( !IsValidPath( strPath ) )
	{
		delete [] pszPath; // this must be added 29.03.99
		return FALSE;
	}
		
	strcpy( pszPath, strPath );
	strupr( pszPath );
	
	if ( pszPath[strlen(pszPath)-1] != '\\' )
		strcat( pszPath, "\\" );
    
	int iLen2 = strlen( pszPath );
	
	for (WORD i = 0; i < iLen2; i++ )
	{
		if ( pszPath[i] == '\\' )
		{
			SetRedraw( FALSE );
			pszPath[i] = '\0';
			hParent = SearchSiblingItem( hParent, pPath );
			if ( !hParent )  // Not found!
				break;
			else
			{								
				UINT uState;
				uState = GetItemState( hParent, TVIS_EXPANDEDONCE );
				if ( uState )
				{
					Expand( hParent, TVE_EXPAND );
					Expand( hParent, TVE_COLLAPSE | TVE_COLLAPSERESET );
					InsertItem("", hParent ); // insert a blank child-item
					Expand( hParent, TVE_EXPAND ); // now, expand send a notification
				}
				else
					Expand( hParent, TVE_EXPAND );
			}
			pPath += strlen(pPath) + 1;
		}
	}

	delete [] pszPath;
	
	if ( hParent ) // Ok the last subpath was found
	{		
		SelectItem( hParent ); // select the last expanded item
		bRet = TRUE;
	}
	else
		bRet = FALSE;

	SetRedraw( TRUE );
    return bRet;
}

HTREEITEM CDirectroyTreeCtrl::SearchSiblingItem( HTREEITEM hItem, LPCTSTR strText)
{
	HTREEITEM hFound = GetChildItem( hItem );
	CString   strTemp;
	while ( hFound )
	{
		strTemp = GetItemText( hFound );
        strTemp.MakeUpper();
		if ( strTemp == strText )
			return hFound;
		hFound = GetNextItem( hFound, TVGN_NEXT );
	}

	return NULL;
}


BOOL CDirectroyTreeCtrl::IsValidPath(LPCTSTR strPath)
{
	// This function check the Pathname
	
	HTREEITEM hChild;
	CString   strItem;
	CString   strTempPath = strPath;
	BOOL      bFound = FALSE;
	CFileFind find;

	hChild = GetChildItem( TVI_ROOT );
	strTempPath.MakeUpper();
	strTempPath.TrimRight('\\');

	while ( hChild )
	{
		strItem = GetItemText( hChild );
		strItem.MakeUpper();
		if ( strItem == strTempPath.Mid( 0, strItem.GetLength() ) )
		{
			bFound = TRUE;
			break;
		}
		hChild = GetNextItem( hChild, TVGN_NEXT );
	}
    
	if ( !bFound )
		return FALSE;

	strTempPath += "\\nul";
	if ( find.FindFile( strTempPath ) )
		return TRUE;
     
	return FALSE;
}

//display the tree
BOOL CDirectroyTreeCtrl::DisplayTree(LPCTSTR strRoot)
{
	/*DWORD dwStyle = GetStyle();   // read the windowstyle
	if ( dwStyle & TVS_EDITLABELS ) 
		ModifyStyle( TVS_EDITLABELS , 0 );*/

	DeleteAllItems();//must do this
	if ( !GetSysImgList() )
		return FALSE;

	m_strRoot = strRoot;
	if ( m_strRoot.Right(1) != '\\' )
		m_strRoot += "\\";
	HTREEITEM hParent = AddItem( TVI_ROOT, m_strRoot );
	DisplayPath( hParent, strRoot );
	return TRUE;	
}

//hittest OnRButtonDown
void CDirectroyTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
        UINT uFlags;
        HTREEITEM htItem = HitTest(point, &uFlags);
        if ((htItem != NULL) && (uFlags & TVHT_ONITEM)) 
		{
                Select(htItem, TVGN_DROPHILITE);
                m_hitSel =htItem;
        }
	   //CTreeCtrl::OnRButtonDown(nFlags, point);
	   //must do remove the above line
}

//hittest OnLButtonDown
void CDirectroyTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
     UINT uFlags;
     HTREEITEM htItem = HitTest(point, &uFlags);
     if ((htItem != NULL) && (uFlags & TVHT_ONITEM))
	 {
          Select(htItem, TVGN_DROPHILITE);
          m_hitSel=htItem;
	 }
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

//OnContextMenu
void CDirectroyTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
        UINT uFlags;
        CPoint ptTree = point;
        ScreenToClient(&ptTree);
        HTREEITEM htItem = HitTest(ptTree, &uFlags);

        if ((htItem != NULL) && (uFlags & TVHT_ONITEM)) 
		{
                ShowPopupMenu( point );
                SetItemState(htItem, 0, TVIS_DROPHILITED);
        }
        if (m_hitSel != NULL)
                Select(m_hitSel, TVGN_DROPHILITE);
}

//ShowPopupMenu
void CDirectroyTreeCtrl::ShowPopupMenu( CPoint& point )
{
        if (point.x == -1 && point.y == -1)
		{
                CRect rect;
                GetClientRect(rect);
                ClientToScreen(rect);

                point = rect.TopLeft();
                point.Offset(5, 5);
        }

        CNewMenu menu;
        VERIFY(menu.LoadMenu(IDR_ROOT_FILE_MENU));

		menu.LoadToolBar(ToolId,RGB(192,192,192));
        CMenu* pPopup = menu.GetSubMenu(0);
        ASSERT(pPopup != NULL);
        CWnd* pWndPopupOwner = this;

        while (pWndPopupOwner->GetStyle() & WS_CHILD)
                pWndPopupOwner = pWndPopupOwner->GetParent();

        pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
                pWndPopupOwner);
}


//OnTreeDblclk
void CDirectroyTreeCtrl::OnTreeDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM item= GetSelectedItem( );
	HTREEITEM parentitem=GetParentItem(item);
	DWORD Atrb=GetFileAttributes(GetFullPath(item));
	//cannot open the folder
	if(!(Atrb & FILE_ATTRIBUTE_DIRECTORY))
	      ShellExecute( GetSafeHwnd(), "open",
		                GetItemText(item), NULL,
				        GetFullPath(parentitem), SW_SHOW);
	*pResult = 0;
}

//OnEndlabeledit
void CDirectroyTreeCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	//get the old name
	CString filename_old=GetFullPath(m_hitSel);
	CEdit* edit=GetEditControl();
    CString  str="";
	edit->GetWindowText(str);
	SetItemText(m_hitSel,str);
	//get the new name
    CString filename_new=GetFullPath(m_hitSel);
	//rename the file
	::rename(filename_old.GetBuffer(filename_old.GetLength()),
		filename_new.GetBuffer(filename_new.GetLength()));
	//release the buffer
	filename_old.ReleaseBuffer();
	filename_new.ReleaseBuffer();
	*pResult = 0;
}
