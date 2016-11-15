#pragma once

#include <queue>
#include "RSceneNode.h"
#include "RActor.h"

namespace rs3 {

class RWeatherEffect;
class REffectSceneNode;

//////////////////////////////////////////////////////////////////////////
// REnvironmentSceneNode
// 날씨및 구름 그림자등을 변경/업데이트 해주는 신노드
//
class REnvironmentSceneNode : public RSceneNode
{
public:

	//////////////////////////////////////////////////////////////////////////
	// Environment Piece를 구분하는 Key : Time 과 Weather
	//
	enum ETime
	{
		DAWN = 0,
		DAYTIME,
		SUNSET,
		EVENING,
		NIGHT,
		TIME_NUM
	};

	enum EWheather
	{
		SUNNY = 0,
		CLOUD,
		RAINY,
		HEAVY_RANIY,
		SNOW,
		HEAVY_SNOW,
		CLEAR,
		WHEATHER_NUM
	};

	// enum - string 변환
	static const char*	ETimeToString(ETime eTime);
	static const char*	EWheatherToString(EWheather eWheather);

	static ETime		StringToETime(const char *szName);
	static EWheather	StringToEWheather(const char *szName);


	//////////////////////////////////////////////////////////////////////////
	// REnvironmentPiece
	// REnvironmentPiece의 5개 Time(DAWN, DAYTIME, ...) 과 7개 Weather(SUNNY, ...) 의
	// 35개 조합을 하나의 묶음(EnvBunch)로 하여 REnvironmentSceneNode에서 관리합니다.
	//
	class REnvironmentPiece
	{
	public:

		// Light
		RVector4		vAmbient;
		RVector4		vDiffuse;
		RVector4		vCharacterAmbient;
		RVector4		vCharacterDiffuse;
		float			fPower;

		// fog
		RVector4		vFogColor;
		bool			bFogDisable;

		// near-far
		float			fNear;
		float			fFar;

		// Wind
		RVector			vWindDirection;
		float			fWindStrength;

		// cloud shadow
		bool			m_bCloudVisible;
		RVector3		m_vCloudShadowTranslate;
		RVector3		m_vCloudShadowScale;


		//////////////////////////////////////////////////////////////////////////
		// Clear EnvironmentPiece
		//
		bool			ReloadEnvPiece(MXmlElement* pChildElement);
		void			Clear(){ ResetEnvPiece(true, true); }


		//////////////////////////////////////////////////////////////////////////
		// sky add / delete
		//
		int				AddSky(const char* _pSzFileName);
		int				AddSky(MXmlElement& xmlElementForSky);
		bool			DeleteSky(int _nIndex);

		// sky layer ordering
		int				GetAllSkiesCount(){ return m_vecSkyArray.size(); }
		void			ChangeSkyOrder( int _nLayerToChange, bool bIncrease );

		// Sky Layer 전체에 영향을 미치는 color
		void			SetSkyMultiplyColorFactor(RVector4 &vec4){ m_vSkyMultiplyColor = vec4; }
		const RVector4&	GetSkyMutiplyColorFactor(){ return m_vSkyMultiplyColor; }

		// 개별 sky에 영향을 미치는 color
		void			SetSkyColor( int _nIndex, DWORD dwColor );
		void			SetSkySpeed( int _nIndex, float f );
		void			SetFogFactor( int _nIndex, float f );

		//////////////////////////////////////////////////////////////////////////
		// Sky Info
		//
		struct SSKY_INFO
		{
			int			nIndex;
			std::string	strName;

			DWORD		dwColor;
			float		fSpeed;
			float		fFogFactor;
		};
		void			GetSkyInfo(int _nIndex, SSKY_INFO& _refSkyInfo);
		void			GetAllSkiesInfo( std::vector< SSKY_INFO >& _refVecSkyInfo );

	private:
		friend class REnvironmentSceneNode;
		friend class REnvironmentBunch;

