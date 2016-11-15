// V_EditEnvWeather.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_EnvWeather.h"

#include "V_MFCHelper.h"
#include "C_ControlMediator.h"
#include "M_ToolCmdMgr.h"

#include "C_XMLConfig.h"
#include "M_ToolSceneManager.h"
#include "VD_DlgCommonFileOpen.h"
#include "RMesh.h"
#include "RSkyManager.h"

#include "RVType.h"
#include "RConfiguration.h"
#include "M_ToolActor.h"

#include "VD_DlgInputCommon.h"
#include "VD_DlgInputEnvSetImport.h"

#include "C_XMLResourceList.h"
#include "C_XMLUserConfig.h"

#include "C_ControlMediatorHelperAction.h" // for entity env
#include "C_ControlMediatorHelperRender.h" // for env modifier

#include "RWeatherEffect.h"

#include "WS_WorkSpaceManager.h"

#include "VR_EtcCollision.h"

#include "EnvSetReflection.h"

using namespace rs3;
using namespace std;

const DWORD WM_PROP_UPDATE_MESSAGE = WM_USER + 0xf0;

// CRollupEnvWeather 대화 상자입니다.
IMPLEMENT_DYNAMIC(CRollupEnvWeather, CDialog)
CRollupEnvWeather::CRollupEnvWeather(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEnvWeather::IDD, pParent)
	// 스카이박스 멤버 초기화
	, m_nSkyColorR(255)
	, m_nSkyColorG(255)
	, m_nSkyColorB(255)
	, m_nSkyFogPercent(0)
	, m_pEnvSetRef(NULL)
	, m_pPropCtrl(NULL)
{
	m_pCurrentSkyManager = NULL;

	m_fEnvUpdateFadingTime_ = 1.0f;

	m_pEnvSetRef = new CEnvSetRef;
}

CRollupEnvWeather::~CRollupEnvWeather()
{
}

BOOL DoDialogColorPick(COLORREF *pdwOutColor, HWND hWnd, COLORREF dwDefaultColor = RGB(255, 255, 255))
{
	CColorDialog dlg( dwDefaultColor , CC_FULLOPEN);
	if(dlg.DoModal() == IDOK)
	{
		*pdwOutColor = dlg.GetColor();
		return TRUE;
	}
	return FALSE;
}

BOOL CRollupEnvWeather::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlSliderSkyFog.SetRange(0, 100, TRUE);

	m_ctlSliderSkyUVAniSpeed.SetRange(0, 4*100, TRUE);
	m_ctlSliderSkyUVAniSpeed.SetPos(100);
	m_ctlSliderSkyUVAniSpeed.SetTicFreq(20); //Set Tic Marks

	UpdateData(FALSE);

	m_fogCheckBox.SetCheck(FALSE);

	m_ctlTimes.AddString(REnvironment::ETimeToString(REnvironment::DAWN));
	m_ctlTimes.AddString(REnvironment::ETimeToString(REnvironment::DAYTIME));
	m_ctlTimes.AddString(REnvironment::ETimeToString(REnvironment::SUNSET));
	m_ctlTimes.AddString(REnvironment::ETimeToString(REnvironment::NIGHT));
	m_ctlTimes.SetCurSel(0);

	m_ctlWeathers.AddString(REnvironment::EWeatherToString(REnvironment::SUNNY));
	m_ctlWeathers.AddString(REnvironment::EWeatherToString(REnvironment::CLOUD));
	m_ctlWeathers.AddString(REnvironment::EWeatherToString(REnvironment::RAINY));
	m_ctlWeathers.AddString(REnvironment::EWeatherToString(REnvironment::HEAVY_RANIY));
	m_ctlWeathers.AddString(REnvironment::EWeatherToString(REnvironment::SNOW));
	m_ctlWeathers.AddString(REnvironment::EWeatherToString(REnvironment::HEAVY_SNOW));
	m_ctlWeathers.SetCurSel(0);

	UpdateSkyLayerList();

	CWnd* pPropertyParent = GetDlgItem( IDC_ENVWEATHER_PROPERTY );
	m_pPropCtrl = m_UI.createPropGridCtrl( pPropertyParent );
	
	CReflectivePropertyGridCtrl::PROPERTY_CHANGE_HANDLER handler(this, &CRollupEnvWeather::OnPropertyChanged );
	m_pPropCtrl->SetCallback( handler );

	return TRUE;
}

