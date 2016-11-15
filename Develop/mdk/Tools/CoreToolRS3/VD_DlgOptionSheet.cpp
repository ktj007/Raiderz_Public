// V_DlgOptionSheet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VD_DlgOptionSheet.h"


// V_DlgOptionSheet

IMPLEMENT_DYNAMIC(CDlgOptionSheet, CPropertySheet)

CDlgOptionSheet::CDlgOptionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_page1);
	AddPage(&m_page2);
}

CDlgOptionSheet::CDlgOptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_page1);
	AddPage(&m_page2);
}

CDlgOptionSheet::~CDlgOptionSheet()
{
}


BEGIN_MESSAGE_MAP(CDlgOptionSheet, CPropertySheet)
END_MESSAGE_MAP()


// V_DlgOptionSheet 메시지 처리기입니다.
