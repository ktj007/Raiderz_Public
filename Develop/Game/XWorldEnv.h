#ifndef _XWORLD_ENV_H
#define _XWORLD_ENV_H

#include "XDef.h"
#include "CSWorldInfo.h"
#include "REnvironmentSet.h"
#include "RSkyManager.h"
#include "REffectSceneNode.h"

class XFieldInfo;
class XWorld;
class XSceneNodeEnvModifier;

// 환경 정보
struct XWorldEnvInfo
{
	GAME_TIME_TYPE		nTime;			///< 시간대
	GAME_WEATHER_TYPE	nWeather;		///< 날씨
};

// 월드 환경
class XWorldEnv
{
private:
	bool				m_bCreated;
	XWorldEnvInfo		m_CurrentInfo;		///< 기본 환경 정보
	REnvironmentSet*	m_pEnvset;			///< 엔진의 Envset
	XWorld*				m_pWorld;
	XSceneNodeEnvModifier*	m_pEnvironmentChanger;	// 환경변화에 따른 변경자
	RWorldSceneNode*	m_pWorldSceneNode;

	void CreateEnvironmentSet();

	static GAME_TIME_TYPE ConvertTime(REnvironment::ETime nTime);
	static REnvironment::ETime ConvertTime(GAME_TIME_TYPE nTime);
	static GAME_WEATHER_TYPE ConvertWeather(REnvironment::EWeather nWeather);
	static REnvironment::EWeather ConvertWeather(GAME_WEATHER_TYPE nWeather);
public:
	XWorldEnv();
	~XWorldEnv();
	bool Create(XWorld* pWorld);		// 테이블 값 설정 등 초기화


	void Destroy();
	bool LoadZoneEnv(const wchar_t* szFileName);
	void Update(float fDelta, RCollisionTree* pCollisionTree);
	void SetEnvSet(const wchar_t* szEnvname);
	bool SetEnv(GAME_TIME_TYPE nTime, GAME_WEATHER_TYPE nWeather, bool bFade = false, float fFadingTime = 0.0f);
	bool SetTime(GAME_TIME_TYPE nTime, bool bFade = false, float fFadingTime = 0.0f);
	bool SetWeather(GAME_WEATHER_TYPE nWeather, bool bFade = false, float fFadingTime = 0.0f);

	void OnWorldLoadingComplete( rs3::RWorldSceneNode* pLoadingRootSceneNode );

	REnvironmentSet*	GetEnvironmentSet()					{ return m_pEnvset;	}
	
	static const GAME_TIME_TYPE		GetDefaultTime()		{ return TIME_DAYTIME; }
	static const GAME_WEATHER_TYPE	GetDefaultWeather()		{ return WEATHER_SUNNY; }

	GAME_TIME_TYPE					GetCurrentTime()		{ return m_CurrentInfo.nTime; }
	GAME_WEATHER_TYPE				GetCurrentWeather()		{ return m_CurrentInfo.nWeather; }
	bool							IsCurrentNight();
};

#endif // _XWORLD_ENV_H