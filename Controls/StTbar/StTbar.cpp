// StTbar.cpp : implementation file
//
// Author: Marius C. 1998 Dec 14.				 // do not remove this line
//					The code is supplied without any waranty. The code can
//					be changed or modified.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StTbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStTbar

CStTbar::CStTbar(){}
CStTbar::~CStTbar(){}
BEGIN_MESSAGE_MAP(CStTbar, CStatic)
	//{{AFX_MSG_MAP(CStTbar)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CStTbar message handlers
// the static is used as a place holder for miniframe wns
void CStTbar::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();
	
	RECT rt; GetWindowRect(&rt);
	GetParent()->ScreenToClient(&rt);
	// hide the place holder, no not destro it I need it to rerout the messages
	ShowWindow(SW_HIDE);
	// make it on the heap as long  CMyMiniFrm::OnNcDestroy call 'delete this' 
	// save me to map one more message
	m_minifrm = new CMyMiniFrm();
	m_minifrm->Create( AfxRegisterWndClass(0,0,0),
				"",WS_VISIBLE|WS_CHILD,rt,GetParent()/*of placeholder*/);
	if (!m_wndToolBar.Create(m_minifrm) ||
		!m_wndToolBar.LoadToolBar(GetDlgCtrlID()))
	{
		TRACE0("Failed to create toolbar\n");
		return ;      // fail to create
	}
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY|CBRS_BORDER_ANY | CBRS_SIZE_DYNAMIC|0x800|CBRS_GRIPPER);
	// show the window
	m_wndToolBar.ShowWindow(SW_SHOW);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_minifrm->EnableDocking(CBRS_ALIGN_ANY);
	// dock horiz if cx of static is bigger than cy
	if(rt.right-rt.left > rt.bottom-rt.top)
		m_minifrm->DockControlBar(&m_wndToolBar,AFX_IDW_DOCKBAR_TOP);	
	else
		// dock verically
		m_minifrm->DockControlBar(&m_wndToolBar,AFX_IDW_DOCKBAR_RIGHT);
	m_minifrm->RecalcLayout();
}

void CStTbar::OnDestroy() 
{
	// nothing to clean
	CStatic::OnDestroy();
}

void	CStTbar::PreTranslate(MSG* pMsg)
{
	// forward this to the toolbar
	// 
	if(/*pMsg->message >= WM_MOUSEFIRST && 
		pMsg->message <= WM_MOUSELAST &&*/
		m_wndToolBar.IsWindowVisible())
			m_wndToolBar.OnUpdateCmdUI(m_minifrm,TRUE);

}

////////////////////////////////////////////////////////////////////////////
// CMyMiniFrm class implementation

CMyMiniFrm::CMyMiniFrm(){}
CMyMiniFrm::~CMyMiniFrm(){}
BEGIN_MESSAGE_MAP(CMyMiniFrm, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CMyMiniFrm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyMiniFrm message handlers
BOOL CMyMiniFrm::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// pass those up in the dialog to leave the OnUpdateUI mechanism to flow
	BOOL br = GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	// leave the default proc to handles the tooltip updating mechanism
	CMiniFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	return br;	// return what the parent returns
}

