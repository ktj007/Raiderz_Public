#pragma once


#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"


class CAddAnimationDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddAnimationDlg)

	//{{AFX_DATA(CAnimationSidebar)  
	enum { IDD = IDD_DIALOG_ADDANIMATION };

	//}}AFX_DATA 

public:
	CAddAnimationDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddAnimationDlg();

	CString m_strAniName;
	CString m_strFileName;
	int m_nLoop;
	CString m_strSpeed;
	bool m_bEditMode;

	void SetEditMode( bool bEdit);

	void SetAniName( const char* szAniName);
	void SetFileName( const char* szFileName);
	void SetLoop( bool bLoop);
	void SetSpeed( float fSpeed);

	const char* GetAniName();
	const char* GetFileName();
	bool GetLoop();
	float GetSpeed();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	//{{AFX_MSG(CAddAnimationDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFindfile();
};