		// REnvironmentPiece 의 기본 값. REnvironmentBunch/REnvironmentSceneNode를 통해서만 생성가능
		REnvironmentPiece() : m_pEnvSceneNode(NULL) { ResetEnvPiece(true, false); }
		~REnvironmentPiece(){ ResetEnvPiece(false, true); }

		// sky color (all skies in layer)
		RVector4			m_vSkyMultiplyColor;

		// sky XML 관련
		static const char*	ELEMENT_FILENAME;	
		static const char*	ELEMENT_SPEED;
		static const char*	ELEMENT_FOGFACTOR;
		static const char*	ELEMENT_COLOR;

		// Sky Actor
		class RSkySceneNode : public RActor
		{
			// REnvironmentPiece만이 제어권리를 가진다.
			friend REnvironmentPiece;

			RSkySceneNode() : RActor(RRP_SKYBOX)
				, m_nOrder(-1)
				, m_dwColor(0xffffffff)
				, m_fUVAniSpeed(1.f)
			{
				m_fFogFactor = 1.0f;
				m_matLocal.MakeIdentity();
				m_bUsingActorPassRenderer = false;
			}
			virtual ~RSkySceneNode(){}

			// RSkySceneNode는 자체적인 렌더링과 업데이트를 가진다.
			virtual void	OnUpdate(){ _ASSERT(!"RSceneSky는 자체적인 업데이트 시스템을 가집니다."); }
			virtual void	OnRender(){ _ASSERT(!"RSceneSky는 자체적인 렌더링 시스템을 가집니다."); }
			virtual void	OnRegisterToRender(){ _ASSERT(!"RSceneSky는 자체적인 렌더링 등록시스템을 가집니다."); }

			DWORD			m_dwColor;		// sky multiply color
			float			m_fFogFactor;	// get/set fog factor
			float			m_fUVAniSpeed;	// UV Animation Speed
			int				m_nOrder;		// layer order sorting 할 때 사용된다.
			static bool		eval_order(RSkySceneNode *a, RSkySceneNode *b);

			// 생성 / 업데이트 / 렌더링
			virtual bool	Create(const char *filename, bool bBackgroundCreation = false);
			void			UpdateSky(){ RActor::OnUpdate(); }
			void			RenderSky(const RVector4 &vMultiplyColor);

			// get sky mesh filename
			const char*		GetFilename();
		};
		std::vector<RSkySceneNode*> m_vecSkyArray;

		// Private Method
		void RenderEnvPiece();
		void UpdateEnvPiece();
		void ReOrderSky();

		void OnAddToCurrnetEnvironment(REnvironmentSceneNode* pEnvironmentSceneNode);
		void OnRemoveFromCurrentEnvironment();

		void ResetEnvPiece(bool bResetInitValue, bool bDeepDeleteSky);
		REnvironmentSceneNode* m_pEnvSceneNode;
	};


	//////////////////////////////////////////////////////////////////////////
	// XML로터 읽기/XML로 출력하기, 업데이트/렌더
	//
	virtual bool		CreateFromXMLFile(const char *szFilename, bool bBackgroundCreation);
	virtual void		SaveToXMLFile(const char *szFilename);


	//////////////////////////////////////////////////////////////////////////
	// EnvironmentBunch, 35개의 EnvPiece 조각들을 하나의 이름으로 묶어(Bunch) 관리한다.
	//
	bool				AddEnvBunch(const char* _pSzEnvBunchName);					// 해당 환경을 추가. 추가후 CurrentEnvBunch를 바꿀수 있다.
	bool				DeleteEnvBunch(const char* _pSzEnvBunchName);				// 해당 환경 삭제. 삭제후 Default EnvBench가 Currnet로 설정

	void				GetAllEnvBunchesList(std::vector<std::string> &vecArray);	// 환경 이름 목록을 얻어온다.
	bool				ClearAllEnvBunches();										// 전체 환경묶음들을 초기화


