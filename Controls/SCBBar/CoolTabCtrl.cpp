////////////////////////////////////////////////////////////////
// CoolTabCtrl.cpp : implementation file                      //
//															  //
// Copyright 2001 WangJun									  //
// All Rights Reserved.										  //
//															  //
// Email: wangjun98@sohu.com								  //
// URL:   www.vckbase.com									  //
//															  //
// 1.0     2001/9/30   First release version.				  //
//															  //
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CoolTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoolTabCtrl

IMPLEMENT_DYNCREATE(CCoolTabCtrl, CWnd)

#define	ITEMBUTTON_HEIGHT		23

static CPen _penW(PS_SOLID,1 ,RGB(255,255,255));
static CPen _penB(PS_SOLID,1 ,RGB(0,0,0));
static CPen _pen3DFace(PS_SOLID,1,GetSysColor(COLOR_3DFACE));
static CPen _pen3DLight(PS_SOLID,1,GetSysColor(COLOR_3DLIGHT));
static CPen _pen3DShadow(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
static CPen _pen3DDKShadow(PS_SOLID,1,GetSysColor(COLOR_3DDKSHADOW));


CCoolTabCtrl::CCoolTabCtrl()
{
	m_nStyle = TCS_DOWN;
	m_nActivePage = -1;
	m_nBorder = 1;
	m_bEraseBkgnd = TRUE;
}

CCoolTabCtrl::~CCoolTabCtrl()
{
	POSITION pos;
	CPageItem* pItem;
	for(pos=m_PageList.GetHeadPosition();pos!=NULL;)
	{
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		if(pItem)
		{	
			if(pItem->m_nStyle == 1 &&pItem->m_pWnd) ///Dialog
				delete pItem->m_pWnd;
			delete pItem;
			pItem=NULL;
		}
	}
	m_PageList.RemoveAll();
}


BEGIN_MESSAGE_MAP(CCoolTabCtrl, CWnd)
	//{{AFX_MSG_MAP(CCoolTabCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCoolTabCtrl message handlers

BOOL CCoolTabCtrl::Create(UINT wStyle, const CRect &rect, CWnd *pParentWnd, UINT nID)
{
	m_nStyle = wStyle & TCS_MASK;
	if(m_nStyle == 0)
		m_nStyle = TCS_UP;
	if (CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(LTGRAY_BRUSH), NULL),
						NULL,
						wStyle&~TCS_MASK | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						rect, 
						pParentWnd, 
						nID))
	{
		LOGFONT logFont;
		ZeroMemory((void*)&logFont,sizeof(logFont));
		strcpy(logFont.lfFaceName,"����");
		logFont.lfHeight = -12;
		logFont.lfWeight = 400;
		logFont.lfCharSet = GB2312_CHARSET;
		logFont.lfOutPrecision = 3;
		logFont.lfClipPrecision = 2; 
		logFont.lfQuality = 1;
		logFont.lfPitchAndFamily = 2;
		m_font.CreateFontIndirect(&logFont);
		SetFont(&m_font);
		return TRUE;
	}
	return FALSE;
}

BOOL CCoolTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	if(m_bEraseBkgnd)
	{
		CRect rect;
		GetWindowRect(&rect);
		ScreenToClient(&rect);
		CBrush brush(GetSysColor(COLOR_3DFACE));
		pDC->FillRect(rect,&brush);
	}
	return TRUE;
}

void CCoolTabCtrl::OnPaint() 
{
	CPaintDC	dc(this);
	CPen	*pOldPen = dc.GetCurrentPen();
	CFont	*pOldFont = dc.SelectObject(&m_font);
	int		nOldBkMode = dc.SetBkMode(TRANSPARENT);
	CPageItem	*pItem;
	POSITION	pos;
	int		nItemIndex = 0;
	
	DrawFrame(&dc);

	for(pos=m_PageList.GetHeadPosition();pos!=NULL;nItemIndex++)
	{
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		if(pItem)
		{
			pItem->Draw(&dc,m_nStyle,(m_nActivePage==nItemIndex)?TRUE:FALSE);
		}

	}
	dc.SetBkMode(nOldBkMode);
	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
}


