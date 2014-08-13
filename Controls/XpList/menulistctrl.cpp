               // menulistctrl.cpp : implementation file
//

#include "stdafx.h"
#include "../../ECQPS.h"
#include "menulistctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuListCtrl
#include "../../Dialog/DlgModuleLocked.h"

CMenuListCtrl::CMenuListCtrl()
{
}

CMenuListCtrl::~CMenuListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMenuListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMenuListCtrl)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_POPML_OPEN, OnPopmlOpen)
	ON_COMMAND(ID_POPML_COPY, OnPopmlCopy)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMenuListCtrl message handlers

void CMenuListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	//AfxMessageBox("OnContextMenu");
	CRect ClientRect;
	GetClientRect(&ClientRect);
	
	CMenu cPopMenu;
	VERIFY(cPopMenu.LoadMenu(IDR_POPMENUML));

	CMenu* pPopup = (CMenu*)cPopMenu.GetSubMenu(0);
	VERIFY(pPopup != NULL);
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x,point.y,
		this);	
}

void CMenuListCtrl::OnPopmlOpen() 
{
	// TODO: Add your command handler code here
	//AfxMessageBox("OnPopmlOpen");
	POSITION pos = this->GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		CString szPath,szFile;
		while (pos)
		{
			int nItem = this->GetNextSelectedItem(pos);
			szPath=this->GetItemText(nItem,5);
			szFile=this->GetItemText(nItem,0);
			szPath.Replace(szFile,"");
			//TRACE1("Item %d was selected!\n", nItem);
			::ShellExecute(NULL, "open",szPath, NULL, NULL, SW_SHOWNORMAL);
		}
	}	
}

void CMenuListCtrl::OnPopmlCopy() 
{
	// TODO: Add your command handler code here
	CFileDialogST	dlg;
	int				nRetValue;
	CString			sFolder;

	POSITION pos = this->GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		CString szPath,dsPath,szFile;
		while (pos)
		{
			int nItem = this->GetNextSelectedItem(pos);
			szPath=this->GetItemText(nItem,5);
			szFile=this->GetItemText(nItem,6);
			nRetValue = dlg.SelectFolder(_T("请选择目的文件夹..."), _T("C:\\"), 
				0x40|BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS, this);
			if (nRetValue == IDOK)
			{
				sFolder = dlg.GetSelectedFolder();
				dsPath=sFolder+"\\"+szFile;
				::CopyFile(szPath,dsPath,FALSE);
				//MessageBox(sFolder, _T("GetSelectedFolder"), MB_ICONINFORMATION);
			} 	
		}
	}			
	
}

void CMenuListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//AfxMessageBox("OnLButtonDblClk");
	UINT uFlags;
	int nItem = this->HitTest(point, &uFlags);

	if (uFlags & LVHT_ONITEM )
	{
		CDlgModuleLocked dlgModule;
		dlgModule.m_strMLName=this->GetItemText(nItem,0);
		dlgModule.m_strMLPath=this->GetItemText(nItem,5);
		dlgModule.DoModal();
	}	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}
