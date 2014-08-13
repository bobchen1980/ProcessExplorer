// MyTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MyTreeCtrl.h"
#include "..\..\MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl

CMyTreeCtrl::CMyTreeCtrl() : m_bEnableLastAction (false)
{
}

CMyTreeCtrl::~CMyTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTreeCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CMyTreeCtrl::XPopupCtrlEvent::OnInit ()
{
    METHOD_PROLOGUE_(CMyTreeCtrl, PopupCtrlEvent);

    TRACE (_T("CMyTreeCtrl::OnInit ()\n"));

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void CMyTreeCtrl::XPopupCtrlEvent::OnShow ()
{
    METHOD_PROLOGUE_(CMyTreeCtrl, PopupCtrlEvent);

    TRACE (_T("CMyTreeCtrl::OnShow ()\n"));
    pThis->SetFocus();
}

///////////////////////////////////////////////////////////////////////////////
void CMyTreeCtrl::XPopupCtrlEvent::OnHide (bool bAbort)
{
    METHOD_PROLOGUE_(CMyTreeCtrl, PopupCtrlEvent);

    TRACE (_T("CMyTreeCtrl::OnHide (Abort = %s)\n"), bAbort ? _T("true") : _T("false"));
	CMainFrame *pMainframe=(CMainFrame *)AfxGetMainWnd()->m_hWnd;
    if ( pMainframe != NULL && !bAbort )
    {	
		pMainframe->ExeBar_HIDDEN_EXE(pThis->GetItemData (pThis->GetSelectedItem()));
	}

}

///////////////////////////////////////////////////////////////////////////////
int CMyTreeCtrl::XPopupCtrlEvent::OnKeyDown (UINT nChar, UINT, UINT)
{
    if ( nChar == VK_RETURN )
    {
        return CPopup::end;
    }
    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
int CMyTreeCtrl::XPopupCtrlEvent::OnLButtonDown (UINT, CPoint pt)
{
    METHOD_PROLOGUE_(CMyTreeCtrl, PopupCtrlEvent);

    UINT uFlags;

    // Store information used in next OnLButtonUp
    pThis->m_bEnableLastAction = pThis->HitTest (pt, &uFlags) == NULL ||
                                 (uFlags & (TVHT_ONITEMBUTTON|TVHT_ONITEMINDENT)) == 0;

    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
int CMyTreeCtrl::XPopupCtrlEvent::OnLButtonUp (UINT, CPoint pt)
{
    METHOD_PROLOGUE_(CMyTreeCtrl, PopupCtrlEvent);

    UINT uFlags;

    if ( pThis->m_bEnableLastAction &&
         pThis->HitTest (pt, &uFlags) != NULL &&
         (uFlags & (TVHT_ONITEMBUTTON|TVHT_ONITEMINDENT)) == 0 )
    {
        return CPopup::end;
    }

    return CPopup::doNothing;
}

///////////////////////////////////////////////////////////////////////////////
IPopupCtrlEvent* CMyTreeCtrl::XPopupCtrlEvent::GetInterfaceOf (HWND)
{
    // Not used here because this control doesn't include other controls
    return NULL;
}