void CCoolTabCtrl::AddPage(CWnd *pWnd, LPCTSTR sText, UINT IconID)
{
	ASSERT(IsWindow(pWnd->m_hWnd));
	CPageItem *pItem;
	pItem = new CPageItem();
	pItem->m_pWnd = pWnd;
	pItem->m_nStyle = 0;//Window
	pItem->m_sText = sText;
	if(IconID)
		pItem->m_hIcon = AfxGetApp()->LoadIcon(IconID);
	else
		pItem->m_hIcon = NULL;
	if(pWnd)
	{
		CRect	rect;
		GetClientRect(rect);
		pWnd->MoveWindow(rect);
	}
	m_PageList.AddTail(pItem);
}

BOOL CCoolTabCtrl::AddPage(CRuntimeClass* pClass,UINT nIDTemplate, LPCTSTR sText, UINT IconID)
{
	CDialog *pDlg = (CDialog*)pClass->CreateObject();
	if(pDlg != NULL)
		if(pDlg->Create(nIDTemplate,this))
		{
			CPageItem *pItem;
			pItem = new CPageItem();
			pItem->m_pWnd = (CWnd*)pDlg;
			pItem->m_nStyle = 1;///Dialog
			pItem->m_sText = sText;
			if(IconID)
				pItem->m_hIcon = AfxGetApp()->LoadIcon(IconID);
			else
				pItem->m_hIcon = NULL;
			CRect	rect;
			GetClientRect(rect);
			pDlg->MoveWindow(rect);
			m_PageList.AddTail(pItem);
			return TRUE;
		}
	return FALSE;
}


void CCoolTabCtrl::GetClientRect(LPRECT lpRect)
{
	CWnd::GetClientRect(lpRect);
	if(m_nStyle&TCS_DOWN)
	{
		lpRect->left += 2;
		lpRect->right -= 2;
		lpRect->top += 2;
		lpRect->bottom -= ITEMBUTTON_HEIGHT;
	}
	else if(m_nStyle&TCS_UP)
	{
		lpRect->left += 2;
		lpRect->right -= 2;
		lpRect->top += ITEMBUTTON_HEIGHT;
		lpRect->bottom -= 2;
	}
}

///////ȡ����Ҫռ�õĿ��/////
UINT CCoolTabCtrl::CPageItem::GetAreaWidth(CDC *pDC)
{
	UINT width = pDC->GetTextExtent(m_sText).cx;
	if(m_hIcon)
		width += 18;
	return width + 9;
}

void CCoolTabCtrl::CPageItem::Draw(CDC *pDC, UINT nStyle, BOOL bActive)
{
	CRect rect = m_rect;
	if(nStyle&TCS_DOWN)
	{
		if(bActive)
		{
			rect.top -= 2;
			CBrush brush(GetSysColor(COLOR_3DFACE));
			pDC->FillRect(rect,&brush);
			rect.top += 1;
		}
		else
			rect.bottom -= 1;
		CBrush brush(GetSysColor(COLOR_3DFACE));
		pDC->FillRect(m_rect,&brush);
		pDC->SelectObject(&_penW);
		pDC->MoveTo(rect.left,rect.top);
		pDC->LineTo(rect.left,rect.bottom-1);
		
		pDC->SelectObject(&_pen3DShadow);
		pDC->MoveTo(rect.left+1,rect.bottom-2);
		pDC->LineTo(rect.left+1,rect.bottom-1);
		pDC->LineTo(rect.right-1,rect.bottom-1);
		pDC->MoveTo(rect.right-2,rect.bottom-2);
		pDC->LineTo(rect.right-1,rect.bottom-2);
		pDC->LineTo(rect.right-1,rect.top-1);

		pDC->SelectObject(&_penB);
		pDC->MoveTo(rect.left+2,rect.bottom);
		pDC->LineTo(rect.right-2,rect.bottom);
		pDC->LineTo(rect.right,rect.bottom-2);
		pDC->MoveTo(rect.right,rect.top);
		pDC->LineTo(rect.right,rect.bottom-1);
	
		rect.top -= 1;
	}//end of TCS_DOWN
	else if(nStyle&TCS_UP)
	{
		CBrush brush(GetSysColor(COLOR_3DFACE));
		if(bActive)
		{
			rect.bottom += 1;
			pDC->FillRect(rect,&brush);
			rect.bottom -= 1;
		}
		else
		{
			rect.top += 2;
			pDC->FillRect(rect,&brush);
		}
		pDC->SelectObject(&_penW);
		pDC->MoveTo(rect.left,rect.bottom);
		pDC->LineTo(rect.left,rect.top + 2);
		pDC->LineTo(rect.left + 2,rect.top);
		pDC->LineTo(rect.right,rect.top);

		pDC->SelectObject(&_pen3DShadow);
		pDC->MoveTo(rect.right - 1,rect.top);
		pDC->LineTo(rect.right - 1,rect.bottom);
			
		pDC->SelectObject(&_penB);
		pDC->MoveTo(rect.right,rect.top + 1);
		pDC->LineTo(rect.right,rect.bottom);	
	}
	///////////����λ��//////////
		rect.left += 5;
		rect.right -= 2;
		///////////��ʾͼ��//////////
		if(rect.Width() > 16 && m_hIcon != NULL)
		{
			::DrawIconEx(pDC->m_hDC,rect.left,rect.top + 3,m_hIcon,16,16,0,NULL,DI_NORMAL);
			rect.left += 18;
		}
		if (!m_sText.IsEmpty())
		{
			///////////��ʾ����//////////
			rect.top += 5;
			CString sText = m_sText;
			int l = sText.GetLength();
			int i;
			for(i=0;i<10 && pDC->GetTextExtent(sText).cx > rect.Width();i++,l-=2)
				sText = sText.Left(l-2);
			if(i > 0)
			{
				sText = sText.Left(l-2);
				sText += "...";
			}
			pDC->DrawText(sText, &rect, DT_LEFT /*| DT_VCENTER */| DT_SINGLELINE);
		}
}

