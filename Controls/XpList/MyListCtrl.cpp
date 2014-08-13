// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MyListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{
}

CMyListCtrl::~CMyListCtrl()
{
}

int CMyListCtrl::GetSelectedItem () const
{
    int nCount = GetItemCount();

    for ( int i = 0; i < nCount; i++ )
    {
        if (GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
        {
            return i;
        }
    }
    return -1;
}

BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CMyListCtrl::XPopupCtrlEvent::OnInit ()
{
    METHOD_PROLOGUE_(CMyListCtrl, PopupCtrlEvent);

    TRACE (_T("CMyListCtrl::OnInit ()\n"));
    pThis->ModifyStyle (LVS_EDITLABELS, 0); // Not allowed for correct work !

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void CMyListCtrl::XPopupCtrlEvent::OnShow ()
{
    METHOD_PROLOGUE_(CMyListCtrl, PopupCtrlEvent);

    TRACE (_T("CMyListCtrl::OnShow ()\n"));
    pThis->SetFocus();
    pThis->Scroll (CSize(20,0)); // To have correct display of the calendar
}

///////////////////////////////////////////////////////////////////////////////
void CMyListCtrl::XPopupCtrlEvent::OnHide (bool bAbort)
{
    METHOD_PROLOGUE_(CMyListCtrl, PopupCtrlEvent);

    TRACE (_T("CMyListCtrl::OnHide (Abort = %s)\n"), bAbort ? _T("true") : _T("false"));

    CEditView* pView = (CEditView*)((CMDIFrameWnd*)AfxGetMainWnd())->GetActiveFrame()->GetActiveView();

    if ( pView != NULL && !bAbort )
    {
        CString sText;
        int nLength = pView->GetWindowTextLength();

        sText.Format (_T("CMyListCtrl::Selection = \"%s\"\r\n"), pThis->GetItemText (pThis->GetSelectedItem(), 0));

        pView->GetEditCtrl().SetSel (nLength + 1, nLength + 1, TRUE);
        //pView->GetEditCtrl().ReplaceSel (sText);
        nLength = pView->GetWindowTextLength();
        pView->GetEditCtrl().SetSel (nLength + 1, nLength + 1);
    }
}

///////////////////////////////////////////////////////////////////////////////
int CMyListCtrl::XPopupCtrlEvent::OnKeyDown (UINT nChar, UINT, UINT)
{
    if ( nChar == VK_RETURN )
    {
        return CPopup::end;
    }
    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
int CMyListCtrl::XPopupCtrlEvent::OnLButtonDown (UINT, CPoint pt)
{
    // Uncomment next line to close by double click instead of simple click
    // return CPopup::doNothing;

    METHOD_PROLOGUE_(CMyListCtrl, PopupCtrlEvent);

    int  hItem;
    UINT uHitFlags = LVHT_ONITEM;

    if ( (hItem = pThis->HitTest (pt, &uHitFlags)) != -1 )
    {
        pThis->SetItemState (hItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
    }
    return CPopup::noSend;
}

///////////////////////////////////////////////////////////////////////////////
int CMyListCtrl::XPopupCtrlEvent::OnLButtonUp (UINT, CPoint pt)
{
    METHOD_PROLOGUE_(CMyListCtrl, PopupCtrlEvent);

    int  hItem;
    UINT uHitFlags = LVHT_ONITEM;

    if ( (hItem = pThis->HitTest (pt, &uHitFlags)) != -1 )
    {
        return CPopup::end;
    }
    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
IPopupCtrlEvent* CMyListCtrl::XPopupCtrlEvent::GetInterfaceOf (HWND)
{
    // Not used here because this control doesn't include other controls
    return NULL;
}
