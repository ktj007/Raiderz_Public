// V_EditLogicList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_EntityEnvironment.h"
#include "VR_EnvWeather.h"

#include "C_ControlMediator.h"
#include "C_ControlCursor.h"
#include "M_ToolLogicObjectManager.h"

#include "WS_WorkSpaceManager.h"
#include "WS_Field.h"

#include "../../../Game/XSceneNodeEnvModifier.h"

const DWORD WM_PROP_UPDATE_MESSAGE = WM_USER + 0xf0;

const char* BGM_DESC[] =
{
	"필드배경음(Field)",

	"새벽(Dawn)",
	"낮(DayTime)",
	"해질녁(SunSet)",
	"밤(Night)",

	"흐림(Clody)",
	"비(Rain)",
	"폭우(Heavy Rain)",
	"폭설(Heavy Snow)",

	"던젼(Dungeon)",
	"분류불가(Normal)"
};

// CRollupEntityEnvironment 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupEntityEnvironment, CDialog)

CRollupEntityEnvironment::CRollupEntityEnvironment(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEntityEnvironment::IDD, pParent)
{
	for(int i = 0; i < WEATHER_NUM; ++i)
		m_nWeather_[i] = 0;
}

CRollupEntityEnvironment::~CRollupEntityEnvironment()
{
}

BOOL CRollupEntityEnvironment::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT rect;
	rect.left	= 7;
	rect.top	= 5;
	rect.right	= rect.left + 250;
	rect.bottom	= rect.top + 320;

	m_ctlFieldInfoProp.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rect, this, (UINT)-1);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CRollupEntityEnvironment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_SUN, m_nWeatherRadio_);

	DDX_Text(pDX, IDC_EDIT_SUN, m_nWeather_[SUN]);
	DDX_Text(pDX, IDC_EDIT_CLD, m_nWeather_[CLD]);
	DDX_Text(pDX, IDC_EDIT_RNY, m_nWeather_[RNY]);
	DDX_Text(pDX, IDC_EDIT_HRN, m_nWeather_[HRN]);
	DDX_Text(pDX, IDC_EDIT_SNW, m_nWeather_[SNW]);
	DDX_Text(pDX, IDC_EDIT_HSW, m_nWeather_[HSW]);
	DDX_Text(pDX, IDC_EDIT_CLR, m_nWeather_[CLR]);
	DDX_Control(pDX, IDC_COMBO_ENTITY_ENV_SET, m_ctlEnvSet_);
}

BEGIN_MESSAGE_MAP(CRollupEntityEnvironment, CDialog)
	ON_BN_CLICKED(IDC_RADIO_SUN, &CRollupEntityEnvironment::OnBnClickedRadioSun)
	ON_BN_CLICKED(IDC_RADIO_CLD, &CRollupEntityEnvironment::OnBnClickedRadioCld)
	ON_BN_CLICKED(IDC_RADIO_RNY, &CRollupEntityEnvironment::OnBnClickedRadioRny)
	ON_BN_CLICKED(IDC_RADIO_HRN, &CRollupEntityEnvironment::OnBnClickedRadioHrn)
	ON_BN_CLICKED(IDC_RADIO_SNW, &CRollupEntityEnvironment::OnBnClickedRadioSnw)
	ON_BN_CLICKED(IDC_RADIO_HSW, &CRollupEntityEnvironment::OnBnClickedRadioHsw)
	ON_BN_CLICKED(IDC_RADIO_CLR, &CRollupEntityEnvironment::OnBnClickedRadioClr)
	ON_EN_CHANGE(IDC_EDIT_SUN, &CRollupEntityEnvironment::OnEnChangeEditSun)
	ON_EN_CHANGE(IDC_EDIT_CLD, &CRollupEntityEnvironment::OnEnChangeEditCld)
	ON_EN_CHANGE(IDC_EDIT_RNY, &CRollupEntityEnvironment::OnEnChangeEditRny)
	ON_EN_CHANGE(IDC_EDIT_HRN, &CRollupEntityEnvironment::OnEnChangeEditHrn)
	ON_EN_CHANGE(IDC_EDIT_SNW, &CRollupEntityEnvironment::OnEnChangeEditSnw)
	ON_EN_CHANGE(IDC_EDIT_HSW, &CRollupEntityEnvironment::OnEnChangeEditHsw)
	ON_EN_CHANGE(IDC_EDIT_CLR, &CRollupEntityEnvironment::OnEnChangeEditClr)
	ON_CBN_SELCHANGE(IDC_COMBO_ENTITY_ENV_SET, &CRollupEntityEnvironment::OnCbnSelchangeComboEntityEnvSet)
	ON_BN_CLICKED(IDC_DEFAULT_BGM_BUTTON, &CRollupEntityEnvironment::OnBnClickedDefaultBgmButton)
