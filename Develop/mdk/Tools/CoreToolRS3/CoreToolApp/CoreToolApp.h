// exe.h : main header file for the EXE application
//

#if !defined(AFX_EXE_H__9DD81252_BDDD_43C2_90E7_DA2881EDF487__INCLUDED_)
#define AFX_EXE_H__9DD81252_BDDD_43C2_90E7_DA2881EDF487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// forward declarations
struct IDocument;

/////////////////////////////////////////////////////////////////////////////
// CExeApp:
// See exe.cpp for the implementation of this class
//

class CExeApp : public CWinAppEx
{
public:
	CExeApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExeApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	// Implementation
	//{{AFX_MSG(CExeApp)
	afx_msg void OnAppAbout();
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	IDocument		*pIDoc;

	void InitDll();
	void ReleaseDll();		
public:
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXE_H__9DD81252_BDDD_43C2_90E7_DA2881EDF487__INCLUDED_)
