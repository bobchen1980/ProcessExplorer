#if !defined(AFX_TBALLOON_H__8363E22A_9845_4086_B3A0_40117DCFDB1F__INCLUDED_)
#define AFX_TBALLOON_H__8363E22A_9845_4086_B3A0_40117DCFDB1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
// class CTBalloon
//
// Author:  Rajesh C K R
// email:   rajeshckr@usa.net
// page:    http://www.cybertalkonline.com
// Copyright © 2000, Rajesh C K R
//
// This code provided "AS IS," without warranty of any kind.
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// version : 1.0



/////////////////////////////////////////////////////////////////////////////
// CTBalloon window
#include "Gradient.h"

class CTBalloon : public CWnd
{
// Construction
public:
	CTBalloon(UINT nWidth, UINT nHeight);
	BOOL CreateAndShow();
	void SetText(CString str);
	void SetLifeTime(UINT secs);


	static UINT m_sActiveCount;

// Attributes
private:
	CRect		m_current_rect;		
	CRect		m_screen_rect;
	UINT		m_nWidth;
	UINT		m_nHeight;
	UINT		m_totaltime;
	UINT		m_lifetime;
	BOOL		m_dir;
	CString		m_text;
	CGradient	m_grad;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTBalloon)
	public:
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTBalloon();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTBalloon)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TBALLOON_H__8363E22A_9845_4086_B3A0_40117DCFDB1F__INCLUDED_)
