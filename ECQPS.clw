; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ECQPS.h"
LastPage=0

ClassCount=11
Class1=CECQPSApp
Class2=CECQPSDoc
Class3=CECQPSView
Class4=CMainFrame

ResourceCount=12
Resource1=IDR_POPMENU
Class5=CAboutDlg
Resource2=IDD_DLGREGBOOT
Resource3=IDR_POPMENUML
Resource4=IDD_ABOUTBOX
Resource5=IDD_DLGMODULELK
Resource6=IDR_MAINMENU
Resource7=IDR_MENULBAR
Class6=CDlgService
Resource8=IDD_DLGSERVICE
Class7=CDlgRegBoot
Resource9=IDR_POPMENUBAR
Class8=CDlgWarning
Class9=CMenuListCtrl
Resource10=IDR_TRAYMENU
Resource11=IDD_DLGWARNING
Class10=CDlgModuleLocked
Class11=CDlgRootkitSys
Resource12=IDR_MAINFRAME

[CLS:CECQPSApp]
Type=0
HeaderFile=ECQPS.h
ImplementationFile=ECQPS.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CECQPSDoc]
Type=0
HeaderFile=ECQPSDoc.h
ImplementationFile=ECQPSDoc.cpp
Filter=W
BaseClass=CDocument
VirtualFilter=DC

[CLS:CECQPSView]
Type=0
HeaderFile=ECQPSView.h
ImplementationFile=ECQPSView.cpp
Filter=W
BaseClass=CMultiColumnSortListView
VirtualFilter=VWC
LastObject=ID_POPMENU_DELETE


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=W
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=ID_MAINMENU_TOP




[CLS:CAboutDlg]
Type=0
HeaderFile=ECQPS.cpp
ImplementationFile=ECQPS.cpp
Filter=W
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDB_DLGBOUT_WEB

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=2
Control1=IDB_DLGBOUT_WEB,button,1342242955
Control2=IDB_DLGBOUT_MAIL,button,1342242955

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_TOOL_REFRESHE
Command2=ID_TOOL_KILL
Command3=ID_TOOL_NORMAL
Command4=ID_TOOL_PERFORMANCE
Command5=ID_TOOL_NETWORK
Command6=ID_TOOL_SERVICE
Command7=ID_TOOL_BOOT
Command8=ID_TOOL_CONSOLE
Command9=ID_TOOL_REGEDIT
Command10=ID_TOOL_NOTPAD
Command11=ID_TOOL_COMPUTEE
Command12=ID_TOOL_HOME
Command13=ID_TOOL_HELP
Command14=ID_TOOL_ABOUT
Command15=ID_TOOL_HIDDEN
CommandCount=15

[TB:IDR_MENULBAR]
Type=1
Class=?
Command1=ID_MENU_REFRESHE
Command2=ID_MENU_KILL
Command3=ID_MENU_NORMAL
Command4=ID_MENU_PERFORMANCE
Command5=ID_MENU_NETWORK
Command6=ID_MENU_SERVICE
Command7=ID_MENU_BOOT
Command8=ID_MENU_CONSOLE
Command9=ID_MENU_REGEDIT
Command10=ID_MENU_COMPUTEE
Command11=ID_MENU_NOTPAD
Command12=ID_MENU_HOME
Command13=ID_MENU_HELP
Command14=ID_MENU_ABOUT
CommandCount=14

[MNU:IDR_MAINMENU]
Type=1
Class=CMainFrame
Command1=ID_MENU_REFRESHE
Command2=ID_MENU_DISABLEDAUTO
Command3=ID_MENU_NORMAL
Command4=ID_MENU_PERFORMANCE
Command5=ID_MENU_NETWORK
Command6=ID_APP_EXIT
Command7=ID_MENU_SERVICE
Command8=ID_MENU_BOOT
Command9=ID_VIEW_INFO_BAR
Command10=ID_VIEW_MODULE_BAR
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_MAINMENU_TOP
Command14=ID_MENU_HELP
Command15=ID_MENU_HOME
Command16=ID_MENU_ABOUT
CommandCount=16

