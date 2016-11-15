// VD_DlgInputSensorWarp.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "VD_DlgInputSensorWarp.h"
#include "VL_ResourceInfo.h"

//void CDlgInputNpc::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_EDIT_NPC_ID, m_nNpcID);
//}

// CDlgInputSensorWarp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputSensorWarp, CDialog)

bool CDlgInputSensorWarp::m_isActive = false;

CDlgInputSensorWarp::CDlgInputSensorWarp(CControlMediator* _pControlMediator, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputSensorWarp::IDD, pParent)
	, m_nCondition(0)
	, m_nField(0)
	, m_nMarker(0)
	, m_strTitle(_T(""))
	, m_nGateType(0)
	, m_nFieldGroupID(0)
	, m_nQuestID(0)
	, m_nWarpFieldType(0)
{
	CDlgInputSensorWarp::m_isActive = true;
	m_pControlMediator_ = _pControlMediator;
}

CDlgInputSensorWarp::~CDlgInputSensorWarp()
{
	CDlgInputSensorWarp::m_isActive = false;
}

void CDlgInputSensorWarp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SENSOR_CONDITION, m_nCondition);
	DDX_Text(pDX, IDC_EDIT_SENSOR_FIELD, m_nField);
	DDX_Text(pDX, IDC_EDIT_SENSOR_MARKER, m_nMarker);
	DDX_Text(pDX, IDC_EDIT_SENSOR_TITLE, m_strTitle);
	DDX_Control(pDX, IDC_EDIT_SENSOR_FIELD, m_edtField);
	DDX_Control(pDX, IDC_EDIT_SENSOR_MARKER, m_edtMarker);
	DDX_Control(pDX, IDC_STATIC_SHARED_GROUP, m_sttSharedFieldGroup);
	DDX_Control(pDX, IDC_STATIC_TRIAL_GROUP, m_sttTrialGroup);
	DDX_Control(pDX, IDC_EDIT_SENSOR_FIELD_GROUP, m_edtFieldGroupID);
	DDX_Control(pDX, IDC_EDIT_SENSOR_QUEST, m_edtQuestID);
	DDX_Control(pDX, IDC_STATIC_NAME_FIELD_GROUP, m_sttFieldGroupID);
	DDX_Control(pDX, IDC_STATIC_NAME_QUEST, m_sttQuestID);
	DDX_Control(pDX, IDC_COMBO_SENSOR_GATE_TYPE, m_cmbGateType);
	DDX_Control(pDX, IDC_BUTTON_OPEN_FIELD_MGR, m_btnField);
	DDX_Control(pDX, IDC_BUTTON_OPEN_FIELD_GROUP_MGR, m_btnFieldGroupID);
	DDX_Control(pDX, IDC_STATIC_NAME_FIELD, m_sttField);
	DDX_Control(pDX, IDC_STATIC_NAME_MARKER, m_sttMarker);
	DDX_CBIndex(pDX, IDC_COMBO_SENSOR_GATE_TYPE, m_nGateType);
	DDX_Text(pDX, IDC_EDIT_SENSOR_FIELD_GROUP, m_nFieldGroupID);
	DDX_Text(pDX, IDC_EDIT_SENSOR_QUEST, m_nQuestID);
	DDX_Radio(pDX, IDC_RADIO_SHARED_WARP_TYPE, m_nWarpFieldType);
}

BOOL CDlgInputSensorWarp::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_nWarpFieldType = 0;

	m_strTitle = "Move to another Field";
	m_nCondition = 0;
	m_nGateType = 0;

	m_nField = 0;
	m_nMarker = 0;

	m_nFieldGroupID = 0;
	m_nQuestID = 0;

	EnableSharedFieldGroup(true);
	EnableTrialFieldGroup(false);
		
	UpdateData(FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgInputSensorWarp, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FIELD_MGR, &CDlgInputSensorWarp::OnBnClickedButtonOpenFieldMgr)	
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FIELD_GROUP_MGR, &CDlgInputSensorWarp::OnBnClickedButtonOpenFieldGroupMgr)
	ON_BN_CLICKED(IDC_RADIO_SHARED_WARP_TYPE, &CDlgInputSensorWarp::OnBnClickedRadioSharedWarpType)
	ON_BN_CLICKED(IDC_RADIO_TRIAL_WARP_TYPE, &CDlgInputSensorWarp::OnBnClickedRadioTrialWarpType)
END_MESSAGE_MAP()


// CDlgInputSensorWarp 메시지 처리기입니다.

void CDlgInputSensorWarp::OnBnClickedButtonOpenFieldMgr()
{
	UpdateData(TRUE);
	CDlgResourceInfo dlg(m_pControlMediator_, CDlgResourceInfo::RES_FIELD);
	if (dlg.DoModal()==IDOK )
	{
		m_nField = dlg.m_nSelectedFieldID;
		UpdateData(FALSE);
	}
}

void CDlgInputSensorWarp::OnBnClickedButtonOpenFieldGroupMgr()
{
	UpdateData(TRUE);
	CDlgResourceInfo dlg(m_pControlMediator_, CDlgResourceInfo::RES_FIELD);
	if (dlg.DoModal()==IDOK )
	{
		m_nField = dlg.m_nSelectedFieldID;
		UpdateData(FALSE);
	}
}

void CDlgInputSensorWarp::EnableSharedFieldGroup(bool isEnable)
{
	if (isEnable)
	{
		m_cmbGateType.SetCurSel(0);
		m_cmbGateType.EnableWindow(false);
	}

	m_sttSharedFieldGroup.EnableWindow(isEnable);

	m_sttField.EnableWindow(isEnable);	
	m_edtField.EnableWindow(isEnable);
	m_btnField.EnableWindow(isEnable);
	m_sttMarker.EnableWindow(isEnable);
	m_edtMarker.EnableWindow(isEnable);
}

void CDlgInputSensorWarp::EnableTrialFieldGroup(bool isEnable)
{
	if (isEnable)
	{
		m_cmbGateType.EnableWindow(true);
	}

	m_sttTrialGroup.EnableWindow(isEnable);
	
	m_sttFieldGroupID.EnableWindow(isEnable);
	m_edtFieldGroupID.EnableWindow(isEnable);
	m_btnFieldGroupID.EnableWindow(isEnable);

	m_sttQuestID.EnableWindow(isEnable);
	m_edtQuestID.EnableWindow(isEnable);
}

void CDlgInputSensorWarp::OnBnClickedRadioSharedWarpType()
{
	EnableSharedFieldGroup(true);
	EnableTrialFieldGroup(false);
}

void CDlgInputSensorWarp::OnBnClickedRadioTrialWarpType()
{
	EnableSharedFieldGroup(false);
	EnableTrialFieldGroup(true);
}
