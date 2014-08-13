///////////////////////////////////////////////////////////////////////////////
//
// ListBoxXP.cpp : implementation file
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListBoxXP.h"
#include "Draw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
CListBoxXP::CListBoxXP ()
{
}

////////////////////////////////////////////////////////////////////////////////
CListBoxXP::~CListBoxXP ()
{
}

////////////////////////////////////////////////////////////////////////////////
void CListBoxXP::PreSubclassWindow ()
{
    ModifyStyleEx (WS_EX_CLIENTEDGE, 0);
    ModifyStyle (0, WS_BORDER, SWP_FRAMECHANGED);
    m_MouseMgr.Init (m_hWnd, MMS_NCPAINT);
}

////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CListBoxXP, CListBox)
    //{{AFX_MSG_MAP(CListBoxXP)
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEOUT()
    ON_WM_NCPAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
void CListBoxXP::OnMouseMove (UINT, CPoint)
{
    Default();
    m_MouseMgr.OnMouseMove();
}

///////////////////////////////////////////////////////////////////////////////
void CListBoxXP::OnMouseOut ()
{
    m_MouseMgr.OnMouseOut();
}

///////////////////////////////////////////////////////////////////////////////
void CListBoxXP::OnNcPaint ()
{
    Default();

    CWindowDC cDC (this);
    CWindowRect rc (this);
    CPenDC pen (cDC, ::GetSysColor (m_MouseMgr.MouseOver() ? COLOR_HIGHLIGHT : COLOR_3DSHADOW));

    cDC.SelectObject (::GetStockObject (NULL_BRUSH));
    cDC.Rectangle (0, 0, rc.Width(), rc.Height());
}
