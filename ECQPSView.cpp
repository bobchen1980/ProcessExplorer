// ECQPSView.cpp : implementation of the CECQPSView class
//

#include "stdafx.h"
#include "ECQPS.h"
#include "MainFrm.h"
#include "ECQPSDoc.h"
#include "ECQPSView.h"

////包含网络处理文件
#include "Network/Pro2Port.h"

///included hidden process
#include "Process/HiddenProcCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CECQPSView

IMPLEMENT_DYNCREATE(CECQPSView, CMultiColumnSortListView)

BEGIN_MESSAGE_MAP(CECQPSView, CMultiColumnSortListView)
//{{AFX_MSG_MAP(CECQPSView)
ON_WM_DESTROY()
ON_WM_LBUTTONDBLCLK()
ON_WM_CONTEXTMENU()
ON_COMMAND(ID_POPMENU_OPEN, OnPopmenuOpen)
ON_COMMAND(ID_POPMENU_KILL, OnPopmenuKill)
ON_COMMAND(ID_POPMENU_DELETE, OnPopmenuDelete)
ON_COMMAND(ID_POPMENU_COPY, OnPopmenuCopy)
ON_WM_LBUTTONDOWN()
ON_COMMAND(ID_POPMENU_AUTOKILL, OnPopmenuAutokill)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CECQPSView construction/destruction

CECQPSView::CECQPSView()
{
	// TODO: add construction code here
	
}

CECQPSView::~CECQPSView()
{
	if(m_PerfMon.IsActive)
		m_PerfMon.Uninitialize();
}

BOOL CECQPSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style = (cs.style & ~LVS_TYPEMASK) | LVS_REPORT;
	cs.style |= LVS_AUTOARRANGE;
	
	return CMultiColumnSortListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CECQPSView drawing

void CECQPSView::OnDraw(CDC* pDC)
{
	CECQPSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CECQPSView diagnostics

#ifdef _DEBUG
void CECQPSView::AssertValid() const
{
	CMultiColumnSortListView::AssertValid();
}

void CECQPSView::Dump(CDumpContext& dc) const
{
	CMultiColumnSortListView::Dump(dc);
}

CECQPSDoc* CECQPSView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CECQPSDoc)));
	return (CECQPSDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CECQPSView message handlers

void CECQPSView::OnInitialUpdate() 
{
	CECQPSDoc* pDoc = GetDocument();
	CMultiColumnSortListView::OnInitialUpdate();
	
	//init taskbarnotifier
	m_ctrlTaskBar.Create(this);
	m_ctrlTaskBar.SetSkin(IDB_BOLOON_BK, 255, 0, 255);
	m_ctrlTaskBar.SetTextFont("Verdana",90,TN_TEXT_NORMAL, TN_TEXT_BOLD);
	m_ctrlTaskBar.SetTextColor(RGB(0,0,0), RGB(255,255,255));
	m_ctrlTaskBar.SetTextRect(CRect(
		10,
		35,
		m_ctrlTaskBar.m_nSkinWidth-10,
		m_ctrlTaskBar.m_nSkinHeight-15));
	m_ctrlTaskBar.SetTransparent(90); //default 58
	///提示泡
	// Create the tool tip
	m_Tip.Create(this);
	//初始第一视图
	UpdateProcessList_Normal("INIT");
	// TODO: Add your specialized code here and/or call the base class
	
}

BOOL CECQPSView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bCreated = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if ( bCreated )
	{
		CListCtrl& listView = GetListCtrl();
		
		int aWidths[] = { 150, 50, 60, 60, 60, 100,400,0 };
		CString strHeading;
		
		for ( int iCol = 0; iCol < C_COLUMNS+1; iCol++) 
		{
			strHeading.LoadString( IDS_PROCESSNAME + iCol );
			listView.InsertColumn( iCol, strHeading, LVCFMT_LEFT,
				aWidths[iCol], iCol );
		}
		
		//保存列表的属性
		SetListProfile();
		//set the extern style of the listview
		SetListExternStyle(0,0L);
		
		//set the bak and fore of the listview
		SetListBackFore(0,0L);
		
		m_cImageList.Create(16, 15, ILC_COLOR32, 16, 64);
		m_cImageList.SetBkColor(RGB(255,255,255));
		m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_UNKNOWN));
		m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_SAFE));
		m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_NOTION));
		m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_VIRUS));
		m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_PROM));
		listView.SetImageList(&m_cImageList, LVSIL_SMALL);
		
	}
	return bCreated;	
}

LRESULT CECQPSView::SetListExternStyle(WPARAM wParam, LPARAM lParam)
{
	CListCtrl& listView = GetListCtrl();
	listView.SetExtendedStyle(GetListViewStyle()|LVS_EX_SUBITEMIMAGES);
	if(ListHasFlatSB())
		FlatSB_EnableScrollBar(listView.m_hWnd,SB_BOTH,ESB_ENABLE_BOTH);
	return TRUE;
}

LRESULT CECQPSView::SetListBackFore(WPARAM wParam, LPARAM lParam)
{
	CListCtrl& listView = GetListCtrl();
	listView.SetBkColor(AfxGetApp()->GetProfileInt(_T("Settings"),
		_T("ColorBack"),RGB(255,255,255)));
	listView.SetTextBkColor(AfxGetApp()->GetProfileInt(_T("Settings"),
		_T("ColorTextBack"),RGB(255,255,255)));
	listView.SetTextColor(AfxGetApp()->GetProfileInt(_T("Settings"),
		_T("ColorText"),RGB(0,0,0)));
	listView.RedrawWindow();
	return TRUE;
}



void CECQPSView::UpdateProcessList_Normal(CString cType)
{
	extern CString cWinVer;
	
	ULONG ulSize, ulNeeded,ulDanger=0;
	
	if(cWinVer.CompareNoCase("Windows98") !=0 )
	{
		EnumProcessesInfo_NT( NULL, 0, &ulNeeded );
	}
	else
	{
		//AfxMessageBox("Win98");
		EnumProcessesInfo_98( NULL, 0, &ulNeeded );
	}
	
	//获取内核总的进程数目
	LPPROCESSINFO32 lparrPsInfo = new PROCESSINFO32[ulNeeded];
	//比较从内核获得的进程和从PSAPI获得进程，注意，从内核可以获得隐藏进程，加入到lparrPsInfo结构中

	if ( NULL == lparrPsInfo ) {
		TRACE("Alloc Memory Fail!");
		return;
	}
	
	ulSize = ulNeeded;
	if(cWinVer.CompareNoCase("Windows98") !=0 )
		EnumProcessesInfo_NT( lparrPsInfo, ulSize, &ulNeeded  );
	else
		EnumProcessesInfo_98( lparrPsInfo, ulSize, &ulNeeded  );
	
	///更新控件的列表
	UpdateNomalListCtrl(cType,lparrPsInfo,ulNeeded);

	delete[] lparrPsInfo;
}


///m_cImageList.Create(16, 15, ILC_COLOR32, 16, 64);

