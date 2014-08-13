// DlgModuleLocked.cpp : implementation file
//

#include "stdafx.h"
#include "../ECQPS.h"
#include "DlgModuleLocked.h"
#include "../Process/NtSystemInfo/SystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgModuleLocked dialog


CDlgModuleLocked::CDlgModuleLocked(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModuleLocked::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgModuleLocked)
	m_strMLName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgModuleLocked::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgModuleLocked)
	DDX_Control(pDX, IDC_MODULE_LIST, m_List);
	DDX_Text(pDX, IDC_DLGMODULE_NAME, m_strMLName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgModuleLocked, CDialog)
	//{{AFX_MSG_MAP(CDlgModuleLocked)
	ON_BN_CLICKED(IDC_DLGMODULE_DELETE, OnDlgmoduleDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgModuleLocked message handlers

void CDlgModuleLocked::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDlgModuleLocked::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDlgModuleLocked::OnDlgmoduleDelete() 
{
	// TODO: Add your control notification handler code here
	if(m_IsAllSafe){
		AfxMessageBox("该模块被系统关键进程所锁定，直接删除将引起系统崩溃!\n\r如果该模块不是微软发布的，可以到安全模式直接删除文件!");
		return;
	}

	CString szTmp;
	szTmp.Format("是否要删除进程中的模块?所有列举的进程都将被关闭!");

	if(MessageBox(szTmp,"警告：删除进程模块",MB_YESNO|MB_DEFBUTTON2) != IDYES)
		return;

	for(int i=0;i<m_List.GetItemCount();i++)
	{
		KillProcess(atol(m_List.GetItemText(i,0)));
	}
	//删除文件 
	Sleep(500);
	try{
		::DeleteFile(m_strMLPath);
	}catch (...) {	}
	//删除文件 
	Sleep(500);
	try{
		::DeleteFile(m_strMLPath);
	}catch (...) {	}
	
	OnCancel() ;

}
BOOL CDlgModuleLocked::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// TODO: Add extra initialization here
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	m_List.SetHeadings( _T("锁定ID,80;锁定进程,150") ); 
	m_List.LoadColumnInfo();          
	FlatSB_EnableScrollBar(m_List,SB_BOTH,ESB_ENABLE_BOTH);
	
	//AfxMessageBox(m_strMLPath);
	m_IsAllSafe=FALSE;
	if(m_strMLPath.GetLength() > 0 )
		WhoUsesModule( m_strMLPath, TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LPCTSTR CDlgModuleLocked::GetFileNamePosition( LPCTSTR lpPath )
{
	LPCTSTR lpAct = lpPath + _tcslen( lpPath );

	while ( lpAct > lpPath && *lpAct != _T('\\') && *lpAct != _T('/') )
		lpAct--;

	if ( lpAct > lpPath )
		lpAct++;

	return lpAct;
}

void CDlgModuleLocked::WhoUsesModule( LPCTSTR lpFileName, BOOL bFullPathCheck )
{
	CString processName;
	BOOL bShow = FALSE;
	SystemProcessInformation::SYSTEM_PROCESS_INFORMATION* p;
	
	SystemProcessInformation pi;
	SystemModuleInformation mi;

	if ( !mi.Refresh() )
	{
		TRACE( _T("SystemModulesInformation::Refresh() failed.\n") );
		return;
	}

	if ( mi.m_ModuleInfos.GetHeadPosition() == NULL )
	{
		TRACE( _T("No module information\n") );
		return;
	}

	pi.Refresh();
			
	TRACE( _T("%-6s  %-20s  %s\n"), _T("PID"), _T("Name"), _T("Path") );
	TRACE( _T("------------------------------------------------------------------\n") );

	for ( POSITION pos = mi.m_ModuleInfos.GetHeadPosition(); pos != NULL; )
	{
		SystemModuleInformation::MODULE_INFO& m = mi.m_ModuleInfos.GetNext(pos);

		if ( bFullPathCheck )
			bShow =	_tcsicmp( m.FullPath, lpFileName ) == 0;
		else
			bShow =	_tcsicmp( GetFileNamePosition(m.FullPath), lpFileName ) == 0;
		
		if ( bShow )
		{
			if ( pi.m_ProcessInfos.Lookup( m.ProcessId, p ) )
			{
				SystemInfoUtils::Unicode2CString( &p->usName, processName );
			}
			else
				processName = "";
				
				CString szPID;
				szPID.Format("%d",m.ProcessId);
				
				m_List.AddItem(szPID,processName);

				if(processName.CompareNoCase(_T("csrss.exe")) ==0 || processName.CompareNoCase(_T("winlogon.exe")) ==0 )
					m_IsAllSafe=TRUE;

				//TRACE( _T("0x%04X  %-20s  %s\n"), 
				//	m.ProcessId, 
				///	processName, 
				//	m.FullPath );
		}
	}
}

