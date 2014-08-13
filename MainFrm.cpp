                                                                                                    // MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ECQPS.h"

#include "MainFrm.h"
#include "ECQPSDoc.h"
#include "ECQPSView.h"

#include "Dialog/dlgservice.h"
#include "Dialog/dlgregboot.h"

#include "Process/NtSystemInfo/SystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
#define ID_TOOL_HIDDENSELECT DROPDOWN(ID_TOOL_HIDDEN)

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_MAINMENU_TOP, OnMainmenuTop)
	ON_UPDATE_COMMAND_UI(ID_MAINMENU_TOP, OnUpdateMainmenuTop)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_INFO_BAR, OnViewInfoBar)
	ON_COMMAND(ID_VIEW_MODULE_BAR, OnViewModuleBar)
	ON_COMMAND(ID_TOOL_REFRESHE, ExeBar_REFRESHE)
	ON_COMMAND(ID_TOOL_KILL, ExeBar_KILL)
	ON_COMMAND(ID_TOOL_NORMAL, ExeBar_NORMAL )
	ON_COMMAND(ID_TOOL_NETWORK, ExeBar_NETWORK )
	ON_COMMAND(ID_TOOL_PERFORMANCE, ExeBar_PERFORMON )
	ON_COMMAND(ID_TOOL_SERVICE, ExeBar_SERVICE )
	ON_COMMAND(ID_TOOL_BOOT, ExeBar_BOOT )
	ON_COMMAND(ID_TOOL_COMPUTEE, ExeBar_COMPUTEE )
	ON_COMMAND(ID_TOOL_CONSOLE, ExeBar_CONSOLE)
	ON_COMMAND(ID_TOOL_REGEDIT, ExeBar_REGEDIT)
	ON_COMMAND(ID_TOOL_NOTPAD, ExeBar_NOTPAD )
	ON_COMMAND(ID_TOOL_HELP, ExeBar_HELP )
	ON_COMMAND(ID_TOOL_HOME, ExeBar_HOME )
	ON_COMMAND(ID_TOOL_ABOUT, ExeBar_ABOUT )
    ON_COMMAND(ID_TOOL_HIDDENSELECT, ExeBar_HIDPROC_SEL)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODULE_BAR, OnUpdateViewModuleBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INFO_BAR, OnUpdateViewInfoBar)
	ON_COMMAND(ID_TOOL_HIDDEN, ExeBar_HIDDENPROC)
	ON_UPDATE_COMMAND_UI(ID_TOOL_HIDDEN, OnUpdateToolHidden)
	ON_COMMAND(ID_MENU_DISABLEDAUTO, ExeBar_DISABLEDAUTO)
	ON_COMMAND(ID_MENU_REFRESHE, ExeBar_REFRESHE)
	ON_COMMAND(ID_MENU_KILL, ExeBar_KILL)
	ON_COMMAND(ID_MENU_NORMAL, ExeBar_NORMAL )
	ON_COMMAND(ID_MENU_NETWORK, ExeBar_NETWORK )
	ON_COMMAND(ID_MENU_PERFORMANCE, ExeBar_PERFORMON )
	ON_COMMAND(ID_MENU_SERVICE, ExeBar_SERVICE )
	ON_COMMAND(ID_MENU_BOOT, ExeBar_BOOT )
	ON_COMMAND(ID_MENU_COMPUTEE, ExeBar_COMPUTEE )
	ON_COMMAND(ID_MENU_CONSOLE, ExeBar_CONSOLE)
	ON_COMMAND(ID_MENU_REGEDIT, ExeBar_REGEDIT)
	ON_COMMAND(ID_MENU_NOTPAD, ExeBar_NOTPAD )
	ON_COMMAND(ID_MENU_HELP, ExeBar_HELP )
	ON_COMMAND(ID_MENU_HOME, ExeBar_HOME )
	ON_COMMAND(ID_MENU_ABOUT, ExeBar_ABOUT )
	ON_MESSAGE(WM_THRADANSWER,HandleThreadMes)  
	ON_UPDATE_COMMAND_UI(ID_MENU_DISABLEDAUTO, OnUpdateMenuDisabledauto)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_UPTIME,
	ID_INDICATOR_NORMAL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():m_Mutex(FALSE,NULL)
{
	// TODO: add member initialization code here
	m_pThreadModule = NULL;
	m_NowViewListType=NOW_NORMAL;
	m_uTimer=0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetMenuStyles();
	//create the toolbar
	SetToolBar(0,0L);
    //_______________________
	//______________________
	//create the statusbar
	SetStatusBar(0,0L);
	//////////-----------------------
	///set infobars
	SetModeleBar(0,0L);
	SetInfoBar(0,0L);

	
	m_uTimer = SetTimer( 1, 5000, NULL );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~FWS_ADDTOTITLE; 
	// 创建一个隐藏窗口
 	if (!::IsWindow(m_wndParent.m_hWnd))
 	{
 		LPCTSTR pstrOwnerClass = AfxRegisterWndClass(0);
		if (!m_wndParent.CreateEx(0, pstrOwnerClass, "", WS_POPUP,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL, 0))
			return FALSE;
 	}

	cs.hwndParent = m_wndParent.m_hWnd;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
    //AfxMessageBox("CMainFrame:Close");
	WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    AfxGetApp()->WriteProfileInt("Frame", "Status", wp.showCmd);
    AfxGetApp()->WriteProfileInt("Frame", "Top",    wp.rcNormalPosition.top);
    AfxGetApp()->WriteProfileInt("Frame", "Left",   wp.rcNormalPosition.left);
    AfxGetApp()->WriteProfileInt("Frame", "Bottom", wp.rcNormalPosition.bottom);
    AfxGetApp()->WriteProfileInt("Frame", "Right",  wp.rcNormalPosition.right);
	KillTimer(m_uTimer);
	CFrameWnd::OnClose();
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CECQPSDoc* pDoc = (CECQPSDoc*)GetActiveDocument();
	CECQPSView* pView = (CECQPSView*)GetActiveView();
	extern CString cWinVer;
	
	switch(m_NowViewListType)
	{
		case NOW_NORMAL:
			pView->UpdateProcessList_Normal("UPDATE");
			break;
		case NOW_HIDDEN:
			pView->UpdateProcessList_HiddenProc(ulHiddenProcType);
			break;
		case NOW_DRIVER:
			pView->UpdateProcessList_HiddenDriver();
			break;
		case NOW_PERFMON:
			pView->UpdateProcessList_Performon("UPDATE");
			break;
		case NOW_NETWORK:
			if(cWinVer.CompareNoCase("Windows2K") ==0 )
				pView->UpdateProcessList_Network2k("INIT");
			else
				pView->UpdateProcessList_NetworkXp("INIT");
			break;
		default:
			pView->UpdateProcessList_Normal("UPDATE");
			break;
	}
	
	CFrameWnd::OnTimer(nIDEvent);
}



void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	if ( m_uTimer )
		KillTimer( m_uTimer );
	//AfxMessageBox("CMainFrame:OnDestroy");
	
}

