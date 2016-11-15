// V_DlgOption.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "MainFrm.h"

#include "C_XMLConfig.h"
#include "C_XMLUserConfig.h"

#include "VD_DlgCommonFileOpen.h"
#include "VD_DlgOption.h"


// CDlgOption 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgOption, CPropertySheet)

//CDlgOption::CDlgOption(CWnd* pParent /*=NULL*/)
//	: CDialog(CDlgOption::IDD, pParent)
//{
//	m_viewID = VIEW_ID::MAP_NEW;
//	m_bActive_ = false;
//	m_pSheet = NULL;
//	
//}

bool CDlgOption::m_isActive = false;

CDlgOption::CDlgOption(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pPage1 = new V_OptionPage1();
	m_pPage2 = new V_OptionPage2();

	AddPage(m_pPage1);
	AddPage(m_pPage2);

	CDlgOption::m_isActive = true;
}

CDlgOption::CDlgOption(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pPage1 = new V_OptionPage1();
	m_pPage2 = new V_OptionPage2();

	AddPage(m_pPage1);
	AddPage(m_pPage2);

	CDlgOption::m_isActive = true;
}


CDlgOption::~CDlgOption()
{
	if (m_pPage1) delete m_pPage1;
	if (m_pPage2) delete m_pPage2;

	CDlgOption::m_isActive = false;
}

BOOL CDlgOption::OnInitDialog(){
	CPropertySheet::OnInitDialog();

	//CTabCtrl* pTab = (CTabCtrl*)GetDlgItem(IDC_OPTION_TAB);
	//pTab->InsertItem(0,"일반");
	//pTab->InsertItem(1,"경로");
	return TRUE;
}

void CDlgOption::InitFromConfig()
{
	CControlMediator* pControlMediator = ((CMainFrame*)AfxGetMainWnd())->GetControlMediator();
	ASSERT(pControlMediator);

	m_pPage1->m_fMinFarZ = (float)CXMLConfig::GetInstance()->fMinFarZ;
	m_pPage1->m_fKeyZoomFactor = (float)CXMLConfig::GetInstance()->fKeyZoomFactor;

	m_pPage1->m_fMaxFarZ         = pControlMediator->m_cameraInfo.FAR_Z;
	m_pPage1->m_fWheelZoomFactor = pControlMediator->m_cameraInfo.WHEEL_ZOOM_STEP;
	m_pPage1->m_fRotateStep      = pControlMediator->m_cameraInfo.ROTATE_STEP;
	m_pPage1->m_fPanStep         = pControlMediator->m_cameraInfo.MOVE_STEP;

	// path
	m_pPage2->m_strDataPath = CXMLConfig::GetInstance()->pcDataPath;
	m_pPage2->m_strTilePath = CXMLConfig::GetInstance()->pcTilepath;
	m_pPage2->m_strTreePath = CXMLConfig::GetInstance()->pcTreepath;
	m_pPage2->m_strEluPath = CXMLConfig::GetInstance()->pcMapObjectPath;
	m_pPage2->m_strEftPath = CXMLConfig::GetInstance()->pcEftpath;
	m_pPage2->m_strShaderPath = CXMLConfig::GetInstance()->pcShaderpath;
	m_pPage2->m_strTexturePath = CXMLConfig::GetInstance()->strTexturePath.c_str();
	//m_pPage2->m_strTexturePath = CXMLConfig::GetInstance()->pcBaseMesh;

	// Text Editor
	m_pPage1->m_strTextEditor = CXMLUserConfig::GetReference().GetTextEditor().c_str();

	//Server Path
	m_pPage2->m_strServerPath = CXMLConfig::GetInstance()->strServerPath.c_str();

	m_pPage1->m_strTextLoginID = CXMLUserConfig::GetReference().GetTextLoginID().c_str();
	m_pPage1->m_strTextLoginIP = CXMLUserConfig::GetReference().GetTextLoginIP().c_str();

	//Options
	m_pPage1->m_nRememberWork = CXMLUserConfig::GetReference().IsRememberWork() ? TRUE : FALSE;
}

//void CDlgOption::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//}
//
BEGIN_MESSAGE_MAP(CDlgOption, CPropertySheet)

END_MESSAGE_MAP()
