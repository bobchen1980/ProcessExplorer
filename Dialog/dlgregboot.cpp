// dlgregboot.cpp : implementation file
//

#include "stdafx.h"
#include "..\ECQPS.h"
#include "dlgregboot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRegBoot dialog


CDlgRegBoot::CDlgRegBoot(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRegBoot::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRegBoot)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgRegBoot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRegBoot)
	DDX_Control(pDX, IDC_REGBOOT_LIST, m_List);
	DDX_Control(pDX, IDC_REGBOOT_DELETE, m_btnDelete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRegBoot, CDialog)
	//{{AFX_MSG_MAP(CDlgRegBoot)
	ON_BN_CLICKED(IDC_REGBOOT_DELETE, OnRegbootDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRegBoot message handlers

void CDlgRegBoot::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
}

void CDlgRegBoot::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDlgRegBoot::OnRegbootDelete() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CString szTmp;
	szTmp.Format("是否删除系统启动项!");

	if(MessageBox(szTmp,"警告：删除系统启动项",MB_YESNO|MB_DEFBUTTON2) != IDYES)
		return;

	CString cRegName,cSubKey;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	else
	{
		while (pos)
		{
			int nItem = m_List.GetNextSelectedItem(pos);
			
			for(int i=0;i<RegCount;i++){
				cRegName=m_List.GetItemText(nItem,0);
					
				// 删除注册表建制
				CRegistry Reg= cBootReg[i].mKey;
				Reg.Open (cBootReg[i].mSub);
				Reg.DeleteValue(cRegName);
			}
			
		}
	}	

	m_List.DeleteAllItems();
	for(int i=0;i<RegCount;i++){
		LoadRegBoot(cBootReg[i].mKey,cBootReg[i].mSub);
	}	
}

BOOL CDlgRegBoot::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// TODO: Add extra initialization here
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_FLATSB);
	m_List.SetHeadings( _T("启动名称,120;启动路径,250") ); 
	m_List.LoadColumnInfo();          
	FlatSB_EnableScrollBar(m_List,SB_BOTH,ESB_ENABLE_BOTH);

	cBootReg[0].mKey=HKEY_CURRENT_USER;
	cBootReg[0].mSub="Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	cBootReg[1].mKey=HKEY_LOCAL_MACHINE;
	cBootReg[1].mSub="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	cBootReg[2].mKey=HKEY_LOCAL_MACHINE;
	cBootReg[2].mSub="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
	cBootReg[3].mKey=HKEY_LOCAL_MACHINE;
	cBootReg[3].mSub="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx";
	cBootReg[4].mKey=HKEY_LOCAL_MACHINE;
	cBootReg[4].mSub="SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices";

	for(int i=0;i<RegCount;i++){
		LoadRegBoot(cBootReg[i].mKey,cBootReg[i].mSub);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRegBoot::LoadRegBoot(HKEY hmKey,CString cSubKey)
{
	CRegistry Reg= hmKey;
	BOOL bReturn;
	CStringArray cRegName;
	CString cRegValue;
	bReturn=Reg.Open (cSubKey);
	bReturn=Reg.EnumValueName(cRegName);
	for(int i=0;i<cRegName.GetSize();i++){
		bReturn=Reg.Read (cRegName[i],&cRegValue);
		//TRACE(cRegName[i]);
		m_List.AddItem(cRegName[i],cRegValue);
	}

}