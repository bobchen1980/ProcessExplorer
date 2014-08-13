// ECQPSDoc.h : interface of the CECQPSDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECQPSDOC_H__2F699A9A_9483_4E2C_A90F_B519EDFCFF0E__INCLUDED_)
#define AFX_ECQPSDOC_H__2F699A9A_9483_4E2C_A90F_B519EDFCFF0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CECQPSDoc : public CDocument
{
protected: // create from serialization only
	CECQPSDoc();
	DECLARE_DYNCREATE(CECQPSDoc)

// Attributes
public:

// Operations
public:
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CECQPSDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CECQPSDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	BOOL InitExePath(CString dPath);
	UINT m_uTimer;
	//{{AFX_MSG(CECQPSDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECQPSDOC_H__2F699A9A_9483_4E2C_A90F_B519EDFCFF0E__INCLUDED_)