END_MESSAGE_MAP()

// CEditEntityEnvironment 메시지 처리기입니다.

void CRollupEntityEnvironment::Listen(CoreMessage& _message)
{
	switch ( _message.nID )
	{
	case VIEW_MESSAGE::BEFORE_SCENE_CLEAR:
		{
			rs3::RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
			XSceneNodeEnvModifier* pEnvModifier = pSceneManager->GetSceneNodeEnvModifier<XSceneNodeEnvModifier>();
			if (pEnvModifier)
				pEnvModifier->DetachNightNodeController( &pSceneManager->GetRootSceneNode() );
		}
	break;

	case VIEW_MESSAGE::DEVICE_CREATED :
		{
			rs3::RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
			pSceneManager->CreateSceneNodeEnvModifier<XSceneNodeEnvModifier>();
		}
	break;

	case VIEW_MESSAGE::LOGIC_LOADED:
		{
			// enable current window
			EnableWindow(TRUE);

			CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
			_ASSERT(pField);
			int nDefaultWeather = 0;
			for(int i = 0; i < WEATHER_NUM; ++i)
			{
				pField->GetWeatherValue(i, m_nWeather_[i]);
				if(m_nWeather_[i] == CWorkSpaceField::WEATHER_DEFAULT)
				{
					nDefaultWeather = i;
				}
			}

			SetWeatherRadio_(nDefaultWeather);

			// property controller
			m_ctlFieldInfoProp.Init(m_pControlMediator_);
		}
		break;
	}
}

void CRollupEntityEnvironment::SetWeatherRadio_(int _nIndex)
{
	m_nWeatherRadio_ = _nIndex;

	GetDlgItem(IDC_EDIT_SUN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CLD)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_RNY)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_HRN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SNW)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_HSW)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_CLR)->EnableWindow(TRUE);

	switch( m_nWeatherRadio_ )
	{
		case SUN:
		{
			GetDlgItem(IDC_EDIT_SUN)->EnableWindow(FALSE);
		}
		break;
		case CLD:
		{
			GetDlgItem(IDC_EDIT_CLD)->EnableWindow(FALSE);
		}
		break;
		case RNY:
		{
			GetDlgItem(IDC_EDIT_RNY)->EnableWindow(FALSE);
		}
		break;
		case HRN:
		{
			GetDlgItem(IDC_EDIT_HRN)->EnableWindow(FALSE);
		}
		break;
		case SNW:
		{
			GetDlgItem(IDC_EDIT_SNW)->EnableWindow(FALSE);
		}
		break;
		case HSW:
		{
			GetDlgItem(IDC_EDIT_HSW)->EnableWindow(FALSE);
		}
		break;
		case CLR:
		{
			GetDlgItem(IDC_EDIT_CLR)->EnableWindow(FALSE);
		}
		break;
	}

	m_nWeather_[_nIndex] = 0;
	UpdateData(FALSE);
}

void CRollupEntityEnvironment::OnCancel()
{
}
void CRollupEntityEnvironment::OnOK()
{
}

void CRollupEntityEnvironment::OnBnClickedRadioSun()
{
	m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->SetWeatherValue(SUN, CWorkSpaceField::WEATHER_DEFAULT);

	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		pField->GetWeatherValue(i, m_nWeather_[i]);
	}
	SetWeatherRadio_(SUN);
}