int CECQPSView::GetProcessIcon(CString strAppName, CString strPathAppName)
{
/*
*	Note: -> there is no need to clean-up the map: knowledge is better
*        -> icon cache mechanism 
	*/
	int nUnk = 0;		//default icon (app without icon)
	int nIcoIndex = -1; 
	
	if (strPathAppName.IsEmpty() || (strPathAppName == "error"))
		return nUnk; //default for unk process
	
	if (strAppName.IsEmpty())
		strAppName = strPathAppName.Right((strPathAppName.GetLength()-1) - strPathAppName.ReverseFind('\\'));
	
	//is it already assigned ?
	Ico2Index::iterator it = m_mProcName2IcoIndex.find(strAppName);
	if (it != m_mProcName2IcoIndex.end())
	{
		//TRACE2("\n Existing map, process: %s, icon: %d\n",(*it).first, m_mProcName2IcoIndex[strAppName]);
		nIcoIndex = m_mProcName2IcoIndex[strAppName];
		return --nIcoIndex;
	}
	
	//add a new record
	HICON hSmallIco = NULL;
	if (NULL == (hSmallIco = ::ExtractIcon(NULL, strPathAppName, 0)))
		return nUnk;
	
	m_cImageList.Add(hSmallIco);
	DestroyIcon(hSmallIco);
	
	nIcoIndex = (m_cImageList.GetImageCount());
	if (nIcoIndex >= 1)
	{
		//TRACE2("\n New map, process: %s, icon: %d\n", strAppName, nIcoIndex);
		m_mProcName2IcoIndex[strAppName] = nIcoIndex;
		return --nIcoIndex;
	}
	else
		return nUnk;
}


void CECQPSView::OnDestroy() 
{
	CMultiColumnSortListView::OnDestroy();
	//AfxMessageBox("CECQPSView:OnDestroy");
	// TODO: Add your message handler code here
	
}

