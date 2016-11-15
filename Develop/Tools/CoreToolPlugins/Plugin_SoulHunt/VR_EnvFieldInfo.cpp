// VR_RollupEnvWeather.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Plugin_SoulHunt.h"
#include "VR_EnvFieldInfo.h"
#include "C_ControlMediator.h"


// CRollupEnvFieldInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupEnvFieldInfo, CDialog)

CRollupEnvFieldInfo::CRollupEnvFieldInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEnvFieldInfo::IDD, pParent)
	, m_strMinX(_T(""))
	, m_strMaxX(_T(""))
	, m_strMinY(_T(""))
	, m_strMaxY(_T(""))
	, m_fAggroLimitZ(0)
{

}

CRollupEnvFieldInfo::~CRollupEnvFieldInfo()
{
}

void CRollupEnvFieldInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MIN_X, m_strMinX);
	DDX_Text(pDX, IDC_EDIT_MAX_X, m_strMaxX);
	DDX_Text(pDX, IDC_EDIT_MIN_Y, m_strMinY);
	DDX_Text(pDX, IDC_EDIT_MAX_Y, m_strMaxY);
	DDX_Text(pDX, IDC_EDIT_AGGRO_Z, m_fAggroLimitZ);
	DDX_Control(pDX, IDC_FIXEDTIME_COMBO, m_FixedTimeCombobox);
}


BEGIN_MESSAGE_MAP(CRollupEnvFieldInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CRollupEnvFieldInfo::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CRollupEnvFieldInfo::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_SETZONESIZE, &CRollupEnvFieldInfo::OnBnClickedButtonSetzonesize)
	ON_BN_CLICKED(IDC_BUTTON_AGGROZ_APPLY, &CRollupEnvFieldInfo::OnBnClickedButtonAggrozApply)
	ON_CBN_SELCHANGE(IDC_FIXEDTIME_COMBO, &CRollupEnvFieldInfo::OnCbnSelchangeFixedtimeCombo)
END_MESSAGE_MAP()

void CRollupEnvFieldInfo::Listen( CoreMessage& _message )
{
	switch( _message.nID )
	{
		// 필드 로딩
	case VIEW_MESSAGE::LOGIC_LOADED :
		{
			UpdateFieldSize();

			UpdateAggroLimitZ();

			UpdateFixedCombobox();

		}
		break;
	}
}

CWorkSpaceField::FIELD_INFO& CRollupEnvFieldInfo::GetFieldInfo()
{
	return m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->m_fieldInfo;
}

bool CRollupEnvFieldInfo::CheckValidation()
{
	MRect rtOrg = GetOriginalBoundingBox();
	MRect rtNew = ObtainFieldSize();

	if (rtNew.left < rtOrg.left ||
		rtNew.right > rtOrg.right ||
		rtNew.top < rtOrg.top ||
		rtNew.bottom > rtOrg.bottom)
	{
		AfxMessageBox("필드 크기가 너무 큽니다.");
		return false;
	}

	if (rtNew.left > rtNew.right ||
		rtNew.top > rtNew.bottom)
	{
		AfxMessageBox("필드 크기 설정이 잘못되었습니다.");
		return false;
	}

	return true;
}


MRect CRollupEnvFieldInfo::ObtainFieldSize()
{
	UpdateData();
	return MRect(
		(int)atoi(m_strMinX.GetString()), // left
		(int)atoi(m_strMinY.GetString()), // top
		(int)atoi(m_strMaxX.GetString()), // right
		(int)atoi(m_strMaxY.GetString()) // bottom
		);
}

void CRollupEnvFieldInfo::UpdateFieldSize()
{
	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();
	m_strMinX.Format("%d", field_info.m_rtBoundary.left);
	m_strMinY.Format("%d", field_info.m_rtBoundary.top);
	m_strMaxX.Format("%d", field_info.m_rtBoundary.right);
	m_strMaxY.Format("%d", field_info.m_rtBoundary.bottom);
	UpdateData(FALSE);
}

void CRollupEnvFieldInfo::UpdateAggroLimitZ()
{
	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();
	m_fAggroLimitZ = field_info.m_fSectorHeight;
	UpdateData(FALSE);
}

MRect CRollupEnvFieldInfo::GetOriginalBoundingBox()
{
	return MRect(
		(int)m_pControlMediator_->m_mapInfo.m_orgX, // left
		(int)m_pControlMediator_->m_mapInfo.m_orgY, // top
		(int)m_pControlMediator_->m_mapInfo.m_width + (int)m_pControlMediator_->m_mapInfo.m_orgX, // right
		(int)m_pControlMediator_->m_mapInfo.m_height + (int)m_pControlMediator_->m_mapInfo.m_orgY // bottom
		);
}

// CRollupEnvFieldInfo 메시지 처리기입니다.

void CRollupEnvFieldInfo::OnBnClickedButtonClear()
{
	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();
	field_info.m_rtBoundary.left = 0;
	field_info.m_rtBoundary.top = 0;
	field_info.m_rtBoundary.right = 0;
	field_info.m_rtBoundary.bottom = 0;
	UpdateFieldSize();
}

void CRollupEnvFieldInfo::OnBnClickedButtonSetzonesize()
{
	MRect rtOrg = GetOriginalBoundingBox();
	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();
	field_info.m_rtBoundary = rtOrg;
	UpdateFieldSize();
}

void CRollupEnvFieldInfo::OnBnClickedButtonApply()
{
	if (!CheckValidation())
		return;

	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();
	field_info.m_rtBoundary = ObtainFieldSize();
}

void CRollupEnvFieldInfo::OnBnClickedButtonAggrozApply()
{
	UpdateData();
	if (m_fAggroLimitZ < 0.0f)
	{
		AfxMessageBox("잘못된 범위입니다. (기본값은 0)");
	}

	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();
	field_info.m_fSectorHeight = m_fAggroLimitZ;
}

void CRollupEnvFieldInfo::UpdateFixedCombobox()
{
	m_FixedTimeCombobox.Clear();

	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();
	int nCurType = field_info.m_TimeInfo.nFixedTime;

	m_FixedTimeCombobox.AddString("선택 안함");
	for(int i = 0; i < MAX_TIME_TYPE; ++i)
	{
		m_FixedTimeCombobox.AddString(m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->GameTime(i));
	}

	m_FixedTimeCombobox.SetCurSel(nCurType + 1);
}


void CRollupEnvFieldInfo::OnCbnSelchangeFixedtimeCombo()
{
	char chFixedTime[32] = {0, };
	m_FixedTimeCombobox.GetLBText(m_FixedTimeCombobox.GetCurSel(), chFixedTime);

	CWorkSpaceField::FIELD_INFO& field_info = GetFieldInfo();

	for (int i = 0; i < MAX_TIME_TYPE; i++)
	{
		if (!_stricmp(chFixedTime, m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->GameTime(GAME_TIME_TYPE(i))))
		{
			field_info.m_TimeInfo.bFixed = true;
			field_info.m_TimeInfo.nFixedTime = (GAME_TIME_TYPE)i;

			return;
		}
	}

	// 선택하지 않음...
	field_info.m_TimeInfo.bFixed = false;
	field_info.m_TimeInfo.nFixedTime = TIME_INVALID;
}