[MNU:IDR_POPMENU]
Type=1
Class=CECQPSView
Command1=ID_POPMENU_AUTOKILL
Command2=ID_POPMENU_OPEN
Command3=ID_POPMENU_COPY
Command4=ID_POPMENU_KILL
Command5=ID_POPMENU_DELETE
CommandCount=5

[MNU:IDR_TRAYMENU]
Type=1
Class=?
Command1=ID_MENU_HELP
Command2=ID_APP_EXIT
Command3=ID_MENU_ABOUT
CommandCount=3

[TB:IDR_POPMENUBAR]
Type=1
Class=?
Command1=ID_POPMENU_KILL
Command2=ID_POPMENU_OPEN
Command3=ID_POPMENU_COPY
Command4=ID_POPMENU_HELP
Command5=ID_POPMENU_DELETE
Command6=ID_POPMENU_AUTOKILL
CommandCount=6

[DLG:IDD_DLGSERVICE]
Type=1
Class=CDlgService
ControlCount=6
Control1=IDCANCEL,button,1342242816
Control2=IDC_LISTSERVICE,SysListView32,1350631429
Control3=IDC_SERVICE_RERFRSH,button,1342242816
Control4=IDC_SERVICE_DELETE,button,1342242816
Control5=IDC_SERVICE_DRIVER,button,1342242816
Control6=IDC_SERVICE_ALL,button,1342242819

[CLS:CDlgService]
Type=0
HeaderFile=dialog\dlgservice.h
ImplementationFile=dialog\dlgservice.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_SERVICE_ALL
VirtualFilter=dWC

[DLG:IDD_DLGREGBOOT]
Type=1
Class=CDlgRegBoot
ControlCount=3
Control1=IDCANCEL,button,1342242816
Control2=IDC_REGBOOT_LIST,SysListView32,1350631425
Control3=IDC_REGBOOT_DELETE,button,1342242816

[CLS:CDlgRegBoot]
Type=0
HeaderFile=dialog\dlgregboot.h
ImplementationFile=dialog\dlgregboot.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_REGBOOT_DELETE
VirtualFilter=dWC

[CLS:CDlgWarning]
Type=0
HeaderFile=dialog\dlgwarning.h
ImplementationFile=dialog\dlgwarning.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

[DLG:IDD_DLGWARNING]
Type=1
Class=CDlgWarning
ControlCount=3
Control1=IDOK,button,1342210048
Control2=IDCANCEL,button,1342275585
Control3=IDC_DLGWARNING_INFO,static,1342308352

[CLS:CMenuListCtrl]
Type=0
HeaderFile=controls\xplist\menulistctrl.h
ImplementationFile=controls\xplist\menulistctrl.cpp
BaseClass=CListCtrl
Filter=W
VirtualFilter=FWC
LastObject=CMenuListCtrl

[MNU:IDR_POPMENUML]
Type=1
Class=CMainFrame
Command1=ID_POPML_OPEN
Command2=ID_POPML_COPY
CommandCount=2

[DLG:IDD_DLGMODULELK]
Type=1
Class=CDlgModuleLocked
ControlCount=5
Control1=IDCANCEL,button,1342242817
Control2=IDC_DLGMODULE_DELETE,button,1342242816
Control3=IDC_MODULE_LIST,SysListView32,1350631445
Control4=IDC_DLGMODULE_NAME,static,1342308352
Control5=IDC_STATIC,static,1342308352

[CLS:CDlgModuleLocked]
Type=0
HeaderFile=dialog\dlgmodulelocked.h
ImplementationFile=dialog\dlgmodulelocked.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_DLGMODULE_NAME

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_TOOL_BOOT
Command2=ID_TOOL_PERFORMANCE
Command3=ID_TOOL_NORMAL
Command4=ID_TOOL_NETWORK
Command5=ID_TOOL_SERVICE
Command6=ID_TOOL_REFRESHE
CommandCount=6

[CLS:CDlgRootkitSys]
Type=0
HeaderFile=dialog\dlgrootkitsys.h
ImplementationFile=dialog\dlgrootkitsys.cpp
BaseClass=CDialog
Filter=D
LastObject=CDlgRootkitSys
VirtualFilter=dWC

