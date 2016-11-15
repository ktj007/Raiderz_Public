//////////////////////////////////////////////////////////////////////////
// REnvironmentSet
// 날씨및 구름 그림자등을 변경/업데이트 해주는 신노드

#pragma once

#include "RSceneNode.h"
#include "REnvironment.h"
#include "MSingleton.h"

namespace rs3 {

class REventBroadcaster;
class RWeatherEffectSet;
class RCollisionTree;

class RS_API REnvironmentSet : public RSceneNode	//, public MSingleton<REnvironmentSet>
{
	// fading 정보
	struct EnvironmentFadingInfo
	{
		REnvironment::SEnvSet *pEnvSet;			///< 환경 세팅
		RWeatherEffect *pWeather;				///< 날씨 이펙트

		REnvironment::ETime eTime;
		REnvironment::EWeather eWeather;

		EnvironmentFadingInfo() : pEnvSet(NULL), pWeather(NULL)
			, eTime(REnvironment::TIME_NUM), eWeather(REnvironment::WEATHER_NUM) {}

		bool Create(RSceneNode* pSceneNode);
		void Destroy();
	};

protected://씬노드로부터 재정의
	virtual void				OnAddScene()	override;

public:
	RTexture* hTillingTexture;
	struct STillingInfo
	{
		float fVisibility;
		RMatrix matTransform;
		RVector vecScale;
		RVector vecTranslate;
	};

	std::vector<STillingInfo> m_vecTilling;

	REnvironmentSet();
	REnvironmentSet(RWeatherEffectSet* pWeatherEffectSet);
	virtual ~REnvironmentSet();

	REnvironment::SEnvSet* REnvironmentSet::GetCurrentEnvSet();

	REnvironment* GetCurrentEnvironment();						// 현재 환경 구조체의 포인터를 가져온다.
	REnvironment* CreateEnvironment(const std::string &strName);// 해당 환경을 생성한다.

	void SetCurrentEnvironment(const std::string &strName);		// 현재 환경 (기본값은 default) 이름을 지정한다.
	void DeleteEnvironment(const std::string &strName);			// 해당 환경을 삭제한다.

	void GetNames(std::vector<std::string> &vecArray);			// 환경 이름 목록을 얻어온다.

	// 데이터 읽기
	bool CreateFromFile(const char *szFilename, bool bBackgroundCreation);
	bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	bool LoadWeatherFromXML();

	// 데이터 저장
	void SaveToFile(const char *szFilename);
	void SaveToXML(MXmlElement &element);

	// ETime, EWeather과 별개로 기본 값을 설정한다. 항상 Set 후에 CommitEnvSet을 해 주어야 씬에 환경이 적용된다.
	void SetDefaultEnvSet();

	// 해당 ETime, EWeather에 대한 EnvSet으로 설정한다.
	bool SetCurrentEnvSet(REnvironment::ETime eTime, REnvironment::EWeather eWeather, bool bFade = false, float fFadingTime = 0.0f, bool bBackgroundLoading = true, RCollisionTree* pCollisionTree = NULL);

	// 날씨를 그릴지 여부 결정
	void SetRenderWeather(bool bRenderWeatherEffect);
	void SetApplyFogEnable(bool bApplyFogEnable ) { m_bApplyFog = bApplyFogEnable; }

	RVector3 GetHDRBrightPassThreshold() { return m_vHDRBrightPassThreshold; }
	float GetHDRBloomBlurDeviation() { return m_fHDRBloomBlurDeviation; }
	float GetHDRBloomBlurMultiplier() { return m_fHDRBloomBlurMultiplier; }

	float GetSubLightIntensity() { return m_fSubLightIntensity; }


	void Update( RCollisionTree* pCollisionTree );

	void CommitEnvSet();

	bool IsBlending();		///< 블렌딩 중인가
	void EndBlending();		///< 블렌딩 끝냄
	float GetBlendingFactor();				///< 블렌딩 팩터 1->0 으로 진행

	// 기존 클라이언트 코드와 호환 작동 되게...
	static REnvironmentSet*	m_pCurrentSetOwn;	// 환경 셋이 생성 되지 않은 상태에서 GetInstancePtr가 호출 되면 새로 임시로 생성해서 돌려준다. 
	static REnvironmentSet*	m_pCurrentSet;
	void					SetCurrent();
	static REnvironmentSet* GetInstancePtr();
	static void				DeleteOwnSet();


	REnvironment::SEnvSet	GetEnvSet()	{ return m_DefaultSet; }
	

protected:
	// 날씨는 WEATHER_NUM 만큼만 가진다. 날씨 관련 이펙트 효과
	RWeatherEffectSet*		m_pWeatherEffectSet;
	bool					m_bExternalWeatherEffectSet;

	// 지정한 환경을 씬에 적용시킨다. 
	// ( bChangeEnv는 이전 eTime, eWeather, Environment Name 이 바뀌었을 때  true를 넣는다. SEnvSet 개체의 값만 바뀌었을 때는 false를 넣는다.)

	void UpdateLoading();
	void UpdateBlending();
	void UpdateWeather( float fTime, RCollisionTree* pCollisionTree );
	void UpdateCloud( float fTime );

	void ClearAll();		// current blending queue 를 모두 클리어
	void Release();			// 리소스까지 정리

	void Init();

private:
	void CommitWeatherEnv(float fBlendFactor);
	void CommitWindEnv(float fBlendFactor);
	void CommitFogEnv(float fBlendFactor);
	void CommitSkyEnv(float fBlendFactor);
	void CommitLightEnv(float fBlendFactor);
	void CommitHDREnv(float fBlendFactor);
	void CommitCloudShadow(float fBlendFactor);


	// 메시위에 스크롤 되는 텍스쳐 타일링 이펙트 이름
	static char *TILINGEFFECT_TEXTURE_FILENAME;

	typedef std::map<std::string, REnvironment*> ENVIRONMENTSET;
	ENVIRONMENTSET m_EnvironmentSetTable;

	// 현재 날씨 셋의 포인터
	REnvironment *m_pCurrentEnvironment;

	// 기본 값
	REnvironment::SEnvSet m_DefaultSet;

	// 현재 시간/날씨의 열거형 값
	REnvironment::ETime m_eCurrentTime;
	REnvironment::EWeather m_eCurrentWeather;

	EnvironmentFadingInfo m_currentSet;				// 현재 적용되는 것.
	EnvironmentFadingInfo m_blendingSet;			// 바로 이전의 환경 상태이며, 현재 블렌딩 중인 것.
	bool				m_bBlendingEnabled;			///< 블렌딩이 활성화 되어있는지 여부
	float				m_fFadingTime;				///< 총 블렌딩 시간
	float				m_fLifeTime;				///< 남은 블렌딩 시간

	bool		m_bRenderWeatherEffect;	// 날씨를 화면에 그릴지 여부
	bool		m_bApplyFog;		///< 포그를 화면에 적용할지 여부

	RVector3	m_vHDRBrightPassThreshold;	///< HDR 블룸 브라이트 패스 값
	float		m_fHDRBloomBlurDeviation;	///< HDR 블룸 블러 분산율
	float		m_fHDRBloomBlurMultiplier;	///< HDR 블룸 블러 가중치

	float		m_fSubLightIntensity;
};

}
