#if !defined(AFX_PPAGE_H__12CD53F5_0DA1_4C86_8061_85AFDD2DA1EF__INCLUDED_)
#define AFX_PPAGE_H__12CD53F5_0DA1_4C86_8061_85AFDD2DA1EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPPage dialog

class CPPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPPage)

// Construction
public:
	CPPage();
	CPPage(int nID);
	~CPPage();

protected:
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPPage)	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PPAGE_H__12CD53F5_0DA1_4C86_8061_85AFDD2DA1EF__INCLUDED_)