void CRollupEnvWeather::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_VIEW_FOG, m_fogCheckBox);

	DDX_Control(pDX, IDC_LIST_ENV, m_ctlListBoxSkies);
	DDX_Control(pDX, IDC_SLIDER1, m_ctlSlideSkyAlpha);

	DDX_Text(pDX, IDC_EDIT_CR, m_nSkyColorR);
	DDX_Text(pDX, IDC_EDIT_CG, m_nSkyColorG);
	DDX_Text(pDX, IDC_EDIT_CB, m_nSkyColorB);

	DDX_Control(pDX, IDC_COMBO_TIMES, m_ctlTimes);
	DDX_Control(pDX, IDC_COMBO_WEATHERS, m_ctlWeathers);
	DDX_Control(pDX, IDC_CHECK_VIEW_ENV, m_ctlEnvViewButton);
	DDX_Control(pDX, IDC_COMBO_ENVSET, m_ctlEnvSet);

	DDX_Control(pDX, IDC_SLIDER_UVSPEED, m_ctlSliderSkyUVAniSpeed);
	DDX_Control(pDX, IDC_SLIDER_SKYFOG, m_ctlSliderSkyFog);
	DDX_Text(pDX, IDC_EDIT_SKYFOG, m_nSkyFogPercent);
	DDV_MinMaxInt(pDX, m_nSkyFogPercent, 0, 100);
}

BEGIN_MESSAGE_MAP(CRollupEnvWeather, CDialog)
	ON_BN_CLICKED(IDC_CHECK_VIEW_FOG, OnBnClickedCheckViewFog)

	ON_BN_CLICKED(IDC_ADD_SKY, &CRollupEnvWeather::OnBnClickedAddSky)
	ON_BN_CLICKED(IDC_DEL_SKY, &CRollupEnvWeather::OnBnClickedDelSky)
	ON_WM_HSCROLL()

	ON_LBN_SELCHANGE(IDC_LIST_ENV, &CRollupEnvWeather::OnLbnSelchangeListEnv)

	ON_STN_CLICKED(IDC_SKY_PICK, OnStnClickedSkyPick)

	ON_BN_CLICKED(IDC_BUTTON_UP, &CRollupEnvWeather::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CRollupEnvWeather::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_CHECK_VIEW_ENV, &CRollupEnvWeather::OnBnClickedCheckViewEnv)
	ON_CBN_SELCHANGE(IDC_COMBO_TIMES, &CRollupEnvWeather::OnCbnSelchangeComboTimes)
	ON_CBN_SELCHANGE(IDC_COMBO_WEATHERS, &CRollupEnvWeather::OnCbnSelchangeComboWeathers)
	ON_CBN_SELCHANGE(IDC_COMBO_ENVSET, &CRollupEnvWeather::OnCbnSelchangeComboEnvset)
	ON_BN_CLICKED(IDC_BUTTON_DELENVSET, &CRollupEnvWeather::OnBnClickedButtonDelenvset)
	ON_BN_CLICKED(IDC_BUTTON_ADDEDENVSET, &CRollupEnvWeather::OnBnClickedButtonAddedenvset)

	ON_BN_CLICKED(IDC_BUTTON_ENV_IMPORT_DEFAULT, &CRollupEnvWeather::OnBnClickedButtonEnvImportDefault)
	ON_BN_CLICKED(IDC_BUTTON_ENV_IMPORT, &CRollupEnvWeather::OnBnClickedButtonEnvImport)
	ON_BN_CLICKED(IDC_BUTTON_ENV_EXPORT, &CRollupEnvWeather::OnBnClickedButtonEnvExport)

	ON_MESSAGE(WM_PROP_UPDATE_MESSAGE,&CRollupEnvWeather::OnPropertyViewNotify)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CEditEnvWeather 메시지 처리기입니다.
void CRollupEnvWeather::OnBnClickedCheckViewFog()
{
	// 포그 보기/안보기
	RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	pCamera->SetFogEnable( m_fogCheckBox.GetCheck() ? true : false );
}

LRESULT CRollupEnvWeather::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = CDialog::WindowProc(message, wParam, lParam);

	switch(message) {
		case WM_PAINT:
		case WM_WINDOWPOSCHANGED:
		case WM_USER_SIDEPANEL_EXPANDED:
		case WM_USER_REBAR_EXPANDED:
		{
			CMFCHelper::FillWnd(GetDlgItem(IDC_SKY_PICK), m_nSkyColorR, m_nSkyColorG, m_nSkyColorB);
		}
		break;
		case WM_USER_MULTISLIDER_CHANGED:
		{
			// MultiSlider를 사용하지 않음.
		}
		break;
	}
	return result;
}

