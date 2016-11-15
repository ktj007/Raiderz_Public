#include "stdafx.h"
#include "XWorldEnv.h"
#include "RSceneManager.h"
#include "XFieldInfo.h"
#include "RSkyManager.h"
#include "RGrassSystem.h"
#include "XWorld.h"
#include "XCameraManager.h"
#include "XSceneNodeEnvModifier.h"
#include "XGlobal.h"
#include "XEngine.h"
#include "XCollisionTree.h"
#include "XGameState.h"

#include "XCutSceneFieldIntroMgr.h"

// 기본 환경셋 이름은 'default' 이다.
const wchar_t* WORLD_ENV_SET_DEFAULT = L"default";

//////////////////////////////////////////////////////////////////////////////////////////
XWorldEnv::XWorldEnv()
: m_pEnvset(NULL), m_pWorld(NULL), m_bCreated(false), m_pEnvironmentChanger(NULL), m_pWorldSceneNode(NULL)
{
}

XWorldEnv::~XWorldEnv()
{
	Destroy();
}

bool XWorldEnv::Create(XWorld* pWorld)
{
	if (m_bCreated)
	{
		_ASSERT(0);	// 이미 Create했다.
		return true;
	}

	m_pWorld = pWorld;
	
	CreateEnvironmentSet();

	m_pWorldSceneNode = m_pWorld->GetWorldSceneNode();

	global.smgr->AddSceneNode(m_pEnvset);
	
	if (m_pWorld->GetInfo())
		m_pEnvset->SetVisible(m_pWorld->GetInfo()->m_bWeatherEffect);

	m_CurrentInfo.nTime = GetDefaultTime();
	m_CurrentInfo.nWeather = GetDefaultWeather();

	rs3::REnvironmentSet* pEnvironmentSet = rs3::REnvironmentSet::GetInstancePtr();
	m_pEnvironmentChanger = new XSceneNodeEnvModifier( pEnvironmentSet, m_pWorldSceneNode );
	m_pEnvironmentChanger->SetQueueSize(1);
	m_bCreated = true;

	return true;
}

void XWorldEnv::Destroy()
{
	if(m_pEnvironmentChanger)
	{
		if (global.smgr)
		{
			m_pEnvironmentChanger->DetachNightNodeController( m_pWorldSceneNode );
		}

		m_pEnvironmentChanger->Clear();
		SAFE_DELETE(m_pEnvironmentChanger);
	}

	if(m_pEnvset) 
	{
		bool bRet = m_pEnvset->RemoveFromParent();
		_ASSERT( bRet );

		delete m_pEnvset;
		m_pEnvset = NULL;
	}

	m_bCreated = false;

	m_pWorldSceneNode = NULL;
}

bool XWorldEnv::LoadZoneEnv(const wchar_t* szFileName)
{
	if (m_pEnvset == NULL) return false;

	XTimeChecker tc;
	tc.Start(XGetNowTime());

	bool bEnvResult = m_pEnvset->CreateFromFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str(), true);
	if ( !bEnvResult )
		return false;

	dlog("  XWorldEnv::LoadZoneEnv() - m_pEnvset->CreateFromFile() (time=%u ms)\n", tc.Check(XGetNowTime()));

	tc.Start(XGetNowTime());

	GAME_TIME_TYPE time = GetDefaultTime();
	GAME_WEATHER_TYPE weather = GetDefaultWeather();

	//인트로 컷씬을 플레이하는 경우라면 인트로 컷씬 스크립트의 내용으로 시간과 날씨값을 채운다.  
	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().SetIntroCutSceneEnv((int*)&time, (int*)&weather);

	gvar.World.nGameTime = time;
	gvar.World.nWeather = weather;

	SetEnvSet(L"default");
	SetEnv(time, weather, false);

	dlog("  XWorldEnv::LoadZoneEnv() - SetEnv() (time=%u ms)\n", tc.Check(XGetNowTime()));

	return true;
}

void XWorldEnv::OnWorldLoadingComplete( rs3::RWorldSceneNode* pLoadingRootSceneNode )
{
	// 밤/낮 바뀌는 노드 수집
	m_pEnvironmentChanger->AttachNightNodeController( pLoadingRootSceneNode );
}

void XWorldEnv::SetEnvSet(const wchar_t* szEnvname)
{
	m_pEnvironmentChanger->SetEnvSet(szEnvname);
}