void CECQPSView::SetListProfile()
{
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("AllowSort"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("DisplayImage"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("DoubleClickOpenFile"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("FlatScrollBar"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("FullRowSelect"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("Gridlines"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("HeaderDragDrop"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("OneClickActivate"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("TrackSelect"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("TwoClickActivate"),YES);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("UnderlineCold"),NO);
    AfxGetApp()->WriteProfileInt(_T("Settings"),
		_T("UnderlineHot"),YES);
	
}



void CECQPSView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    CMainFrame *pMainWnd=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	
	CListCtrl& lcProcess = GetListCtrl();	
	
	UINT uFlags;
	DWORD dwPID;
	
	int nItem = lcProcess.HitTest(point, &uFlags);
	
	if (uFlags & LVHT_ONITEM )
	{
		///得到线程的ID地址 PID=2
		dwPID=atol(lcProcess.GetItemText(nItem,PROCESS_PID));
		pMainWnd->StartModuleThread(dwPID);
	}	
	
	CMultiColumnSortListView::OnLButtonDblClk(nFlags, point);
}

DWORD CECQPSView::GetTotalThreadList(DWORD dwOwnerPID)
{
    HANDLE        hThreadSnap = NULL; 
    BOOL          bRet        = FALSE; 
    THREADENTRY32 te32        = {0}; 
	DWORD dwTotalThread = 0;
	
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
    if (hThreadSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
	
    te32.dwSize = sizeof(THREADENTRY32); 
    if (Thread32First(hThreadSnap, &te32)) 
    { 
        do 
        { 
            if (te32.th32OwnerProcessID == dwOwnerPID) 
            { 
				dwTotalThread++;
			} 
        } 
        while (Thread32Next(hThreadSnap, &te32)); 
    } 
	
    CloseHandle (hThreadSnap); 
	
    return (dwTotalThread); 
}

CString CECQPSView::GetProcessWarn(CString sName, CString sPath ,CString& sInfo)
{
	extern CString strDataName;
	//CMainFrame* pMainFrame=(CMainFrame *)AfxGetMainWnd();
	CStringFile sfText;
	CString		szTmp,strName;
	extern CString	szSystemPath,szWindowPath;
	
	CFileVersionInfo m_FileInfo;
	m_FileInfo.ReadVersionInfo(sPath);
	CString strVendor=m_FileInfo.GetVersionInfo(0);
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	////定义常见的软件厂商
	CStringArray strComVendor;
	strComVendor.Add("Microsoft Corporation");
	strComVendor.Add("Macromedia Inc.");
	strComVendor.Add("Macromedia, Inc.");
	strComVendor.Add("Macrovision Europe Ltd.");
	strComVendor.Add("ECQ Corporation");
	strComVendor.Add("Symantec Corporation");
	strComVendor.Add("Adobe Systems, Incorporated");
	strComVendor.Add("Adobe Systems Incorporated");
	strComVendor.Add("ACD Systems, Ltd.");
	strComVendor.Add("ACD Systems, Ltd.");
	strComVendor.Add("Vmware, Inc.");
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	
	sPath.MakeUpper();
	sName.MakeUpper();
	//sPath.Replace("\\??\\","");
	sPath.Replace(szSystemPath,"SYSTEMROOT");
	sPath.Replace(szWindowPath,"WINDOWROOT");
	if(sPath.Find("\\PROGRAM FILES") >-1 )
		sPath="PROGRAMROOT"+sPath.Mid(sPath.Find("PROGRAM FILES")+strlen("PROGRAM FILES"),sPath.GetLength());
	if(sPath.Find("\\PROGRA~1") >-1 )
		sPath="PROGRAMROOT"+sPath.Mid(sPath.Find("PROGRA~1")+strlen("PROGRA~1"),sPath.GetLength());
	//TRACE("szSystemPath:%s,%s\n",szSystemPath,sPath);
	// When the given file can be opened
	//if(sName.CollateNoCase("StyleXPService.exe")==0)
	//	AfxMessageBox(sPath);
	if(sfText.Open(strDataName)) 
	{
		szTmp=sfText.FindString(sPath,sName);
		///AfxMessageBox(szTmp);
		if( szTmp == "NONE" ){
			////////////判断是可疑还是程序
			for(int i=0;i<strComVendor.GetSize();i++)
			{
				if(strVendor.CollateNoCase(strComVendor[i]) ==0)
				{
					sInfo=strVendor+"公司开发的应用程序";
					return "程序";
				}
			}
			sInfo="可疑进程/非系统进程/未认证";
			return "可疑";
		}
		if( szTmp.Left(4) == "Part"){
			sInfo=szTmp.Right(szTmp.GetLength()-1-szTmp.ReverseFind('$'));
			//AfxMessageBox(szTmp.Left(5));
			if( szTmp.GetAt(4) != 'S' ) 
				return "危险";				
		}
		if( szTmp.GetLength() >4 )
		{
			sInfo=szTmp.Right(szTmp.GetLength()-1-szTmp.ReverseFind('$'));
			
			if(  szTmp.GetAt(0)== 'S' ) 
				return "系统";
			else if( szTmp.GetAt(0) == 'P' ) 
				return "程序";
			else 
				return "危险";		
		}
		sfText.Close(); 
	}	
	
	return "可疑";
}



void CECQPSView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	///设置当前选择的单项	
	
	CRect ClientRect;
	GetClientRect(&ClientRect);
	
	BCMenu cPopMenu;
	VERIFY(cPopMenu.LoadMenu(IDR_POPMENU));
	
	BCMenu* pPopup = (BCMenu*)cPopMenu.GetSubMenu(0);
	VERIFY(pPopup != NULL);
	CWnd* pWndPopupOwner = this->GetParent();
	////设置菜单的图标
	BCMenu::SetMenuDrawMode(BCMENU_DRAWMODE_XP);
	cPopMenu.LoadMenu(IDR_POPMENUBAR);
	static UINT popToolBars[] = { IDR_POPMENUBAR,};
	cPopMenu.LoadToolbars( popToolBars, sizeof(popToolBars)/sizeof(UINT) );
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,point.x,point.y,
		pWndPopupOwner);	
}

void CECQPSView::OnPopmenuOpen() 
{
	// TODO: Add your command handler code here
	CListCtrl& lcProcess = GetListCtrl();	
	
	POSITION pos = lcProcess.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		CString szPath,szFile;
		while (pos)
		{
			int nItem = lcProcess.GetNextSelectedItem(pos);
			szPath=lcProcess.GetItemText(nItem,PROCESS_PATH);
			szFile=lcProcess.GetItemText(nItem,PROCESS_FILE);
			szPath.Replace(szFile,"");
			//TRACE1("Item %d was selected!\n", nItem);
			//AfxMessageBox(szPath);
			//AfxMessageBox(szFile);
			::ShellExecute(NULL, "open",szPath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	
}

void CECQPSView::OnPopmenuAutokill() 
{
	// TODO: Add your command handler code here
	CListCtrl& lcProcess = GetListCtrl();	
	
	POSITION pos = lcProcess.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		CString szPID,szTmp;
		while (pos)
		{
			int nItem = lcProcess.GetNextSelectedItem(pos);
			szPID=lcProcess.GetItemText(nItem,PROCESS_PID);
			//TRACE1("Item %d was selected!\n", nItem);
			szTmp.Format("是否通知[%s]进程自行退出?\n自行退出将释放与进程相关\n所有DLL库文件和资源!",szPID);
			CDlgWarning dlgWarning;
			dlgWarning.m_WarnInfo.SetCaption(szTmp);			
			if(dlgWarning.DoModal() == IDOK)
				TerminateApp( atol( szPID ) , 1 );
		}
	}
}

void CECQPSView::UpdateProcessList_Performon(CString cType)
{
	///////更新主列表视图的Column
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("Windows98") ==0 )
	{
		return;
	}
	
	extern CString	szSystemPath,szWindowPath;
	CListCtrl& lcProcess =GetListCtrl();
	if(cType.CompareNoCase("INIT") == 0)
	{
		//初始性能基数
		if (!m_PerfMon.Initialize())
		{
			//AfxMessageBox("不能加载性能数据!");
			return;
		}
		
	}	
	///加载进程
	extern CString cWinVer;
	ULONG ulSize, ulNeeded;
	CString szPID,szPath,szTmpName,szCPUName,szMEMName;			
	
	if(cWinVer.CompareNoCase("Windows98") !=0 )
		EnumProcessesInfo_NT( NULL, 0, &ulNeeded );
	else
		EnumProcessesInfo_98( NULL, 0, &ulNeeded );
	
	LPPROCESSINFO32 lparrPsInfo = new PROCESSINFO32[ulNeeded];
	if ( NULL == lparrPsInfo ) {
		TRACE("Alloc Memory Fail!");
		return;
	}
	
	ulSize = ulNeeded;
	if(cWinVer.CompareNoCase("Windows98") !=0 )
		EnumProcessesInfo_NT( lparrPsInfo, ulSize, &ulNeeded  );
	else
		EnumProcessesInfo_98( lparrPsInfo, ulSize, &ulNeeded  );
	
	if(cType.CompareNoCase("UPDATE") == 0)
	{		
		m_CurrentPIdIndex.clear();
		for ( ULONG ulIndex = 0; ulIndex < ulNeeded; ulIndex++ ) {			
			szPID.Format("%d",lparrPsInfo[ulIndex].dwPID);
			m_CurrentPIdIndex[szPID]=NULL;
		}
		
		for(Process2Index::iterator pIx=m_ProcessIdIndex.begin();pIx!=m_ProcessIdIndex.end();pIx++)
		{
			if( m_CurrentPIdIndex.find(pIx->first) == m_CurrentPIdIndex.end() )
			{
				lcProcess.DeleteItem(pIx->second);
			}
		}		
	}
	else{
		m_ProcessIdIndex.clear();
		lcProcess.DeleteAllItems();
	}
	
	CString       strKernelTime,strUserTime,strRuntime,strKUtime;
	SYSTEMTIME  timeKernel,timeUserTime;
	COleDateTime timeCreation,timeNow ;
	COleDateTimeSpan    timeDiff;
	timeNow = COleDateTime::GetCurrentTime();
	///枚举当前进程
	for ( ULONG ulIndex = 0; ulIndex < ulNeeded; ulIndex++ ) {
		
		szPID.Format("%d",lparrPsInfo[ulIndex].dwPID);
		////判断是否新的进程
		Process2Index::iterator it = m_ProcessIdIndex.find(szPID);
		if (it == m_ProcessIdIndex.end())
		{					
			int nIcon=GetProcessIcon(lparrPsInfo[ulIndex].strName,lparrPsInfo[ulIndex].strPath);
			CString szName  = lparrPsInfo[ulIndex].strName;
			
			timeCreation=lparrPsInfo[ulIndex].ftCreation;
			
            FileTimeToSystemTime(&lparrPsInfo[ulIndex].ftKernel, &timeKernel);
            strKernelTime.Format("%.2u:%.2u:%.2u", timeKernel.wHour, timeKernel.wMinute, timeKernel.wSecond);
			
            FileTimeToSystemTime(&lparrPsInfo[ulIndex].ftUser, &timeUserTime);
            strUserTime.Format("%.2u:%.2u:%.2u", timeUserTime.wHour, timeUserTime.wMinute, timeUserTime.wSecond);				
			
			/////////////////////////////////////////
			int nItemInserted = lcProcess.InsertItem(
				LVIF_IMAGE | LVIF_TEXT, ulIndex, szName,
				INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, nIcon,NULL);
			
			if( lparrPsInfo[ulIndex].dwPID== 4 || lparrPsInfo[ulIndex].dwPID == 8 || lparrPsInfo[ulIndex].dwPID == 0 )
			{	
				strKernelTime="";
				strUserTime="";
				strKUtime="";
			}
			else
			{
				timeDiff = timeNow - timeCreation;
				strRuntime.Format("%uD:%uH:%uM:%uS", timeDiff.GetDays(), timeDiff.GetHours(), timeDiff.GetMinutes(), timeDiff.GetSeconds());
				strKUtime=strKernelTime+" ["+strUserTime+"]";
			}
			
			szPath=lparrPsInfo[ulIndex].strPath;
			szPath.Replace("\\??\\","");
			szPath.Replace("\\SystemRoot",szWindowPath);
			
			
			lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, 0);
			//lcProcess.SetItem(nItemInserted, 2,  LVIF_TEXT, szAlarm,0, 0, 0, 0);
			//lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szPriority,0, 0, 0, 0);
			lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT,strKUtime,0, 0, 0, 0);
			lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, strRuntime,0, 0, 0, 0);
			lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szPath,0, 0, 0, 0);
			
			///如果确定是新的进程，则也加入记数器
			szTmpName=lparrPsInfo[ulIndex].strName;
			szTmpName.MakeUpper();
			szTmpName.Replace(".EXE","");
			if(szTmpName.CompareNoCase("SYSTEM IDLE PROCESS") == 0 )
				szTmpName="Idle";
			szCPUName.Format("\\Process(%s)\\%% Processor Time",szTmpName);
			szMEMName.Format("\\Process(%s)\\Working Set",szTmpName);
			
			int nCPU=m_PerfMon.AddCounter(szCPUName);   ///后续加的基数为+1
			int nMEM=m_PerfMon.AddCounter(szMEMName);
			lcProcess.SetItemData(nItemInserted,(DWORD)nCPU); ///用于循环获得性能数据
			//TRACE("szName:%d,%d,%s\n",nCPU,szMEMName,szCPUName);
			///保存当前新加激活的进程
			m_ProcessIdIndex[szPID]=nItemInserted; 
		}
	}	
	
	delete[] lparrPsInfo;
    ///填充性能数据
	UpdateProcessList_GetCounterData(-1);
}

void CECQPSView::UpdateProcessList_GetCounterData(UINT nItemInserted)
{
	CListCtrl& lcProcess =GetListCtrl();
	
	if (!m_PerfMon.CollectQueryData())
	{
		///AfxMessageBox("Failed Query!");
		return;
	}
	for(int nIndex=0;nIndex<lcProcess.GetItemCount();nIndex++)
	{
		// update counters	
		CString strCpu,strMem;
		//TRACE("%d,%d\n",nIndex,lcProcess.GetItemCount());
		int nCPU = (int)lcProcess.GetItemData(nIndex);
		int nMEM = nCPU+1;   /////连续两个计数器间隔为1
		
		long lcpu = m_PerfMon.GetCounterValue(nCPU);
		long lmem = m_PerfMon.GetCounterValue(nMEM);
		//TRACE("%d,%d,%d,%d\n",lcpu,lmem,nCPU,nMEM);
		
		/// update dialog
		if (lcpu != -999)
		{
			strCpu.Format("%d%%", lcpu);
			lcProcess.SetItemText(nIndex,2,strCpu);
		}
		
		if (lmem != -999)
		{
			strMem.Format("%dK", lmem/1024);
			lcProcess.SetItemText(nIndex,3,strMem);			
		}
	}
}




void CECQPSView::UpdateProcessList_Network2k(CString cType)
{
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("Windows98") ==0 )
	{
		return;
	}
	
	CListCtrl& lcProcess =GetListCtrl();
	lcProcess.DeleteAllItems();			
	
	HANDLE hTcpHandle;
	HANDLE hUdpHandle;
	HANDLE hSection;
	
	if(!InitNTDLL())
		return;
	
	HANDLE hMapPhysicalMemory = OpenPhysicalMemory(hSection);
	
	hTcpHandle = OpenDeviceTcpUdp(L"\\Device\\TCP");
	hUdpHandle = OpenDeviceTcpUdp(L"\\Device\\UDP");
	
    PULONG pBuffer = GetHandleList();
	
	if (pBuffer == NULL)	return;
	
	PSYSTEM_HANDLE_INFORMATION objTcpAddress = NULL;
	PSYSTEM_HANDLE_INFORMATION objUdpAddress = NULL;
	
	objTcpAddress = (PSYSTEM_HANDLE_INFORMATION)GetTcpUdpObject(pBuffer, hTcpHandle, GetCurrentProcessId());
	
	PTE pteTCPCur;
	if (!GetPTE(objTcpAddress->Object, hMapPhysicalMemory, hSection, pteTCPCur))
	{
		//AfxMessageBox("GetPTE Failed");
		return ;
	}
	
	objUdpAddress = (PSYSTEM_HANDLE_INFORMATION)GetTcpUdpObject(pBuffer, hUdpHandle, GetCurrentProcessId());
	
	PTE pteUDPCur;
	if (!GetPTE(objUdpAddress->Object, hMapPhysicalMemory, hSection, pteUDPCur))
	{
		//AfxMessageBox("GetPTE2 Failed");
		return;
	}
	
	OVERLAPPED    Overlap;
	
	HANDLE hEvent = CreateEvent(0, 1, 0, 0);
	
	Overlap.Internal     = 0;
	Overlap.InternalHigh = 0;
	Overlap.Offset       = 0;
	Overlap.OffsetHigh   = 0;
	Overlap.hEvent       = hEvent;
	
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		TRACE("Failed to take process snapshot. Process names will not be shown.\n\n");
	}	
	
	int nCount = *pBuffer;
    PSYSTEM_HANDLE_INFORMATION pProcesses = (PSYSTEM_HANDLE_INFORMATION)(pBuffer + 1);
	
	CString szPID,szPort,szDestAddr,szState;	
	
    for (int i = 0; i < nCount; i++)
    {
		//TRACE("PID = %4d \n", pProcesses->ProcessId);
		if (pProcesses->ObjectTypeNumber == objTcpAddress->ObjectTypeNumber)
		{
			PTE pte;
			if (!GetPTE(pProcesses->Object, hMapPhysicalMemory, hSection, pte))
			{
				pProcesses++;
				continue;
			}
			
			if ((pte.NoCache == 1 || pte.NoCache == 2) &&
				(pteTCPCur.WriteTable == pte.WriteTable))
			{
				HANDLE hProc = NULL, DupHandle=NULL;
				int  i = 0, portflag = 0;
				u_short openport;
				u_short destport;
				
				hProc = OpenProcess(PROCESS_DUP_HANDLE,
					0,
					pProcesses->ProcessId);
				
				if (hProc)
				{
					DuplicateHandle(hProc,
						(HANDLE)pProcesses->Handle,
						GetCurrentProcess(),
						&DupHandle,
						0,
						FALSE,
						2);
					
					CloseHandle(hProc);
					
					if (DupHandle)
					{
						TDI_CONNECTION_INFO    TdiConnInfo={0};
						TDI_CONNECTION_INFORMATION TdiConnInformation = {0};
						DWORD dwRetu=0;    
						
						if (pte.NoCache == 0x02)
						{
							TdiConnInformation.RemoteAddressLength = 4; 
							if (DeviceIoControl(
								DupHandle,
								0x210012,
								&TdiConnInformation,
								sizeof(TdiConnInformation),
								&TdiConnInfo,
								sizeof(TdiConnInfo),
								NULL,
								&Overlap))
							{	
								char szProcPath[256],szProcName[256];
								openport = ntohs((u_short)TdiConnInfo.ReceivedTsdus);
								destport = ntohs((u_short)TdiConnInfo.TransmittedTsdus);
								if (openport != 0)
								{
									TRACE("TCP  PID = %4d PORT = %6d %s,%s\n", pProcesses->ProcessId, openport,
										ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),szProcPath);								}
								////更新到列表
								/////////////////////////////////////////
								szPort.Format("%d",openport);
								szPID.Format("%d",pProcesses->ProcessId);
								int nItemInserted = lcProcess.InsertItem(
									LVIF_IMAGE | LVIF_TEXT, i, ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),
									INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, 0,NULL);
								///IP地址转换
								in_addr ina ;
								char   szDest [16];
								ina.S_un.S_addr = 0;//TdiConnInfo.TransmittedTsdus;
								strcpy( szDest , inet_ntoa( ina )  ) ;
								szDestAddr.Format("%s:%d",szDest,destport);
								szState.Format("%s",IpConvert2State((u_short)TdiConnInfo.State) );
								lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 2,  LVIF_TEXT, "TCP",0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szPort,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT, szDest,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, szState,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szProcPath,0, 0, 0, 0);
								
							}
							else
							{
								long lError = GetLastError();
							}
						}
						else if (pte.NoCache == 0x01)
						{
							TdiConnInformation.RemoteAddressLength = 3; 
							if (DeviceIoControl(DupHandle, 0x210012,
								&TdiConnInformation, sizeof(TdiConnInformation),
								&TdiConnInfo, sizeof(TdiConnInfo),
								NULL, &Overlap))
							{
								char szProcPath[256],szProcName[256];
								openport = ntohs((u_short)TdiConnInfo.ReceivedTsdus);
								destport = ntohs((u_short)TdiConnInfo.TransmittedTsdus);
								
								if (openport != 0)
								{
									TRACE("TCP  PID = %4d R %d PORT = %6d  %s,%s\n", pProcesses->ProcessId,ntohs((u_short)TdiConnInformation.RemoteAddress), openport, 
										ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),szProcPath);
									szPort.Format("%d",openport);
									szPID.Format("%d",pProcesses->ProcessId);
									
									int nItemInserted = lcProcess.InsertItem(
										LVIF_IMAGE | LVIF_TEXT, i, ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),
										INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, 0,NULL);
									///IP地址转换
									in_addr ina ;
									char   szDest [16];
									ina.S_un.S_addr = 0;///(u_short)TdiConnInfo.TransmittedTsdus;
									strcpy( szDest , inet_ntoa( ina )  ) ;
									szDestAddr.Format("%s:%d",szDest,destport);
									szState.Format("%s",IpConvert2State((u_short)TdiConnInfo.State) );
									
									lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 2,  LVIF_TEXT, "TCP",0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szPort,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT, szDestAddr,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, szState,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szProcPath,0, 0, 0, 0);
								}
							}
							else
							{
								long lError = GetLastError();
							}
						}
						CloseHandle(DupHandle);
					}
				}
			}
		}
		pProcesses++;
	}
	
	nCount = *pBuffer;
    pProcesses = (PSYSTEM_HANDLE_INFORMATION)(pBuffer + 1);
	
    for (i = 0; i < nCount; i++)
    {
		if (pProcesses->ObjectTypeNumber == objUdpAddress->ObjectTypeNumber)
		{
			PTE pte;
			if (!GetPTE(pProcesses->Object, hMapPhysicalMemory, hSection, pte))
			{
				pProcesses++;
				continue;
			}
			
			if ((pte.NoCache == 1 || pte.NoCache == 2) &&
				(pteUDPCur.WriteTable == pte.WriteTable))
			{
				HANDLE hProc = NULL, DupHandle=NULL;
				int  i = 0, portflag = 0;
				u_short openport;
				u_short destport;
				
				hProc = OpenProcess(PROCESS_DUP_HANDLE,
					0,
					pProcesses->ProcessId);
				
				if (hProc)
				{
					DuplicateHandle(hProc,
						(HANDLE)pProcesses->Handle,
						GetCurrentProcess(),
						&DupHandle,
						0,
						FALSE,
						2);
					
					CloseHandle(hProc);
					
					if (DupHandle)
					{
						TDI_CONNECTION_INFO    TdiConnInfo={0};
						TDI_CONNECTION_INFORMATION TdiConnInformation = {0};
						DWORD dwRetu=0;    
						
						if (pte.NoCache == 0x02)
						{
							TdiConnInformation.RemoteAddressLength = 4; 
							if (DeviceIoControl(
								DupHandle,
								0x210012,
								&TdiConnInformation,
								sizeof(TdiConnInformation),
								&TdiConnInfo,
								sizeof(TdiConnInfo),
								NULL,
								&Overlap))
							{
								char szProcPath[256],szProcName[256];
								openport = ntohs((u_short)TdiConnInfo.ReceivedTsdus);
								destport = ntohs((u_short)TdiConnInfo.TransmittedTsdus);
								if (openport != 0)
								{
									TRACE("TCP  PID = %4d PORT = %6d %s,%s\n", pProcesses->ProcessId, openport,
										ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),szProcPath);								}
								////更新到列表
								/////////////////////////////////////////
								szPort.Format("%d",openport);
								szPID.Format("%d",pProcesses->ProcessId);
								int nItemInserted = lcProcess.InsertItem(
									LVIF_IMAGE | LVIF_TEXT, i, ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),
									INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, 0,NULL);
								///IP地址转换
								in_addr ina ;
								char   szDest [16];
								ina.S_un.S_addr = 0;//TdiConnInfo.TransmittedTsdus;
								strcpy( szDest , inet_ntoa( ina )  ) ;
								szDestAddr.Format("%s:%d",szDest,destport);
								szState.Format("%s",IpConvert2State((u_short)TdiConnInfo.State) );
								lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 2,  LVIF_TEXT, "UDP",0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szPort,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT, szDest,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, szState,0, 0, 0, 0);
								lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szProcPath,0, 0, 0, 0);
							}
							else
							{
								long lError = GetLastError();
							}
						}
						else if (pte.NoCache == 0x01)
						{
							TdiConnInformation.RemoteAddressLength = 3; 
							if (DeviceIoControl(DupHandle, 0x210012,
								&TdiConnInformation, sizeof(TdiConnInformation),
								&TdiConnInfo, sizeof(TdiConnInfo),
								NULL, &Overlap))
							{
								char szProcPath[256],szProcName[256];
								openport = ntohs((u_short)TdiConnInfo.ReceivedTsdus);
								destport = ntohs((u_short)TdiConnInfo.TransmittedTsdus);
								
								if (openport != 0)
								{
									TRACE("TCP  PID = %4d R %d PORT = %6d  %s,%s\n", pProcesses->ProcessId,ntohs((u_short)TdiConnInformation.RemoteAddress), openport, 
										ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),szProcPath);
									szPort.Format("%d",openport);
									szPID.Format("%d",pProcesses->ProcessId);
									
									int nItemInserted = lcProcess.InsertItem(
										LVIF_IMAGE | LVIF_TEXT, i, ProcessPidToName(hProcessSnap, pProcesses->ProcessId, szProcPath,szProcName),
										INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, 0,NULL);
									///IP地址转换
									in_addr ina ;
									char   szDest [16];
									ina.S_un.S_addr = 0;///(u_short)TdiConnInfo.TransmittedTsdus;
									strcpy( szDest , inet_ntoa( ina )  ) ;
									szDestAddr.Format("%s:%d",szDest,destport);
									szState.Format("%s",IpConvert2State((u_short)TdiConnInfo.State) );
									
									lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 2,  LVIF_TEXT, "UDP",0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szPort,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT, szDestAddr,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, szState,0, 0, 0, 0);
									lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szProcPath,0, 0, 0, 0);
								}
							}
							else
							{
								long lError = GetLastError();
							}
						}
						CloseHandle(DupHandle);
					}
				}
			}
		}
		pProcesses++;
	}
	
	delete pBuffer;
	CloseHandle(hEvent);
	CloseHandle(hProcessSnap);
	
	return;
	
}