void CRollupEnvWeather::Listen(CoreMessage& _message)
{
	switch (_message.nID)
	{
	case VIEW_MESSAGE::SCENE_LOADED :
	case VIEW_MESSAGE::SCENE_IMPORTED :
		{
			// 신이 로드되었을때 visible at night 노드목록을 갱신한다
			m_pControlMediator_->GetToolSceneManager()->ResetEnvModifierRootScene();
		}
		break;
	}

	switch( _message.nID )
	{
	case VIEW_MESSAGE::DEVICE_CREATED :
		{
			//skybox
			//UpdateListbox();
			m_ctlSlideSkyAlpha.SetRange(0, 255);
			m_ctlSlideSkyAlpha.SetPos(0);
		}
	break;
	case VIEW_MESSAGE::SCENE_LOADED :
	case VIEW_MESSAGE::LOGIC_LOADED :
		{
			// ui 활성
			SetUIActivity_(true);

			// reset current content
			// 씬 매니저와 그 내부값은 메디에이터의 초기화 과정에서 초기화되어
			// 넘어져 오니, 현재 Setting 되어있던 Sky만 NULL로~
			ResetCurrentEnvironmetObjectPtr_(NULL, NULL);
			m_ctlEnvSet.ResetContent();

			// fog 로드하기
			DWORD dwColor;
			float fFogNear,fFogFar;
			m_pControlMediator_->GetToolCamera()->GetFogInfo(fFogNear, fFogFar, dwColor);

			// env set load
			rs3::RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
			rs3::REnvironmentSet *pEnvSet = m_pControlMediator_->GetCurrentEnvironmentSet();
			_ASSERT(pEnvSet);
			std::vector<string> vecArray;
			pEnvSet->GetNames(vecArray);

			for ( UINT ui = 0; ui<vecArray.size(); ++ui )
				int nIndex = m_ctlEnvSet.AddString(vecArray[ui].c_str());

			// config로 저장된 EnvSet 읽기
			int time, weather;
			bool visible;
			std::string envSetName;
			CXMLUserConfig::GetReference().GetEnvironmentInfo(visible, time, weather, envSetName);
			_ASSERT( time >= 0 );
			_ASSERT( weather >= 0);
			_ASSERT( !envSetName.empty() );
			SetWeatherInfo( visible ? 1 : 0, time, weather, envSetName);
		}
	break;
	}
}

void CRollupEnvWeather::UpdateSkyLayerList()
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	if ( pSceneManager == NULL )
		return;

	m_ctlListBoxSkies.ResetContent();

	REnvironment::SEnvSet *pSet = GetCurrentEnvSet();
	if ( pSet == NULL )
		return;
	
	m_pCurrentSkyManager = pSet->pSkyManager;
	if ( m_pCurrentSkyManager )
	{
		// 0번 Layer (현재는 0번 레이어만 사용된다.)
		SKYARRAY *pArray = m_pCurrentSkyManager->GetArray(0);

		// set sky-box list
		m_ctlSlideSkyAlpha.SetPos(0);
		for ( UINT ui = 0; ui<pArray->size(); ++ui )
		{
			RSkyInfo &info = (pArray[0])[ui];
			if ( info.pSky )
			{
				std::string strName = info.strFilename;
				int token = strName.rfind("/") + 1;
				int nIndex = m_ctlListBoxSkies.AddString(strName.substr(token, strName.length() - token).c_str());
				m_ctlListBoxSkies.SetItemDataPtr(nIndex,(void*)info.pSky);
			}
		}
	}
}

// SkyBox를 추가한다.
void CRollupEnvWeather::OnBnClickedAddSky()
{
	REnvironment::SEnvSet *pSet = GetCurrentEnvSet();
	if(!pSet) return;

	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcSkyPath, 256 );
	CListViewOpenDlg dlg(szfull);

	dlg.SetTitle("Skybox");
	dlg.SetFilter("Skybox Object File\0*.elu;");

	if(dlg.DoModal() == IDOK)
	{
		RSkyInfo kInfo;

		string fileName = dlg.GetFileName();

		kInfo.strFilename = fileName;
		if (kInfo.strFilename.empty())
			return;

		kInfo.pSky = new RSceneSky( &kInfo );
		kInfo.skyRefCount = 1;
		if ( kInfo.pSky->Create(kInfo.strFilename.c_str(), false) == false )
		{
			SAFE_DELETE(kInfo.pSky);
			kInfo.strFilename.clear();
			return;
		}

		pSet->pSkyManager->AddSky(0, kInfo);
		
		UpdateEnvironment();
		UpdateSkyLayerList();
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}
}

void CRollupEnvWeather::OnBnClickedDelSky()
{
	int nSel = m_ctlListBoxSkies.GetCurSel();
	if ( nSel < 0 )		return;

	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	if ( pSceneManager == NULL )
		return;

	RSceneSky *pSkyNode = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nSel);

	REnvironment::SEnvSet *pSet = GetCurrentEnvSet();
	if ( pSet )
	{
		int nIndex = pSet->pSkyManager->FindSky(0, pSkyNode);
		pSet->pSkyManager->DelSky(0, nIndex);

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}

	UpdateSkyLayerList();
}

void CRollupEnvWeather::UpdateScrollBars()
{
	int nSel = m_ctlListBoxSkies.GetCurSel();
	if ( nSel >= 0 )
	{
		REnvironment::SEnvSet *pSet = GetCurrentEnvSet();
		if ( pSet )
		{
			RSceneSky *pSkyNode = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nSel);

			int nIndex = pSet->pSkyManager->FindSky(0, pSkyNode);
			if ( nIndex >= 0 )
			{
				RSkyInfo *pInfo = pSet->pSkyManager->GetSkyInfo(0, nIndex);
				_ASSERT(pInfo);

				pInfo->dwColor = D3DCOLOR_ARGB(m_ctlSlideSkyAlpha.GetPos(), m_nSkyColorR, m_nSkyColorG, m_nSkyColorB);

				pInfo->fUVAniSpeed = m_ctlSliderSkyUVAniSpeed.GetPos() / 100.f;
				pInfo->fFogFactor = (100 - m_ctlSliderSkyFog.GetPos()) / 100.f;
				
				pSkyNode->SetSpeed(pInfo->fUVAniSpeed);
				
				m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
				m_nSkyFogPercent = m_ctlSliderSkyFog.GetPos();
			}
		}
	}

	UpdateData(FALSE);
}

