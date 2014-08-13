#if !defined(AFX_MYLISTCTRL_H__1B11B977_4211_11D3_BDB7_9C1828CB1060__INCLUDED_)
#define AFX_MYLISTCTRL_H__1B11B977_4211_11D3_BDB7_9C1828CB1060__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file
//
#include "Popup.h"

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window

class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();

// Attributes
public:


// Operations
public:
    int GetSelectedItem () const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////
    // CPopup events
    //
public:
    class XPopupCtrlEvent : public IPopupCtrlEvent
    {
    public:
        virtual bool OnInit ();
        virtual void OnShow ();
        virtual void OnHide (bool bAbort);
        virtual int OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
        virtual int OnLButtonDown (UINT nFlags, CPoint point);
        virtual int OnLButtonUp (UINT nFlags, CPoint point);
        virtual IPopupCtrlEvent* GetInterfaceOf (HWND hWnd);

    } m_xPopupCtrlEvent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__1B11B977_4211_11D3_BDB7_9C1828CB1060__INCLUDED_)