LRESULT CMainFrame::SetToolBar(WPARAM wParam, LPARAM lParam)
{
	//set 256color bitmap
	//______________________________________________________________
	HBITMAP hbm = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_BITMAP,
		0,0, // cx,cy
		LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS );
	CBitmap bm;
	
	bm.Attach(hbm);
	
	//工具栏大小在rc文件中也要设置
	m_ilToolBar.Create(16,16, ILC_COLOR32, 4, 4);
	m_ilToolBar.Add(&bm,(CBitmap*)NULL);
    //_________________________________________________________________

	
	//Creat the Xp toolbar
	if (!m_wndXPToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		|CBRS_GRIPPER |CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndXPToolBar.LoadToolBar(IDR_MAINFRAME)||
		!m_wndXPToolBar.GetToolBarCtrl().SetImageList(&m_ilToolBar))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
    //m_Database.Create ("管理进程库",WS_CHILD|SS_LEFT, CRect(0,0,50,12), &m_wndXPToolBar, ID_TOOL_HIDDEN);
	//m_wndXPToolBar.InsertControl (21, m_Database);

	/////////注意与状态栏的冲突
	//m_wndXPToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndXPToolBar);
    // Initialization of the ListCtrl
	//弹出窗口的消息处理在MyListCtrl.cpp文件中，即放在控件中进行处理void CMyListCtrl::XPopupCtrlEvent::OnHide (bool bAbort)
    m_wndXPToolBar.SetButtonStyle (m_wndXPToolBar.CommandToIndex (ID_TOOL_HIDDEN), m_wndXPToolBar.GetButtonStyle(m_wndXPToolBar.CommandToIndex (ID_TOOL_HIDDEN))|TBSTYLE_DROPDOWN);
    m_treePopup.Create (WS_CHILD|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_DISABLEDRAGDROP, CRect (0, 0, 150, 100), this, 1);
    HTREEITEM hRoot = m_treePopup.InsertItem (_T("检测隐藏进程")), h1 = hRoot,hitem;
    hitem = m_treePopup.InsertItem (_T("追踪内核动态列表"), hRoot);
	m_treePopup.SetItemData(hitem,0x1);
    hitem = m_treePopup.InsertItem (_T("追踪句柄动态列表"), hRoot);
	m_treePopup.SetItemData(hitem,0x2);
    m_treePopup.Expand (hRoot, TVE_EXPAND);
    m_treePopup.SelectItem (h1);
    hRoot = m_treePopup.InsertItem (_T("检测Rootkit"));
    hitem = m_treePopup.InsertItem (_T("追踪当前加载驱动"), hRoot);
 	m_treePopup.SetItemData(hitem,0x8);
    m_treePopup.Expand (hRoot, TVE_EXPAND);

	
	/*    m_wndXPToolBar.SetButtonStyle (m_wndXPToolBar.CommandToIndex (ID_TOOL_HIDDEN), m_wndXPToolBar.GetButtonStyle(m_wndXPToolBar.CommandToIndex (ID_TOOL_HIDDEN))|TBSTYLE_DROPDOWN);
    m_lstPopup.Create (WS_CHILD|LVS_ICON|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_AUTOARRANGE|LVS_ALIGNTOP, CRect (0, 0, 195, 125), this, 2);
    m_lstPopup.SetIconSpacing (127, 20);
    m_lstPopup.SetExtendedStyle (LVS_EX_TRACKSELECT);

    m_lstPopup.InsertItem (0, _T("追踪内核动态列表检测隐藏进程"));
    m_lstPopup.InsertItem (1, _T("追踪句柄动态列表检测隐藏进程"));
	*/
	
	EnableToolTips(TRUE);	

	return TRUE;
}

BOOL CMainFrame::SetMenuStyles()
{
	//初始化置菜单为XP风格显示
	BCMenu::SetMenuDrawMode(BCMENU_DRAWMODE_XP);
	m_menu.LoadMenu(IDR_MAINMENU);
	static UINT uToolBars[] = { IDR_MENULBAR, 
		                      };
	m_menu.LoadToolbars( uToolBars, sizeof(uToolBars)/sizeof(UINT) );
	OnUpdateFrameMenu(m_menu.m_hMenu );

	return TRUE;

}

