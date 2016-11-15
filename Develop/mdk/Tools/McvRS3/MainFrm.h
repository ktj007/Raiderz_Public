// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__2EF8C2CA_29B0_453B_A924_2960A36BE9B0__INCLUDED_)
#define AFX_MAINFRM_H__2EF8C2CA_29B0_453B_A924_2960A36BE9B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlToolBar.h"
#include "FrameControlBar.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	BOOL	m_bActivate;

// Operations
public:

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CControlToolBar m_controlBar;
	CFrameControlBar m_cFrameControlBar;
	CDialogBar	m_outputBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	CControlToolBar* GetControlToolBar() { return &m_controlBar; }
	CFrameControlBar* GetFrameControlBar() { return &m_cFrameControlBar; }
	CDialogBar*		GetOutputBar() { return &m_outputBar; }
	afx_msg void OnClose();
	afx_msg void OnPartsColor();
	afx_msg void OnBkcolor();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__2EF8C2CA_29B0_453B_A924_2960A36BE9B0__INCLUDED_)