void CRollupEnvWeather::OnPropertyChanged()
{
	if( m_ctlEnvViewButton.GetCheck() )
		ApplyEnvironment();
}

void CRollupEnvWeather::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateScrollBars();	

	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRollupEnvWeather::OnLbnSelchangeListEnv()
{
	int nSel = m_ctlListBoxSkies.GetCurSel();
	if ( nSel < 0 )		return;

	if ( m_pCurrentSkyManager )
	{
		RSceneSky *pSkyNode = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nSel);

		int nIndex = m_pCurrentSkyManager->FindSky(0, pSkyNode);
		if ( nIndex >= 0 )
		{
			RSkyInfo *pInfo = m_pCurrentSkyManager->GetSkyInfo(0, nIndex);
			_ASSERT(pInfo);

			DWORD dwColor = pInfo->dwColor;

			int nAlpha = ( dwColor >> 24 ) & 0xff;
			m_ctlSlideSkyAlpha.SetPos(nAlpha);

			m_nSkyColorR = ( dwColor >> 16 ) & 0xff;
			m_nSkyColorG = ( dwColor >> 8 ) & 0xff;
			m_nSkyColorB = ( dwColor ) & 0xff;

			m_ctlSliderSkyUVAniSpeed.SetPos(pInfo->fUVAniSpeed*100.f);
			m_ctlSliderSkyFog.SetPos( 100 - (pInfo->fFogFactor *100.0f ));
			m_nSkyFogPercent = m_ctlSliderSkyFog.GetPos();

			CMFCHelper::FillWnd(GetDlgItem(IDC_SKY_PICK), m_nSkyColorR, m_nSkyColorG, m_nSkyColorB);
			UpdateData(FALSE);
		}
	}
}

void CRollupEnvWeather::OnStnClickedSkyPick()
{
	static DWORD rgbCurrent;		// initial color selection
	rgbCurrent = RGB(m_nSkyColorR, m_nSkyColorG, m_nSkyColorB);

	DWORD dwColor;

	if ( DoDialogColorPick(&dwColor, m_hWndTop, rgbCurrent))
	{
		int nSel = m_ctlListBoxSkies.GetCurSel();
		if ( nSel < 0 )		return;

		if ( m_pCurrentSkyManager )
		{
			RSceneSky *pSkyNode = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nSel);
			int nIndex = m_pCurrentSkyManager->FindSky(0, pSkyNode);

			if ( nIndex >= 0 )
			{
				RSkyInfo *pInfo = m_pCurrentSkyManager->GetSkyInfo(0, nIndex);
				_ASSERT(pInfo);

				m_nSkyColorR = GetRValue(dwColor);
				m_nSkyColorG = GetGValue(dwColor);
				m_nSkyColorB = GetBValue(dwColor);

				pInfo->dwColor = D3DCOLOR_ARGB(m_ctlSlideSkyAlpha.GetPos(), m_nSkyColorR, m_nSkyColorG, m_nSkyColorB);
			}


			UpdateData(FALSE);
			CMFCHelper::FillWnd(GetDlgItem(IDC_SKY_PICK), m_nSkyColorR, m_nSkyColorG, m_nSkyColorB);

			m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		}
	}
}

void CRollupEnvWeather::OnBnClickedButtonUp()
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	if ( pSceneManager == NULL )
		return;
	if ( m_pCurrentSkyManager == NULL )
		return;

	int nSel = m_ctlListBoxSkies.GetCurSel();
	if ( nSel < 1 )		return;

	int nMinus = nSel-1;
	int nPlus = nSel;

	RSceneSky *pSky1 = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nMinus);
	RSceneSky *pSky2 = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nPlus);

	RSkyInfo *pInfo1 = m_pCurrentSkyManager->GetSkyInfo(0, m_pCurrentSkyManager->FindSky(0, pSky1));
	RSkyInfo *pInfo2 = m_pCurrentSkyManager->GetSkyInfo(0, m_pCurrentSkyManager->FindSky(0, pSky2));
	_ASSERT(pInfo1);
	_ASSERT(pInfo2);

	std::swap(pInfo1->nOrder, pInfo2->nOrder);
	CString str1, str2;

	m_ctlListBoxSkies.GetText(nMinus, str1);
	m_ctlListBoxSkies.GetText(nPlus, str2);

	m_ctlListBoxSkies.DeleteString(nMinus);
	m_ctlListBoxSkies.DeleteString(nMinus);
	
	int nIndex;
	nIndex = m_ctlListBoxSkies.InsertString(nMinus, str1);
	m_ctlListBoxSkies.SetItemDataPtr(nIndex,(void*)pSky1);
	nIndex = m_ctlListBoxSkies.InsertString(nMinus, str2);
	m_ctlListBoxSkies.SetItemDataPtr(nIndex,(void*)pSky2);

	m_ctlListBoxSkies.SetCurSel(nMinus);
	m_pCurrentSkyManager->UpdateSkyLayer();

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
}

