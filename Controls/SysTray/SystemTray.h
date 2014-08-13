 #if !defined(AFX_SYSTEMTRAY_H__48272E0E_9FD1_4CFF_996B_CE36ED449FFF__INCLUDED_)
#define AFX_SYSTEMTRAY_H__48272E0E_9FD1_4CFF_996B_CE36ED449FFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSystemTray : public CWnd
{
// Construction/destruction
public:
    CSystemTray();
    virtual ~CSystemTray();

    DECLARE_DYNAMIC(CSystemTray)

// Operations
public:
    BOOL Create(UINT uCallbackMessage, LPCTSTR lpszTip, HICON hIcon, UINT uID);

    BOOL SetIcon(UINT nIDResource);

    BOOL SetNotificationWnd(CWnd* pNotifyWnd);
    virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSystemTray)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
    NOTIFYICONDATA  m_NotifyIconData;

// Generated message map functions
protected:
	//{{AFX_MSG(CSystemTray)
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMTRAY_H__48272E0E_9FD1_4CFF_996B_CE36ED449FFF__INCLUDED_)