LRESULT CMainFrame::SetStatusBar(WPARAM wParam, LPARAM lParam)
{
	
	//create the status bar
	if (!m_wndXPStatusBar.Create(this) ||
		!m_wndXPStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	return TRUE;
}

//SetModeleBar
LRESULT CMainFrame::SetModeleBar(WPARAM wParam, LPARAM lParam)
{
	if (!m_wndModuleBar.Create(_T("动态模块"), this, 
		CSize(230,150),true,MODULE_BAR))
	{
		TRACE0("Failed to create infobar\n");
		return -1;
	}
	
	//设置Tab框
	m_TabCtrlModule.Create(TCS_DOWN|WS_CHILD|WS_VISIBLE,
		CRect(0,0,100,100),&m_wndModuleBar,1050);
		
	if(!m_wndModuleList.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL,
				  CRect(0,0,0,0),&m_TabCtrlModule,3010))
		return -1;
	
	//设置listctrl可以整行选择和网格条纹
	m_wndModuleList.SetExtendedStyle(m_wndModuleList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	FlatSB_EnableScrollBar(m_wndModuleList,SB_BOTH,ESB_ENABLE_BOTH);

	if(!m_wndSuspiconList.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL,
				  CRect(0,0,0,0),&m_TabCtrlModule,3010))
		return -1;
	//设置listctrl可以整行选择和网格条纹
	m_wndSuspiconList.SetExtendedStyle(m_wndSuspiconList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	FlatSB_EnableScrollBar(m_wndSuspiconList,SB_BOTH,ESB_ENABLE_BOTH);

	m_TabCtrlModule.AddPage(&m_wndSuspiconList,"可疑模块",IDI_ICON_SUSPICON);
	m_TabCtrlModule.AddPage(&m_wndModuleList,"全部模块",IDI_ICON_ALLMODULE);
	m_wndModuleList.ShowWindow(SW_HIDE);
	m_TabCtrlModule.UpdateWindow();
	
	int aWidths[] = { 150, 140, 140, 120, 60,400 };
	CString strHeading;	
	for ( int iCol = 0; iCol < M_COLUMNS; iCol++) 
	{
		strHeading.LoadString( IDS_MODULENAME + iCol );
		m_wndModuleList.InsertColumn( iCol, strHeading, LVCFMT_LEFT,
				aWidths[iCol], iCol );
		///可疑模块的列表
		m_wndSuspiconList.InsertColumn( iCol, strHeading, LVCFMT_LEFT,
				aWidths[iCol], iCol );
	}
	
	m_wndModuleBar.SetBarStyle(m_wndModuleBar.GetBarStyle() |
		CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	
	m_wndModuleBar.EnableDocking(GetBarDockStyle(FOR_INFOBAR));
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndModuleBar, AFX_IDW_DOCKBAR_BOTTOM);

	return TRUE;
}

//SetInfoBar
LRESULT CMainFrame::SetInfoBar(WPARAM wParam, LPARAM lParam)
{
	if (!m_wndInfoBar.Create(_T("属性窗口"), this, 
		CSize(230,100),true,INFO_BAR))
	{
		TRACE0("Failed to create infobar\n");
		return -1;
	}
	//设置Tab框
	m_TabCtrlInfo.Create(TCS_DOWN|WS_CHILD|WS_VISIBLE,
		CRect(0,0,100,100),&m_wndInfoBar,1020);
	
	//属性框
	if(!m_wndInfoList.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT,
		CRect(0,0,0,0),&m_TabCtrlInfo,1012))
		return -1;
	//设置listctrl可以整行选择和网格条纹
	m_wndInfoList.SetExtendedStyle(m_wndInfoList.GetExtendedStyle()
		|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	FlatSB_EnableScrollBar(m_wndInfoList,SB_BOTH,ESB_ENABLE_BOTH);
	m_wndInfoList.InsertColumn( 0, "属性", LVCFMT_LEFT,60, 0 );
	m_wndInfoList.InsertColumn( 1, "值", LVCFMT_LEFT,300, 1 );
	
	
	if(!m_wndDLLList.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT,
		CRect(0,0,0,0),&m_TabCtrlInfo,1014))
		return -1;
	//设置listctrl可以整行选择和网格条纹
	m_wndDLLList.SetExtendedStyle(m_wndDLLList.GetExtendedStyle()
		|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	m_wndDLLList.SetExtendedStyle(m_wndDLLList.GetExtendedStyle()|LVS_EX_FLATSB);
	FlatSB_EnableScrollBar(m_wndDLLList,SB_BOTH,ESB_ENABLE_BOTH);	
	m_wndDLLList.InsertColumn( 0, "DLL库", LVCFMT_LEFT,60, 0 );
	m_wndDLLList.InsertColumn( 1, "调用函数", LVCFMT_LEFT,300, 1 );

	
	if(!m_wndPEList.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_READONLY,
		CRect(0,0,0,0),&m_TabCtrlInfo,1016))
		return -1;
	m_wndPEList.setFont( 9 );
	//m_wndPEList.bkColor( RGB(255,255,255) );
	m_TabCtrlInfo.AddPage(&m_wndInfoList,"属性",IDI_ICON_PROPERT);
	m_TabCtrlInfo.AddPage(&m_wndDLLList,"导入库",IDI_ICON_LOCKED);
	m_TabCtrlInfo.AddPage(&m_wndPEList,"PE分析",IDI_ICON_PEINFO);
	m_wndDLLList.ShowWindow(SW_HIDE);
	m_wndPEList.ShowWindow(SW_HIDE);
	m_TabCtrlInfo.UpdateWindow();
	/////
	m_wndInfoBar.SetBarStyle(m_wndInfoBar.GetBarStyle() |
		CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	
	m_wndInfoBar.EnableDocking(GetBarDockStyle(FOR_INFOBAR));
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndInfoBar, AFX_IDW_DOCKBAR_RIGHT);

	return TRUE;
}

void CMainFrame::OnUpdateMainmenuTop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(GetExStyle() & WS_EX_TOPMOST);		
}

void CMainFrame::OnUpdateViewModuleBar(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndModuleBar.IsVisible());		
}

void CMainFrame::OnUpdateViewInfoBar(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndModuleBar.IsVisible());		
}
void CMainFrame::OnViewInfoBar() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndInfoBar, !m_wndInfoBar.IsVisible(), FALSE);
	
}

void CMainFrame::OnViewModuleBar() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndModuleBar, !m_wndModuleBar.IsVisible(), FALSE);
	
}

BOOL CMainFrame::StartModuleThread(DWORD dwPID)
{
	if(dwPID >0 ){
		m_dwPID = dwPID;
		m_pThreadModule=AfxBeginThread(Thread_GetModuleList,this);
	}
	else{
		m_wndModuleList.DeleteAllItems();
	}

    return (TRUE); 

}
UINT Thread_GetModuleList(LPVOID param)
{
    CMainFrame *pMainWnd=(CMainFrame *)AfxGetApp()->m_pMainWnd;

	BOOL          bRet        = FALSE; 
    HANDLE        hModuleSnap = NULL; 
    MODULEENTRY32 me32        = {0}; 

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pMainWnd->m_dwPID); 
    if (hModuleSnap == INVALID_HANDLE_VALUE) 
       return (FALSE); 

    // Fill the size of the structure before using it. 
	SendMessage(pMainWnd->m_hWnd ,WM_THRADANSWER, (WPARAM)TR_INIT, (LPARAM)TR_INIT);	

    me32.dwSize = sizeof(MODULEENTRY32); 
    // Walk the module list of the process, and find the module of 
    // interest. Then copy the information to the buffer pointed 
    // to by lpMe32 so that it can be returned to the caller. 

	CSingleLock SingleLock(&pMainWnd->m_Mutex);
	SingleLock.Lock();

    if (Module32First(hModuleSnap, &me32)) 
    { 
        do 
        { 
			if( me32.th32ProcessID == pMainWnd->m_dwPID ) {
	
				CString crArray[5];
				crArray[0]=me32.szModule;
				crArray[1]=me32.dwSize;
				crArray[2]=me32.GlblcntUsage;
				crArray[3]=me32.ProccntUsage;
				crArray[4]=me32.szExePath;
								
				SendMessage(pMainWnd->m_hWnd,WM_THRADANSWER, (WPARAM)TR_EXECUTE, (LPARAM)crArray );	

			}
        } 
        while (Module32Next(hModuleSnap, &me32)); 
 
        bRet = TRUE;   // if this sets bRet to FALSE, dwModuleID 
                         // no longer exists in specified process 
    } 
    else 
        bRet = FALSE;           // could not walk module list 
 
    // Do not forget to clean up the snapshot object. 
	SendMessage(pMainWnd->m_hWnd,WM_THRADANSWER, (WPARAM)TR_END, (LPARAM)TR_END);	

    CloseHandle (hModuleSnap); 
    
	return (bRet); 

}

LRESULT CMainFrame::HandleThreadMes(WPARAM wParam, CString lParam[])
{

	switch(wParam){
		
		case TR_INIT:
			SetInfoContent("Properties");
			m_wndModuleList.DeleteAllItems();
			m_wndSuspiconList.DeleteAllItems();
			break;
		
		case TR_EXECUTE:
			SetModuleContent( lParam );
			break;
		
		case TR_END:

			break;

		default:
			break;
		
	}
	
	return 1;
}

