// dlgrootkitsys.cpp : implementation file
//


#include "stdafx.h"
#include "ECQPS.h"
#include "dlgrootkitsys.h"

//#include "Process\HiddenProcCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRootkitSys dialog


CDlgRootkitSys::CDlgRootkitSys(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRootkitSys::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRootkitSys)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgRootkitSys::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRootkitSys)
	DDX_Control(pDX, IDC_LISTROOTSYS, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRootkitSys, CDialog)
	//{{AFX_MSG_MAP(CDlgRootkitSys)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRootkitSys message handlers

void CDlgRootkitSys::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CDlgRootkitSys::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

BOOL CDlgRootkitSys::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	////����ListctrlΪ������ʽ
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	m_List.SetHeadings( _T("���,40;�ļ�����,450") ); ///������ͷ��Ϣ
	m_List.LoadColumnInfo();          
	FlatSB_EnableScrollBar(m_List,SB_BOTH,ESB_ENABLE_BOTH);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRootkitSys::CheckHiddenDriver()
{

}
