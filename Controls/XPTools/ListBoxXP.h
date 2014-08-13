///////////////////////////////////////////////////////////////////////////////
//
// ListBoxXP.h
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////
class CListBoxXP : public CListBox
{
// Construction
public:
    CListBoxXP();

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListBoxXP)
    public:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CListBoxXP();

protected:
    CMouseMgr m_MouseMgr;

    // Generated message map functions
protected:
    //{{AFX_MSG(CListBoxXP)
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseOut();
    afx_msg void OnNcPaint();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
