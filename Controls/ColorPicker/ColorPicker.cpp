////////////////////////////////////////////////////////////////
// ColorPicker.cpp : implementation file                      //
//															  //
// Copyright 2001 WangJun									  //
// All Rights Reserved.										  //
//															  //
// Email: wangjun98@sohu.com								  //
// URL:   http://www.vckbase.com							  //
//															  //
// 1.0     2001/10/6   First release version.				  //
//															  //
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LPARAM_X(lp)                        ((int)(short)LOWORD(lp))
#define LPARAM_Y(lp)                        ((int)(short)HIWORD(lp))

#define	WM_SETCOLOR							WM_USER+1
#define WM_COLORDLG							WM_USER+2

#define	IDC_COLORDLG_BUTTON					100

CPen	_pen3DDKShadow(PS_SOLID,1,::GetSysColor(COLOR_3DDKSHADOW));
CPen	_penW(PS_SOLID,1,RGB(0xff,0xff,0xff));
CPen	_penB(PS_SOLID,1,RGB(0,0,0));


/////////////////////////////////////////////////////////////////////////////
// CColorPicker

CColorPicker::CColorPicker()
{
	const unsigned char pvANDPlaneC[]={0xff,0xf1,0xff,0xe0,0xff,0xc0,0xff,0x00,0xff,0x81,0xff,0x03,0xfe,0x07,0xfc,0x17,
	0xf8,0x3f,0xf0,0x7f,0xe0,0xff,0xc1,0xff,0x83,0xff,0x87,0xff,0x0f,0xff,0x3f,0xff};
	const unsigned char pvXORPlaneC[]={0x00,0x00,0x00,0x0c,0x00,0x08,0x00,0x10,0x00,0x00,0x00,0x40,0x00,0xe0,0x01,0xc0,
	0x03,0x80,0x07,0x00,0x0e,0x00,0x1c,0x00,0x38,0x00,0x30,0x00,0x40,0x00,0x00,0x00};
	int cxCursor = ::GetSystemMetrics(SM_CXCURSOR);
	int cyCursor = ::GetSystemMetrics(SM_CYCURSOR);
	m_hCursorStraw = NULL;
	if(cxCursor >=16 && cxCursor < 100 && cyCursor < 100 && cyCursor >= 16)
	{
		int size = cxCursor*cyCursor/8;
		unsigned char *pvANDPlane = new unsigned char[size];
		unsigned char *pvXORPlane = new unsigned char[size];
		if(pvANDPlane && pvXORPlane)
		{
			memset(pvANDPlane,0xff,size);
			memset(pvXORPlane,0x00,size);
			for(int j=0;j<16;j++)
				for(int i=0;i<2;i++)
				{
					*(pvANDPlane+j*cxCursor/8 + i) = *(pvANDPlaneC + j*2 + i);
					*(pvXORPlane+j*cxCursor/8 + i) = *(pvXORPlaneC + j*2 + i);
				}
			m_hCursorStraw = ::CreateCursor(::AfxGetInstanceHandle(),0,15,cxCursor,cyCursor,pvANDPlane,pvXORPlane);
			delete pvANDPlane;
			delete pvXORPlane;
		}
	}
	m_CurrentColor = COLORREF(::GetSysColor(COLOR_3DFACE));
	m_hwndBuddy	= NULL;
	m_hPaletteWnd = NULL;
	m_bPaletteWndActive = FALSE;
}

CColorPicker::~CColorPicker()
{
	::DestroyCursor(m_hCursorStraw);
}


BEGIN_MESSAGE_MAP(CColorPicker, CButton)
	//{{AFX_MSG_MAP(CColorPicker)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPicker message handlers

void CColorPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
}

