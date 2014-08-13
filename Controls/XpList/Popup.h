///////////////////////////////////////////////////////////////////////////////
//
// Popup.h
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

interface IPopupCtrlEvent;

///////////////////////////////////////////////////////////////////////////////
class CPopup : public CMiniFrameWnd
{
public:
    // Return value of OnKeyDown, OnLButtonDown and OnLButtonUp callbacks
    enum
    {
        doNothing = 0,  // No action
        noSend    = 1,  // Do not send the current message to the control
        end       = 2,  // Hide popup normally
        abort     = 3,  // Hide popup with flag 'abort'
    };

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPopup)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void PostNcDestroy();
    virtual int OnToolHitTest(CPoint pt, TOOLINFO* pTI) const;
    //}}AFX_VIRTUAL

    bool Display (CWnd* pControl, CWnd* pCaller, CRect& rcCaller, IPopupCtrlEvent* pEvents = NULL, DWORD dwStyles = 0);

// Implementation
protected:
    CWnd* m_pControl;
    CWnd* m_pCaller;
    CRect m_rcCaller;
    IPopupCtrlEvent* m_pEvents;
    HWND m_hPrevParentCtrl;
    bool m_bDestroy;

protected:
    CPopup ();
    virtual ~CPopup () {};

    void EndPopup (bool bAbort = false, bool bSetFocus = true);

    // Generated message map functions
protected:
    //{{AFX_MSG(CPopup)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnActivate(UINT, CWnd*, BOOL);
    afx_msg void OnCancelMode();
    afx_msg LRESULT OnDelayDestroy(WPARAM, LPARAM);
    afx_msg void OnNcPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNCREATE(CPopup)
};

///////////////////////////////////////////////////////////////////////////////
// Style indicates to draw special 'tab' border arround rcCaller and popped control
#define WS_TABBORDER (WS_BORDER|0x00000001L)


///////////////////////////////////////////////////////////////////////////////
// {600029F0-14EE-11D3-941C-00105A66E1E2}
DEFINE_GUID(IID_IPopupCtrlEvent, 0x600029F0, 0x14EE, 0x11D3, 0x94, 0x1C, 0x00, 0x10, 0x5A, 0x66, 0xE1, 0xE2);

interface IPopupCtrlEvent
{
    virtual bool OnInit () = 0;
    virtual void OnShow () = 0;
    virtual void OnHide (bool bAbort) = 0;
    virtual int OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags) = 0;
    virtual int OnLButtonDown (UINT nFlags, CPoint point) = 0;
    virtual int OnLButtonUp (UINT nFlags, CPoint point) = 0;
    virtual IPopupCtrlEvent* GetInterfaceOf (HWND hWnd) = 0;
};