	//////////////////////////////////////////////////////////////////////////
	// EnvironmentPiece. 시간/날씨별 환경조각(EnvPiece)들은 Queue로 관리하며
	// 현재의 EnvPiece가 변경될때 자연스럽게 환경이 변경되는것등이 지원됩니다.
	// (이렇게 현재의 EnvPiece가 다음 EnvPiece로 변경되는 과정을 EnvProcess라고 합니다.)
	//
	REnvironmentPiece*	GetEnvPiece(const char* pSzEnvBunchName, ETime eTime, EWheather eWeather);
	REnvironmentPiece*	GetCurrentEnvPiece(){ return m_pCurrentEnvPiece; }
	bool				SetCurrentEnvPiece(const char* pSzEnvBunchName, ETime eTime, EWheather eWeather, float fFadingTime = -1.0f);


	//////////////////////////////////////////////////////////////////////////
	// cloud shadow
	//
	RTexture* hTillingTexture;
	struct STillingInfo
	{
		float fVisibility;
		RMatrix matTransform;
		RVector vecScale;
		RVector vecTranslate;
	};
	std::vector<STillingInfo> m_vecTilling;


	//////////////////////////////////////////////////////////////////////////
	// weather effect
	//
	bool				LoadWeatherFromXML();
	void				SetRenderWeather(bool bRenderWeatherEffect);


protected:
	// RSceneNode로써 생성 / 업데이트 / 렌더링
	virtual bool		CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void		SaveToXML(MXmlElement &element);

	virtual void		OnUpdate();
	virtual void		OnRender();

private:
	friend class REnvironmentPiece;
	friend class RSingletonSceneManager;

	// 씬 매니저에서만 생성
	REnvironmentSceneNode();
	virtual ~REnvironmentSceneNode();

	// 씬 매니저에 의해, 씬매니저가 만들어지고 소멸될때 한번씩만 불린다.
	void	OnAddToSceneManager(RSingletonSceneManager* pSingletionSceneManager);
	void	OnRemoveFromSceneManager();

	// 메시위에 스크롤 되는 텍스쳐 타일링 이펙트 이름
	static char *TILINGEFFECT_TEXTURE_FILENAME;

	typedef std::pair<int, int>	ENV_KEY;
	typedef std::map<ENV_KEY, REnvironmentPiece*> ENV_BUNCH;
	typedef std::map< std::string, map<ENV_KEY, REnvironmentPiece*> > ENV_TABLE;
	ENV_TABLE m_kEnvTable;

	// 현재 환경
	std::string			m_strCurrentEnvBunch;
	ETime				m_eCurrentTime;
	EWheather			m_eCurrentWeather;
	REnvironmentPiece*	m_pCurrentEnvPiece;

	// fading 정보
	struct SPTR
	{
		REnvironmentPiece*	pEnvPiece;
		RWeatherEffect*		pWeather;
		float				fFadingTime;
		float				fLifeTime;

		ETime				eTime;
		EWheather			eWeather;

		SPTR() : pEnvPiece(NULL), pWeather(NULL), fFadingTime(-1.0f), fLifeTime(0.0f)
			, eTime(TIME_NUM), eWeather(WHEATHER_NUM) {}
	};

	std::queue<SPTR>	m_queueEnvNexts;		// Queue에 있는 것이 다음에 들어올 것.
	SPTR				m_currentSet;			// 현재 적용되는 것.
	bool				m_bRenderWeatherEffect;	// 날씨를 화면에 그릴지 여부

	void				SetCurrentEnvPieceToDefault(){ SetCurrentEnvPiece("default", DAWN, SUNNY); }
	void				DeleteAllEnvBunch();

	void				ResetEnvProcess();
	void				UpdateEnvProcess();

	void				RemoveQueue();
	void				RemoveCurrent();
	void				Release();

	// 날씨는 WEATHER_NUM 만큼만 가진다. 날씨 관련 이펙트 효과
	RWeatherEffect*		m_pWeatherEffectTable[WHEATHER_NUM];

	// enum - string data
	static char*		m_pcTimeTable[TIME_NUM];
	static char*		m_pcWheatherTable[WHEATHER_NUM];

	bool				m_bEnvPieceChanged;
};

}