void CRollupEnvWeather::OnBnClickedButtonDown()
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	if ( pSceneManager == NULL )
		return;
	if ( m_pCurrentSkyManager == NULL )
		return;

	int nSel = m_ctlListBoxSkies.GetCurSel();
	if ( nSel >= m_ctlListBoxSkies.GetCount()-1 )		return;

	int nMinus = nSel;
	int nPlus = nSel+1;

	RSceneSky *pSky1 = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nMinus);
	RSceneSky *pSky2 = (RSceneSky*)m_ctlListBoxSkies.GetItemDataPtr(nPlus);

	RSkyInfo *pInfo1 = m_pCurrentSkyManager->GetSkyInfo(0, m_pCurrentSkyManager->FindSky(0, pSky1));
	RSkyInfo *pInfo2 = m_pCurrentSkyManager->GetSkyInfo(0, m_pCurrentSkyManager->FindSky(0, pSky2));
	_ASSERT(pInfo1);
	_ASSERT(pInfo2);

	std::swap(pInfo1->nOrder, pInfo2->nOrder);
	CString str1, str2;

	m_ctlListBoxSkies.GetText(nMinus, str1);
	m_ctlListBoxSkies.GetText(nPlus, str2);

	m_ctlListBoxSkies.DeleteString(nMinus);
	m_ctlListBoxSkies.DeleteString(nMinus);
	
	int nIndex;
	nIndex = m_ctlListBoxSkies.InsertString(nMinus, str1);
	m_ctlListBoxSkies.SetItemDataPtr(nIndex,(void*)pSky1);
	nIndex = m_ctlListBoxSkies.InsertString(nMinus, str2);
	m_ctlListBoxSkies.SetItemDataPtr(nIndex,(void*)pSky2);
	m_ctlListBoxSkies.SetCurSel(nPlus);

	m_pCurrentSkyManager->UpdateSkyLayer();
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
}


// 환경보기가 켜지지 않았을때 기본 환경 세팅
void CRollupEnvWeather::ApplyDefaultEnvironment()
{
	REnvironmentSet *pEnv = m_pControlMediator_->GetCurrentEnvironmentSet();
	pEnv->SetDefaultEnvSet();
}

// 환경세팅을 뷰에 적용한다
void CRollupEnvWeather::ApplyEnvironment()
{
	REnvironmentSet *pEnv = m_pControlMediator_->GetCurrentEnvironmentSet();

	// fog는 fog 보기 checkbox에 의존되어 결정된다.
	pEnv->SetApplyFogEnable( m_fogCheckBox.GetCheck() ? true : false );
	pEnv->CommitEnvSet();

	// Set Terrain
	int nTime = m_ctlTimes.GetCurSel();
	_ASSERT( nTime >= 0 );
	REnvironment::ETime eTime = (REnvironment::ETime)nTime;

	bool bNight = false;
	if (REnvironment::NIGHT == eTime)
		bNight = true;
}

void CRollupEnvWeather::UpdateCurrentEnvSet()
{
	if( m_pControlMediator_->m_mapInfo.m_bLoaded == false ) return;

	REnvironmentSet* pEnvionmentSet = m_pControlMediator_->GetCurrentEnvironmentSet();
	REnvironment *pEnv = pEnvionmentSet->GetCurrentEnvironment();
	if(!pEnv) return ;

	int nTime = m_ctlTimes.GetCurSel();
	if ( nTime < 0 )	return ;
	int nWeather = m_ctlWeathers.GetCurSel();
	if ( nWeather < 0 )	return ;

	// 현재 Time과 Weather이 있는가 ?
	REnvironment::ETime eTime = (REnvironment::ETime)nTime;
	REnvironment::EWeather eWeather = (REnvironment::EWeather)nWeather;

	bool bEnableFade = true;
	if (m_fEnvUpdateFadingTime_ <= 0.f)
	{
		bEnableFade = false;
		m_fEnvUpdateFadingTime_ = 0.f;
	}

	CRollupEtcCollision* pCollision = m_pControlMediator_->GetViewPtr<CRollupEtcCollision>();
	if( pCollision->GetCollisionTree().GetCollisionTree() == NULL )
		pCollision->GetCollisionTree().Load( pCollision->GetCollisionFileName().c_str() );

	m_pControlMediator_->GetToolSceneManager()->SetEnvModifier(eTime, eWeather, bEnableFade, m_fEnvUpdateFadingTime_, pCollision->GetCollisionTree().GetCollisionTree());
}

