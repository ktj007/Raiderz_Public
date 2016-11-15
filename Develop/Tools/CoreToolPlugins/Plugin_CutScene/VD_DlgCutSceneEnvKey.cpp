// VD_DlgCutSceneEnvKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "C_ControlMediator.h"
#include "VD_DlgCutSceneEnvKey.h"

#include "REnvironmentSet.h"
#include "M_ToolSceneManager.h"


// CDlgCutSceneEnvKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneEnvKey, CDialog)

CDlgCutSceneEnvKey::CDlgCutSceneEnvKey(CControlMediator* _pControlMediator, float fTime, int nEnvTime, int nEnvWeather, const char* pSzSetName, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneEnvKey::IDD, pParent)
{
	m_pControlMediator_ = _pControlMediator;
	m_fTime = fTime;

	m_nEnvTime = nEnvTime;
	m_nEnvWeather = nEnvWeather;
	m_strEnvSetName = pSzSetName;
}

CDlgCutSceneEnvKey::~CDlgCutSceneEnvKey()
{
}

void CDlgCutSceneEnvKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ENVKEY_TIME, m_fTime);
	DDX_Control(pDX, IDC_COMBO_ENVKEY_SET, m_ctlEnvSet);
	DDX_Control(pDX, IDC_COMBO_ENVKEY_ENVTIME, m_ctlEnvTimes);
	DDX_Control(pDX, IDC_COMBO_ENVKEY_ENVWEATHER, m_ctlEnvWeathers);
}

BOOL CDlgCutSceneEnvKey::OnInitDialog()
{
	CDialog::OnInitDialog();

	using namespace rs3;

	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	REnvironmentSet *pEnvSet = m_pControlMediator_->GetCurrentEnvironmentSet();
	_ASSERT(pEnvSet);
	std::vector<string> vecArray;
	pEnvSet->GetNames(vecArray);

	int nSelected = -1;
	for ( UINT ui = 0; ui<vecArray.size(); ++ui )
	{
		int nId = m_ctlEnvSet.AddString(vecArray[ui].c_str());
		if (vecArray[ui] == m_strEnvSetName)
		{
			nSelected = nId;
		}
	}

	if (nSelected != -1)
	{
		m_ctlEnvSet.SetCurSel(nSelected);
	}

	m_ctlEnvTimes.AddString(REnvironment::ETimeToString(REnvironment::DAWN));
	m_ctlEnvTimes.AddString(REnvironment::ETimeToString(REnvironment::DAYTIME));
	m_ctlEnvTimes.AddString(REnvironment::ETimeToString(REnvironment::SUNSET));
	m_ctlEnvTimes.AddString(REnvironment::ETimeToString(REnvironment::NIGHT));
	m_ctlEnvTimes.SetCurSel(m_nEnvTime);

	m_ctlEnvWeathers.AddString(REnvironment::EWeatherToString(REnvironment::SUNNY));
	m_ctlEnvWeathers.AddString(REnvironment::EWeatherToString(REnvironment::CLOUD));
	m_ctlEnvWeathers.AddString(REnvironment::EWeatherToString(REnvironment::RAINY));
	m_ctlEnvWeathers.AddString(REnvironment::EWeatherToString(REnvironment::HEAVY_RANIY));
	m_ctlEnvWeathers.AddString(REnvironment::EWeatherToString(REnvironment::SNOW));
	m_ctlEnvWeathers.AddString(REnvironment::EWeatherToString(REnvironment::HEAVY_SNOW));
	m_ctlEnvWeathers.SetCurSel(m_nEnvWeather);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgCutSceneEnvKey, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CDlgCutSceneEnvKey 메시지 처리기입니다.

void CDlgCutSceneEnvKey::GetEnvKeyInfo( float& fTime, int& nEnvTime, int& nEnvWeather, std::string& rEnvSetName )
{
	fTime = m_fTime;
	nEnvTime = m_nEnvTime;
	nEnvWeather = m_nEnvWeather;

	rEnvSetName = m_strEnvSetName;
}

void CDlgCutSceneEnvKey::OnDestroy()
{
	m_nEnvTime = m_ctlEnvTimes.GetCurSel();
	m_nEnvWeather = m_ctlEnvWeathers.GetCurSel();

	CString strName;
	m_ctlEnvSet.GetLBText(m_ctlEnvSet.GetCurSel(),strName);
	m_strEnvSetName = strName.GetString();

	CDialog::OnDestroy();
}