void CRollupEntityEnvironment::OnBnClickedRadioCld()
{
	m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->SetWeatherValue(CLD, CWorkSpaceField::WEATHER_DEFAULT);

	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		pField->GetWeatherValue(i, m_nWeather_[i]);
	}
	SetWeatherRadio_(CLD);
}

void CRollupEntityEnvironment::OnBnClickedRadioRny()
{
	m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->SetWeatherValue(RNY, CWorkSpaceField::WEATHER_DEFAULT);

	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		pField->GetWeatherValue(i, m_nWeather_[i]);
	}
	SetWeatherRadio_(RNY);
}

void CRollupEntityEnvironment::OnBnClickedRadioHrn()
{
	m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->SetWeatherValue(HRN, CWorkSpaceField::WEATHER_DEFAULT);

	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		pField->GetWeatherValue(i, m_nWeather_[i]);
	}
	SetWeatherRadio_(HRN);
}

void CRollupEntityEnvironment::OnBnClickedRadioSnw()
{
	m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->SetWeatherValue(SNW, CWorkSpaceField::WEATHER_DEFAULT);

	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		pField->GetWeatherValue(i, m_nWeather_[i]);
	}
	SetWeatherRadio_(SNW);
}

void CRollupEntityEnvironment::OnBnClickedRadioHsw()
{
	m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->SetWeatherValue(HSW, CWorkSpaceField::WEATHER_DEFAULT);

	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		pField->GetWeatherValue(i, m_nWeather_[i]);
	}
	SetWeatherRadio_(HSW);
}

void CRollupEntityEnvironment::OnBnClickedRadioClr()
{
	m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->SetWeatherValue(CLR, CWorkSpaceField::WEATHER_DEFAULT);

	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		pField->GetWeatherValue(i, m_nWeather_[i]);
	}
	SetWeatherRadio_(CLR);
}

void CRollupEntityEnvironment::ValidUpdateWeather_(int _sel, CWnd* _pWnd)
{
	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	_ASSERT(pField);

	UpdateData(TRUE);
	int total = 0;
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		if(i != m_nWeatherRadio_)
			total += m_nWeather_[i];
	}
	if(total > 100)
	{
		AfxMessageBox("Greater than 100 percent of the total weather.");
		m_nWeather_[_sel] = 0;
		pField->SetWeatherValue(_sel, 0);
		UpdateData(FALSE);
		CEdit* pEdit = (CEdit*)_pWnd;
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);//전체 선택
		return;
	}
	pField->SetWeatherValue(_sel, m_nWeather_[_sel]);
	return;
}

void CRollupEntityEnvironment::OnEnChangeEditSun()
{
	ValidUpdateWeather_(SUN, GetDlgItem(IDC_EDIT_SUN));
}

void CRollupEntityEnvironment::OnEnChangeEditCld()
{
	ValidUpdateWeather_(CLD, GetDlgItem(IDC_EDIT_CLD));
}

void CRollupEntityEnvironment::OnEnChangeEditRny()
{
	ValidUpdateWeather_(RNY, GetDlgItem(IDC_EDIT_RNY));
}

void CRollupEntityEnvironment::OnEnChangeEditHrn()
{
	ValidUpdateWeather_(HRN, GetDlgItem(IDC_EDIT_HRN));
}

void CRollupEntityEnvironment::OnEnChangeEditSnw()
{
	ValidUpdateWeather_(SNW, GetDlgItem(IDC_EDIT_SNW));
}

void CRollupEntityEnvironment::OnEnChangeEditHsw()
{
	ValidUpdateWeather_(HSW, GetDlgItem(IDC_EDIT_HSW));
}

void CRollupEntityEnvironment::OnEnChangeEditClr()
{
	ValidUpdateWeather_(CLR, GetDlgItem(IDC_EDIT_CLR));
}