void CECQPSView::UpdateProcessList_NetworkXp(CString cType)
{
	extern CString cWinVer;
	if(cWinVer.CompareNoCase("Windows98") ==0 )
	{
		return;
	}
	
	CListCtrl& lcProcess =GetListCtrl();
	lcProcess.DeleteAllItems();			
	
	// Get the tables of TCP and UDP endpoints with process IDs
	DWORD		error=0;
	HANDLE		hProcessSnap;
	PMIB_TCPEXTABLE tcpExTable;
	PMIB_UDPEXTABLE udpExTable;
	CString szPID,szLocalPort,szRemotePort,szLocalAddr,szRemoteAddr,szState;
	
	/////定义xp下获取端口的隐藏函数
	pAllocateAndGetTcpExTableFromStack_Def dpAllocateAndGetTcpExTableFromStack;
	pAllocateAndGetUdpExTableFromStack_Def dpAllocateAndGetUdpExTableFromStack;
	
	dpAllocateAndGetTcpExTableFromStack = (pAllocateAndGetTcpExTableFromStack_Def) GetProcAddress( LoadLibrary( "iphlpapi.dll"), 
		"AllocateAndGetTcpExTableFromStack" );
	if( !dpAllocateAndGetTcpExTableFromStack ) return ;
	
	dpAllocateAndGetUdpExTableFromStack = (pAllocateAndGetUdpExTableFromStack_Def) GetProcAddress( LoadLibrary( "iphlpapi.dll"), 
		"AllocateAndGetUdpExTableFromStack" );
	if( !dpAllocateAndGetUdpExTableFromStack ) return ;
	
	error = dpAllocateAndGetTcpExTableFromStack( &tcpExTable, TRUE, GetProcessHeap(), 2, 2 );
	
	if( error ) {
		return ;
	}
	error = dpAllocateAndGetUdpExTableFromStack( &udpExTable, TRUE, GetProcessHeap(), 2, 2 );
	if( error ) {
		return ;
	}
	
	//
	// Get a process snapshot. Note that we won't be guaranteed to 
	// exactly match a PID against a process name because a process could have exited 
	// and the PID gotten reused between our endpoint and process snapshots.
	//
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE ) {		
		return ;
	}
	
	char szProcPath[256],szProcName[256];
	//
	// Dump the TCP table
	//
	for(int i = 0; i < tcpExTable->dwNumEntries; i++ ) {
		
		if(tcpExTable->table[i].dwLocalAddr ==0 || tcpExTable->table[i].dwProcessId <=0 ) continue;
		int nItemInserted = lcProcess.InsertItem(
			LVIF_IMAGE | LVIF_TEXT, i, ProcessPidToName(hProcessSnap, tcpExTable->table[i].dwProcessId, szProcPath,szProcName),
			INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, 0,NULL);
		
		///TRACE("Tcp:%d\n",tcpExTable->table[i].dwProcessId);
		szPID.Format("%d",tcpExTable->table[i].dwProcessId );
		szLocalAddr.Format("%d",htons(tcpExTable->table[i].dwLocalAddr) );
		szLocalPort.Format("%d",htons(tcpExTable->table[i].dwLocalPort) );
		szRemoteAddr.Format("%d",htons(tcpExTable->table[i].dwRemoteAddr) );
		szRemotePort.Format("%d",htons(tcpExTable->table[i].dwRemotePort) );
		in_addr ina ;
		char   szDest [16];
		ina.S_un.S_addr = tcpExTable->table[i].dwLocalAddr;
		strcpy( szDest , inet_ntoa( ina )  ) ;
		szRemoteAddr.Format("%s:%s",szDest,szRemotePort);
		szState.Format("%s",IpConvert2State(tcpExTable->table[i].dwState));
		
		lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 2,  LVIF_TEXT, "TCP",0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szLocalPort,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT, szRemoteAddr,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, szState,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szProcPath,0, 0, 0, 0);
	}
	//
	// Dump the UDP table
	//
	for( i = 0; i < udpExTable->dwNumEntries; i++ ) {
		//TRACE("Udp:%d",udpExTable->table[i].dwProcessId);		
		if(tcpExTable->table[i].dwLocalAddr ==0 || tcpExTable->table[i].dwProcessId <=0 ) continue;
		int nItemInserted = lcProcess.InsertItem(
			LVIF_IMAGE | LVIF_TEXT, i, ProcessPidToName(hProcessSnap, tcpExTable->table[i].dwProcessId, szProcPath,szProcName),
			INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, 0,NULL);
		
		//TRACE("Udp:%d\n",tcpExTable->table[i].dwProcessId);
		
		szPID.Format("%d",tcpExTable->table[i].dwProcessId );
		szLocalAddr.Format("%d",htons(tcpExTable->table[i].dwLocalAddr) );
		szLocalPort.Format("%d",htons(tcpExTable->table[i].dwLocalPort) );
		szRemoteAddr.Format("%d",htons(tcpExTable->table[i].dwRemoteAddr) );
		szRemotePort.Format("%d",htons(tcpExTable->table[i].dwRemotePort) );
		in_addr ina ;
		char   szDest [16];
		ina.S_un.S_addr = tcpExTable->table[i].dwLocalAddr ;
		strcpy( szDest , inet_ntoa( ina )  ) ;
		szRemoteAddr.Format("%s:%s",szDest,szRemotePort);
		szState.Format("%s",IpConvert2State(tcpExTable->table[i].dwState));
		
		lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 2,  LVIF_TEXT, "UDP",0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szLocalPort,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT, szRemoteAddr,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, szState,0, 0, 0, 0);
		lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szProcPath,0, 0, 0, 0);
	}
	
	return ;
}

