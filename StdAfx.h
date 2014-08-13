// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0706C40D_F52C_48DD_B411_FD08F3D5F661__INCLUDED_)
#define AFX_STDAFX_H__0706C40D_F52C_48DD_B411_FD08F3D5F661__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxmt.h>			// MFC support for Windows Common Controls
#include <aclapi.h>

//include the resource header
#include <shlwapi.h>
#include <version.h>
#include <vdmdbg.h>
#include <Afxtempl.h>
#include <map>
#include <Winbase.h>
#include <WINBASE.H>
#include "string.h"
#include "resource.h"

#pragma comment(lib,"version.lib")
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")

#pragma warning(disable: 4786)
#pragma warning(disable: 4530)
/////关闭map的编译警告
using std::map;
typedef map<CString, DWORD> Ico2Index;
typedef map<CString, DWORD> Process2Index;

//////
#define YES            1
#define NO             2

#define PROCESS_PID    1    ///线程列表中的Coulumn位置
#define PROCESS_PATH  6    ///线程列表中的Coulumn位置
#define PROCESS_FILE  0    ///线程列表中的Coulumn位置

#define NOW_NORMAL     1    //当前是正常视图
#define NOW_PERFMON    2    //当前是性能视图
#define	NOW_NETWORK    3    //当前是网络视图
#define	NOW_HIDDEN     4    //当前是隐藏视图
#define NOW_DRIVER     5

#define C_COLUMNS       7   //线程List数目
#define M_COLUMNS       6   //模块List数目
#define COLUMN_FILE     0
#define COLUMN_PATH     1
#define COLUMN_HITS     2
#define COLUMN_LAST     3
#define COLUMN_CMD      4
#define COLUMN_URL      5

#define IN_RUN         0
#define IN_RUNSERVICE  1
#define IN_WININI      2
#define IN_MENU        3

#define POS_TASKBAR    0
#define POS_SYSTRAY    1
#define POS_BOTH       2
#define POS_NONE       3

#define INFO_BAR       1
#define MODULE_BAR     2

#define FOR_TOOLBAR    1
#define FOR_CONFIGBAR  2
#define FOR_INFOBAR    3

//define my message
#define WSM_DOCHIT               WM_USER+1
#define WSM_CGIDONE              WM_USER+2

#define	WM_ICON_NOTIFY			 WM_USER+100
#define WM_ICON_ADD              WM_USER+101
#define WM_ICON_REMOVE           WM_USER+102
#define WM_TASK_ADD              WM_USER+103
#define WM_TASK_REMOVE           WM_USER+104
#define WM_SET_LIST_EXTERN_STYLE WM_USER+105
#define WM_SET_MENU_TITLE        WM_USER+106
#define WM_SET_TOOLBAR           WM_USER+107
#define WM_SET_STATUSBAR         WM_USER+108
#define WM_SET_LOGO_BUTTON       WM_USER+109
#define WM_SET_BACK_FORE         WM_USER+110
#define WM_ADD_SYS_MENU          WM_USER+111
#define WM_REMOVE_SYS_MENU       WM_USER+112
#define WM_SET_CONFIG_BAR        WM_USER+113
#define WM_SET_INFO_BAR          WM_USER+114
#define WM_REMOVE_BAR_TITLE      WM_USER+115
#define WM_DISPLAY_ROOT_TREE     WM_USER+116
#define WM_THRADANSWER           WM_USER+117
#define TR_INIT  0
#define TR_EXECUTE   1
#define TR_END     2


///////控件
#include "Process/ProcessFunction.h"
#include "Process/perfmon.h"
#include "Registry/Registry.h"

#include "Controls/XPTools/Tools.h"
#include "Controls/XPTools/ToolBarXP.h"
#include "Controls/XPTools/StatusBarXP.h"
#include "Controls/SysTray/SystemTray.h"
#include "Controls/MutipleSort/MultiColumnSortListView.h"
#include "Controls/XPMenu/BCMenu.h"
#include "Controls/FileFunc/file_ver.h"
#include "Controls/FloatControl/CoolControlsManager.h"
#include "Controls/StaticEdit/editex.h"
#include "Controls/TrayBool/TaskbarNotifier.h"
#include "Controls/TrayBool/XInfoTip.h"
#include "Controls/XpList/menulistctrl.h"
#include "Controls/XpList/MyListCtrl.h"
#include "Controls/XpList/MyTreeCtrl.h"

#include "Dialog/FileDialogST.h"
#include "Dialog/dlgwarning.h"

#define	_SCB_REPLACE_MINIFRAME
#include "Controls/SCBBar/sizecbar.h"
#include "Controls/SCBBar/scbarg.h"
#include "Controls/SCBBar/CoolTabCtrl.h"
#include "Controls/FileFunc/StringFile.h"

/////////
DWORD GetListViewStyle();
BOOL ListHasFlatSB();
CString JudgeOperatingSystemType();
DWORD GetBarDockStyle(int ibar);

#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0706C40D_F52C_48DD_B411_FD08F3D5F661__INCLUDED_)
