// VD_DlgCutSceneActorType.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"

#include "VD_DlgCutSceneActorType.h"
#include "VD_DlgInputCommon.h"

#include "VL_ResourceInfo.h"

#include "V_MFCHelper.h"

// CDlgCutSceneActorType 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneActorType, CDialog)

CDlgCutSceneActorType::CDlgCutSceneActorType(CControlMediator* pControlMediator, const char* _pSzActorType, const char* _pSzActorName, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneActorType::IDD, pParent)
{
	CDlgInputCommon::m_isActive = true;
	m_pControlMediator_ = pControlMediator;

	std::string strType;
	if (_pSzActorType == NULL)
	{
		strType = "player";
	}
	else
	{
		strType = _pSzActorType;
	}

	if (strType == "player")
	{
		m_nActorType = 0;
		m_nNPCID = 1;
	}
	else
	{
		m_nActorType = 1;
		//npc_ : 4
		m_nNPCID = atoi( strType.substr(4).c_str() );
	}

	m_strNpcName = _pSzActorName;
}

CDlgCutSceneActorType::~CDlgCutSceneActorType()
{
	CDlgInputCommon::m_isActive = false;
}

void CDlgCutSceneActorType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_CUTSCENE_PLAYER, m_nActorType);
	DDX_Text(pDX, IDC_EDIT_NPC_ID, m_nNPCID);
	DDX_Text(pDX, IDC_EDIT_NPC_NAME, m_strNpcName);
}


BEGIN_MESSAGE_MAP(CDlgCutSceneActorType, CDialog)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_PLAYER, &CDlgCutSceneActorType::OnBnClickedRadioCutscenePlayer)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_NPC, &CDlgCutSceneActorType::OnBnClickedRadioCutsceneNpc)
	ON_BN_CLICKED(IDC_BUTTON_CUTSCENE_NPC_SELECT, &CDlgCutSceneActorType::OnBnClickedButtonCutsceneNpcSelect)
END_MESSAGE_MAP()

void CDlgCutSceneActorType::SetEnablePlayer()
{
	m_nActorType = 0;
	GetDlgItem(IDC_EDIT_NPC_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CUTSCENE_NPC_SELECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_NPC_NAME)->EnableWindow(FALSE);
}

void CDlgCutSceneActorType::SetEnableNPC()
{
	m_nActorType = 1;
	GetDlgItem(IDC_EDIT_NPC_ID)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CUTSCENE_NPC_SELECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_NPC_NAME)->EnableWindow(TRUE);
}

// CDlgCutSceneActorType 메시지 처리기입니다.

void CDlgCutSceneActorType::OnBnClickedRadioCutscenePlayer()
{
	SetEnablePlayer();
}

void CDlgCutSceneActorType::OnBnClickedRadioCutsceneNpc()
{
	SetEnableNPC();
}

BOOL CDlgCutSceneActorType::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_nActorType == 0)
	{
		SetEnablePlayer();
	}
	else
	{
		SetEnableNPC();
	}

	return TRUE;
}

void CDlgCutSceneActorType::OnBnClickedButtonCutsceneNpcSelect()
{
	CDlgResourceInfo* pDlg = CMFCHelper::CreateAppDlgModalOKed<CDlgResourceInfo>(m_pControlMediator_, CDlgResourceInfo::RES_NPC);
	if (pDlg)
	{
		m_nNPCID = pDlg->m_nSelectedNpcID;
		UpdateData(FALSE);
		delete pDlg;
	}
}