void CECQPSView::OnPopmenuKill() 
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	CListCtrl& lcProcess = GetListCtrl();	
	
	POSITION pos = lcProcess.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		CString szPID,szTmp;
		while (pos)
		{
			int nItem = lcProcess.GetNextSelectedItem(pos);
			szPID=lcProcess.GetItemText(nItem,PROCESS_PID);
			//TRACE1("Item %d was selected!\n", nItem);
			szTmp.Format("是否强行关闭PID为[%s]进程?\n关闭系统进程可能出现异常!",szPID);			
			CDlgWarning dlgWarning;
			dlgWarning.m_WarnInfo.SetCaption(szTmp);			
			if(dlgWarning.DoModal() == IDOK)
				KillProcess( atol( szPID ) );
		}
	}	
}

void CECQPSView::OnPopmenuDelete() 
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	CListCtrl& lcProcess = GetListCtrl();	
	
	POSITION pos = lcProcess.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		CString szPID,szTmp,szPath;
		while (pos)
		{
			int nItem = lcProcess.GetNextSelectedItem(pos);
			szPID=lcProcess.GetItemText(nItem,PROCESS_PID);
			szPath=lcProcess.GetItemText(nItem,PROCESS_PATH);
			
			szTmp.Format("是否删除ID为%s的线程文件?\n请慎重执行删除操作!",szPID);
			CDlgWarning dlgWarning;
			dlgWarning.m_WarnInfo.SetCaption(szTmp);			
			if(dlgWarning.DoModal() == IDOK){
				try{
					KillProcess( atol( szPID ) );
					Sleep(500);
					::DeleteFile(szPath);
					Sleep(500);
					::DeleteFile(szPath);
				}
				catch(...){}
			}
		}
	}	
}