REnvironment::SEnvSet* CRollupEnvWeather::GetCurrentEnvSet()
{
	if( m_pControlMediator_->m_mapInfo.m_bLoaded == false ) return NULL;

	return m_pControlMediator_->GetCurrentEnvironmentSet()->GetCurrentEnvSet();
}

// 현재 환경 세트의 상태가 바뀌었을 때.
void CRollupEnvWeather::UpdateEnvironment()
{
	if ( m_pControlMediator_->m_mapInfo.m_bLoaded == false ) return;

	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	REnvironment *pEnv = m_pControlMediator_->GetCurrentEnvironmentSet()->GetCurrentEnvironment();

	if ( pEnv == NULL || pSceneManager == NULL )
	{
		ApplyDefaultEnvironment();
		return;
	}

	UpdateCurrentEnvSet();

	// 현재 Time과 Weather이 있는가 ?
	REnvironment::SEnvSet *pSet = GetCurrentEnvSet();
	if (pSet == NULL)
		return;

	// 환경 적용
	if ( m_ctlEnvViewButton.GetCheck())
		ApplyEnvironment();
	else
		ApplyDefaultEnvironment();

	UpdateData(FALSE);

	// value to ui
	m_pEnvSetRef->SetEnvSet( pSet );
	m_pPropCtrl->RemoveAll();
	if( m_ctlEnvViewButton.GetCheck() && pSet )
	{
		m_UI.BuildUIForObject( m_pPropCtrl, m_pEnvSetRef );
	}
	m_pPropCtrl->ExpandAll( true );
}

void CRollupEnvWeather::OnBnClickedCheckViewEnv()
{
	UpdateEnvironment();
}

void CRollupEnvWeather::OnCbnSelchangeComboTimes()
{
	UpdateEnvironment();
	UpdateSkyLayerList();
}

void CRollupEnvWeather::OnCbnSelchangeComboWeathers()
{
	UpdateEnvironment();
	UpdateSkyLayerList();
}

void CRollupEnvWeather::OnCbnSelchangeComboEnvset()
{
	UpdateComboEnvSet_();
	if(m_pControlMediator_->m_mapInfo.m_bLoaded && m_pControlMediator_->m_mapInfo.m_bSceneEditableMap == false)
	{
		int cur_sel = m_ctlEnvSet.GetCurSel();
		CString cstrName;
		m_ctlEnvSet.GetLBText(cur_sel, cstrName);

		CControlMediatorHelperEntityAction* pEntityActionHelper =  m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
		if (pEntityActionHelper)
		{
			pEntityActionHelper->OnChagnedEnvWeather(cstrName.GetString());
		}
	}
}

void CRollupEnvWeather::UpdateComboEnvSet_()
{
	int nSel = m_ctlEnvSet.GetCurSel();
	if ( nSel < 0 )	return;

	CString strText;
	m_ctlEnvSet.GetLBText(nSel, strText);

	std::string strName = strText.GetString();
	m_pControlMediator_->GetCurrentEnvironmentSet()->SetCurrentEnvironment(strName);

	UpdateEnvironment();
	UpdateSkyLayerList();
}

void CRollupEnvWeather::OnBnClickedButtonDelenvset()
{
	REnvironmentSet *pEnvSet = m_pControlMediator_->GetCurrentEnvironmentSet();
	if ( pEnvSet == NULL )
		return;

	int nSel = m_ctlEnvSet.GetCurSel();
	if ( nSel < 0 )	return;

	//default 는 지우지 못한다.
	if( nSel == 0 )
	{
		AfxMessageBox("default 환경값은 지우지 못합니다.");
		return;
	}

	CString strName;
	m_ctlEnvSet.GetLBText(nSel,strName);

	pEnvSet->DeleteEnvironment(strName.GetString());
	pEnvSet->SetCurrentEnvironment("default");

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);

	m_ctlEnvSet.DeleteString(nSel);
	m_ctlEnvSet.SetWindowText("");

	//set to default if exist
	SetEnvSetName("default");
}

void CRollupEnvWeather::OnBnClickedButtonAddedenvset()
{
	REnvironmentSet *pEnvSet = m_pControlMediator_->GetCurrentEnvironmentSet();
	if ( pEnvSet == NULL )
		return;

	CDlgInputCommon Dlg("default", "Env Input", NULL);
	if ( Dlg.DoModal() == IDOK )
	{
		const char *szName = Dlg.GetInputData();
		REnvironment *pEnv = pEnvSet->CreateEnvironment(szName);

		if ( pEnv )
		{
			int nIndex = m_ctlEnvSet.AddString(szName);
			m_ctlEnvSet.SetItemDataPtr(nIndex,(void*)pEnv);
			m_ctlEnvSet.SetCurSel(nIndex);
			m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			OnCbnSelchangeComboEnvset();
		}
	}
}