BOOL CMainFrame::SetModuleContent(CString lParam[])
{
	CString szTmp,szSize,szVersion;
	//TRACE("%s,%s\n",lParam[0]);
	m_wndModuleList.InsertItem(0,lParam[0]);
	m_wndModuleList.SetItemText(0, 5, lParam[4]);

	CFileVersionInfo m_FileInfo;
	
	m_FileInfo.ReadVersionInfo(lParam[4]);

	if (!m_FileInfo.IsValid()){
		m_wndModuleList.SetItemText(0, 1, NULL);
		m_wndModuleList.SetItemText(0, 2, NULL);
		m_wndModuleList.SetItemText(0, 3, NULL);
		m_wndModuleList.SetItemText(0, 4, NULL);
	}	
	else
	{
		szVersion.Format(_T("%s/%s"),
			m_FileInfo.GetFileVersionString(),
			m_FileInfo.GetProductVersionString()
			);
		//获取文件属性信息
		//声明结构对象
		WIN32_FIND_DATA FindFileData;
		SYSTEMTIME stUTC, stLocal;
		CString ftCreate, ftAccess, ftWrite;
		FindClose(FindFirstFile(lParam[4],&FindFileData));
		
		FileTimeToSystemTime(&FindFileData.ftCreationTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		wsprintf(ftCreate.GetBuffer(100), TEXT("%02d-%02d-%02d  %02d:%02d"),
			stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
			stLocal.wHour, stLocal.wMinute);
		
		FileTimeToSystemTime(&FindFileData.ftLastAccessTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		wsprintf(ftAccess.GetBuffer(100), TEXT("%02d-%02d-%02d  %02d:%02d"),
			stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
			stLocal.wHour, stLocal.wMinute);
		
		FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
		wsprintf(ftWrite.GetBuffer(100), TEXT("%02d-%02d-%02d  %02d:%02d"),
			stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
			stLocal.wHour, stLocal.wMinute);
		
		
		szSize.Format("%dB",(FindFileData.nFileSizeHigh<<16)+FindFileData.nFileSizeLow);
		
		m_wndModuleList.SetItemText(0, 1, szVersion);
		m_wndModuleList.SetItemText(0, 2, m_FileInfo.GetVersionInfo(0));
		m_wndModuleList.SetItemText(0, 3, ftCreate);
		m_wndModuleList.SetItemText(0, 4, szSize);

		if( m_FileInfo.GetVersionInfo(0).CollateNoCase("Microsoft Corporation") !=0 ){
			m_wndSuspiconList.InsertItem(0,lParam[0]);
			m_wndSuspiconList.SetItemText(0, 5, lParam[4]);
			m_wndSuspiconList.SetItemText(0, 1, szVersion);
			m_wndSuspiconList.SetItemText(0, 2, m_FileInfo.GetVersionInfo(0));
			m_wndSuspiconList.SetItemText(0, 3, ftCreate);
			m_wndSuspiconList.SetItemText(0, 4, szSize);
		}

	}
	
	return TRUE;
}

BOOL CMainFrame::SetInfoContent(CString cType)
{
	// TODO: Add your command handler code here
	CString szPID,szPath,szFile,szTmp,szSize,szDebug,szCharacter;
	CString szLanguage,szTargetOS,szVersion,szFileType,szIntro;

	CMainFrame* pMainWnd=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	CECQPSView* pView = (CECQPSView *)pMainWnd->GetActiveView();
	CListCtrl& listView = pView->GetListCtrl();

	szPID.Format("%d",m_dwPID);
	TRACE("GetListItemFromClm:%d\n",GetListItemFromClm(listView,szPID));
	szPath=listView.GetItemText(GetListItemFromClm(listView,szPID),6);

	//清空列表信息
	m_wndInfoList.DeleteAllItems();
	m_wndDLLList.DeleteAllItems();
	m_wndPEList.SetWindowText(NULL);

	TRACE("szPath:%s\n",szPath);

	CFileVersionInfo m_FileInfo;
	
	m_FileInfo.ReadVersionInfo(szPath);

	if (!m_FileInfo.IsValid()){
		//AfxMessageBox("提示：文件不支持版本查询!");
		return FALSE;
	}

	if(m_FileInfo.IsDebugVersion())
		szDebug="Debug";
	if(m_FileInfo.IsPatched())
		szDebug="Patched";
	if(m_FileInfo.IsPreRelease())
		szDebug="Release";
	if(m_FileInfo.IsPrivateBuild())
		szDebug="PrivateBuild";
	if(m_FileInfo.IsSpecialBuild())
		szDebug="SpecialBuild";
	else
		szDebug="Release";
	
	szFileType.Format(_T("%s (0x%X)"), 
		m_FileInfo.GetFileType(2), 
		m_FileInfo.GetFileType()
		);
	
	szLanguage.Format(_T("%s (0x%X)"), 
		m_FileInfo.GetLanguageName(),
		m_FileInfo.GetLanguageId()
		);
	
	szTargetOS.Format(_T("%s (0x%X)"), 
							 m_FileInfo.GetTargetOs(3), 
							 m_FileInfo.GetTargetOs()
							 );
	
	szVersion.Format(_T("%s/%s"),
		m_FileInfo.GetFileVersionString(),
		m_FileInfo.GetProductVersionString()
		);
	
	
	//获取文件属性信息
	//声明结构对象
	WIN32_FIND_DATA FindFileData;
    SYSTEMTIME stUTC, stLocal;
	CString ftCreate, ftAccess, ftWrite;
	FindClose(FindFirstFile(szPath,&FindFileData));
	
    FileTimeToSystemTime(&FindFileData.ftCreationTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
    wsprintf(ftCreate.GetBuffer(100), TEXT("%02d-%02d-%02d  %02d:%02d"),
        stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
        stLocal.wHour, stLocal.wMinute);

    FileTimeToSystemTime(&FindFileData.ftLastAccessTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
    wsprintf(ftAccess.GetBuffer(100), TEXT("%02d-%02d-%02d  %02d:%02d"),
        stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
        stLocal.wHour, stLocal.wMinute);
 
	FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
    wsprintf(ftWrite.GetBuffer(100), TEXT("%02d-%02d-%02d  %02d:%02d"),
        stLocal.wYear, stLocal.wMonth, stLocal.wDay, 
        stLocal.wHour, stLocal.wMinute);
	
	DWORD dwSize=(FindFileData.nFileSizeHigh<<16)+FindFileData.nFileSizeLow;
	szSize.Format("%dB,%dK,%dM",dwSize,dwSize/1024,dwSize/1024/1024);

	
	szCharacter.Format(_T("0x%X%s"), 
							m_FileInfo.GetCharSet(), 
							m_FileInfo.GetCharSetName()
						   );

	//扩展的PE信息
	CStringArray strRelatedDll;
	CString  strPEInfo;
	///分配大小，防止益出
	//strRelatedDll.SetSize(1000,-1);

	strPEInfo=DetectPeInfo(szPath.GetBuffer(126),strRelatedDll);


	///插入基本属性框
	m_wndInfoList.InsertItem(0,"厂商信息");
	m_wndInfoList.InsertItem(1,"版本号码");
	m_wndInfoList.InsertItem(2,"文件大小");
	m_wndInfoList.InsertItem(3,"创建时间");
	m_wndInfoList.InsertItem(4,"修改时间");
	m_wndInfoList.InsertItem(5,"访问时间");
	m_wndInfoList.InsertItem(6,"字符集合");
	m_wndInfoList.InsertItem(7,"语言版本");
	m_wndInfoList.InsertItem(8,"内部名称");
	m_wndInfoList.InsertItem(9,"发行版本");
	m_wndInfoList.InsertItem(10,"合法版权");

	m_wndInfoList.SetItemText(0,1,m_FileInfo.GetVersionInfo(0));
	m_wndInfoList.SetItemText(1,1,szVersion);
	m_wndInfoList.SetItemText(2,1,szSize);
	m_wndInfoList.SetItemText(3,1,ftCreate);
	m_wndInfoList.SetItemText(4,1,ftWrite);
	m_wndInfoList.SetItemText(5,1,ftAccess);
	m_wndInfoList.SetItemText(6,1,szCharacter);
	m_wndInfoList.SetItemText(7,1,szLanguage);
	m_wndInfoList.SetItemText(8,1,m_FileInfo.GetVersionInfo(3));
	m_wndInfoList.SetItemText(9,1,szDebug);
	m_wndInfoList.SetItemText(10,1,m_FileInfo.GetVersionInfo(4));
	
	///插入静态连接框
	for (int n = 0; n < strRelatedDll.GetSize(); n++)
	{
		CString szDllName,szDllFunc;
		szDllFunc=strRelatedDll[n];
		szDllName=szDllFunc.Left( szDllFunc.Find("&") );
		szDllFunc.Replace(szDllName+"&","");
		
		m_wndDLLList.InsertItem(0,szDllName);
		m_wndDLLList.SetItemText(0,1,szDllFunc);
	}
	///插入PE分析信息
	m_wndPEList.SetWindowText(strPEInfo);
	return TRUE;
}



UINT CMainFrame::GetListItemFromClm(CListCtrl& listView ,CString szText)
{
	//CMainFrame* pMainWnd=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	//CECQPSView* pView = (CECQPSView *)pMainWnd->GetActiveView();
	//CListCtrl& listView = pView->GetListCtrl();
	///PID = 1
	UINT bRen=0; 
	for(UINT nItem=0;nItem<listView.GetItemCount();nItem++)
		if(szText.CompareNoCase( listView.GetItemText(nItem,PROCESS_PID) ) ==0 )
			return nItem;
	return bRen;
}

CString CMainFrame::DetectPeInfo(char *lpFileName, CStringArray &cRelatedDll)
{
	HANDLE hFile;						//handle for the file detecting 
	HANDLE hMapping;					//handle for the mapping file detecting
	void *pBasePointer;					//pointer for the mapping file begin
	IMAGE_DOS_HEADER *imDos_Headers;	//定义DOS头
	IMAGE_NT_HEADERS *imNT_Headers;		//定义PE头
	IMAGE_SECTION_HEADER *imSECTION_Headers; //定义SECTION表头
	IMAGE_IMPORT_DESCRIPTOR  *imImport_Descript;									
	char CFileString[20000]="";							//保存记录
	CString szBlank="    ";

	hFile=CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,0);		//Create the File handle
	if (hFile==INVALID_HANDLE_VALUE){	//test File Handle
		GetLastError(); 
		return "非PE文件格式"; 
	} 
	if (!(hMapping=CreateFileMapping(hFile,0,PAGE_READONLY|SEC_COMMIT,0,0,0)))
	{									//Create the File Map and test
		CloseHandle(hFile);
		CloseHandle(hMapping);
		return "非PE文件格式";
	}
	if (!(pBasePointer=::MapViewOfFile(hMapping,FILE_MAP_READ,0,0,0)))
	{									
		CloseHandle(hFile);
		CloseHandle(hMapping);
		return "非PE文件格式";
	}
	imDos_Headers=(IMAGE_DOS_HEADER *)pBasePointer;	//设置初始指针地址
	if(!(imDos_Headers->e_magic ==IMAGE_DOS_SIGNATURE))
	{
		CloseHandle(hFile);
		CloseHandle(hMapping);
		return "非PE文件格式";
	}
	imNT_Headers=(IMAGE_NT_HEADERS *)((char *)pBasePointer+imDos_Headers->e_lfanew);//NT头指针地址

	if( imNT_Headers->Signature != IMAGE_NT_SIGNATURE )//判断是否是PE格式文件
	{
		CloseHandle(hFile);
		CloseHandle(hMapping);
		strcat(CFileString,"文件格式：");
		switch ((unsigned short)imNT_Headers->Signature)
		{
		case IMAGE_DOS_SIGNATURE:
			strcat(CFileString,"MS-DOS signature");
			break;
		case IMAGE_OS2_SIGNATURE:
			strcat(CFileString,"Win16 or OS/2 signature");
			break;
		case IMAGE_OS2_SIGNATURE_LE:
			strcat(CFileString,"Win16, OS/2 or VxD signature");
			break;
		default:
			strcat(CFileString,"unknown signature, probably MS-DOS");
			break;
		}
		return "非PE文件格式";
	}

	char buffer[1000]="";

	wsprintf(buffer,"%ld",(long)((char *)imNT_Headers-(char *)imDos_Headers));//long类型转换char
	strcat(CFileString," DOS Stud:\t");
	strcat(CFileString,buffer);
	strcat(CFileString,"\r\n 支持CPU:\t");

	switch (imNT_Headers->FileHeader .Machine )			//判断文件对CPU的运行要求
	{
	case IMAGE_FILE_MACHINE_I386:
		strcat(CFileString,"Intel 80386\r\n");
		break;
	case 0x014d:
		strcat(CFileString,"Intel 80486\r\n");
		break;
	case 0x014e:
		strcat(CFileString,"Intel Pentium\r\n");
		break;
	case 0x0160:
		strcat(CFileString,"R3000 (MIPS), big endian\r\n");
		break;
	case IMAGE_FILE_MACHINE_R3000:
		strcat(CFileString,"R3000 (MIPS), little endian\r\n");
		break;
	case IMAGE_FILE_MACHINE_R4000:
		strcat(CFileString,"R4000 (MIPS), little endian\r\n");
		break;
	case IMAGE_FILE_MACHINE_R10000:
		strcat(CFileString,"R10000 (MIPS), little endian\r\n");
		break;
	case IMAGE_FILE_MACHINE_ALPHA:
		strcat(CFileString,"DEC Alpha_AXP\r\n");
		break;
	case IMAGE_FILE_MACHINE_POWERPC:
		strcat(CFileString,"Power PC\r\n");
		break;
	default:
		strcat(CFileString,"Unknown Processor: %04x\r\n");
		break;
	}

	wsprintf(buffer," PE节数目:\t%d\r\n",imNT_Headers->FileHeader .NumberOfSections );
	strcat(CFileString,buffer);
	//wsprintf(buffer,"文件创建的时间：%s\r\n",asctime(gmtime((const time_t *)&imNT_Headers->FileHeader .TimeDateStamp)));
	//strcat(CFileString,buffer);
	wsprintf(buffer," 选项Size:\t%d\r\n",imNT_Headers->FileHeader .SizeOfOptionalHeader );
	strcat(CFileString,buffer);
	wsprintf(buffer," 文件Flag:\t%d\r\n",imNT_Headers->FileHeader .Characteristics);
	strcat(CFileString,buffer);
	wsprintf(buffer," 标志字位:\t%d\r\n",imNT_Headers->OptionalHeader .Magic );
	strcat(CFileString,buffer);
	wsprintf(buffer," Link版本:\t%d.%d\r\n",imNT_Headers->OptionalHeader .MajorLinkerVersion ,imNT_Headers->OptionalHeader .MinorLinkerVersion );
	strcat(CFileString,buffer);
	wsprintf(buffer," 代码段长度:\t%d\r\n",imNT_Headers->OptionalHeader .SizeOfCode );
	strcat(CFileString,buffer);
	wsprintf(buffer," Init数据块大小:\t%d\r\n",imNT_Headers->OptionalHeader .SizeOfInitializedData );
	strcat(CFileString,buffer);
	wsprintf(buffer," UnInit数据块大小:\t%d\r\n",imNT_Headers->OptionalHeader .SizeOfUninitializedData );
	strcat(CFileString,buffer);
	wsprintf(buffer," 程序段起始RVA:\t%d\r\n",imNT_Headers->OptionalHeader .AddressOfEntryPoint );
	strcat(CFileString,buffer);
	wsprintf(buffer," 代码段起始RVA:\t%d\r\n",imNT_Headers->OptionalHeader .BaseOfCode );
	strcat(CFileString,buffer);
	wsprintf(buffer," 数据段起始RVA:\t%d\r\n",imNT_Headers->OptionalHeader .BaseOfData );
	strcat(CFileString,buffer);
	wsprintf(buffer," 内存装载地址:\t%d\r\n",imNT_Headers->OptionalHeader .ImageBase );
	strcat(CFileString,buffer);
	wsprintf(buffer," 内存节对齐粒度:\t%d\r\n",imNT_Headers->OptionalHeader .SectionAlignment );
	strcat(CFileString,buffer);
	wsprintf(buffer," 文件节对齐粒度:\t%d\r\n",imNT_Headers->OptionalHeader .FileAlignment );
	strcat(CFileString,buffer);
	wsprintf(buffer," 系统定义版本号:\t%d.%d\r\n",imNT_Headers->OptionalHeader .MajorOperatingSystemVersion ,imNT_Headers->OptionalHeader .MinorOperatingSystemVersion );
	strcat(CFileString,buffer);
	wsprintf(buffer," 自定义版本号:\t%d.%d\r\n",imNT_Headers->OptionalHeader .MajorImageVersion  ,imNT_Headers->OptionalHeader .MinorImageVersion );
	strcat(CFileString,buffer);
	wsprintf(buffer," 子系统版本号:\t%d.%d\r\n",imNT_Headers->OptionalHeader .MajorSubsystemVersion   ,imNT_Headers->OptionalHeader .MinorSubsystemVersion );
	strcat(CFileString,buffer);
	wsprintf(buffer," 内存PE映像:\t%d\r\n",imNT_Headers->OptionalHeader .SizeOfImage );
	strcat(CFileString,buffer);
	wsprintf(buffer," 头+节表大小:\t%d\r\n",imNT_Headers->OptionalHeader .SizeOfHeaders );
	strcat(CFileString,buffer);
	wsprintf(buffer," CheckSum Sum:\t%d\r\n",imNT_Headers->OptionalHeader .CheckSum );
	strcat(CFileString,buffer);

    switch (imNT_Headers->OptionalHeader .Subsystem )
	{													//对要求子系统进行判断
	case IMAGE_SUBSYSTEM_NATIVE:
		wsprintf(buffer,"uses no subsystem");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_GUI:
		wsprintf(buffer,"uses Win32 graphical subsystem");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_CUI:
		wsprintf(buffer,"uses Win32 console subsystem");
		break;
	case IMAGE_SUBSYSTEM_OS2_CUI:
		wsprintf(buffer,"uses OS/2 console subsystem");
		break;
	case IMAGE_SUBSYSTEM_POSIX_CUI:
		wsprintf(buffer,"uses Posix console subsystem");
		break;
	default:
		wsprintf(buffer,"uses unknown subsystem");
		break;
	}
	strcat(CFileString,"\r\n");//以下是对节表进行操作
	int i,directory; ///////////////////////////////////
	CString strDllName;
	for(i=0,imSECTION_Headers =(IMAGE_SECTION_HEADER *)((char *)imNT_Headers+sizeof(IMAGE_NT_HEADERS));i<imNT_Headers->FileHeader .NumberOfSections;i++,imSECTION_Headers++)
	{
		///防止益出CFileString定义的长度
		if(strlen(CFileString) >18000 )
						return CFileString;

		//////////////////////////////////节头分析
		wsprintf(buffer,"节表序号%d    %s\r\n",i+1,imSECTION_Headers->Name);
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sVirtualAddress:%d\r\n",szBlank,imSECTION_Headers->VirtualAddress );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sSizeOfRawData:%d\r\n",szBlank,imSECTION_Headers->SizeOfRawData );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sPointerToRawData:%d\r\n",szBlank,imSECTION_Headers->PointerToRawData );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sPointerToRelocations:%d\r\n",szBlank,imSECTION_Headers->PointerToRelocations );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sPointerToLinenumbers:%d\r\n",szBlank,imSECTION_Headers->PointerToLinenumbers );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sNumberOfRelocations:%d\r\n",szBlank,imSECTION_Headers->NumberOfRelocations );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sNumberOfLinenumbers:%d\r\n",szBlank,imSECTION_Headers->NumberOfLinenumbers );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sPhysicalAddress:%d\r\n",szBlank,imSECTION_Headers->Misc .PhysicalAddress );
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sVirtualSize:%d\r\n",szBlank,imSECTION_Headers->Misc .VirtualSize);
			strcat(CFileString,buffer);
		wsprintf(buffer,"%sCharacteristics:%d\r\n",szBlank,imSECTION_Headers->Characteristics );
			strcat(CFileString,buffer);
		/////////////////////////////////////

/***********************************************************/
#define isin(address,start,length) ((char*)(address)>=(char*)(start) && (char*)(address)<(char*)(start)+(length))		

		for(directory = 0; directory < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; directory++)
		{
			if(imNT_Headers->OptionalHeader .DataDirectory[directory].VirtualAddress && isin(imNT_Headers->OptionalHeader .DataDirectory[directory].VirtualAddress,imSECTION_Headers->VirtualAddress,imSECTION_Headers->SizeOfRawData))
			{
				if(directory==IMAGE_DIRECTORY_ENTRY_IMPORT)
				{									//进入引入表

#define adr(rva) (void *)((char *)((char *)pBasePointer+imSECTION_Headers->PointerToRawData)+((DWORD)(rva)-imSECTION_Headers->VirtualAddress))
					
					imImport_Descript=(IMAGE_IMPORT_DESCRIPTOR  *) adr(imNT_Headers->OptionalHeader .DataDirectory [directory].VirtualAddress);
					for(int o=0; !IsBadReadPtr(imImport_Descript,sizeof(*imImport_Descript)) && imImport_Descript->Name ;imImport_Descript++,o++)
					{								//引入表的引入文件名
						char *p1=(char *)adr(imImport_Descript->Name);
						
						strDllName=(char *)adr(imImport_Descript->Name);		//把DLL文件保存在结构指针中
						//cRelatedDll.Add(strDllName);
						//wsprintf(buffer,"引入文件名 : %s\r\n",(char *)adr(imImport_Descript->Name));
						//strcat(CFileString,buffer);
						
						IMAGE_THUNK_DATA *imThunk_data;
						IMAGE_IMPORT_BY_NAME *imImport_Name;
						
						if(imImport_Descript->OriginalFirstThunk)	//两种情况，1、为0时，2、为1时
							imThunk_data=(IMAGE_THUNK_DATA *)adr(imImport_Descript->OriginalFirstThunk);
						else
							imThunk_data=(IMAGE_THUNK_DATA *)adr(imImport_Descript->FirstThunk);
						
						for(int b=0,nVarName=0; imThunk_data->u1 .Ordinal;b++,imThunk_data++,nVarName++)
						{							//引入表的引入函数名					
							if(IMAGE_SNAP_BY_ORDINAL(imThunk_data->u1.Ordinal))//引入函数时分两种情况
							{
								//wsprintf(buffer,"序数引入：%d\r\n",IMAGE_ORDINAL(imThunk_data->u1 .Ordinal ));
								//strcat(CFileString,buffer);

								itoa(IMAGE_ORDINAL(imThunk_data->u1 .Ordinal),buffer,5);

								//	sDllName->varname[nVarName] =buffer;
								//VarName[nDllName][nVarName]=buffer;

							}
							else
							{
								imImport_Name=(IMAGE_IMPORT_BY_NAME *) adr(imThunk_data->u1.AddressOfData);
								//wsprintf(buffer,"引入函数:%s\r\n",(char *)imImport_Name->Name);
								//strcat(CFileString,buffer);
								if(cRelatedDll.GetSize() > 800 )
									return CFileString;
								cRelatedDll.Add(strDllName+"&"+(char *)(imImport_Name->Name));
								
								//VarName[nDllName][nVarName]=(char *)imImport_Name->Name;
							}
						}
					}

				}
#undef adr
					
			}
#undef isin
		
		}
	}
	strcat(CFileString,"\0");
	CloseHandle(hFile);
	CloseHandle(hMapping);

	return CFileString;
}