void CECQPSView::OnPopmenuCopy() 
{
	CListCtrl& lcProcess = GetListCtrl();	
	CFileDialogST	dlg;
	int				nRetValue;
	CString			sFolder;
	
	POSITION pos = lcProcess.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		CString szPath,dsPath,szFile;
		while (pos)
		{
			int nItem = lcProcess.GetNextSelectedItem(pos);
			szPath=lcProcess.GetItemText(nItem,PROCESS_PATH);
			szFile=lcProcess.GetItemText(nItem,PROCESS_FILE);
			nRetValue = dlg.SelectFolder(_T("请选择目的文件夹..."), _T("C:\\"), 
				0x40|BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS, this);
			if (nRetValue == IDOK)
			{
				sFolder = dlg.GetSelectedFolder();
				dsPath=sFolder+"\\"+szFile;
				::CopyFile(szPath,dsPath,FALSE);
				//MessageBox(sFolder, _T("GetSelectedFolder"), MB_ICONINFORMATION);
			} 	
		}
	}			
}

void CECQPSView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    CMainFrame *pMainWnd=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	
	CListCtrl& lcProcess = GetListCtrl();	
	
	UINT uFlags;
	
	int nItem = lcProcess.HitTest(point, &uFlags);
	
	if (uFlags & LVHT_ONITEM )
	{
		m_Tip.Show(lcProcess.GetItemText(nItem,PROCESS_PATH+1));
	}	
	
	CMultiColumnSortListView::OnLButtonDown(nFlags, point);
}

