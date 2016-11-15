#pragma once

#include "VD_OptionPage1.h"
#include "VD_OptionPage2.h"

// CDlgOption 대화 상자입니다.

class CDlgOption : public CPropertySheet
{
	DECLARE_DYNAMIC(CDlgOption)

public:
	//CDlgOption(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDlgOption(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDlgOption(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	virtual ~CDlgOption();

	virtual BOOL OnInitDialog();
	void InitFromConfig();

	static bool m_isActive;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OPTION };

protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	V_OptionPage1*		m_pPage1;
	V_OptionPage2*		m_pPage2;
};