void CMainFrame::ExeBar_REFRESHE()
{
	CECQPSDoc* pDoc = (CECQPSDoc*)GetActiveDocument();
	CECQPSView* pView = (CECQPSView*)GetActiveView();
	extern CString cWinVer;
	
	switch(m_NowViewListType)
	{
		case NOW_NORMAL:
			pView->UpdateProcessList_Normal("INIT");
			break;
		case NOW_HIDDEN:
			pView->UpdateProcessList_HiddenProc(ulHiddenProcType);
			break;
		case NOW_DRIVER:
			pView->UpdateProcessList_HiddenDriver();
			break;
		case NOW_PERFMON:
			pView->UpdateProcessList_Performon("INIT");
			break;
		case NOW_NETWORK:
			if(cWinVer.CompareNoCase("Windows2K") ==0 )
				pView->UpdateProcessList_Network2k("INIT");
			else
				pView->UpdateProcessList_NetworkXp("INIT");
			break;
		default:
			pView->UpdateProcessList_Normal("INIT");
			break;
	}
	
}
void CMainFrame::ExeBar_KILL()
{
	CECQPSView* pView = (CECQPSView*)GetActiveView();
	pView->OnPopmenuAutokill();

}
void CMainFrame::ExeBar_NORMAL()
{
	m_NowViewListType=NOW_NORMAL;
	BeginWaitCursor(); // display the hourglass cursor
	
	CECQPSView* pView = (CECQPSView *)GetActiveView();
	CListCtrl& listView = pView->GetListCtrl();
	listView.DeleteAllItems();

	LV_COLUMN lvcolumn;
	CString strHeading;
	for(int i=2;i<=5;i++)
	{
		strHeading.LoadString( IDS_PROCESSWARN + i-2 );
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT 
						| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = strHeading.GetBuffer(256);
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		lvcolumn.cx = 80;
		listView.SetColumn(i, &lvcolumn);
	}

	pView->UpdateProcessList_Normal("INIT");
}