//检查隐藏进程
DWORD CECQPSView::CheckHiddenProc(LPPROCESSINFO32 lpProcInfo, ULONG ulNeeded,ULONG ulOp)
{
	//检查隐藏进程 w2k winxp 对截取ZwQuerySystemInformation的进程调用有效
	LINKED_LIST procList;
	DWORD dwCount;
	PROCINFO *pList = NULL;
	
	if(!getNativeAPIs())
	{
		AfxMessageBox("Failed to get addresses of Native APIs!\n");
		return ulNeeded;
	}

	initList(&procList);

	if(ulOp == 0x1){
		dwCount = getProcessList(&procList);
	}
	else
	{
		dwCount = getHandleProcessList(&procList);
		//TRACE1("getHandleProcessList:%d\n",dwCount);
	}

	
	if(dwCount <= 0)
		goto done;


	pList = (PROCINFO *)malloc(dwCount * sizeof(PROCINFO));
	if(pList)
	{
		LINKED_LIST_NODE *curNode = procList.head;
		DWORD i = 0;

		while(curNode)
		{
			PROCINFO *pInfo = (PROCINFO *)curNode->data;
			
			if(i >= dwCount)		// just in case
				break;

			pList[i].pId = pInfo->pId;
			pList[i].pFlag = pInfo->pFlag;
			memcpy(pList[i].pName, pInfo->pName, NT_PROCNAMELEN);
			//TRACE1("TRACE HiddenProc:%d,%5d,%16s\n",i,pInfo->pId,pInfo->pName);

			curNode = curNode->next;
			i++;
		}

        //比较进程列表
		for(DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
		{
			// walk through all processes
			//TRACE1("CheckHiddenProc:%d.",dwIndex);
			//TRACE1(" %d\n", pList[dwIndex].pId);
			//TRACE1("CheckHiddenProc:%d.",dwIndex);
			//TRACE1("CheckHiddenProc:%d\n", lpProcInfo[dwIndex+1].dwPID);
			BOOL foundProc = FALSE;
			for(DWORD ulIndex=0 ; ulIndex< ulNeeded;ulIndex++)
			{
				if(strlen(lpProcInfo[ulIndex].strName) > 15)		// to handle WinXP which gives long name
					lpProcInfo[ulIndex].strName[15] = 0;

				//TRACE1(" %d ",lpProcInfo[ulIndex].dwPID);

				if(stricmp(lpProcInfo[ulIndex].strName, pList[dwIndex].pName) == 0 &&
				   lpProcInfo[ulIndex].dwPID == pList[dwIndex].pId)
				{
					lpProcInfo[ulIndex].dwFlag=0x18;
					foundProc = TRUE;
					//TRACE1("Fountd:%d\n",pList[dwIndex].pId);
					break;
				}
			}
			if(!foundProc && ulNeeded <= dwCount){
				TRACE1("!foundProc:%s\n",pList[dwIndex].pName);
				TRACE1("!foundProc:%8X ",pList[dwIndex].pFlag);
				lpProcInfo[ulNeeded].dwPID=pList[dwIndex].pId;
				strcpy(lpProcInfo[ulNeeded].strName,pList[dwIndex].pName);
				if(pList[dwIndex].pFlag & 0x8)
					lpProcInfo[ulNeeded].dwFlag=0x8;
				else
					lpProcInfo[ulNeeded].dwFlag=0x10;

				strcpy(lpProcInfo[ulNeeded].strPath,"");
				GetHiddenProcMoreInfo(lpProcInfo,ulNeeded);
				ulNeeded++;
				//TRACE1("ulNeeded:%d",ulNeeded);
			}
		}
		
	}
	
done:
	freeList(&procList);
	if(pList)
		free(pList);

	return ulNeeded;
}

DWORD CECQPSView::GetAllProcNumFromKernel(ULONG ulNeeded,ULONG ulOp)
{
	LINKED_LIST procList;
	DWORD dwCount = 0;
	
	if(!getNativeAPIs())
	{
		AfxMessageBox("Failed to get addresses of Native APIs!\n");
		return ulNeeded;
	}

	initList(&procList);

	if(ulOp == 0x1){
		dwCount = getProcessList(&procList);
	}
	else
	{
		dwCount = getHandleProcessList(&procList);
		//TRACE1("getHandleProcessList:%d\n",dwCount);
	}

	
	if(dwCount <= 0)
		dwCount=ulNeeded-1;

	freeList(&procList);
	
	return dwCount;
	
}

void CECQPSView::UpdateNomalListCtrl(CString cType,LPPROCESSINFO32 lparrPsInfo, ULONG ulNeeded)
{
	extern CString	szSystemPath,szWindowPath;
	CString szPID,szThread,szIntro,szAlarm,szTmpName,szPath,szHidden;
	ULONG ulDanger=0;

	CListCtrl& lcProcess = GetListCtrl();	
	if(cType.CollateNoCase("UPDATE") == 0)
	{		
		m_CurrentPIdIndex.clear();
		for ( ULONG ulIndex = 0; ulIndex < ulNeeded; ulIndex++ ) {			
			szPID.Format("%d",lparrPsInfo[ulIndex].dwPID);
			m_CurrentPIdIndex[szPID]=NULL;
		}
		
		for(Process2Index::iterator pIx=m_ProcessIdIndex.begin();pIx!=m_ProcessIdIndex.end();pIx++)
		{
			if( m_CurrentPIdIndex.find(pIx->first) == m_CurrentPIdIndex.end() )
			{
				//////UpdateProcessList_Normal("INIT");
				lcProcess.DeleteItem(pIx->second);
			}
		}		
	}
	else{
		m_ProcessIdIndex.clear();
		lcProcess.DeleteAllItems();
	}
	
	
	///枚举当前进程
	for ( ULONG ulIndex = 0; ulIndex < ulNeeded; ulIndex++ ) {
		
		szPID.Format("%d",lparrPsInfo[ulIndex].dwPID);
		////判断是否新的进程
		Process2Index::iterator it = m_ProcessIdIndex.find(szPID);
		if (it == m_ProcessIdIndex.end())
		{
			szPath=lparrPsInfo[ulIndex].strPath;
			szPath.Replace("\\??\\","");
			szPath.Replace("\\SystemRoot",szWindowPath);
			
			szAlarm=GetProcessWarn(lparrPsInfo[ulIndex].strName,szPath,szIntro);
			
			int nIcon=GetProcessIcon(lparrPsInfo[ulIndex].strName,lparrPsInfo[ulIndex].strPath);
			CString szName  = lparrPsInfo[ulIndex].strName;
			CString       strCreateTime;
			COleDateTime  timeCreation;
			timeCreation = lparrPsInfo[ulIndex].ftCreation;
			
			strCreateTime.Format("%02d-%02d %02d:%02d:%02d", 
				timeCreation.GetMonth(),
				timeCreation.GetDay(),
				timeCreation.GetHour(),
				timeCreation.GetMinute(),
				timeCreation.GetSecond());
			if( lparrPsInfo[ulIndex].dwPID== 4 || lparrPsInfo[ulIndex].dwPID == 8 || lparrPsInfo[ulIndex].dwPID == 0 )
				strCreateTime="";
			
			
			CString  szPriority;
			switch(lparrPsInfo[ulIndex].dwPriority){
			case HIGH_PRIORITY_CLASS:
				szPriority="高";	
				break;
			case IDLE_PRIORITY_CLASS:
				szPriority="低";	
				break;
			case NORMAL_PRIORITY_CLASS:
				szPriority="正常";	
				break;
			case REALTIME_PRIORITY_CLASS:
				szPriority="实时";	
				break;
			default:
				szPriority="";	
				break;					
			}
			
			
			/*//排序是通过成员函数SortItems来进行的。SortItems成员函数有两个参数。第一个是排序所用的排序回调函数，第二个是需传入的参数。 
			排序回调函数是排序的关键。它有三个入口参数：第一和第二个为进行比较的两项的参数；第三参数为排序时传入的参数即SortItems中传入的参数。 
			比较过程是这样的： 
			如果，第一项应在第二项的前面，则返回一个负值； 
			如果，第一项应在第二项的后面，则返回一个正值； 
			否则，第一项与第二项相等，则返回零。 
			这里，需要注意的是，回调函数是在对什么进行比较。回调函数进行比较的两项，是在插入列表项的参数数据项。也就是说，在插入列表项时，必须设置参数项（LVIF_PARAM）。这样才能对列表项的每一项进行比较，而这往往是我们忽略的参数。
			*/////////////////////////////////////////
			int nItemInserted = lcProcess.InsertItem(
				LVIF_IMAGE | LVIF_TEXT, ulIndex, szName,
				INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, nIcon,NULL);
			
			UINT vIcon;
			if(szAlarm =="系统" )	vIcon=1;	
			else if(szAlarm =="危险" ) vIcon=3;		
			else if(szAlarm =="程序" )	vIcon=4;
			else{vIcon=2;szAlarm="可疑";}

			if(lparrPsInfo[ulIndex].dwFlag == 0x10)   //0x18正常 0x10隐藏 0x8销毁
			{
				szAlarm = "隐藏" ;
				vIcon=3;	
			}
			if(lparrPsInfo[ulIndex].dwFlag == 0x8)   //0x18正常 0x10隐藏 0x8销毁
			{
				szAlarm = "销毁" ;
				vIcon=4;	
			}

			lcProcess.SetItem(nItemInserted, 1,  LVIF_TEXT, szPID,0, 0, 0, NULL);
			lcProcess.SetItem(nItemInserted, 2,  LVIF_IMAGE|LVIF_TEXT , szAlarm,vIcon, 0, 0, NULL);
			
			lcProcess.SetItem(nItemInserted, 3,  LVIF_TEXT, szPriority,0, 0, 0, NULL);
			szThread.Format("%d",GetTotalThreadList(lparrPsInfo[ulIndex].dwPID));
			lcProcess.SetItem(nItemInserted, 4,  LVIF_TEXT, szThread,0, 0, 0, NULL);
			lcProcess.SetItem(nItemInserted, 5,  LVIF_TEXT, strCreateTime,0, 0, 0, NULL);
			lcProcess.SetItem(nItemInserted, 6,  LVIF_TEXT, szPath,0, 0, 0, NULL);
			lcProcess.SetItem(nItemInserted, 7,  LVIF_TEXT, szIntro,0, 0, 0, NULL);
			
			if(cType.CollateNoCase("UPDATE") == 0) ///针对新进程弹出提示框
			{
				CString szPopInfo;
				szPopInfo.Format("%s:%s\n\r启动:%s\n\r路径:%s",szAlarm,szName,strCreateTime,szPath);
				m_ctrlTaskBar.Show(szPopInfo);
				
			}			
			///保存当前新加激活的进程
			m_ProcessIdIndex[szPID]=nItemInserted; 
		}
	}	
	///设置状态栏
	
	CString strProcTotal,strProcDang;
	strProcTotal.Format( ID_INDICATOR_UPTIME,ulNeeded);
	for(int nd=0;nd<lcProcess.GetItemCount();nd++)
		if(lcProcess.GetItemText(nd,PROCESS_PID+1) != "系统" )
			ulDanger++;
	strProcDang.Format( ID_INDICATOR_NORMAL,ulDanger);
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndXPStatusBar.SetPaneText( 1, strProcTotal, TRUE );
	pMainFrame->m_wndXPStatusBar.SetPaneText( 2, strProcDang, TRUE );
}

void CECQPSView::UpdateProcessList_HiddenProc(ULONG ulOp)
{
	ULONG ulNeeded,ulSize;
	DWORD dwTotalSystemProc;
	EnumProcessesInfo_NT( NULL, 0, &ulNeeded );

	extern CString cWinVer;
	if(cWinVer.CompareNoCase("WindowsXP") ==0 )
		gKernelVersion = WINXP;
	else if(cWinVer.CompareNoCase("Windows2K") ==0 )
		gKernelVersion = WIN2K;
	else
		gKernelVersion = NOT_SUPPORTED;
	//获取内核总的进程数目
	//LPPROCESSINFO32 lparrPsInfo = new PROCESSINFO32[ulNeeded];
	//比较从内核获得的进程和从PSAPI获得进程，注意，从内核可以获得隐藏进程，加入到lparrPsInfo结构中
	//重要: EnumProcessesInfo_NT返回的进程中包含了sytem idle,但是从内核并不得到这个进程,因此比较的时候 ulNeeded-1
	dwTotalSystemProc=GetAllProcNumFromKernel(ulNeeded,ulOp);
	if(dwTotalSystemProc < ulNeeded){
		UpdateProcessList_Normal("INIT");
		return;
	}
		
	
	LPPROCESSINFO32 lparrPsInfo = new PROCESSINFO32[dwTotalSystemProc+1];
	if ( NULL == lparrPsInfo ) {
		TRACE("Alloc Memory Fail!");
		return;
	}
	ulSize=ulNeeded;
	EnumProcessesInfo_NT( lparrPsInfo, ulSize, &ulNeeded  );
	
	TRACE1("ulNeeded pre:%d",ulNeeded);
	ulNeeded=CheckHiddenProc(lparrPsInfo,ulNeeded,ulOp);
	TRACE1("ulNeeded back:%d\n",ulNeeded);
		
	UpdateNomalListCtrl("INIT",lparrPsInfo,ulNeeded);
	
	delete[] lparrPsInfo;	
	
}

void CECQPSView::UpdateProcessList_HiddenDriver()
{
	extern CString	szSystemPath,szWindowPath;
	LINKED_LIST drvList;
	DWORD count,ulIndex=0;
    ULONG cbBuffer = 0x8000;

	if(!getNativeAPIs())
	{
		AfxMessageBox("Failed to get addresses of Native APIs!\n");
		return ;
	}

	initList(&drvList);
	count = getDriverList(&drvList);
	TRACE1("\nTotal number of drivers = %d\n", count);

	CString szNo,szAd,szPath,szSize,szFile,szCop,szWarn="Null";
	CFileVersionInfo m_FileInfo;
	CListCtrl& lcDriver = GetListCtrl();	
	lcDriver.DeleteAllItems();
	LINKED_LIST_NODE *curNode = drvList.head;

	if(count <= 0)
		goto done;

	//TRACE("Driver/Module list by traversal of PsLoadedModuleList\n\n");
	//processMultipleItems(&drvList, compareDriverList, pBuffer);				

	while(curNode)
	{
		DRIVERINFO *dInfo = (DRIVERINFO *)curNode->data;
			
		szAd.Format("%X",dInfo->loadAddr);
		szNo.Format("%d",ulIndex);
		szPath.Format("%s",dInfo->drvName);
		szSize.Format("%d",dInfo->imageSize);
		szPath.Replace("\\??\\","");
		if(szPath.Find("\\WINDOWS\\system32") <1)
			szPath.Replace("\\WINDOWS\\system32",szSystemPath);
		szPath.Replace("\\SystemRoot",szWindowPath);
		int nIndex = szPath.ReverseFind('\\') +1 ;
		szFile = szPath.Right(szPath.GetLength()-nIndex);
		if(szPath.ReverseFind('\\') ==-1 ) szPath = szSystemPath + "\\drivers\\" +szPath;

		m_FileInfo.ReadVersionInfo(szPath);
		if (m_FileInfo.IsValid()){
			szCop = m_FileInfo.GetVersionInfo(SFI_COMPANYNAME);
			szWarn =m_FileInfo.GetVersionInfo(SFI_FILEVERSION);
		}

		int nItemInserted = lcDriver.InsertItem(
			LVIF_IMAGE | LVIF_TEXT, ulIndex++, szFile,
			INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, NULL,NULL);
		lcDriver.SetItem(nItemInserted, 1,  LVIF_TEXT, szNo,0, 0, 0, NULL);
		lcDriver.SetItem(nItemInserted, 2,  LVIF_TEXT, szCop,0, 0, 0, NULL);
		lcDriver.SetItem(nItemInserted, 3,  LVIF_TEXT , szAd,0, 0, 0, NULL);
		lcDriver.SetItem(nItemInserted, 4,  LVIF_TEXT , szSize,0, 0, 0, NULL);
		lcDriver.SetItem(nItemInserted, 5,  LVIF_TEXT , szWarn,0, 0, 0, NULL);
		lcDriver.SetItem(nItemInserted, 6,  LVIF_TEXT , szPath,0, 0, 0, NULL);
		//TRACE1("%s:%d\n",szName,ulIndex);
		curNode = curNode->next;
	}	

done:
	freeList(&drvList);
	return;
}
