# Microsoft Developer Studio Project File - Name="ECQPS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ECQPS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ECQPS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ECQPS.mak" CFG="ECQPS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ECQPS - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ECQPS - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ECQPS - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\ExeBin"
# PROP Intermediate_Dir "..\ExeBin\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "WINNT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"..\ExeBin/ECQ-PS.exe"

!ELSEIF  "$(CFG)" == "ECQPS - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\ExeBin"
# PROP Intermediate_Dir "..\ExeBin\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "WINNT" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"..\ExeBin/ECQ-PSD.exe" /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "ECQPS - Win32 Release"
# Name "ECQPS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Group "MutipleSort"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\MutipleSort\MultiColumnSortListView.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\MutipleSort\SortableHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\MutipleSort\SortClass.cpp
# End Source File
# End Group
# Begin Group "XpMenu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XPMenu\BCMenu.cpp
# End Source File
# End Group
# Begin Group "XPTools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XPTools\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\XPTools\StatusBarXP.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\XPTools\ToolBarXP.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\XPTools\Tools.cpp
# End Source File
# End Group
# Begin Group "SCBBar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\SCBBar\CoolTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\SCBBar\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\SCBBar\sizecbar.cpp
# End Source File
# End Group
# Begin Group "FileFunc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\FileFunc\file_ver.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\FileFunc\StringFile.cpp
# End Source File
# End Group
# Begin Group "SortList"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\SortListCtrl\SortHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\SortListCtrl\SortListCtrl.cpp
# End Source File
# End Group
# Begin Group "MenuList"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XpList\menulistctrl.cpp
# ADD CPP /I "..\.."
# End Source File
# End Group
# Begin Group "Xplist"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XpList\MyTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\XpList\Popup.cpp
# End Source File
# End Group
# End Group
# Begin Group "Process"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Process\HiddenProcLinkedList.cpp
# End Source File
# Begin Source File

SOURCE=.\Process\perfmon.cpp
# End Source File
# Begin Source File

SOURCE=.\Process\ProcessFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\Process\NtSystemInfo\SystemInfo.cpp
# End Source File
# End Group
# Begin Group "General"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ECQPS.cpp
# End Source File
# Begin Source File

SOURCE=.\ECQPSDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ECQPSView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# End Group
# Begin Group "Registry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Registry\Registry.cpp
# End Source File
# End Group
# Begin Group "Pro2Port"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\Pro2Port.cpp
# End Source File
# End Group
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dialog\DlgModuleLocked.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog\dlgregboot.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dialog\dlgservice.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Dialog\dlgwarning.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Service\EnumNtSrv.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog\FileDialogST.cpp
# End Source File
# End Group
# Begin Group "FloatControl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\FloatControl\CoolControlsManager.cpp
# End Source File
# End Group
# Begin Group "StaticEdit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\StaticEdit\ColorStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\StaticEdit\editex.cpp
# End Source File
# End Group
# Begin Group "SystemTray"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\SysTray\SystemTray.cpp
# End Source File
# End Group
# Begin Group "TrayBool"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\TrayBool\TaskbarNotifier.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\TrayBool\XInfoTip.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ECQPS.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Controls Header"

# PROP Default_Filter ""
# Begin Group "MutipleSort Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\MutipleSort\MultiColumnSortListView.h
# End Source File
# Begin Source File

SOURCE=.\Controls\MutipleSort\SortableHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Controls\MutipleSort\SortClass.h
# End Source File
# End Group
# Begin Group "XpMenu Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XPMenu\BCMenu.h
# End Source File
# End Group
# Begin Group "XpTools Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XPTools\Draw.h
# End Source File
# Begin Source File

SOURCE=.\Controls\XPTools\StatusBarXP.h
# End Source File
# Begin Source File

SOURCE=.\Controls\XPTools\ToolbarXP.h
# End Source File
# Begin Source File

SOURCE=.\Controls\XPTools\Tools.h
# End Source File
# End Group
# Begin Group "SCBBar Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\SCBBar\CoolTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Controls\SCBBar\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\Controls\SCBBar\sizecbar.h
# End Source File
# End Group
# Begin Group "FileFunc Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\FileFunc\file_ver.h
# End Source File
# Begin Source File

