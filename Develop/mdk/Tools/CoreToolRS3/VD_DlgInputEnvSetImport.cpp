// VD_DlgInputEnvSetImport.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgInputEnvSetImport.h"
#include "REnvironment.h"


// CDlgInputEnvSetImport 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputEnvSetImport, CDialog)

CDlgInputEnvSetImport::CDlgInputEnvSetImport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputEnvSetImport::IDD, pParent)
{

}

CDlgInputEnvSetImport::CDlgInputEnvSetImport(int _nTime, int _nWeather)
	: CDialog(CDlgInputEnvSetImport::IDD, NULL)
{
	m_nTime = _nTime;
	m_nWeather = _nWeather;
}

CDlgInputEnvSetImport::~CDlgInputEnvSetImport()
{
}

void CDlgInputEnvSetImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_TIMES_IMPORT, m_ctlTimesToImport_);
	DDX_Control(pDX, IDC_COMBO_WEATHERS_IMPORT, m_ctlWeathersToImport_);
}

BOOL CDlgInputEnvSetImport::OnInitDialog()
{
	CDialog::OnInitDialog();

	using namespace rs3;
	m_ctlTimesToImport_.AddString(REnvironment::ETimeToString(REnvironment::DAWN));
	m_ctlTimesToImport_.AddString(REnvironment::ETimeToString(REnvironment::DAYTIME));
	m_ctlTimesToImport_.AddString(REnvironment::ETimeToString(REnvironment::SUNSET));
	m_ctlTimesToImport_.AddString(REnvironment::ETimeToString(REnvironment::NIGHT));
	m_ctlTimesToImport_.SetCurSel(m_nTime);

	m_ctlWeathersToImport_.AddString(REnvironment::EWeatherToString(REnvironment::SUNNY));
	m_ctlWeathersToImport_.AddString(REnvironment::EWeatherToString(REnvironment::CLOUD));
	m_ctlWeathersToImport_.AddString(REnvironment::EWeatherToString(REnvironment::RAINY));
	m_ctlWeathersToImport_.AddString(REnvironment::EWeatherToString(REnvironment::HEAVY_RANIY));
	m_ctlWeathersToImport_.AddString(REnvironment::EWeatherToString(REnvironment::SNOW));
	m_ctlWeathersToImport_.AddString(REnvironment::EWeatherToString(REnvironment::HEAVY_SNOW));
	m_ctlWeathersToImport_.SetCurSel(m_nWeather);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgInputEnvSetImport, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TIMES_IMPORT, &CDlgInputEnvSetImport::OnCbnSelchangeComboTimesImport)
	ON_CBN_SELCHANGE(IDC_COMBO_WEATHERS_IMPORT, &CDlgInputEnvSetImport::OnCbnSelchangeComboWeathersImport)
END_MESSAGE_MAP()


// CDlgInputEnvSetImport 메시지 처리기입니다.

void CDlgInputEnvSetImport::OnCbnSelchangeComboTimesImport()
{
	m_nTime = m_ctlTimesToImport_.GetCurSel();
}

void CDlgInputEnvSetImport::OnCbnSelchangeComboWeathersImport()
{
	m_nWeather = m_ctlWeathersToImport_.GetCurSel();
}