bool XWorldEnv::SetEnv(GAME_TIME_TYPE nTime, GAME_WEATHER_TYPE nWeather, bool bFade, float fFadingTime)
{
//	bool bResult = m_pEnvset->SetCurrentEnvSet(ConvertTime(nTime), ConvertWeather(nWeather), bFade, fFadingTime);
// 	REnvironment::SEnvSet *pEnv = m_pEnvset->GetCurrentEnvSet();
// 	if ( pEnv )
// 	{
// 		bool bFogEnable = !pEnv->bFogDisable;
// 
// 		rs3::RCameraSceneNode* pCamera = global.smgr->GetPrimaryCamera();
// 		pCamera->SetFogEnable(bFogEnable);
// 		pCamera->ApplyFog();
// 	}

	rs3::RCollisionTree* pCollisionTree = NULL;
	if( gg.currentgamestate )
	{
		if( gg.currentgamestate->GetWorld() )
			pCollisionTree = static_cast<rs3::RCollisionTree*>(gg.currentgamestate->GetWorld()->GetCollisionTree());
	}

	bool bResult = m_pEnvironmentChanger->SetCurrentEnvSet( ConvertTime(nTime), ConvertWeather(nWeather), bFade, fFadingTime, pCollisionTree );

	m_CurrentInfo.nTime = nTime;
	m_CurrentInfo.nWeather = nWeather;

	return bResult;
}

bool XWorldEnv::SetTime(GAME_TIME_TYPE nTime, bool bFade, float fFadingTime)
{
	return SetEnv(nTime, m_CurrentInfo.nWeather, bFade, fFadingTime);
}

bool XWorldEnv::SetWeather(GAME_WEATHER_TYPE nWeather, bool bFade, float fFadingTime)
{
	return SetEnv(m_CurrentInfo.nTime, nWeather, bFade, fFadingTime);
}

void XWorldEnv::Update(float fDelta, RCollisionTree* pCollisionTree)
{
	if(m_pEnvironmentChanger)
		m_pEnvironmentChanger->Update( pCollisionTree );
}

GAME_TIME_TYPE XWorldEnv::ConvertTime(REnvironment::ETime nTime)
{
	switch (nTime)
	{
	case REnvironment::DAWN:	return TIME_DAWN;
	case REnvironment::DAYTIME: return TIME_DAYTIME;
	case REnvironment::SUNSET:	return TIME_SUNSET;
	case REnvironment::NIGHT:	return TIME_NIGHT;
	default:
		_ASSERT(0);		// 없는건 추가해 주어야 합니다.
	}
	return GetDefaultTime();
}

REnvironment::ETime XWorldEnv::ConvertTime(GAME_TIME_TYPE nTime)
{
	switch (nTime)
	{
	case TIME_DAWN:		return REnvironment::DAWN;
	case TIME_DAYTIME:	return REnvironment::DAYTIME;
	case TIME_SUNSET:	return REnvironment::SUNSET;
	case TIME_NIGHT:	return REnvironment::NIGHT;
	default:
		_ASSERT(0);		// 없는건 추가해 주어야 합니다.
	}
	return REnvironment::DAYTIME;
}

GAME_WEATHER_TYPE XWorldEnv::ConvertWeather(REnvironment::EWeather nWeather)
{
	switch (nWeather)
	{
	case REnvironment::SUNNY:		return WEATHER_SUNNY;
	case REnvironment::CLOUD:		return WEATHER_CLOUDY;
	case REnvironment::RAINY:		return WEATHER_RAINY;
	case REnvironment::HEAVY_RANIY: return WEATHER_HEAVY_RAINY;
	case REnvironment::SNOW:		return WEATHER_SNOWY;
	case REnvironment::HEAVY_SNOW:	return WEATHER_HEAVY_SNOWY;
	default:
		_ASSERT(0);		// 없는건 추가해 주어야 합니다.
	}
	return GetDefaultWeather();
}

REnvironment::EWeather XWorldEnv::ConvertWeather(GAME_WEATHER_TYPE nWeather)
{
	switch (nWeather)
	{
	case WEATHER_SUNNY:			return REnvironment::SUNNY;
	case WEATHER_CLOUDY:		return REnvironment::CLOUD;
	case WEATHER_RAINY:			return REnvironment::RAINY;
	case WEATHER_HEAVY_RAINY:	return REnvironment::HEAVY_RANIY;
	case WEATHER_SNOWY:			return REnvironment::SNOW;
	case WEATHER_HEAVY_SNOWY:	return REnvironment::HEAVY_SNOW;
	default:
		_ASSERT(0);		// 없는건 추가해 주어야 합니다.
	}
	return REnvironment::SUNNY;
}


bool XWorldEnv::IsCurrentNight()
{
	if (m_CurrentInfo.nTime == TIME_NIGHT) return true;
	return false;
}

void XWorldEnv::CreateEnvironmentSet()
{
	if (m_pEnvset) 
	{
		delete m_pEnvset;
	}

	RWeatherEffectSet* pWeatherEffectSet = global.engine->GetWeatherEffectSet();

	m_pEnvset = new REnvironmentSet(pWeatherEffectSet);
}