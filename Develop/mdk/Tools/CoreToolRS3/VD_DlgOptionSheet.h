#pragma once
#include "VL_ListenableView.h"
#include "VD_OptionPage1.h"
#include "VD_OptionPage2.h"

// V_DlgOptionSheet

class CDlgOptionSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDlgOptionSheet)

public:
	CDlgOptionSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDlgOptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CDlgOptionSheet();

	enum { IDD = IDD_OPTION };
protected:
	DECLARE_MESSAGE_MAP()
private:
	V_OptionPage1		m_page1;
	V_OptionPage2		m_page2;
};


