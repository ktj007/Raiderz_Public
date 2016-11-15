// VD_DlgCutSceneNewMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgCutSceneNewMap.h"
#include "VD_DlgInputCommon.h"

#include "VL_ResourceInfo.h"
#include "MLocale.h"

// CDlgCutSceneNewMap 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneNewMap, CDialog)

CDlgCutSceneNewMap::CDlgCutSceneNewMap(CControlMediator* pControlMediator, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneNewMap::IDD, pParent)
{
	m_pControlMediator_ = pControlMediator;
	CDlgInputCommon::m_isActive = true;
}

CDlgCutSceneNewMap::~CDlgCutSceneNewMap()
{
	CDlgInputCommon::m_isActive = false;
}

void CDlgCutSceneNewMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CUTSCENEFILENAME	, m_strFileName);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_FIELD, m_nFieldId);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_DURATION, m_fDuration);
}


BEGIN_MESSAGE_MAP(CDlgCutSceneNewMap, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CUTSCENE_FIELD_OPEN, &CDlgCutSceneNewMap::OnBnClickedButtonCutsceneFieldOpen)
END_MESSAGE_MAP()


// CDlgCutSceneNewMap 메시지 처리기입니다.

void CDlgCutSceneNewMap::OnBnClickedButtonCutsceneFieldOpen()
{
	UpdateData(TRUE);
	CDlgResourceInfo dlg(m_pControlMediator_, CDlgResourceInfo::RES_FIELD);
	if (dlg.DoModal()==IDOK )
	{
		m_nFieldId = dlg.m_nSelectedFieldID;
		UpdateData(FALSE);
	}
}

bool CDlgCutSceneNewMap::MakeNewCutSceneFile()
{
	MXml xml;
	char buf[8196];
	sprintf_s( buf,
"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>	\
<maiet>	\
	<clapper desc=\"%s\" field=\"%d\" duration=\"%.3f\" in=\"none\" out=\"none\" />	\
	<camera/>	\
	<event/>	\
</maiet>", (LPCTSTR)(m_strName), m_nFieldId, m_fDuration );

	std::string strUtf = MLocale::ConvAnsiToUTF8(buf);
	bool bRet = xml.LoadStream(strUtf.c_str());
	if (bRet)
	{
		return xml.SaveFile((LPCTSTR)m_strFileName);
	}

	return false;
}
