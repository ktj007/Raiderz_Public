#if !defined(AFX_PSHEET_H__7D3A4EE1_68D8_4793_B6B7_BD2ECF426A10__INCLUDED_)
#define AFX_PSHEET_H__7D3A4EE1_68D8_4793_B6B7_BD2ECF426A10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// psheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPSheet

#define WM_ADJUST WM_USER + 111

class CPSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPSheet)

// Construction
public:
	CPSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPSheet();
    
// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPSheet();

	virtual BOOL	Create(CWnd* pParentWnd, DWORD dwStyle = (DWORD)-1, DWORD dwExStyle = 0);
    virtual void	AdjustPages();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPSheet)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnAdjust(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSHEET_H__7D3A4EE1_68D8_4793_B6B7_BD2ECF426A10__INCLUDED_)
