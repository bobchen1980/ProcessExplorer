// stdafx.cpp : source file that includes just the standard includes
//	ECQPS.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CString JudgeOperatingSystemType()
{
	OSVERSIONINFO OsVersionInfo;
	OsVersionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&OsVersionInfo);
	if(OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		if(OsVersionInfo.dwMajorVersion>4) return "Windows98";
		else if(OsVersionInfo.dwMajorVersion=4)
		{
			if(OsVersionInfo.dwMinorVersion>0) return "Windows98";
			else return "Windows95";
		}
		else return "Windows3.1";
	}
	else if( OsVersionInfo.dwPlatformId== VER_PLATFORM_WIN32_NT )
	{
		if ( OsVersionInfo.dwMajorVersion == 5 && OsVersionInfo.dwMinorVersion == 2 )
            return "Windows2003";
		
		if ( OsVersionInfo.dwMajorVersion == 5 && OsVersionInfo.dwMinorVersion == 1 )
            return "WindowsXP";
		
		if ( OsVersionInfo.dwMajorVersion == 5 && OsVersionInfo.dwMinorVersion == 0 )
            return "Windows2K";
		
		if ( OsVersionInfo.dwMajorVersion <= 4 )
            return "WindowsNT";

		return "WindowsXP";

	}
	else if(OsVersionInfo.dwPlatformId== VER_PLATFORM_WIN32s)
	{
		return "Windows3.1";
	}
	else return "NoName";
}
//get the style of the list view
DWORD GetListViewStyle()
{
	DWORD dwListViewStyle=0;
	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("FlatScrollBar"),NO)==YES)
	     dwListViewStyle|=LVS_EX_FLATSB;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("FullRowSelect"),NO)==YES)
	     dwListViewStyle|=LVS_EX_FULLROWSELECT;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("Gridlines"),NO)==YES)
	     dwListViewStyle|=LVS_EX_GRIDLINES;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("HeaderDragDrop"),NO)==YES)
	     dwListViewStyle|=LVS_EX_HEADERDRAGDROP;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("OneClickActivate"),NO)==YES)
	     dwListViewStyle|=LVS_EX_ONECLICKACTIVATE;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("TrackSelect"),NO)==YES)
	     dwListViewStyle|=LVS_EX_TRACKSELECT;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("TwoClickActivate"),NO)==YES)
	     dwListViewStyle|=LVS_EX_TWOCLICKACTIVATE;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("UnderlineCold"),NO)==YES)
	     dwListViewStyle|=LVS_EX_UNDERLINECOLD;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("UnderlineHot"),NO)==YES)
	     dwListViewStyle|=LVS_EX_UNDERLINEHOT;

	return dwListViewStyle;
}

//is or not the the scroll bar of listview flat
BOOL ListHasFlatSB()
{
	BOOL bHasFlatSB;
	return bHasFlatSB=(AfxGetApp()->GetProfileInt(_T("Settings"),
		_T("FlatScrollBar"),NO)==YES)?TRUE:FALSE;
}

//get the dock bar docking style
DWORD GetBarDockStyle(int ibar)
{
 DWORD dwDockStyle=0;
 if (ibar==FOR_TOOLBAR)
 {
	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("DockAny"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_ANY;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("DockBottom"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_BOTTOM;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("DockLeft"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_LEFT;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("DockMultiple"),YES)==YES)
	     dwDockStyle|=CBRS_FLOAT_MULTI;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("DockTop"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_TOP;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("DockRight"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_RIGHT;
 }
 else if(ibar==FOR_CONFIGBAR)
 {
	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("ConfigDockAny"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_ANY;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("ConfigDockBottom"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_BOTTOM;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("ConfigDockLeft"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_LEFT;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("ConfigDockMutiple"),YES)==YES)
	     dwDockStyle|=CBRS_FLOAT_MULTI;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("ConfigDockTop"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_TOP;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("ConfigDockRight"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_RIGHT;
 }
 else if(ibar==FOR_INFOBAR)
 {
	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("InfoBarDockAny"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_ANY;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("InfoBarDockBottom"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_BOTTOM;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("InfoBarDockLeft"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_LEFT;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("InfoBarDockMutiple"),YES)==YES)
	     dwDockStyle|=CBRS_FLOAT_MULTI;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("InfoBarDockTop"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_TOP;

	if(AfxGetApp()->GetProfileInt(_T("Settings"),
			  _T("InfoBarDockRight"),YES)==YES)
	     dwDockStyle|=CBRS_ALIGN_RIGHT;
 }
 return dwDockStyle;
}