BOOL CColorPicker::OnEraseBkgnd(CDC* pDC) 
{
	CPen		*pOldPen;
	COLORREF	clr3DFace = GetSysColor(COLOR_3DFACE);
	CRect		rect;
	GetClientRect(&rect);
	if(!m_bPaletteWndActive)
	{
		pDC->Draw3dRect (rect,RGB(0xff,0xff,0xff),GetSysColor (COLOR_3DDKSHADOW));
		rect.DeflateRect(1,1);
		pDC->Draw3dRect (rect,clr3DFace,clr3DFace);
		rect.InflateRect(1,1);
	}
	rect.DeflateRect(3,3);
	pDC->FillSolidRect(rect,m_CurrentColor);
	rect.left -= 1;rect.top -= 1;
	pOldPen = pDC->SelectObject(&_pen3DDKShadow);
	pDC->MoveTo(rect.right - 1,rect.top);
	pDC->LineTo(rect.left,rect.top);
	pDC->LineTo(rect.left,rect.bottom);
	pDC->SelectObject(&_penW);
	pDC->LineTo(rect.right - 6,rect.bottom);
	pDC->LineTo(rect.right - 6,rect.bottom - 4);
	pDC->LineTo(rect.right,rect.bottom - 4);
	pDC->MoveTo(rect.right,rect.top);
	pDC->LineTo(rect.right,rect.bottom - 3);
	
	rect.right += 1;
	rect.bottom += 1;
	rect.left = rect.right - 6;
	rect.top = rect.bottom - 4;
	pDC->FillSolidRect(rect,clr3DFace);

	pDC->SelectObject(&_penB);
	pDC->MoveTo(rect.left + 1,rect.top + 1);
	pDC->LineTo(rect.right,rect.top + 1);
	pDC->MoveTo(rect.left + 2,rect.top + 2);
	pDC->LineTo(rect.right-1,rect.top + 2);
	pDC->MoveTo(rect.left + 3,rect.top + 3);
	pDC->LineTo(rect.right-2,rect.top + 3);
	
	pDC->SelectObject(pOldPen);

	return TRUE;
}

void CColorPicker::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CButton::OnLButtonDown(nFlags, point);
	::ClientToScreen(this->m_hWnd,&point);
	if(CreatePaletteWindow())
	{
		nFlags = (UINT)m_CurrentColor;
		::PostMessage(m_hPaletteWnd,WM_LBUTTONDOWN,nFlags,MAKELPARAM(point.x,point.y));
	}
	HWND hwndParent = GetParent()->m_hWnd;
	if(hwndParent)
	{
		POINT	p1,p2;
		RECT	rect;
		::GetWindowRect(m_hWnd,&rect);
		p1.x = rect.left;
		p1.y = rect.top;
		p2.x = rect.right;
		p2.y = rect.bottom;
		::ScreenToClient(hwndParent,&p1);
		::ScreenToClient(hwndParent,&p2);
		rect.left = p1.x;
		rect.top = p1.y;
		rect.right = p2.x;
		rect.bottom = p2.y;
		::InvalidateRect(hwndParent,&rect,TRUE);
	}
}

void CColorPicker::OnLButtonUp(UINT nFlags, CPoint point) 
{
	DestroyPaletteWindow();
	if((COLORREF)nFlags != m_CurrentColor)
		SetColor((COLORREF)nFlags);
	else
		Invalidate();
}

