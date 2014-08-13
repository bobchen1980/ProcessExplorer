/****************************************************************/
/*																*/
/*  SYSTEMTRAY.CPP												*/
/*																*/
/*  Implementation of the CSystemTray class						*/
/*	This class is a part of the Popup killer utility			*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2002						*/
/*																*/
/*  Last updated: 20 februari 2002								*/
/*																*/
/****************************************************************/

#include "stdafx.h"
#include "SystemTray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSystemTray, CWnd)


/********************************************************************/
/*																	*/
/* Function name : CSystemTray::CSystemTray							*/
/* Description   : Constructor										*/
/*																	*/
/********************************************************************/
CSystemTray::CSystemTray()
{
    ZeroMemory(&m_NotifyIconData, sizeof(m_NotifyIconData));
}


/********************************************************************/
/*																	*/
/* Function name : CSystemTray::~CSystemTray						*/
/* Description   : Destructor										*/
/*																	*/
/********************************************************************/
CSystemTray::~CSystemTray()
{
	// remove icon from system tray
    m_NotifyIconData.uFlags = 0;
    Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);

	// destroy dummy window
    DestroyWindow();
}


BEGIN_MESSAGE_MAP(CSystemTray, CWnd)
	//{{AFX_MSG_MAP(CSystemTray)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/********************************************************************/
/*																	*/
/* Function name : CSystemTray::Create								*/
/* Description   : Create systemtray icon							*/
/*																	*/
/********************************************************************/
BOOL CSystemTray::Create(UINT uCallbackMessage, LPCTSTR lpszToolTip, HICON hIcon, UINT uID)
{
	// create dummy window
    if (!CWnd::CreateEx(0, AfxRegisterWndClass(0), "CSystemTray Dummy Window", WS_POPUP, 0,0,0,0, NULL, 0))
		return FALSE;

    // setup NOTIFYICONDATA
    m_NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    m_NotifyIconData.hWnd   = m_hWnd;
    m_NotifyIconData.uID    = uID;
    m_NotifyIconData.hIcon  = hIcon;
    m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_NotifyIconData.uCallbackMessage = uCallbackMessage;
    lstrcpy(m_NotifyIconData.szTip, lpszToolTip);

    // add tray icon
    return Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);
}


/********************************************************************/
/*																	*/
/* Function name : CSystemTray::SetIcon								*/
/* Description   : Modify icon in the system tray					*/
/*																	*/
/********************************************************************/
BOOL CSystemTray::SetIcon(UINT nIDResource)
{
    HICON hIcon = AfxGetApp()->LoadIcon(nIDResource);
    m_NotifyIconData.uFlags = NIF_ICON;
    m_NotifyIconData.hIcon = hIcon;
    return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
}


/********************************************************************/
/*																	*/
/* Function name : CSystemTray::SetNotificationWnd					*/
/* Description   : Modify notification window						*/
/*																	*/
/********************************************************************/
BOOL CSystemTray::SetNotificationWnd(CWnd* pWnd)
{
    m_NotifyIconData.hWnd = pWnd->GetSafeHwnd();
    m_NotifyIconData.uFlags = 0;
    return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
}


/********************************************************************/
/*																	*/
/* Function name : CSystemTray::OnTrayNotification					*/
/* Description   : Handle notification messages						*/
/*																	*/
/********************************************************************/
LRESULT CSystemTray::OnTrayNotification(UINT wParam, LONG lParam) 
{
    // is it for us ?
    if (wParam != m_NotifyIconData.uID)
        return 0L;

    CWnd *pMainWnd = AfxGetMainWnd();

    switch(LOWORD(lParam))
	{
		case WM_RBUTTONUP:
		{    
			// right button click brings up a context menu
			CMenu menu, *pSubMenu;
			if (!menu.LoadMenu(m_NotifyIconData.uID)) 
				return 0;
			
			pSubMenu = menu.GetSubMenu(0);

			if (pSubMenu == NULL)
				return 0;

			CPoint pos;
			GetCursorPos(&pos);

			// display popup menu
			pSubMenu->TrackPopupMenu(0, pos.x, pos.y, pMainWnd, NULL);

			menu.DestroyMenu();
			break;
		} 
    
		case WM_LBUTTONDBLCLK:
		{
			// double click brings up the main dialog
			pMainWnd->ShowWindow(SW_RESTORE);
			pMainWnd->SetForegroundWindow();
			break;
		}
		default:
			break;
	}
    return 1;
}


/********************************************************************/
/*																	*/
/* Function name : CSystemTray::WindowProc							*/
/* Description   : Dispatch messages through the window’s message	*/
/*				   map												*/
/*																	*/
/********************************************************************/
LRESULT CSystemTray::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    if (uMsg == m_NotifyIconData.uCallbackMessage)
	{
        return OnTrayNotification(wParam, lParam);
	}
	return CWnd::WindowProc(uMsg, wParam, lParam);
}