////�����ߴ�////
void CCoolTabCtrl::AutoSize()
{
	UINT PageCount = m_PageList.GetCount();
	if(PageCount < 1) return;
	CPageItem	*pItem;
	POSITION	pos;
	CRect		rect,ClientRect,ItemRect;

	GetClientRect(ClientRect);
	ClientRect.DeflateRect(m_nBorder+1,m_nBorder+1);
	GetWindowRect(rect);
	ScreenToClient(rect);

	CDC* pDC = GetDC();
	CFont	*pOldFont = pDC->SelectObject(&m_font);

	if(m_nStyle&TCS_DOWN)
	{
		rect.left += 6;
		rect.right -= 6;
		rect.bottom -= 1;
		rect.top = rect.bottom - ITEMBUTTON_HEIGHT + 1;
	}
	else if(m_nStyle&TCS_UP)
	{
		rect.left += 2;
		rect.right -= 6;
		rect.bottom = rect.top + ITEMBUTTON_HEIGHT;
	}
	ItemRect = rect;
	int AreaWidth = 0,ItemMaxWidth,ItemIndex=0;
	ItemMaxWidth = rect.Width()/m_PageList.GetCount();
	BOOL	bMonoSpace = ((m_nStyle&TCS_MONOSPACE) == TCS_MONOSPACE)?1:0;
	////////���ð�ť��ʼ��ȣ����õ���ť����ռ�õ��ܿ��///////
	for(pos=m_PageList.GetHeadPosition();pos!=NULL;ItemIndex++)
	{
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		if(pItem)
		{
			if(!bMonoSpace)
				ItemMaxWidth = pItem->GetAreaWidth(pDC);
			AreaWidth += ItemMaxWidth;
			ItemRect.right = ItemRect.left+ItemMaxWidth-1;
			pItem->m_rect = ItemRect;
			ItemRect.left = ItemRect.right + 1;
			if(pItem->m_pWnd)
				pItem->m_pWnd->MoveWindow(ClientRect);
		}
	}
	////////����Ҫ�Ŀռ����ʵ�ʿռ�ʱ���е���////
	if(AreaWidth > rect.Width() && !bMonoSpace)
	{
		ItemRect = rect;
		int AreaWidth,MaxWidth = rect.Width()/PageCount;
		for(pos=m_PageList.GetHeadPosition();pos!=NULL;)
		{
			pItem=(CPageItem*)m_PageList.GetNext(pos);
			if(pItem)
			{
				AreaWidth = pItem->GetAreaWidth(pDC);
				ItemMaxWidth = (ItemMaxWidth < AreaWidth)?MaxWidth:AreaWidth;
				ItemRect.right = ItemRect.left+ItemMaxWidth;
				pItem->m_rect = ItemRect;
				ItemRect.left = ItemRect.right + 1;
			}
		}
	}
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
}

void CCoolTabCtrl::SetActivePage(int nIndex)
{
	if(nIndex == m_nActivePage)
		return;
	CPageItem *pOldItem,*pItem;
	if(m_nActivePage >= 0)
	{
		pOldItem = (CPageItem*)GetPageItem(m_nActivePage);
		if(pOldItem)
		{
			pOldItem->m_pWnd->ShowWindow(SW_HIDE);
		}
	}
	pItem = (CPageItem*)GetPageItem(nIndex);
	if(!pItem) return;
	m_nActivePage = nIndex;
	if(m_nStyle&TCS_ANIMATE)
		SetTimer(100,15,NULL);///Ϊ����95��NT4��Ҳ�ж���Ч����û��ʹ��AnimateWindow����
	else
		pItem->m_pWnd->ShowWindow(SW_SHOW);
}

