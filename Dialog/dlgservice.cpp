// dlgservice.cpp : implementation file
//

#include "stdafx.h"
#include "..\ECQPS.h"
#include "dlgservice.h"
#include "..\Service/EnumNtSrv.h"
#include <Winsvc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgService dialog


CDlgService::CDlgService(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgService::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgService)
	m_IsAll = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgService::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgService)
	DDX_Control(pDX, IDC_SERVICE_DRIVER, m_btnViewDriver);
	DDX_Control(pDX, IDC_LISTSERVICE, m_List);
	DDX_Check(pDX, IDC_SERVICE_ALL, m_IsAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgService, CDialog)
	//{{AFX_MSG_MAP(CDlgService)
	ON_BN_CLICKED(IDC_SERVICE_RERFRSH, OnServiceRerfrsh)
	ON_BN_CLICKED(IDC_SERVICE_DELETE, OnServiceDelete)
	ON_BN_CLICKED(IDC_SERVICE_DRIVER, OnServiceDriver)
	ON_BN_CLICKED(IDC_SERVICE_ALL, OnServiceAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgService message handlers

void CDlgService::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CDlgService::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CDlgService::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_IsAll=FALSE;
	m_IsService=TRUE;
	// TODO: Add extra initialization here
	////����ListctrlΪ������ʽ
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	m_List.SetHeadings( _T("����,80;����,120;����,40;����,40;·��,150") ); ///������ͷ��Ϣ
	m_List.LoadColumnInfo();          
	FlatSB_EnableScrollBar(m_List,SB_BOTH,ESB_ENABLE_BOTH);

	UpdateData(FALSE);
	// Sort the list according to the Name column	
	LoadNTServices(	SERVICE_WIN32,SERVICE_ACTIVE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CDlgService::LoadNTServices(DWORD dwType,DWORD dwState)
{

	TNtServiceInfoList srvList;
	TNtServiceInfo::EnumServices(dwType, dwState, &srvList);
	
	m_List.DeleteAllItems();
	int nIndex=0;
	TNtServiceInfoList::iterator it;
	for (it = srvList.begin(); it != srvList.end(); it ++) {
		///SortList�÷�
		m_List.AddItem(it->m_strServiceName,it->m_strDisplayName,it->GetServiceType(),it->GetStartType(),it->m_strBinaryPath);
	}

}

void CDlgService::OnServiceRerfrsh() 
{
	// TODO: Add your control notification handler code here
	m_List.DeleteAllItems();
	LoadNTServices(	SERVICE_WIN32,SERVICE_ACTIVE);
	
}

void CDlgService::OnServiceDelete() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CString m_ServiceName;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	else
	{
		while (pos)
		{
			int nItem = m_List.GetNextSelectedItem(pos);
			TRACE1("Item %d was selected!\n", nItem);
			m_ServiceName=m_List.GetItemText(nItem,0);
			// you could do your own processing on nItem here
		}
	}
	if(MessageBox("�Ƿ�ȷ��Ҫɾ��2Kƽ̨ϵͳ����?\n������ѡ��,����ɾ����ֻ����װ����!","���棺ɾ��ϵͳ����",MB_YESNO|MB_DEFBUTTON2) != IDYES)
		return;

	DWORD dwDesiredAccess = SERVICE_ALL_ACCESS|GENERIC_WRITE;
	
	// First open the handle to the SCM
	SC_HANDLE schSCManager = OpenSCManager( 
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		GENERIC_WRITE);  // full access rights 

	if (schSCManager == NULL) 
	{
		AfxMessageBox("���󣺲��ܴ򿪷���!",MB_OK|MB_ICONSTOP);
		return;
	}
	
	// Now open the handle to the service
	SC_HANDLE myServiceHandle = OpenService(schSCManager,
		m_ServiceName.GetBuffer(0),
		dwDesiredAccess);
	
	if (myServiceHandle == NULL) 
	{
		return;
	}
	else
	{
		// Ok delete the service
		if(!DeleteService(myServiceHandle))
		{
			DWORD er = GetLastError();
			switch(er)
			{
			case ERROR_ACCESS_DENIED: AfxMessageBox("����Ȩ�޲���!");break;
			case ERROR_SERVICE_MARKED_FOR_DELETE: AfxMessageBox("�Ѿ�ɾ������!");break;
			default:AfxMessageBox("δ֪��������!");break;
			}
			return;
		}
		AfxMessageBox("��ϲ�������Ѿ������ɾ��!\r�����ϵͳ���񣬽�������!");
	}

	CloseServiceHandle(schSCManager); // close the handle to SCM
	CloseServiceHandle(myServiceHandle); // close the handle to service, we will reopen it if we need it further
	myServiceHandle = 0;
	schSCManager = 0;

	m_List.DeleteAllItems();
	LoadNTServices(	SERVICE_WIN32,SERVICE_ACTIVE);	
}

void CDlgService::OnServiceDriver() 
{
	// TODO: Add your control notification handler code here
	m_List.DeleteAllItems();
	if(m_IsService){
		LoadNTServices(	SERVICE_DRIVER,SERVICE_ACTIVE|SERVICE_INACTIVE);
		m_btnViewDriver.SetWindowText("�鿴����");
		m_IsService=FALSE;
	}
	else{
		LoadNTServices(	SERVICE_WIN32,SERVICE_ACTIVE);
		m_btnViewDriver.SetWindowText("�鿴����");
		m_IsService=TRUE;
	}
	
}

void CDlgService::OnServiceAll() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	m_List.DeleteAllItems();
	UpdateData(FALSE);
	if(m_IsAll){
		LoadNTServices(	SERVICE_WIN32,SERVICE_ACTIVE|SERVICE_INACTIVE);
		m_IsAll=FALSE;
	}
	else{
		LoadNTServices(	SERVICE_WIN32,SERVICE_ACTIVE);
		m_IsAll=TRUE;
	}	
}
