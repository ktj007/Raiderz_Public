#pragma once


#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"


class CEventDlg : public CDialog
{
	DECLARE_DYNAMIC(CEventDlg)

	//{{AFX_DATA(CAnimationSidebar)  
	enum { IDD = IDD_DIALOG_EVENT };

	CString m_strFrame;
	int m_nMaxFrame;
	int m_nName;
	CString m_strName;
	CString m_strParam1;
	CString m_strParam2;
	CString m_strParam3;
	int m_nUncondition;
	//}}AFX_DATA 

public:
	CEventDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEventDlg();

	void UpdateEventName();

	void SetFrame( int nFrame);
	void SetMaxFrame( int nMaxFrame);
	void SetName( const char* szName);
	void SetParam1( const char* szParam);
	void SetParam2( const char* szParam);
	void SetParam3( const char* szParam);
	void SetUncondition( bool bSet);

	int GetFrame();
	const char* GetName();
	const char* GetParam1();
	const char* GetParam2();
	const char* GetParam3();
	bool GetUncondition();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	//{{AFX_MSG(CAnimationSidebar)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboEventname();
};