LONG FAR PASCAL CColorPicker::PaletteWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	HDC					hDC;
	PAINTSTRUCT			ps;
	POINT				point;
	CRect				crtColorRect(5,125,40,143);
	static	POINT		oldPoint;
	static	BOOL		bMouseMoved = FALSE;
	static	HWND		hwndParent;
	static	HWND		hwndColorBtn;
	static	HFONT		hFont = NULL;
	static	COLORREF	crtColor,oldColor;
	static	int			iActiveRect = -1;
	switch(nMsg)
	{
	case WM_CREATE:
		{
		hwndParent = ((LPCREATESTRUCT)lParam)->hwndParent;
		crtColor = ::GetSysColor(COLOR_3DFACE);
		LOGFONT logFont;
		ZeroMemory((void*)&logFont,sizeof(logFont));
		strcpy(logFont.lfFaceName,"宋体");
		logFont.lfHeight = -12;
		logFont.lfWeight = 400;
		logFont.lfCharSet = GB2312_CHARSET;
		logFont.lfOutPrecision = 3;
		logFont.lfClipPrecision = 2; 
		logFont.lfQuality = 1;
		logFont.lfPitchAndFamily = 2;
		hFont = ::CreateFontIndirect(&logFont);
		////色彩对话框按钮
		hwndColorBtn = ::CreateWindow("BUTTON","",WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,167,126,16,16,hWnd,HMENU(IDC_COLORDLG_BUTTON),::AfxGetInstanceHandle(),NULL);
		if(hwndColorBtn)
			::ShowWindow(hwndColorBtn,SW_SHOW);
		}
		break;
	case WM_DESTROY:
		::DeleteObject(hFont);
		break;
	case WM_DRAWITEM:
		{
		LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lParam;
		if(lpDis->CtlID == IDC_COLORDLG_BUTTON)
		{
			CRect		rect(3,3,8,8);
			::FillRect(lpDis->hDC,&lpDis->rcItem,(HBRUSH)::GetStockObject(LTGRAY_BRUSH));
			HPEN	hOldPen = (HPEN)::SelectObject(lpDis->hDC,_penW);
			if(lpDis->itemState == 17)
			{
				::MoveToEx(lpDis->hDC,lpDis->rcItem.right-1,lpDis->rcItem.top,NULL);
				::LineTo(lpDis->hDC,lpDis->rcItem.right-1,lpDis->rcItem.bottom-1);
				::LineTo(lpDis->hDC,lpDis->rcItem.left,lpDis->rcItem.bottom-1);
				::SelectObject(lpDis->hDC,_penB);
				::LineTo(lpDis->hDC,lpDis->rcItem.left,lpDis->rcItem.top);
				::LineTo(lpDis->hDC,lpDis->rcItem.right-1,lpDis->rcItem.top);
				rect.OffsetRect(1,1);
			}
			else
			{
				::MoveToEx(lpDis->hDC,lpDis->rcItem.right-1,lpDis->rcItem.top,NULL);
				::LineTo(lpDis->hDC,lpDis->rcItem.left,lpDis->rcItem.top);
				::LineTo(lpDis->hDC,lpDis->rcItem.left,lpDis->rcItem.bottom-1);
				::SelectObject(lpDis->hDC,_penB);
				::LineTo(lpDis->hDC,lpDis->rcItem.right-1,lpDis->rcItem.bottom-1);
				::LineTo(lpDis->hDC,lpDis->rcItem.right-1,lpDis->rcItem.top);
			}
			::SelectObject(lpDis->hDC,_pen3DDKShadow);
			::Rectangle(lpDis->hDC,rect.left-1,rect.top-1,rect.right+1,rect.bottom+1);
			::SetBkColor(lpDis->hDC, RGB(0xff,0,0));
			::ExtTextOut(lpDis->hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			rect.OffsetRect(2,2);
			::Rectangle(lpDis->hDC,rect.left-1,rect.top-1,rect.right+1,rect.bottom+1);
			::SetBkColor(lpDis->hDC, RGB(0,0xff,0));
			::ExtTextOut(lpDis->hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			rect.OffsetRect(2,2);
			::Rectangle(lpDis->hDC,rect.left-1,rect.top-1,rect.right+1,rect.bottom+1);
			::SetBkColor(lpDis->hDC, RGB(0xff,0xff,0));
			::ExtTextOut(lpDis->hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			::SelectObject(lpDis->hDC,hOldPen);
		}
		}
		break;
	case WM_SETCOLOR:
		crtColor = (COLORREF)wParam;
		oldColor = crtColor;
		break;
	case WM_COMMAND:
		::PostMessage(hwndParent,WM_COLORDLG,NULL,NULL);
		break;
	case WM_MOUSEMOVE:
		{
		BOOL bInCtrlArea = FALSE;
		point.x = LPARAM_X(lParam);
		point.y = LPARAM_Y(lParam);
		::ClientToScreen(hWnd,&point);
		if(point.x != oldPoint.x || point.y != oldPoint.y)
		{
			bMouseMoved = TRUE;
			HWND	hwndPoint = ::WindowFromPoint(point);
			if(hwndPoint)
			{
				COLORREF	crtTMPColor;
				CRect		rect;
				HDC			hDC;
				hDC = ::GetWindowDC(hwndPoint);
				::GetWindowRect(hwndPoint,&rect);
				crtTMPColor = GetPixel(hDC,point.x-rect.left,point.y-rect.top);
				::ReleaseDC(hwndPoint,hDC);
				RECT redrawRect;
				redrawRect.left = 0;
				redrawRect.right = 150;
				redrawRect.top = 120;
				redrawRect.bottom = 145;
				::GetWindowRect(hWnd,&rect);
				if(!rect.PtInRect(point))//不在本窗口
				{
					crtColor = crtTMPColor;
					::InvalidateRect(hWnd,&redrawRect,TRUE);
				}
				else
				{
					rect.bottom = rect.top + 120;
					int iCurrentActiveRect;
					if(rect.PtInRect(point))///判断鼠标是否在当前窗口的色彩区域
					{
						////计算位于哪个方框内////
						point.x -= rect.left;
						point.y -= rect.top;
						if(point.x%10 != 0 && point.y%10 != 0)
							iCurrentActiveRect = point.y/10*19 + point.x/10;
						else
							iCurrentActiveRect = -1;
					}
					else
					{
						iCurrentActiveRect = -1;
						crtColor = oldColor;
						if(hwndColorBtn == hwndPoint)
							::ReleaseCapture();
						else
							bInCtrlArea = TRUE;

					}
					if(iCurrentActiveRect != iActiveRect)
					{
						///清除原焦点
						CRect ActiveRect;
						ActiveRect.left = iActiveRect%19*10;
						ActiveRect.top = iActiveRect/19*10;
						ActiveRect.right = ActiveRect.left + 10;
						ActiveRect.bottom = ActiveRect.top + 10;
						::InvalidateRect(hWnd,&ActiveRect,TRUE);
						///新焦点
						if(iCurrentActiveRect != -1)
						{
							crtColor = crtTMPColor;
							ActiveRect.left = iCurrentActiveRect%19*10;
							ActiveRect.top = iCurrentActiveRect/19*10;
							ActiveRect.right = ActiveRect.left + 10;
							ActiveRect.bottom = ActiveRect.top + 10;
							::InvalidateRect(hWnd,&ActiveRect,TRUE);
						}
						::InvalidateRect(hWnd,&redrawRect,TRUE);
						iActiveRect = iCurrentActiveRect;
					}
				}
			}
		}
		if(bInCtrlArea)
			::PostMessage(hwndParent,WM_SETCURSOR,NULL,MAKELPARAM(1,0));
		else
			::PostMessage(hwndParent,WM_SETCURSOR,NULL,NULL);
		}
		break;
	case WM_ACTIVATE:
		if(LOWORD(wParam) != WA_INACTIVE)
			break;
	case WM_LBUTTONUP:
		point.x = LPARAM_X(lParam);
		point.y = LPARAM_Y(lParam);
		if(bMouseMoved || ( point.x == oldPoint.x && point.y == oldPoint.y ))
			::PostMessage(hwndParent,WM_LBUTTONUP,(UINT)crtColor,lParam);
		break;
	case WM_LBUTTONDOWN:
		oldPoint.x = LPARAM_X(lParam);
		oldPoint.y = LPARAM_Y(lParam);
		bMouseMoved = FALSE;
		::InvalidateRect(hWnd,&crtColorRect,TRUE);
		break;
	case WM_ERASEBKGND:
		{
		hDC = (HDC)wParam;
		HPEN oldPen = (HPEN)::SelectObject(hDC,_penB);
		for(int i=0;i<18;i++)
		{
			::MoveToEx(hDC,9+i*10,0,NULL);
			::LineTo(hDC,9+i*10,120);
		}
		for(i=0;i<12;i++)
		{
			::MoveToEx(hDC,0,9+i*10,NULL);
			::LineTo(hDC,190,9+i*10);
		}
		CRect btmRect(0,120,191,150);
		::FillRect(hDC,btmRect,(HBRUSH)::GetStockObject(LTGRAY_BRUSH));
		::Rectangle(hDC,crtColorRect.left-1,crtColorRect.top-1,crtColorRect.right + 1,crtColorRect.bottom + 1);
		::SelectObject(hDC,oldPen);
		}
		break;
	case WM_PAINT:
		{
		hDC = ::BeginPaint(hWnd,&ps);
		CDC dc;
		dc.Attach(hDC);
		CRect	rect;
		int x1,y1,i,j,k;
		UCHAR R=255,G=255,B=255;
		for(i=0;i<12;i++)
		{
			y1 = i*10;
			rect.SetRect(0,y1,9,y1+9);
			if(i == 10)
				R=G=B=0x17;
			dc.FillSolidRect(rect,RGB(R,G,B));
			R -= 0x17;
			G -= 0x17;
			B -= 0x17;
		}
		R = G = B = 0;
		for(k = 0;k < 2;k++)
		for(j = 0;j < 18;j++)
		for(i = 0;i < 6;i++)
		{
			x1 = 10+j*10;
			y1 = k*60+i*10;
			rect.SetRect(x1,y1,x1+9,y1+9);
			dc.FillSolidRect(rect,RGB(R,G,B));
			if(B == 0xff)
			{
				B = 0x00;
				if(G == 0xff)
				{
					G = 0x00;
					R += 0x33;
				}
				else
					G += 0x33;
			}
			else
				B += 0x33;
		}
		if(iActiveRect != -1)
		{
			CRect ActiveRect;
			ActiveRect.left = iActiveRect%19*10;
			ActiveRect.top = iActiveRect/19*10;
			ActiveRect.right = ActiveRect.left + 10;
			ActiveRect.bottom = ActiveRect.top + 10;
			dc.DrawFocusRect(ActiveRect);
		}
		rect.SetRect(0,y1+130,60,y1+140);
		dc.FillSolidRect(crtColorRect,crtColor);
		char strColor[8]="#";
		sprintf(strColor+1,"%02X%02X%02X",GetRValue(crtColor),GetGValue(crtColor),GetBValue(crtColor));
		HFONT hOldFont = (HFONT)dc.SelectObject(hFont);
		dc.SetBkMode(TRANSPARENT);
		dc.TextOut(80,127,strColor,7);
		dc.SelectObject(hOldFont);
		dc.Detach();
		::EndPaint(hWnd,&ps);
		}
		break;
	default:
		return(::DefWindowProc(hWnd,nMsg,wParam,lParam));
	}
	return NULL;
}

BOOL CColorPicker::CreatePaletteWindow()
{
	if(!m_bPaletteWndActive)
	{
		// 创建调色板子窗口
		WNDCLASS wndcls;
		wndcls.style = CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = PaletteWndProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = ::AfxGetInstanceHandle();
		wndcls.hIcon = NULL;
		wndcls.hCursor = NULL;
		wndcls.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = "ColorPalette";
		if (!::RegisterClass(&wndcls))
			AfxThrowResourceException();
		HDC hDC = ::GetDC(m_hWnd);
		int scrWidth = ::GetDeviceCaps(hDC,HORZRES);
		int scrHeight = ::GetDeviceCaps(hDC,VERTRES);
		::ReleaseDC(m_hWnd,hDC);
		CRect		rect;
		GetWindowRect(rect);
		rect.top = rect.bottom - 1;
		if(rect.left > scrWidth-191)
			rect.left = scrWidth-191;
		else if(rect.left < 0)
			rect.left = 0;
		if(rect.top > scrHeight-150)
			rect.top = rect.top-150;
		rect.bottom = rect.top + 150;
		rect.right = rect.left + 191;

		if(!(m_hPaletteWnd = ::CreateWindowEx(WS_EX_TOPMOST,"ColorPalette","Palatte",WS_POPUP|WS_BORDER|WS_VISIBLE,rect.left,rect.top,rect.Width(),rect.Height(),m_hWnd,NULL,wndcls.hInstance,NULL)))
			return FALSE;
	}
	::PostMessage(m_hPaletteWnd,WM_SETCOLOR,(WPARAM)m_CurrentColor,NULL);
	::ShowWindow(m_hPaletteWnd,SW_SHOW);
	::SetCapture(m_hPaletteWnd);
	m_bPaletteWndActive = TRUE;
	return TRUE;
}

void CColorPicker::DestroyPaletteWindow()
{
	::DestroyWindow(m_hPaletteWnd);
	::UnregisterClass("ColorPalette",::AfxGetInstanceHandle());
	m_bPaletteWndActive = FALSE;
	m_hPaletteWnd = NULL;
}

BOOL CColorPicker::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_bPaletteWndActive && m_hCursorStraw && nHitTest != 1)
		::SetCursor(m_hCursorStraw);
	else
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return TRUE;
}

