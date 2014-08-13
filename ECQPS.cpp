      // ECQPS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ECQPS.h"

#include "MainFrm.h"
#include "ECQPSDoc.h"
#include "ECQPSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CECQPSApp

BEGIN_MESSAGE_MAP(CECQPSApp, CWinApp)
	//{{AFX_MSG_MAP(CECQPSApp)
	//ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_TOOL_REFRESHE, ExeToolBarCommand(ID_TOOL_REFRESHE) )
	//ON_COMMAND(ID_TOOL_REFRESHE, ExeToolBarCommand(ID_TOOL_REFRESHE))
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CECQPSApp construction

CECQPSApp::CECQPSApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CECQPSApp object

CECQPSApp theApp;
CString cWinVer;
CString strExePath;
CString strDataName;
CString szWindowPath;
CString szSystemPath;

/////////////////////////////////////////////////////////////////////////////
// CECQPSApp initialization

BOOL CECQPSApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	//此程序只能运行一次，用互斥量来判断程序是否已运行
    HANDLE m_hMutex=CreateMutex(NULL,TRUE, m_pszAppName); 
    if(GetLastError()==ERROR_ALREADY_EXISTS) { return FALSE; }
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Ecq.Name"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	///安装全局钩子函数
	
	//读取DLL
	static HINSTANCE hinstDLL; 
	typedef BOOL (CALLBACK *_InstallKeyHook)(); 
	_InstallKeyHook InstallKeyHook;
	if(hinstDLL=LoadLibrary((LPCTSTR)"KeyHook.dll"))
	{
	   InstallKeyHook=(_InstallKeyHook)GetProcAddress(hinstDLL, "InstallKeyHook"); 
	   InstallKeyHook();
	}	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
    // Install the CoolControls 
    GetCtrlManager().InstallHook();	

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CECQPSDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CECQPSView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// Create the tray icon
	// Create the tray icon
	HICON mIcon=AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME) );
	
	if (!m_TrayIcon.Create(WM_ICON_NOTIFY,			// Icon notify message to use
		_T("ECQ-PS [WWW.ECQ.NAME]"),		// tooltip
		mIcon,		// Icon
		IDR_TRAYMENU))	
		return -1;
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE); 
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();             

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	HICON m_hIcon;
	CBitmap m_bmpBackground;

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CBitmapButton	m_About_Web;
	CBitmapButton	m_About_Mail;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDlgboutWeb();
	afx_msg void OnDlgboutMail();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDB_DLGBOUT_WEB, m_About_Web);
	DDX_Control(pDX, IDB_DLGBOUT_MAIL, m_About_Mail);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDB_DLGBOUT_WEB, OnDlgboutWeb)
	ON_BN_CLICKED(IDB_DLGBOUT_MAIL, OnDlgboutMail)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CECQPSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CECQPSApp message handlers
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ModifyStyle( WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME );//设置图标

	m_bmpBackground.LoadBitmap(IDB_DLGBOUT_BK);	/// 加载背景
	m_About_Web.LoadBitmaps(IDB_DLGBOUT_WEB,0,0,IDB_DLGBOUT_WEB);
	m_About_Mail.LoadBitmaps(IDB_DLGBOUT_MAIL,0,0,IDB_DLGBOUT_MAIL);

	SetWindowPos(NULL,300,210,300,210,SWP_SHOWWINDOW);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




void CAboutDlg::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
//		CDialog::OnPaint();
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		CDC dcMem; 
		dcMem.CreateCompatibleDC(&dc); 
		BITMAP bitMap;
		m_bmpBackground.GetBitmap(&bitMap);
		CBitmap *pbmpOld=dcMem.SelectObject(&m_bmpBackground);
		dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,bitMap.bmWidth,bitMap.bmHeight,SRCCOPY);
		//dc.BitBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,SRCCOPY);
	}	
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));
	OnOK();
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CAboutDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CAboutDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnOK();
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CAboutDlg::OnDlgboutWeb() 
{
	// TODO: Add your control notification handler code here
	::ShellExecute(NULL, "open","http://www.ecq.name", NULL, NULL, SW_SHOWNORMAL);
	
}

void CAboutDlg::OnDlgboutMail() 
{
	// TODO: Add your control notification handler code here
	::ShellExecute(NULL, "open","mailto:cxl@mail.ioa.ac.cn", NULL, NULL, SW_SHOWNORMAL);
	
}

int CECQPSApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	//AfxMessageBox("ExitInstance");
	return CWinApp::ExitInstance();
}



BOOL CECQPSApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CAboutDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnOK();
	
	CDialog::OnLButtonUp(nFlags, point);
}
