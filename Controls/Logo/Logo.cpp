// Logo.cpp : implementation file
//

#include "stdafx.h"
#include "Logo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLogo

CLogo::CLogo()
{
}

CLogo::~CLogo()
{
}


BEGIN_MESSAGE_MAP(CLogo, CBitmapButton)
	//{{AFX_MSG_MAP(CLogo)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogo message handlers

void CLogo::SetPosition()
{
	if(!::IsWindow(m_hWnd))
		return;

	CDockBar* pTopDockBar = GetParentDockBar();
	if(!pTopDockBar)
		return;

	CRect TopDockBarRect;
	pTopDockBar->GetClientRect(TopDockBarRect);

	CRect LogoRect;
	GetWindowRect(LogoRect);
	pTopDockBar->ScreenToClient(LogoRect);	

	int nLeft = TopDockBarRect.right-LogoRect.Width()-::GetSystemMetrics(SM_CXEDGE);
	int nTop = TopDockBarRect.top;

	if(nLeft != LogoRect.left ||
	   nTop != LogoRect.top)
		SetWindowPos(NULL, 
					 nLeft,
					 nTop,
					 0,
					 0,
					 SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);

	CRect UnionToolbarRect;

	for(int nI=0;nI<pTopDockBar->m_arrBars.GetSize();nI++)
	{
		CWnd* pWnd = static_cast<CWnd*>(pTopDockBar->m_arrBars[nI]);
		if(!pWnd ||
		   !AfxIsValidAddress(pWnd,sizeof(CWnd)) || 
		   !::IsWindow(*pWnd))
				continue;

		CRect ToolBarRect;
		pWnd->GetWindowRect(&ToolBarRect);
		UnionToolbarRect.UnionRect(UnionToolbarRect,ToolBarRect);
	}

	GetWindowRect(LogoRect);

	UnionToolbarRect.IntersectRect(UnionToolbarRect,LogoRect);
	if(UnionToolbarRect.IsRectEmpty())
	{
		if(!IsWindowVisible())
			ShowWindow(SW_SHOW);
	}
	else
	{
		if(IsWindowVisible())
			ShowWindow(SW_HIDE);
	}
}

CDockBar* CLogo::GetParentDockBar()
{
	CWnd* pParentWnd =GetParent();
	if(!pParentWnd ||
	   !pParentWnd->IsKindOf(RUNTIME_CLASS(CDockBar)))
		return NULL;

	return static_cast<CDockBar*>(pParentWnd);
}

void CLogo::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CBitmapButton::OnMouseMove(nFlags, point);

	TRACKMOUSEEVENT TrackMouseEventStructure;
	ZeroMemory(&TrackMouseEventStructure,sizeof(TrackMouseEventStructure));
	TrackMouseEventStructure.cbSize = sizeof(TrackMouseEventStructure);
	TrackMouseEventStructure.hwndTrack = m_hWnd;
	TrackMouseEventStructure.dwFlags = TME_LEAVE;
	TrackMouseEventStructure.dwHoverTime = HOVER_DEFAULT;

	_TrackMouseEvent(&TrackMouseEventStructure);

	if ((nFlags & MK_LBUTTON)!=0 ||
		(nFlags & MK_RBUTTON)!=0) 
	  return;
	
	CString MessageLine;
	if(MessageLine.LoadString(GetDlgCtrlID()))
	{
		int nDelPos = MessageLine.Find(_T('\n'));
		if(nDelPos != -1)
			MessageLine = MessageLine.Left(nDelPos);
	}

	CWnd*  pMainWnd = ::AfxGetMainWnd();
	if(pMainWnd)
		pMainWnd->SendMessage(WM_SETMESSAGESTRING,0,LPARAM(LPCTSTR(MessageLine)));
}

LRESULT CLogo::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_MOUSELEAVE)
	{
		CWnd*  pMainWnd = ::AfxGetMainWnd();
		if(pMainWnd)
			pMainWnd->SendMessage(WM_POPMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
	}

	return CBitmapButton::DefWindowProc(message, wParam, lParam);
}

int CLogo::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
	CString ToolTipText;

	if(::IsWindow(m_hWnd) && ToolTipText.LoadString(GetDlgCtrlID()))
	{
		int nDelPos = ToolTipText.Find(_T('\n'));
		if(nDelPos != -1)
			ToolTipText = ToolTipText.Right(ToolTipText.GetLength()-nDelPos-1);
	}

	if(ToolTipText.IsEmpty())
		return -1;

	int nLen = ToolTipText.GetLength();
	PTCHAR pText = PTCHAR(::malloc((nLen+1)*sizeof(TCHAR)));
	if(!pText)
		return -1;

	::_tcscpy(pText,ToolTipText.LockBuffer());
	ToolTipText.UnlockBuffer();
	pTI->lpszText = pText;
	pTI->hwnd = m_hWnd;

	CRect ClientRect;
	GetClientRect(ClientRect);
	pTI->rect.left = ClientRect.left;
	pTI->rect.top = ClientRect.top;
	pTI->rect.right = ClientRect.right;
	pTI->rect.bottom = ClientRect.bottom;

	return 1;
}

void CLogo::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CBitmapButton::OnLButtonDown(nFlags, point);

	CWnd* pMainWnd = ::AfxGetMainWnd();
	if(pMainWnd)
		pMainWnd->SendMessage(ON_LOGO_CLICK);
}


int CLogo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBitmapButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ASSERT(GetParentDockBar());

	EnableToolTips();
		
	return 0;
}