SOURCE=.\Controls\FileFunc\StringFile.h
# End Source File
# End Group
# Begin Group "SortList Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\SortListCtrl\SortHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Controls\SortListCtrl\SortListCtrl.H
# End Source File
# End Group
# Begin Group "TrayBool Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\TrayBool\TaskbarNotifier.h
# End Source File
# Begin Source File

SOURCE=.\Controls\TrayBool\XInfoTip.h
# End Source File
# End Group
# Begin Group "XpList Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XpList\MyTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Controls\XpList\Popup.h
# End Source File
# End Group
# End Group
# Begin Group "Process Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Process\HiddenProcCheck.h
# End Source File
# Begin Source File

SOURCE=.\Process\HiddenProcLinkedList.h
# End Source File
# Begin Source File

SOURCE=.\Process\pdh_def.H
# End Source File
# Begin Source File

SOURCE=.\Process\perfmon.h
# End Source File
# Begin Source File

SOURCE=.\Process\ProcessFunction.h
# End Source File
# Begin Source File

SOURCE=.\Process\NtSystemInfo\SystemInfo.h
# End Source File
# End Group
# Begin Group "General Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ECQPS.h
# End Source File
# Begin Source File

SOURCE=.\ECQPSDoc.h
# End Source File
# Begin Source File

SOURCE=.\ECQPSView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# End Group
# Begin Group "Registry Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Registry\Registry.h
# End Source File
# End Group
# Begin Group "Pro2Port Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\Pro2Port.h
# End Source File
# End Group
# Begin Group "Dialog Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dialog\DlgModuleLocked.h
# End Source File
# Begin Source File

SOURCE=.\Dialog\dlgregboot.h
# End Source File
# Begin Source File

SOURCE=.\Dialog\dlgservice.h
# End Source File
# Begin Source File

SOURCE=.\Dialog\dlgwarning.h
# End Source File
# Begin Source File

SOURCE=.\Service\EnumNtSrv.h
# End Source File
# Begin Source File

SOURCE=.\Dialog\FileDialogST.h
# End Source File
# End Group
# Begin Group "FloatControl Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\FloatControl\CoolControlsManager.h
# End Source File
# End Group
# Begin Group "StaticEdit Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\StaticEdit\ColorStatic.h
# End Source File
# Begin Source File

SOURCE=.\Controls\StaticEdit\editex.h
# End Source File
# Begin Source File

SOURCE=.\Controls\StaticEdit\logfont.h
# End Source File
# End Group
# Begin Group "SystemTray Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\SysTray\SystemTray.h
# End Source File
# End Group
# Begin Group "MenuList Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\XpList\menulistctrl.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "BMP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\arrowdow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ArrowUp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\blank.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\10.ico
# End Source File
# Begin Source File

SOURCE=.\res\35.ico
# End Source File
# Begin Source File

SOURCE=.\res\autokill.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ballonbk.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\blue.ico
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Clientinfo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dglwarning.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dglwarning_cl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dglwarning_ok.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dlgabout.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dlgabout_mail.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dlgabout_web.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ECQPS.ico
# End Source File
# Begin Source File

SOURCE=.\res\ECQPS.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ECQPSDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_not.ico
# End Source File
# Begin Source File

SOURCE=.\res\images.bmp
# End Source File
# Begin Source File

SOURCE=.\res\listctxmenu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\locked.ico
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logosel.bmp
# End Source File
# Begin Source File

SOURCE=".\res\MS-DOS Batch File.ico"
# End Source File
# Begin Source File

SOURCE=.\res\pe.ico
# End Source File
# Begin Source File

SOURCE=.\res\perfor.ico
# End Source File
# Begin Source File

SOURCE=.\res\rootfile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\safe.ico
# End Source File
# Begin Source File

SOURCE=.\res\smimages.bmp
# End Source File
# Begin Source File

SOURCE=.\res\systray.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\virus.ico
# End Source File
# Begin Source File

SOURCE=.\res\warn.ico
# End Source File
# Begin Source File

SOURCE=.\res\warning.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