void CRollupEnvWeather::OnBnClickedButtonEnvImportDefault()
{
	REnvironmentSet *pEnvironmentSet = m_pControlMediator_->GetCurrentEnvironmentSet();
	if ( pEnvironmentSet == NULL ) 
		return;

	REnvironment *pEnvironment = pEnvironmentSet->GetCurrentEnvironment();

	CFileDialog dlg(TRUE,".eni");
	dlg.m_ofn.lpstrFilter = "Environment Information (*.eni)\0*.eni";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
	dlg.m_ofn.lpstrInitialDir = CXMLResourceList::GetReference().GetZoneUtilsPath();

	if (dlg.DoModal() == IDOK)
	{
		MXml kXml;
		if ( !kXml.LoadFile(dlg.GetPathName().GetString()))
		{
			AfxMessageBox("잘못된 환경 정보파일");
			return;
		}

		MXmlElement *pRoot = kXml.Doc()->FirstChildElement("ENV_CONFIG");
		if(pRoot == NULL)
		{
			AfxMessageBox("잘못된 환경 정보파일");
			return;
		}

		// 기본으로 초기화
		ApplyDefaultEnvironment();
		pEnvironment->CreateFromXML(*pRoot, false);
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}
	UpdateEnvironment();
}

void CRollupEnvWeather::OnBnClickedButtonEnvImport()
{
	REnvironmentSet *pEnvironmentSet = m_pControlMediator_->GetCurrentEnvironmentSet();
	if ( pEnvironmentSet == NULL ) 
		return;

	int nTime = m_ctlTimes.GetCurSel();
	if ( nTime < 0 )	return;

	int nWeather = m_ctlWeathers.GetCurSel();
	if ( nWeather < 0 )	return;

	CFileDialog dlg(TRUE,".eni");
	dlg.m_ofn.lpstrFilter = "Environment Information (*.eni)\0*.eni";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
	dlg.m_ofn.lpstrInitialDir = CXMLResourceList::GetReference().GetZoneUtilsPath();

	if (dlg.DoModal() == IDOK)
	{
		MXml kXml;
		if ( !kXml.LoadFile(dlg.GetPathName().GetString()))
		{
			AfxMessageBox("잘못된 환경 정보파일");
			return;
		}

		MXmlElement *pRoot = kXml.Doc()->FirstChildElement("ENV_CONFIG");
		if(pRoot == NULL)
		{
			AfxMessageBox("잘못된 환경 정보파일");
			return;
		}

		int nTimeToImport(nTime), nWeatherToImport(nWeather);
		CDlgInputEnvSetImport dlg_import(nTimeToImport, nWeatherToImport);
		if (dlg_import.DoModal() == IDOK)
		{
			nTimeToImport = dlg_import.m_nTime;
			nWeatherToImport = dlg_import.m_nWeather;
		}
		else
			return;

		for(MXmlElement *pChildElement = pRoot->FirstChildElement("ENVSET");
			pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
		{
			int nLoadedTime, nLoadedWeather;

			nLoadedTime = REnvironment::StringToETime( pChildElement->Attribute("time"));
			if ( nLoadedTime != nTimeToImport )
				continue;

			nLoadedWeather = REnvironment::StringToEWeather( pChildElement->Attribute("weather"));
			if ( nLoadedWeather != nWeatherToImport )
				continue;

			// 기본으로 초기화
			ApplyDefaultEnvironment();
			REnvironment *pEnv = m_pControlMediator_->GetCurrentEnvironmentSet()->GetCurrentEnvironment();
			REnvironment::SEnvSet* pEnvSet = pEnv->CreateEnv((REnvironment::ETime)nTime,(REnvironment::EWeather)nWeather);
			pEnv->ReadEnvSetContents(pEnvSet, pChildElement);
			break;
		}
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}

	UpdateEnvironment();
}

void CRollupEnvWeather::OnBnClickedButtonEnvExport()
{
	REnvironmentSet *pEnvironmentSet = m_pControlMediator_->GetCurrentEnvironmentSet();
	if ( pEnvironmentSet == NULL ) 
		return;

	CFileDialog dlg(FALSE,".eni", "default.eni");
	dlg.m_ofn.lpstrFilter = "Environment Information (*.eni)\0*.eni";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
	dlg.m_ofn.lpstrInitialDir = CXMLResourceList::GetReference().GetZoneUtilsPath();

	if (dlg.DoModal() == IDOK)
	{
		CString fileName(dlg.GetPathName());
		MXml kXml;
		MXmlDeclaration* pkDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
		kXml.Doc()->LinkEndChild(pkDecl);

		MXmlElement *pChildElement = new MXmlElement("ENV_CONFIG");
		kXml.Doc()->LinkEndChild(pChildElement);

		REnvironment *pEnv = m_pControlMediator_->GetCurrentEnvironmentSet()->GetCurrentEnvironment();
		pEnv->SaveToXML(*pChildElement);
		kXml.SaveFile(fileName.GetString());
	}
}

void CRollupEnvWeather::GetWeatherInfo(int& _viewBOOL, int& _time, int& _weather, std::string& _strEnvSet)
{
	_viewBOOL = m_ctlEnvViewButton.GetCheck();
	_time = m_ctlTimes.GetCurSel();
	_weather = m_ctlWeathers.GetCurSel();

	int env_set = m_ctlEnvSet.GetCurSel();
	if( env_set < 0 )
	{
		_strEnvSet = "default";
		return;
	}
	CString cstr_name;
	m_ctlEnvSet.GetLBText(env_set, cstr_name);
	if(cstr_name.IsEmpty())
		_strEnvSet = "default";
	else
		_strEnvSet = cstr_name.GetString();
}

void CRollupEnvWeather::SetWeatherInfo(int _viewBOOL, int _time, int _weather, const std::string& _strEnvSet, float fFadingTime)
{
	m_ctlEnvViewButton.SetCheck(_viewBOOL);
	m_ctlTimes.SetCurSel(_time);
	m_ctlWeathers.SetCurSel(_weather);

	SetEnvSetName(_strEnvSet);

	//최종 업데이트
	float fOrgFadingTime = m_fEnvUpdateFadingTime_;
	m_fEnvUpdateFadingTime_ = fFadingTime;
	UpdateEnvironment();
	m_fEnvUpdateFadingTime_ = fOrgFadingTime;
}

void CRollupEnvWeather::GetEnvSetName(std::vector<std::string>& _refStringVec)
{
	int env_set_size = m_ctlEnvSet.GetCount();
	CString cstrEnvSetName;
	for(int i = 0; i < env_set_size; ++i)
	{
		m_ctlEnvSet.GetLBText(i, cstrEnvSetName);
		_refStringVec.push_back(cstrEnvSetName.GetString());
	}
}

void CRollupEnvWeather::SetEnvSetName(const std::string& _strEnvSetName)
{
	int nSelected = m_ctlEnvSet.SelectString(0, _strEnvSetName.c_str());
	if(nSelected < 0 ) return;

	m_ctlEnvSet.SetCurSel(nSelected);
	UpdateComboEnvSet_();
}

void CRollupEnvWeather::SetUIActivity_(bool _bActive)
{
	BOOL bActive = _bActive ? TRUE : FALSE;

	//Valid Editor
	GetDlgItem(IDC_LIST_ENV)->EnableWindow(bActive);
	GetDlgItem(IDC_SLIDER1)->EnableWindow(bActive);

	GetDlgItem(IDC_EDIT_CR)->EnableWindow(bActive);
	GetDlgItem(IDC_EDIT_CG)->EnableWindow(bActive);
	GetDlgItem(IDC_EDIT_CB)->EnableWindow(bActive);

	GetDlgItem(IDC_ADD_SKY)->EnableWindow(bActive);
	GetDlgItem(IDC_DEL_SKY)->EnableWindow(bActive);

	GetDlgItem(IDC_SKY_PICK)->EnableWindow(bActive);

	GetDlgItem(IDC_BUTTON_UP)->EnableWindow(bActive);
	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(bActive);

	GetDlgItem(IDC_BUTTON_DELENVSET)->EnableWindow(bActive);
	GetDlgItem(IDC_BUTTON_ADDEDENVSET)->EnableWindow(bActive);

	GetDlgItem(IDC_BUTTON_ENV_IMPORT_DEFAULT)->EnableWindow(bActive);
	GetDlgItem(IDC_BUTTON_ENV_IMPORT)->EnableWindow(bActive);
	GetDlgItem(IDC_BUTTON_ENV_EXPORT)->EnableWindow(bActive);
}

void CRollupEnvWeather::ResetCurrentEnvironmetObjectPtr_(RToolSceneManager* pSceneManager, REnvironment::SEnvSet *pSet)
{
	if(pSceneManager == NULL)
	{
		m_pCurrentSkyManager = NULL;
	}
	else
	{
		// sky manager 초기화
		if ( m_pCurrentSkyManager )
			m_pCurrentSkyManager->RemoveFromParent();

		if(pSet)
		{
			m_pCurrentSkyManager = pSet->pSkyManager;
			if ( m_pCurrentSkyManager )
				pSceneManager->AddSceneNode(m_pCurrentSkyManager);
		}
	}
}

LRESULT CRollupEnvWeather::OnPropertyViewNotify(WPARAM wParam, LPARAM lParam)
{
	if (m_ctlEnvViewButton.GetCheck())
		ApplyEnvironment();
	return 0;
}

void CRollupEnvWeather::ToggleFog()
{
	//Toggle
	UINT bFogView = m_fogCheckBox.GetCheck()? FALSE : TRUE;

	m_fogCheckBox.SetCheck(bFogView);
	OnBnClickedCheckViewFog();
}

void CRollupEnvWeather::OnDestroy()
{
	if(m_pPropCtrl)
	{
		m_pPropCtrl->DestroyWindow();
		SAFE_DELETE(m_pPropCtrl);
	}
	SAFE_DELETE( m_pEnvSetRef );

	__super::OnDestroy();
}