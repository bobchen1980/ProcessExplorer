// dlgwarning.cpp : implementation file
//

#include "stdafx.h"
#include "..\ECQPS.h"
#include "dlgwarning.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWarning dialog


CDlgWarning::CDlgWarning(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWarning::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWarning)
	//}}AFX_DATA_INIT
}


void CDlgWarning::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWarning)
	DDX_Control(pDX, IDC_DLGWARNING_INFO, m_WarnInfo);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_OK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWarning, CDialog)
	//{{AFX_MSG_MAP(CDlgWarning)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWarning message handlers

BOOL CDlgWarning::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ModifyStyle( WS_CAPTION, WS_MINIMIZEBOX, SWP_DRAWFRAME );//…Ë÷√Õº±Í

	m_bmpBackground.LoadBitmap(IDB_DLGWARNING_BK);	/// º”‘ÿ±≥æ∞
	
	//m_OK.LoadBitmaps(IDB_DLGWARNING_CANCEL,IDB_DLGWARNING_CANCEL);	
	//m_Cancel.LoadBitmaps(IDB_DLGWARNING_OK,IDB_DLGWARNING_OK);	
	m_WarnInfo.SetTextColor(RGB(0,130,255));

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWarning::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
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



void CDlgWarning::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));
	
	CDialog::OnLButtonDown(nFlags, point);
}