void CRollupEntityEnvironment::SetEnvironmentSet(const char* _pSzEnvSetName, bool _bUpdateEnvSet)
{
	int nSelected = m_ctlEnvSet_.SelectString(0, _pSzEnvSetName);
	if( nSelected < 0 ) return;

	m_ctlEnvSet_.SetCurSel(nSelected);
	if(_bUpdateEnvSet)
		UpdateEnvSet_();
}

void CRollupEntityEnvironment::ResetEntityEnvironment(std::vector<std::string>& _refVecString)
{
	m_ctlEnvSet_.ResetContent();
	for(std::vector<std::string>::iterator itr = _refVecString.begin();
		itr != _refVecString.end(); ++itr)
	{
		m_ctlEnvSet_.AddString(itr->c_str());
	}
}

void CRollupEntityEnvironment::OnCbnSelchangeComboEntityEnvSet()
{
	UpdateEnvSet_();
}

void CRollupEntityEnvironment::UpdateEnvSet_()
{
	int sel = m_ctlEnvSet_.GetCurSel();
	if(sel < 0) return;

	CString cstrEnvSetName;
	m_ctlEnvSet_.GetLBText(sel, cstrEnvSetName);
	m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->SetEnvSetName(cstrEnvSetName.GetString());
}

void CRollupEntityEnvironment::GetCurrentEnvironmentSet(std::string& _refString)
{
	int sel = m_ctlEnvSet_.GetCurSel();
	if(sel < 0) return;

	CString cstrEnvSetName;
	m_ctlEnvSet_.GetLBText(sel, cstrEnvSetName);
	_refString = cstrEnvSetName.GetString();
}

CRollupEntityEnvironment::CPropertyView::CPropertyView()
{
	m_bTown = false;
	m_bReturnable = true;
	m_bTutorial = false;
	
	m_nMaxGatherMine = 0;
	m_nMaxGatherHerb = 0;
	m_nMaxGatherRelic = 0;
	m_nMaxGatherLumber = 0;

	m_nQPVPNPC1 = 0;
	m_nQPVPNPC2 = 0;
	m_nQPVPRebirth1 = 0;
	m_nQPVPRebirth2 = 0;

	m_pControlMediator_ = NULL;
}

CRollupEntityEnvironment::CPropertyView::~CPropertyView()
{

}

void CRollupEntityEnvironment::CPropertyView::Init(CControlMediator* pControlMediator)
{
	m_pControlMediator_ = pControlMediator;
	if (m_pControlMediator_)
	{
		const CWorkSpaceField::FIELD_INFO& fieldInfo = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->m_fieldInfo;

		m_bTown = fieldInfo.m_bTownField;
		m_bReturnable = fieldInfo.m_bReturnable;
		m_bTutorial = fieldInfo.m_bTutorial;

		m_nMaxGatherMine = fieldInfo.m_nMaxGatherMine;
		m_nMaxGatherHerb = fieldInfo.m_nMaxGatherHerb;
		m_nMaxGatherRelic = fieldInfo.m_nMaxGatherRelic;
		m_nMaxGatherLumber = fieldInfo.m_nMaxGatherLumber;

		m_nQPVPNPC1 = fieldInfo.m_nQPVPNPC1;
		m_nQPVPNPC2 = fieldInfo.m_nQPVPNPC2;
		m_nQPVPRebirth1 = fieldInfo.m_nQPVPRebirth1;
		m_nQPVPRebirth2 = fieldInfo.m_nQPVPRebirth2;

		for (int i=0; i<BGM_MAX; i++ )
		{
			m_strBGM[i]	= fieldInfo.m_strBGM[i].c_str();
		}

		FreshProperty();
	}
}