void* CCoolTabCtrl::GetPageItem(UINT nIndex)
{
	CPageItem *pItem = NULL;
	POSITION pos = m_PageList.FindIndex(nIndex);
	if(pos)
		pItem = (CPageItem*)m_PageList.GetAt(pos);
	return pItem;
}

void CCoolTabCtrl::UpdateWindow()
{
 	AutoSize();
 	if(m_nActivePage < 0)
 		SetActivePage(0);
 	else
 		SetActivePage(m_nActivePage);
 	Invalidate();
}

void CCoolTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT nItemIndex=0;
	POSITION pos;
	CPageItem *pItem;
	for(pos=m_PageList.GetHeadPosition();pos!=NULL;nItemIndex++)
	{
		pItem=(CPageItem*)m_PageList.GetNext(pos);
		if(pItem)
		{
			if(pItem->m_rect.PtInRect(point))
			{
				SetActivePage(nItemIndex);
				Invalidate();
				break;
			}
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CCoolTabCtrl::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CWnd::OnSizing(fwSide, pRect);
	m_bEraseBkgnd = FALSE;
	AutoSize();
}

void CCoolTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	m_bEraseBkgnd = TRUE;
	AutoSize();
	Invalidate();
}

void CCoolTabCtrl::DrawFrame(CDC *pDC)
{
	CRect rect;
	GetClientRect(&rect);
	switch(m_nBorder)
	{
	case 1:
			pDC->Draw3dRect(rect,RGB(0xff,0xff,0xff),RGB(0,0,0));
		break;
	case 2:
			pDC->Draw3dRect(rect,RGB(0xff,0xff,0xff),RGB(0,0,0));
			rect.DeflateRect(1, 1);
			pDC->Draw3dRect(rect, GetSysColor(COLOR_3DFACE), GetSysColor(COLOR_3DSHADOW));
		break;
	default:break;
	}
}



void CCoolTabCtrl::SetStyle(UINT style)
{
	m_nStyle = style;
}

UINT CCoolTabCtrl::GetStyle()
{
	return m_nStyle;
}

void CCoolTabCtrl::OnTimer(UINT nIDEvent) 
{
	static	step,times = 10;
	UINT	width;
	CRect	rect;
	GetClientRect(&rect);
	rect.DeflateRect(m_nBorder+1,m_nBorder+1);
	POSITION pos = m_PageList.FindIndex(m_nActivePage);
	if(pos)
	{
		CPageItem *pItem = (CPageItem*)m_PageList.GetAt(pos);
		if(pItem)
		{
			width = rect.Width();
			if(times == 10)
			{
				rect.right = 10;
				rect.left = rect.right - width + 1;
				step = width/times;
				times--;
				m_bEraseBkgnd = FALSE;
				pItem->m_pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				if(times <= 0)
				{
					times = 10;
					KillTimer(nIDEvent);
					m_bEraseBkgnd = TRUE;
				}
				else
				{
					rect.right = width - times*step;
					rect.left = rect.right - width + 1;
					times--;
				}
			}
			pItem->m_pWnd->MoveWindow(rect);
		}
	}
}

void CCoolTabCtrl::SetStyleAnimate(BOOL bEnable)
{
	UINT style = GetStyle();
	if(bEnable)
		style |= TCS_ANIMATE;
	else
		style &= ~TCS_ANIMATE;
	SetStyle(style);
}

void CCoolTabCtrl::SetStyleDirection(int Direction)
{
	UINT style = GetStyle();
	switch(Direction)
	{
	case TCS_DOWN:
		style &= ~TCS_UP;
		style |= TCS_DOWN;
		break;
	case TCS_UP:
	default:
		style &= ~TCS_DOWN;
		style |= TCS_UP;
		break;
	}
	SetStyle(style);
	UpdateWindow();
}

void CCoolTabCtrl::SetStyleMonoSpace(BOOL bEnable)
{
	UINT style = GetStyle();
	if(bEnable)
		style |= TCS_MONOSPACE;
	else
		style &= ~TCS_MONOSPACE;
	SetStyle(style);
	UpdateWindow();
}
