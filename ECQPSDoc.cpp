        // ECQPSDoc.cpp : implementation of the CECQPSDoc class
//

#include "stdafx.h"
#include "ECQPS.h"
#include "MainFrm.h"
#include "ECQPSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CECQPSDoc

IMPLEMENT_DYNCREATE(CECQPSDoc, CDocument)

BEGIN_MESSAGE_MAP(CECQPSDoc, CDocument)
	//{{AFX_MSG_MAP(CECQPSDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CECQPSDoc construction/destruction

CECQPSDoc::CECQPSDoc()
{
	// TODO: add one-time construction code here
}

CECQPSDoc::~CECQPSDoc()
{
}

BOOL CECQPSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	///新建文档的时候判断操作系统的类型

	extern CString cWinVer;
	cWinVer=JudgeOperatingSystemType();

	//Update UpdateProcessPrivilege, 
	//If Not Update, You Can't Kill System Process
	if ( UpdateProcessPrivilege( ::GetCurrentProcess(), SE_DEBUG_NAME ) ) {
		TRACE("UpdateProcessPrivilege Succeed!\r\n");
	}
	else {
		TRACE("Fail!");
	}
	
	InitExePath("NULL");

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CECQPSDoc serialization

void CECQPSDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CECQPSDoc diagnostics

#ifdef _DEBUG
void CECQPSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CECQPSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CECQPSDoc commands

BOOL CECQPSDoc::InitExePath(CString dPath)
{
	///初始程序路径
	TCHAR exeFullpath[MAX_PATH];
	GetModuleFileName(NULL,exeFullpath,MAX_PATH);
	CString strTmp;
	extern CString strExePath;
	extern CString strDataName;
	strExePath.Format("%s",exeFullpath);
	strExePath.MakeUpper();
	strTmp=strExePath.Mid(strExePath.ReverseFind('\\'),MAX_PATH);
	strExePath.Replace(strTmp,"");

	strDataName=strExePath+"\\Process.dat";
	//AfxMessageBox(strExePath);
	extern CString	szSystemPath,szWindowPath;
	char lpszSystemInfo[MAX_PATH],lpszWindowInfo[MAX_PATH];

	if( GetSystemDirectory(lpszSystemInfo, MAX_PATH) ) 
		szSystemPath.Format("%s",lpszSystemInfo);
    if( GetWindowsDirectory(lpszWindowInfo, MAX_PATH) )
		szWindowPath.Format("%s",lpszWindowInfo);
	szSystemPath.MakeUpper();
	szWindowPath.MakeUpper();

	return TRUE;
}

void CECQPSDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	//AfxMessageBox("OnCloseDocument");
	CDocument::OnCloseDocument();
}
