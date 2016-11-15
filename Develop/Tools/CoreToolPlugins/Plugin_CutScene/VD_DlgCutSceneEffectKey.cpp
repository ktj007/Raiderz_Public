// VD_DlgCutSceneEffectKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"

#include "C_ControlMediator.h"
#include "VD_DlgCutSceneEffectKey.h"
#include "VR_CutSceneObjectList.h"
#include "VR_EntityObjectList.h"

#include "../../Game/GameCommon/XEffectInfo.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

// CDlgCutSceneEffectKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneEffectKey, CDialog)

CDlgCutSceneEffectKey::CDlgCutSceneEffectKey(CControlMediator* _pControlMediator, float fTime, float fDurationTime, vec3 vPos, vec3 vDir, vec3 vUp, const string& strEffectName, const string& strEffectInfo, CWnd* pParent)
: CDialog(CDlgCutSceneEffectKey::IDD, pParent)
{
	m_pEffectInfoMgr = NULL;

	CRollupEntityObjectList* pEntityObjecList = _pControlMediator->GetViewPtr<CRollupEntityObjectList>();
	if(pEntityObjecList)
	{
		m_pEffectInfoMgr = pEntityObjecList->GetEffectInfoMgr();
	}

	m_fTime = fTime;
	m_fDurationTime = fDurationTime;

	m_vPos = RVector3::ZERO;
	m_vDir = RVector3::AXISY;
	m_vUp = RVector3::AXISZ;

	m_strEffectName = strEffectName.c_str();
	if (strEffectInfo == "fixed" || strEffectInfo == "")
	{
		m_nFixedOrFlow = 0;

		m_vPos = vPos;
		m_vDir = vDir;
		m_vUp = vUp;
	}
	else if (strEffectInfo.find("npc:") != std::string::npos)
	{
		m_nFixedOrFlow = 1;
		size_t npc_tok_next = 4; //npc:
		size_t bone_tok_prev = strEffectInfo.find(",bone:");
		size_t bone_tok_next = bone_tok_prev + 6; //,bone:

		m_strNpcName = strEffectInfo.substr(npc_tok_next, bone_tok_prev - npc_tok_next).c_str();
		m_strBoneName = strEffectInfo.substr(bone_tok_next).c_str();
	}
	else if (strEffectInfo == "camera")
	{
		m_nFixedOrFlow = 2;
	}
	else if (strEffectInfo.find("post:") != std::string::npos)
	{
		m_nFixedOrFlow = 3;
		size_t post_tok_next = 5; //post:
		m_strPostActor = strEffectInfo.substr(post_tok_next).c_str();
	}

	char buf[256] = {0, };
	_stprintf(buf, "%.3f, %.3f, %.3f", m_vPos.x, m_vPos.y, m_vPos.z);
	m_strPos = buf;
	_stprintf(buf, "%.3f, %.3f, %.3f", m_vDir.x, m_vDir.y, m_vDir.z);
	m_strDir = buf;
	_stprintf(buf, "%.3f, %.3f, %.3f", m_vUp.x, m_vUp.y, m_vUp.z);
	m_strUp = buf;
}

CDlgCutSceneEffectKey::~CDlgCutSceneEffectKey()
{
}

void CDlgCutSceneEffectKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_EFFECTKEY_TIME, m_fTime);
	DDX_Text(pDX, IDC_EDIT_EFFECTKEY_DURATIONTIME, m_fDurationTime);
	DDX_Text(pDX, IDC_EDIT_EFFECTKEY_POS, m_strPos);
	DDX_Text(pDX, IDC_EDIT_EFFECTKEY_DIR, m_strDir);
	DDX_Text(pDX, IDC_EDIT_EFFECTKEY_UP, m_strUp);

	DDX_Text(pDX, IDC_EDIT_CUTSCENE_EFFECT_NAME, m_strEffectName);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_EFFECT_NPC, m_strNpcName);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_EFFECT_BONE, m_strBoneName);
	DDX_Text(pDX, IDC_EDIT_CUTSCENE_EFFECT_POST_ACTOR, m_strPostActor);
	
	DDX_Radio(pDX, IDC_RADIO_CUTSCENE_EFFECT_FIXED, m_nFixedOrFlow);
}

void CDlgCutSceneEffectKey::GetEffectData( float& fTime, float& fDurationTime, vec3& vPos, vec3& vDir, vec3& vUp, std::string& _rEffectName, std::string& _rEffectInfo )
{
	fTime = m_fTime;
	fDurationTime = m_fDurationTime;

	_stscanf(m_strPos,"%f, %f, %f",&vPos.x,&vPos.y,&vPos.z);
	_stscanf(m_strDir,"%f, %f, %f",&vDir.x,&vDir.y,&vDir.z);
	_stscanf(m_strUp,"%f, %f, %f",&vUp.x,&vUp.y,&vUp.z);

	_rEffectName = (LPCTSTR)(m_strEffectName);

	switch (m_nFixedOrFlow)
	{
	case 0:
		m_strEffectInfo = "fixed";
		break;

	case 1:
		m_strEffectInfo = "npc:" + m_strNpcName + ",bone:" + m_strBoneName;
		break;

	case 2:
		m_strEffectInfo = "camera";
		break;

	case 3:
		m_strEffectInfo = "post:" + m_strPostActor;
		break;
	}
	_rEffectInfo = (LPCTSTR)(m_strEffectInfo);
}

BEGIN_MESSAGE_MAP(CDlgCutSceneEffectKey, CDialog)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_EFFECT_FIXED, &CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectFixed)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_EFFECT_FOLLOW, &CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectFollow)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_EFFECT_CAMERA, &CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectCamera)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_EFFECT_POST, &CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectPost)
END_MESSAGE_MAP()

void CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectFixed()
{
	m_nFixedOrFlow = 0;

	GetDlgItem(IDC_EDIT_EFFECTKEY_POS)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_DIR)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_UP)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_NPC)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_BONE)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectFollow()
{
	m_nFixedOrFlow = 1;

	GetDlgItem(IDC_EDIT_EFFECTKEY_POS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_DIR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_UP)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_NPC)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_BONE)->EnableWindow(TRUE);

	UpdateData(FALSE);
}

void CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectCamera()
{
	m_nFixedOrFlow = 2;

	GetDlgItem(IDC_EDIT_EFFECTKEY_POS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_DIR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_UP)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_NPC)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_BONE)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

void CDlgCutSceneEffectKey::OnBnClickedRadioCutsceneEffectPost()
{
	m_nFixedOrFlow = 3;

	GetDlgItem(IDC_EDIT_EFFECTKEY_POS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_DIR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EFFECTKEY_UP)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_NPC)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CUTSCENE_EFFECT_BONE)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

BOOL CDlgCutSceneEffectKey::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch (m_nFixedOrFlow)
	{
	case 0:
		OnBnClickedRadioCutsceneEffectFixed();
		break;

	case 1:
		OnBnClickedRadioCutsceneEffectFollow();
		break;

	case 2:
		OnBnClickedRadioCutsceneEffectCamera();
		break;

	case 3:
		OnBnClickedRadioCutsceneEffectPost();
		break;
	}

	return TRUE;
}

#pragma warning( pop )