void CMainFrame::ExeBar_NETWORK ()
{
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("Windows98") ==0 )
	{
		AfxMessageBox("本功能不支持Windows Me/98 !");
		return;
	}	

	m_NowViewListType=NOW_NETWORK;
	BeginWaitCursor(); // display the hourglass cursor

	CECQPSView* pView = (CECQPSView *)GetActiveView();
	CListCtrl& listView = pView->GetListCtrl();

	listView.DeleteAllItems();

	LV_COLUMN lvcolumn;
	CString strHeading;
	for(UINT i=2;i<=5;i++)
	{
		strHeading.LoadString( IDS_NETWORK_SRC + i-2 );
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT 
						| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = strHeading.GetBuffer(256);
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		lvcolumn.cx = 60;
		if(i==4) lvcolumn.cx = 120;
		listView.SetColumn(i, &lvcolumn);
	}

	if(cWinVer.CompareNoCase("Windows2K") ==0 )
		pView->UpdateProcessList_Network2k("INIT");
	else
		pView->UpdateProcessList_NetworkXp("INIT");

}
void CMainFrame::ExeBar_PERFORMON ()
{
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("Windows98") ==0 )
	{
		AfxMessageBox("本功能不支持Windows Me/98 !");
		return;
	}	
	
	m_NowViewListType=NOW_PERFMON;
	BeginWaitCursor(); // display the hourglass cursor

	//开启性能记数器
	CRegistry Reg= HKEY_LOCAL_MACHINE;
	CStringArray cRegName;
	CString cRegValue;
	Reg.Open ("SYSTEM\\CurrentControlSet\\Services\\PerfProc\\Performance");
	Reg.DeleteValue("Disable Performance Counters");
	/*////////////////////////读取注册表
	Reg.EnumValueName(cRegName);
	for(int i=0;i<cRegName.GetSize();i++){
		Reg.Read (cRegName[i],&cRegValue);
		TRACE(cRegName[i]);
	}*/////////////////////////////////////////

	CECQPSView* pView = (CECQPSView *)GetActiveView();
	CListCtrl& listView = pView->GetListCtrl();
	listView.DeleteAllItems();

	LV_COLUMN lvcolumn;
	CString strHeading;
	for(int i=2;i<=5;i++)
	{
		strHeading.LoadString( IDS_PERM_CPU + i-2 );
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT 
						| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = strHeading.GetBuffer(256);
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		lvcolumn.cx = 80;
		listView.SetColumn(i, &lvcolumn);
	}

	pView->UpdateProcessList_Performon("INIT");
 
}
void CMainFrame::ExeBar_SERVICE ()
{
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("Windows98") ==0 )
	{
		AfxMessageBox("本功能不支持Windows Me/98 !");
		return;
	}	

	CDlgService dlgService;
	dlgService.DoModal();
}
void CMainFrame::ExeBar_BOOT ()
{
	CDlgRegBoot dlgRegBoot;
	dlgRegBoot.DoModal();
}
void CMainFrame::ExeBar_COMPUTEE ()
{
	extern CString szSystemPath;
	::ShellExecute(NULL, "open",szSystemPath+"\\calc.exe",NULL, NULL, SW_SHOWNORMAL);
}
void CMainFrame::ExeBar_CONSOLE()
{
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("Windows98") ==0 )
	{
		AfxMessageBox("本功能不支持Windows Me/98 !");
		return;
	}	

	extern CString szSystemPath;
	::ShellExecute(NULL, "open",szSystemPath+"\\services.msc","/s", NULL, SW_SHOWNORMAL);
}
void CMainFrame::ExeBar_REGEDIT()
{
	extern CString szWindowPath;
	::ShellExecute(NULL, "open",szWindowPath+"\\regedit.exe",NULL, NULL, SW_SHOWNORMAL);
}
void CMainFrame::ExeBar_NOTPAD ()
{
	extern CString szWindowPath;
	::ShellExecute(NULL, "open",szWindowPath+"\\notepad.exe",NULL, NULL, SW_SHOWNORMAL);
}
void CMainFrame::ExeBar_HELP ()
{
	extern CString strExePath;
	//AfxMessageBox(strExePath);
	::ShellExecute(NULL, "open",strExePath+"\\ReadMe.chm",NULL, NULL, SW_SHOWNORMAL);

}
void CMainFrame::ExeBar_HOME ()
{
	::ShellExecute(NULL, "open","http://www.ecq.name", NULL, NULL, SW_SHOWNORMAL);
}
void CMainFrame::ExeBar_ABOUT()
{
	extern CECQPSApp theApp;
	theApp.OnAppAbout();
}
void CMainFrame::ExeBar_HIDPROC_SEL()
{
    CRect rcCaller;

    // Get rect of toolbar item
    m_wndXPToolBar.GetItemRect (m_wndXPToolBar.CommandToIndex (ID_TOOL_HIDDEN), rcCaller);
    m_wndXPToolBar.ClientToScreen (rcCaller);
    rcCaller.top++;
    rcCaller.bottom--;

    CPopup* pPopup = (CPopup*)RUNTIME_CLASS(CPopup)->CreateObject();

    // Popup the ListCtrl
    VERIFY(pPopup->Display (&m_treePopup, this, rcCaller, &m_treePopup.m_xPopupCtrlEvent, WS_TABBORDER));
}
BOOL CMainFrame::SetSystemTray()
{
	return TRUE;
}