void CColorPicker::GetColor(CString& strColor)
{
	char cColor[8]="#";
	sprintf(cColor+1,"%02X%02X%02X",GetRValue(m_CurrentColor),GetGValue(m_CurrentColor),GetBValue(m_CurrentColor));
	strColor = cColor;
}

COLORREF CColorPicker::GetColor()
{
	return m_CurrentColor;
}

LRESULT CColorPicker::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_COLORDLG)
	{
		DestroyPaletteWindow();
		CColorDialog dlg;
		dlg.m_cc.Flags |= CC_FULLOPEN;
		if(dlg.DoModal() == IDOK)
			SetColor(dlg.GetColor());
		return TRUE;
	}
	else
		return CButton::DefWindowProc(message, wParam, lParam);
}

void CColorPicker::SetBuddy(HWND hWnd)
{
	m_hwndBuddy = hWnd;
}

void CColorPicker::SetColor(COLORREF ref)
{
	m_CurrentColor = ref;
	::PostMessage(GetParent()->m_hWnd,WM_COMMAND,MAKELPARAM(::GetWindowLong(m_hWnd,GWL_ID),BN_CLICKED),(LPARAM)m_hWnd);
	if(m_hwndBuddy)
	{
		CString strColor;
		GetColor(strColor);
		::SetWindowText(m_hwndBuddy,strColor);
	}
	Invalidate();
}