void CRollupEntityEnvironment::CPropertyView::FreshProperty()
{
	_ASSERT(m_pControlMediator_);
	RemoveAll();

	CMFCPropertyGridProperty* fieldProperty = new CMFCPropertyGridProperty( _T("Field Info") );

	// 타운
	AddPropertyItem(fieldProperty, "Town", m_bTown, true);

	// 귀환
	AddPropertyItem(fieldProperty, "Return", m_bReturnable, true);

	// 튜토리얼
	AddPropertyItem(fieldProperty, "Tutorial", m_bTutorial, false);

	// QPVPNPC
	AddPropertyItem( fieldProperty, "QPVPNPC1", m_nQPVPNPC1, true );
	AddPropertyItem( fieldProperty, "QPVPNPC2", m_nQPVPNPC2, true );

	// QPVPMarker
	AddPropertyItem( fieldProperty, "QPVPRebirth1", m_nQPVPRebirth1, true );
	AddPropertyItem( fieldProperty, "QPVPRebirth2", m_nQPVPRebirth2, true );

	// 채집
	CMFCPropertyGridProperty* gatherProperty = AddPropertyGroupItem( fieldProperty, "Collected Objs", true );
	AddPropertyItem( gatherProperty, "Mineral", m_nMaxGatherMine, true );
	AddPropertyItem( gatherProperty, "Herbs", m_nMaxGatherHerb, true );
	AddPropertyItem( gatherProperty, "Relics", m_nMaxGatherRelic, true );
	AddPropertyItem( gatherProperty, "Lumber", m_nMaxGatherLumber, true );	

	// 배경음
	CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
	const SOUNDFILELIST &mapBgmList = m_pControlMediator_->GetToolLogicObjectMgr()->GetBGMList();

	//필드음
	V_CCoreToolPropertyGridCtrl::AddPropertyBGMItem( mapBgmList, fieldProperty, BGM_DESC[0], m_strBGM[0]);

	//환경음
	CMFCPropertyGridProperty* bgmProperty = AddPropertyGroupItem( fieldProperty, "Env. Sound", true );
	for (int i=1; i<BGM_MAX; i++ )
		V_CCoreToolPropertyGridCtrl::AddPropertyBGMItem( mapBgmList, bgmProperty, BGM_DESC[i], m_strBGM[i]);

	// 최종 더하기
	AddProperty(fieldProperty);
}

void CRollupEntityEnvironment::CPropertyView::OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const
{
	if (m_pControlMediator_)
	{
		CCoreToolPropertyGridCtrl::OnPropertyChanged( pProp );
		//m_pControlMediator_->

		CWorkSpaceField::FIELD_INFO& fieldInfo = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->m_fieldInfo;

		fieldInfo.m_bTownField = m_bTown;
		fieldInfo.m_bReturnable = m_bReturnable;
		fieldInfo.m_bTutorial = m_bTutorial;

		fieldInfo.m_nMaxGatherMine = m_nMaxGatherMine;
		fieldInfo.m_nMaxGatherHerb = m_nMaxGatherHerb;
		fieldInfo.m_nMaxGatherRelic = m_nMaxGatherRelic;
		fieldInfo.m_nMaxGatherLumber = m_nMaxGatherLumber;

		fieldInfo.m_nQPVPNPC1 = m_nQPVPNPC1;
		fieldInfo.m_nQPVPNPC2 = m_nQPVPNPC2;
		fieldInfo.m_nQPVPRebirth1 = m_nQPVPRebirth1;
		fieldInfo.m_nQPVPRebirth2 = m_nQPVPRebirth2;

		for (int i=0; i<BGM_MAX; i++ )
		{
			fieldInfo.m_strBGM[i]	= _bstr_t(m_strBGM[i]);
		}

		GetParent()->SendMessage(WM_PROP_UPDATE_MESSAGE);
	}
}

void CRollupEntityEnvironment::OnBnClickedDefaultBgmButton()
{
	if (m_pControlMediator_)
	{
		CWorkSpaceField* pWSField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
		if(pWSField)
		{
			pWSField->LoadBGMXML("default_BGM.xml");
		}

		const CWorkSpaceField::FIELD_INFO& fieldInfo = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->m_fieldInfo;
		for (int i=0; i<BGM_MAX; i++ )
		{
			m_ctlFieldInfoProp.m_strBGM[i]	= fieldInfo.m_strBGM[i].c_str();
		}

		m_ctlFieldInfoProp.FreshProperty();
	}
}