void CMainFrame::OnMainmenuTop() 
{
	// TODO: Add your command handler code here
	if ( GetExStyle() & WS_EX_TOPMOST )
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE);                
	}
	else
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE);
      }	
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nID){
	case SC_MAXIMIZE:
		//AfxMessageBox("最大化-置窗口最前");
		m_wndParent.SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		break;
	case SC_RESTORE:
		//AfxMessageBox("最大化-置窗口最前");
		m_wndParent.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE| SWP_NOMOVE );
		break;
	case SC_MINIMIZE:
		//AfxMessageBox("最小化-隐藏窗口");
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
		m_wndParent.ShowWindow(SW_HIDE);
		break;
	default:
		break;
	}
	CFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
		m_wndParent.ShowWindow(SW_HIDE);
	}
	// TODO: Add your message handler code here
	
	// Do not call /*CFrameWnd*/CFrameWnd::OnPaint() for painting messages
}


void CMainFrame::ExeBar_HIDDENPROC() 
{
	// TODO: Add your command handler code here
    CRect rcCaller;

    // Get rect of toolbar item
    m_wndXPToolBar.GetItemRect (m_wndXPToolBar.CommandToIndex (ID_TOOL_HIDDEN), rcCaller);
    m_wndXPToolBar.ClientToScreen (rcCaller);
    rcCaller.top++;
    rcCaller.bottom--;

    CPopup* pPopup = (CPopup*)RUNTIME_CLASS(CPopup)->CreateObject();

    // Popup the ListCtrl
    VERIFY(pPopup->Display (&m_treePopup, this, rcCaller, &m_treePopup.m_xPopupCtrlEvent, WS_TABBORDER));
	
}

