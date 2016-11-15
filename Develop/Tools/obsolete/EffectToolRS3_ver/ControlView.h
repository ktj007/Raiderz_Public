#if !defined(AFX_CONTROLVIEW_H__034CEBAB_12E6_48AC_829A_89603BEB0FA5__INCLUDED_)
#define AFX_CONTROLVIEW_H__034CEBAB_12E6_48AC_829A_89603BEB0FA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlView.h : header file
//

#include "PageEffect.h"
#include "PageMaterial.h"
#include "PageParticle.h"
#include "PageEmitter.h"

/////////////////////////////////////////////////////////////////////////////
// CControlView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CControlView : public CFormView
{
protected:
	CControlView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CControlView)

// Form Data
public:
	//{{AFX_DATA(CControlView)
	enum { IDD = IDD_CONTROL_VIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

   CPSheet  m_PSheet;
   CPage1	m_Page1;
   CPageEmitter m_PageEmitter;
   CPage2	m_Page2;
   CPage3	m_Page3;
   //CPage4	m_Page4;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL 

protected:
	virtual ~CControlView();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CControlView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLVIEW_H__034CEBAB_12E6_48AC_829A_89603BEB0FA5__INCLUDED_)
