///////////////////////////////////////////////////////////////////////////////
// class CTBalloon
//
// Author:  Rajesh C K R
// email:   rajeshckr@usa.net
// page:    http://www.cybertalkonline.com
// Copyright © 2000, Rajesh C K R
//
// This code provided "AS IS," without warranty of any kind.
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// version : 1.0


#include "stdafx.h"
#include "TBalloon.h"
#include "Gradient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTBalloon

#define	BALLOON_SHOW_TIMER	0x10
#define TIMER_MS			20
#define BALLOON_LIFETIME	(3*1000)
#define STEP_SIZE			5

UINT CTBalloon::m_sActiveCount;


CTBalloon::CTBalloon(UINT nWidth, UINT nHeight): m_nWidth(nWidth), m_nHeight(nHeight)
{	
	SystemParametersInfo(SPI_GETWORKAREA, 0, &m_screen_rect, 0);
	m_totaltime = 0;
	m_lifetime = BALLOON_LIFETIME;
	m_dir = TRUE;
}

CTBalloon::~CTBalloon()
{
}


BEGIN_MESSAGE_MAP(CTBalloon, CWnd)
	//{{AFX_MSG_MAP(CTBalloon)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTBalloon::SetText(CString str)
{
	m_text =str;
}


void CTBalloon::SetLifeTime(UINT secs)
{
	m_lifetime = secs * 1000;
}

BOOL CTBalloon::CreateAndShow()
{
	
	const char * p = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW );

	CRect rect;
	rect.top = m_screen_rect.bottom - m_nHeight;
	rect.bottom = m_screen_rect.bottom;
	rect.left = m_screen_rect.right - m_nWidth - 10 - (m_sActiveCount * 10) ;
	rect.right = m_screen_rect.right - 10 - (m_sActiveCount * 10);

	DWORD dwStyle =   WS_POPUP; 
	DWORD dwExStyle = WS_EX_LEFT|WS_EX_TOOLWINDOW |WS_EX_CLIENTEDGE | WS_EX_TOPMOST | WS_EX_WINDOWEDGE ; 


	BOOL ret = CreateEx(dwExStyle,p,NULL,dwStyle,rect,NULL,NULL);

	m_sActiveCount++;

	m_current_rect.top = m_current_rect.bottom = m_screen_rect.bottom;
	m_current_rect.left = rect.left;
	m_current_rect.right = rect.right;


	COLORREF lightblue = RGB(255,0,0);
	COLORREF white = RGB(255,255,255);


	m_grad.SetDirection(CGradient::RTL);
	m_grad.SetGradientColorsX(3,lightblue,lightblue,white);

	
	SetTimer(BALLOON_SHOW_TIMER,TIMER_MS,NULL);
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CTBalloon message handlers



void CTBalloon::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CBrush br(0xFF0000);
	CRect rect(0,0,m_current_rect.Width(),m_current_rect.Height());
	m_grad.DrawLinearGradient(&dc,rect);

	dc.SelectStockObject(ANSI_VAR_FONT);
	dc.SetBkMode(TRANSPARENT);
	CRect temp = rect;
	UINT height = dc.DrawText(m_text,-1,temp,DT_CENTER|DT_WORDBREAK|DT_CALCRECT);

	rect.top = (rect.Height() - height)/2;
	dc.DrawText(m_text,-1,rect,DT_CENTER|DT_WORDBREAK);

	// Do not call CWnd::OnPaint() for painting messages
}


void CTBalloon::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	m_totaltime += TIMER_MS;

	if (m_totaltime > m_lifetime)
	{
		m_dir = FALSE;
	}

	if (m_dir)
	{
		m_current_rect.top = ((m_current_rect.top -	STEP_SIZE) < (m_current_rect.bottom - m_nHeight))? m_current_rect.bottom - m_nHeight : m_current_rect.top - STEP_SIZE;
	}
	else
	{
		m_current_rect.top = ((m_current_rect.top + STEP_SIZE) > (m_current_rect.bottom))? m_current_rect.bottom : m_current_rect.top + STEP_SIZE;
	}

	
	MoveWindow(&m_current_rect);

	if (m_current_rect.top == m_current_rect.bottom)
	{
		KillTimer(BALLOON_SHOW_TIMER);
		DestroyWindow();
		return;
	}

	CWnd * pWnd = GetFocus();
	ShowWindow(SW_RESTORE);
	if (pWnd)
		pWnd->SetFocus();
	
	
	CWnd::OnTimer(nIDEvent);
}

void CTBalloon::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class

	delete this;
	m_sActiveCount--;
}