void CMainFrame::OnUpdateToolHidden(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    pCmdUI->SetCheck (m_treePopup.IsWindowVisible() ? 2 : 0);
	
}

void CMainFrame::ExeBar_HIDDEN_EXE(ULONG ulOp)
{
	BeginWaitCursor(); // display the hourglass cursor
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("WindowsXP") ==0 || cWinVer.CompareNoCase("Windows2K") ==0) //cWinVer.CompareNoCase("Windows2003") ==0
	{
		CMainFrame *pMainframe = (CMainFrame *)AfxGetMainWnd();
		pMainframe->ulHiddenProcType=ulOp;
		CECQPSView* pView = (CECQPSView*)pMainframe->GetActiveFrame()->GetActiveView();
		if( ulOp < 0x8 ){
			pMainframe->m_NowViewListType=NOW_HIDDEN;
			CListCtrl& listView = pView->GetListCtrl();
			listView.DeleteAllItems();

			LV_COLUMN lvcolumn;
			CString strHeading;
			for(int i=2;i<=5;i++)
			{
				strHeading.LoadString( IDS_PROCESSWARN + i-2 );
				lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT 
								| LVCF_WIDTH | LVCF_ORDER;
				lvcolumn.fmt = LVCFMT_LEFT;
				lvcolumn.pszText = strHeading.GetBuffer(256);
				lvcolumn.iSubItem = i;
				lvcolumn.iOrder = i;
				lvcolumn.cx = 80;
				listView.SetColumn(i, &lvcolumn);
			}
			pView->UpdateProcessList_HiddenProc(ulOp);
		}
		if(  ulOp == 0x8 )
		{
			pMainframe->m_NowViewListType=NOW_DRIVER;
			CListCtrl& listView = pView->GetListCtrl();
			listView.DeleteAllItems();
			LV_COLUMN lvcolumn;
			CString strHeading;
			for(int i=2;i<=5;i++)
			{
				strHeading.LoadString( IDS_ROOTKITSYS_NO + i-2 );
				lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT 
								| LVCF_WIDTH | LVCF_ORDER;
				lvcolumn.fmt = LVCFMT_LEFT;
				lvcolumn.pszText = strHeading.GetBuffer(256);
				lvcolumn.iSubItem = i;
				lvcolumn.iOrder = i;
				lvcolumn.cx = 80;
				listView.SetColumn(i, &lvcolumn);
			}
			pView->UpdateProcessList_HiddenDriver();
			TRACE("UpdateProcessList_HiddenDriver\n");
		}
	}

}

void CMainFrame::ExeBar_DISABLEDAUTO() 
{
	// TODO: Add your command handler code here
	//m_uTimer = SetTimer( 1, 5000, NULL );
	if ( m_uTimer )
	{
		KillTimer(m_uTimer);
		m_uTimer = 0 ;
	}
	else
	{
		m_uTimer = SetTimer( 1, 5000, NULL );
     }	
	
}

void CMainFrame::OnUpdateMenuDisabledauto(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	pCmdUI->SetCheck(!m_uTimer);		
	
}
