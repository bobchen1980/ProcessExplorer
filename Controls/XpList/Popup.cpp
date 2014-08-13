///////////////////////////////////////////////////////////////////////////////
//
// Popup.cpp : implementation file
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <objbase.h>
#include <initguid.h>
#include "Popup.h"
#include "Draw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////////
CPopup::CPopup () : m_pControl (NULL), m_pCaller (NULL), m_pEvents (NULL),
                    m_hPrevParentCtrl (NULL), m_bDestroy (false)
{
}

///////////////////////////////////////////////////////////////////////////////
bool CPopup::Display (CWnd* pControl, CWnd* pCaller, CRect& rcCaller, IPopupCtrlEvent* pEvents, DWORD dwStyles)
{
    if ( pControl == NULL )
    {
        delete this;
        return false;
    }
    m_pControl = pControl;
    m_pCaller = pCaller;
    m_rcCaller = rcCaller;
    m_pEvents = pEvents;

    if ( m_pEvents != NULL && !m_pEvents->OnInit() )
    {
        delete this;
        return false;
    }
    // Calculate the most appropriate position for the left-top corner of control
    // By default, at left and bottom of the caller
    CPoint pt (rcCaller.left, rcCaller.bottom);
    CWindowRect rcCtrl (m_pControl);

    if ( rcCtrl.Width() == 0 || rcCtrl.Height() == 0 )
    {
        return false;
    }
    int nOffset = 0;

    if ( dwStyles & WS_DLGFRAME )
    {
        rcCtrl.InflateRect (4, 4);
        nOffset = 1;
    }
    if ( dwStyles & WS_BORDER )
    {
        rcCtrl.right += 2;
        rcCtrl.bottom += 2;
    }
    // Alignment at right if necessary
    if ( pt.x + rcCtrl.Width() > ::GetSystemMetrics (SM_CXSCREEN) )
    {
        pt.x = rcCaller.right-rcCtrl.Width();
    }
    // Alignment at top if necessary
    if ( pt.y + rcCtrl.Height() > ::GetSystemMetrics (SM_CYSCREEN) )
    {
        pt.y = rcCaller.top-rcCtrl.Height();
    }
    // Adjustments to keep control into screen
    if ( pt.x + rcCtrl.Width() > ::GetSystemMetrics (SM_CXSCREEN) )
    {
        pt.x = ::GetSystemMetrics (SM_CXSCREEN)-rcCtrl.Width();
    }
    if ( pt.y + rcCtrl.Height() > ::GetSystemMetrics (SM_CYSCREEN) )
    {
        pt.y = ::GetSystemMetrics (SM_CYSCREEN)-rcCtrl.Height();
    }
    if ( pt.x < 0 ) pt.x = 0;
    if ( pt.y < 0 ) pt.y = 0;

    if ( !CMiniFrameWnd::Create (NULL, _T(""), WS_POPUP|dwStyles|(m_pCaller != NULL ? MFS_SYNCACTIVE : 0),
                                 CRect (pt.x, pt.y, pt.x+rcCtrl.Width(), pt.y+rcCtrl.Height()),
                                 m_pControl->GetParent()) )
    {
        return false;
    }
    m_hPrevParentCtrl = m_pControl->SetParent (this)->GetSafeHwnd();
    m_pControl->SetWindowPos (NULL, nOffset, nOffset, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
    // Don't use SWP_SHOWWINDOW because OnShowWindow isn't call
    m_pControl->ShowWindow (SW_SHOW);
    EnableToolTips (TRUE);
    ShowWindow (SW_SHOW);

    if ( m_pEvents != NULL )
    {
        m_pEvents->OnShow();
    }
    return true;
}

// user defined message (different of any Win32/MFC message?)
#define WM_DELAYDESTROY (WM_USER+0x0100)

///////////////////////////////////////////////////////////////////////////////
void CPopup::EndPopup (bool bAbort, bool bSetFocus)
{
    if ( m_pEvents != NULL )
    {
        m_pEvents->OnHide (bAbort);
    }
    ASSERT (m_pControl->m_hWnd != NULL);

    m_pControl->ShowWindow (SW_HIDE);

    ASSERT (m_hPrevParentCtrl != NULL);
    ::SetParent (m_pControl->m_hWnd, m_hPrevParentCtrl);

    // Do a local copy of the pointer because DestroyWindow delete object
    // and m_pCaller will not be valid after
    CWnd* pCaller = m_pCaller;

    if ( !m_bDestroy )
    {
        m_bDestroy = true;
        PostMessage (WM_DELAYDESTROY);
    }
    if ( bSetFocus && pCaller != NULL )
    {
        pCaller->SetFocus();
    }
}

///////////////////////////////////////////////////////////////////////////////
BOOL CPopup::PreTranslateMessage (MSG* pMsg)
{
    bool bEnd = false, bAbort = false, bResult = false;
    int  nCode = doNothing;

    switch ( pMsg->message )
    {
        case WM_KEYDOWN:
            // Default action for <Escape> key
            if ( pMsg->wParam == VK_ESCAPE )
            {
                bEnd = bAbort = bResult = true;
                break;
            }
            if ( m_pEvents != NULL )
            {
                IPopupCtrlEvent* pEvents = m_pEvents;

                // Search for the interface for the correct object (referenced by pMsg->hwnd)
                if ( pMsg->hwnd != m_pControl->m_hWnd )
                {
                    pEvents = m_pEvents->GetInterfaceOf (pMsg->hwnd);
                }
                if ( pEvents != NULL )
                {
                    nCode = pEvents->OnKeyDown (pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
                }
            }
            break;

        case WM_LBUTTONDOWN:
            if ( m_pEvents != NULL )
            {
                IPopupCtrlEvent* pEvents = m_pEvents;

                // Search for the interface for the correct object
                if ( pMsg->hwnd != m_pControl->m_hWnd )
                {
                    pEvents = m_pEvents->GetInterfaceOf (pMsg->hwnd);
                }
                if ( pEvents != NULL )
                {
                    nCode = pEvents->OnLButtonDown (pMsg->wParam, CPoint ((short)LOWORD(pMsg->lParam), (short)HIWORD(pMsg->lParam)));
                }
            }
            break;

        case WM_LBUTTONUP:
            if ( m_pEvents != NULL )
            {
                IPopupCtrlEvent* pEvents = m_pEvents;

                // Search for the interface for the correct object
                if ( pMsg->hwnd != m_pControl->m_hWnd )
                {
                    pEvents = m_pEvents->GetInterfaceOf (pMsg->hwnd);
                }
                if ( pEvents != NULL )
                {
                    nCode = pEvents->OnLButtonUp (pMsg->wParam, CPoint ((short)LOWORD(pMsg->lParam), (short)HIWORD(pMsg->lParam)));
                }
            }
            break;
    }
    switch ( nCode )
    {
        case noSend:
            bResult = true;
            break;

        case end:
            bEnd = bResult = true;
            break;

        case abort:
            bEnd = bAbort = bResult = true;
            break;
    }
    if ( !bResult )
    {
        bResult = CMiniFrameWnd::PreTranslateMessage (pMsg) != 0;
    }
    if ( bEnd )
    {
        EndPopup (bAbort);
    }
    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::PostNcDestroy ()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP (CPopup, CMiniFrameWnd)
    //{{AFX_MSG_MAP(CPopup)
    ON_WM_ERASEBKGND()
    ON_WM_ACTIVATE()
    ON_WM_CANCELMODE()
    ON_MESSAGE(WM_DELAYDESTROY, OnDelayDestroy)
    ON_WM_NCPAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP ()

IMPLEMENT_DYNCREATE (CPopup, CMiniFrameWnd);

///////////////////////////////////////////////////////////////////////////////
BOOL CPopup::OnEraseBkgnd (CDC* pDC)
{
    pDC->FillSolidRect (CClientRect (this), ::GetSysColor (COLOR_3DFACE));

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::OnNcPaint ()
{
    Default();

    if ( (GetStyle()&WS_TABBORDER) == WS_TABBORDER ) 
    {
        CWindowDC cDC (this);
        CWindowRect rc (this);
        CPenDC pen (cDC, HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), 20, 0));

        if ( m_rcCaller.bottom == rc.top )
        {
            cDC.MoveTo (1, 0);
            cDC.LineTo (m_rcCaller.Width()-1, 0);
            pen.Color (::GetSysColor (COLOR_3DSHADOW));
            cDC.LineTo (rc.Width()-1, 0);
            cDC.LineTo (rc.Width()-1, rc.Height()-1);
            cDC.LineTo (0, rc.Height()-1);
            cDC.LineTo (0, -1);
        }
        else
        {
            cDC.MoveTo (1, rc.Height()-1);
            cDC.LineTo (m_rcCaller.Width()-1, rc.Height()-1);
            pen.Color (::GetSysColor (COLOR_3DSHADOW));
            cDC.LineTo (rc.Width()-1, rc.Height()-1);
            cDC.LineTo (rc.Width()-1, 0);
            cDC.LineTo (0, 0);
            cDC.LineTo (0, rc.Height());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::OnActivate (UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CMiniFrameWnd::OnActivate (nState, pWndOther, bMinimized);

    if ( nState == WA_INACTIVE && !m_bDestroy )
    {
        EndPopup (true, false);
    }
}

///////////////////////////////////////////////////////////////////////////////
void CPopup::OnCancelMode ()
{
    if ( !m_bDestroy )
    {
        EndPopup (true, false);
    }
}

///////////////////////////////////////////////////////////////////////////////
LRESULT CPopup::OnDelayDestroy (WPARAM, LPARAM)
{
    DestroyWindow();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int CPopup::OnToolHitTest (CPoint, TOOLINFO* pTI) const
{
    UNUSED(pTI);

#ifdef _DEBUG
    if ( m_pControl != NULL )
    {
        if ( pTI != NULL && pTI->cbSize >= 40/*sizeof(TOOLINFO)*/ )
        {
            // setup the TOOLINFO structure
            pTI->hwnd = m_hWnd;
            pTI->uId = (UINT)m_pControl->m_hWnd;
            pTI->uFlags |= TTF_IDISHWND;
            pTI->lpszText = _tcsdup (_T("Press <Esc> to hide me !"));
        }
        return m_pControl->GetDlgCtrlID();
    }
#endif
    return -1;
}
