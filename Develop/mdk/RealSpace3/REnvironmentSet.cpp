#include "stdafx.h"
#include "RDevice.h"
#include "REnvironmentSet.h"
#include "RTerrain.h"
#include "RWaterPassRenderer.h"
//#include "MZFileSystem.h"
#include "RConfiguration.h"
#include "RDirectionalLightSceneNode.h"


////#include "RTreeSystem.h"
#include "RTreeResourceMgr.h"
#include "RTreePassRenderer.h"
#include "RGrassSystem.h"
#include "RWeatherEffectSet.h"

#include "RActor.h"
#include "RSkyManager.h"
#include "RUtil.h"

#include "mmsystem.h"
#include "MTime.h"

//////////////////////////////////////////////////////////////////////////
// REnvironmentSet

namespace rs3 {

static bool bCallDel = true;

char*				REnvironmentSet::TILINGEFFECT_TEXTURE_FILENAME = "cloudshadow.tga";
REnvironmentSet*	REnvironmentSet::m_pCurrentSet = NULL;
REnvironmentSet*	REnvironmentSet::m_pCurrentSetOwn = NULL;

// static value
REnvironmentSet::REnvironmentSet()
: m_pWeatherEffectSet(new RWeatherEffectSet()), m_bExternalWeatherEffectSet(false)
{
	Init();
}

REnvironmentSet::REnvironmentSet( RWeatherEffectSet* pWeatherEffectSet )
: m_pWeatherEffectSet(pWeatherEffectSet), m_bExternalWeatherEffectSet(true)
{
	Init();
}

void REnvironmentSet::Init()
{
	hTillingTexture = R_NONE;
	m_bRenderWeatherEffect = true;
	m_pCurrentEnvironment = NULL;

	m_pCurrentEnvironment = CreateEnvironment("default");

	m_DefaultSet.vAmbient = RVector4(0.3f, 0.3f, 0.3f, 1.0f);
	m_DefaultSet.vDiffuse = RVector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_DefaultSet.fPower = 1.0f;
	m_DefaultSet.fWindStrength = 0.f;
	m_DefaultSet.pSkyManager = NULL;
	m_DefaultSet.fSubLightIntensity = 1.0f;

	m_eCurrentTime = REnvironment::TIME_NUM;
	m_eCurrentWeather = REnvironment::WEATHER_NUM;

	//mlog("REnvironment 생성자\n");
	m_vecTilling.reserve(4);

	SetVisible(true);
	m_eCurrentTime = REnvironment::TIME_NUM;
	m_eCurrentWeather = REnvironment::WEATHER_NUM;

	m_currentSet.pEnvSet = &m_DefaultSet;
	m_bBlendingEnabled = false;

	m_bApplyFog = true;

	m_vHDRBrightPassThreshold.Set(1.0f, 0.9f, 0.9f);
	m_fHDRBloomBlurDeviation = 3.f;
	m_fHDRBloomBlurMultiplier = 2.f;

	m_fSubLightIntensity = 1.0f;
}

REnvironmentSet::~REnvironmentSet()
{
	Release();

	if (!m_bExternalWeatherEffectSet)
	{
		delete m_pWeatherEffectSet;
	}
	m_pWeatherEffectSet = NULL;


	if(bCallDel)
		DeleteOwnSet();

	if( m_pCurrentSet == this)
		m_pCurrentSet = NULL;
}

void REnvironmentSet::ClearAll()
{
	m_currentSet.Destroy();
	m_blendingSet.Destroy();
}

void REnvironmentSet::Release()
{
	ClearAll();

	if (REngine::GetDevicePtr())
	{
		REngine::GetDevice().DeleteTexture(hTillingTexture);
		hTillingTexture = NULL;
	}

	m_vecTilling.clear();

	for (ENVIRONMENTSET::iterator it = m_EnvironmentSetTable.begin(); it != m_EnvironmentSetTable.end(); ++it)
	{
		REnvironment *pEnv = it->second;
		SAFE_DELETE(pEnv);
	}

	m_EnvironmentSetTable.clear();
	m_pCurrentEnvironment = NULL;

	m_pWeatherEffectSet->Release();
}

REnvironment* REnvironmentSet::GetCurrentEnvironment()
{
	return m_pCurrentEnvironment;
}

REnvironment* REnvironmentSet::CreateEnvironment(const std::string &strName)
{
	DeleteEnvironment(strName);

	//초기 기본값을 채워서 생성
	REnvironment *pEnvironment = new REnvironment();
	pEnvironment->FillEnvFromDefault();

	m_EnvironmentSetTable[strName] = pEnvironment;
	return pEnvironment;
}

void REnvironmentSet::SetCurrentEnvironment(const std::string &strName)
{
	m_pCurrentEnvironment = m_EnvironmentSetTable[strName];
	_ASSERT(m_pCurrentEnvironment);
}

void REnvironmentSet::DeleteEnvironment(const std::string &strName)
{
	ENVIRONMENTSET::iterator it = m_EnvironmentSetTable.find(strName);
	if (it != m_EnvironmentSetTable.end())
	{
		delete it->second;
		m_EnvironmentSetTable.erase(it);
	}
}


bool REnvironmentSet::CreateFromFile(const char *szFilename, bool bBackgroundCreation)
{
	MTimeChecker tc;
	tc.Start(timeGetTime()); // ---------------v

	MFile mzf;
	if(!mzf.Open(szFilename))
	{
		mlog("REnvironment-set: File not found ...\n");
		return false;
	}

	//mlog("REnvironmentSet::CreateFromFile - 1 (time=%u ms)\n", tc.Check(timeGetTime()));

	tc.Start(timeGetTime()); // ---------------v

	Release();

	//mlog("REnvironmentSet::CreateFromFile - 2 (time=%u ms)\n", tc.Check(timeGetTime()));

	tc.Start(timeGetTime()); // ---------------v

	char *buffer = new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;
	
	MXml aXml;
	if(!aXml.LoadStream(buffer))
	{
		SAFE_DELETE_ARRAY(buffer);
		return false;
	}

	SAFE_DELETE_ARRAY(buffer);

	//mlog("REnvironmentSet::CreateFromFile - 3 (time=%u ms)\n", tc.Check(timeGetTime()));

	tc.Start(timeGetTime()); // ---------------v

	MXmlElement *pElement = aXml.Doc()->FirstChildElement("ROOT");
	if (pElement == NULL)
	{
		mlog("REnvironment-set: isn't ROOT\n");
		return false;
	}
	//
	if (REngine::GetDevicePtr())
	{
		if (hTillingTexture!=R_NONE)
			REngine::GetDevice().DeleteTexture(hTillingTexture);

		hTillingTexture = REngine::GetDevice().CreateTexture(TILINGEFFECT_TEXTURE_FILENAME, RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE);
		if (hTillingTexture==R_NONE)
			mlog("cloud texture file not found...\n");
	}

	//mlog("REnvironmentSet::CreateFromFile - 4 (time=%u ms)\n", tc.Check(timeGetTime()));

	tc.Start(timeGetTime()); // ---------------v

	bool bResult = LoadWeatherFromXML();
	_ASSERT(bResult);

	//mlog("REnvironmentSet::CreateFromFile - 5 (time=%u ms)\n", tc.Check(timeGetTime()));

	tc.Start(timeGetTime()); // ---------------v

	bool bCreateFromXmlRet = CreateFromXML(*pElement, bBackgroundCreation);

	//mlog("REnvironmentSet::CreateFromFile - 6 (time=%u ms)\n", tc.Check(timeGetTime()));

	return bCreateFromXmlRet;
}


bool REnvironmentSet::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	MXmlElement *pChildElement = element.FirstChildElement("ENV");
	if (pChildElement == NULL)
	{
		mlog("REnvironment-set: isn't ENV\n");
		return false;
	}

	const char *szFirstName = pChildElement->Attribute("name");

	for (; pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
	{
		const char *szName = pChildElement->Attribute("name");
		REnvironment *pEnv = CreateEnvironment(szName);

		if(!pEnv->CreateFromXML(*pChildElement, bBackgroundCreation))
		{
			mlog("REnvironemt-set: failed loading ENVSET\n");
			Release();
			m_pCurrentEnvironment = CreateEnvironment("default");
			return false;
		}
	}

	SetCurrentEnvironment(szFirstName);
	return true;
}

void REnvironmentSet::SaveToFile(const char *szFilename)
{
	{
		MXml xml;
		MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
		xml.Doc()->LinkEndChild(pDec);
		MXmlElement *pRootElement = new MXmlElement("ROOT");
		xml.LinkEndChild(pRootElement);

		SaveToXML(*pRootElement);
		xml.SaveFile(szFilename);
	}

	m_pWeatherEffectSet->SaveXML();

}

void REnvironmentSet::SaveToXML(MXmlElement &element)
{
	for (ENVIRONMENTSET::iterator it = m_EnvironmentSetTable.begin(); it != m_EnvironmentSetTable.end(); ++it)
	{
		const std::string &strName = it->first;
		REnvironment *pEnv = it->second;

		MXmlElement *pElement = new MXmlElement("ENV");
		pElement->SetAttribute("name",strName);
		_SetContents(pElement,"");

		element.LinkEndChild(pElement);
		pEnv->SaveToXML(*pElement);
	}
}

void REnvironmentSet::GetNames(std::vector<std::string> &vecArray)
{
	vecArray.clear();
	vecArray.reserve(m_EnvironmentSetTable.size());

	for (ENVIRONMENTSET::iterator it = m_EnvironmentSetTable.begin(); it != m_EnvironmentSetTable.end(); ++it)
	{
		const std::string &strName = it->first;
		vecArray.push_back(strName);
	}
}

REnvironment::SEnvSet* REnvironmentSet::GetCurrentEnvSet()
{
	return m_currentSet.pEnvSet;
}

void REnvironmentSet::SetDefaultEnvSet()
{
	ClearAll();

	m_currentSet.pEnvSet = &m_DefaultSet;
	m_currentSet.pWeather = NULL;

	m_eCurrentWeather = REnvironment::WEATHER_NUM;
	m_eCurrentTime = REnvironment::TIME_NUM;

	CommitEnvSet();
	REngine::BroadcastEvent(RE_ENVIRONMENT_CHANGE);	
}

/// 블렌딩 팩터 1->0 으로 진행
float REnvironmentSet::GetBlendingFactor()
{
	float fAlpha = 0.0f;

	if(m_bBlendingEnabled)
	{
		if (m_fLifeTime > 0.0f)
			fAlpha = m_fLifeTime/m_fFadingTime;
	}

	return fAlpha;
}

void REnvironmentSet::CommitEnvSet()
{
	_ASSERT(m_pCurrentEnvironment);

	if ( m_eCurrentWeather == REnvironment::WEATHER_NUM || m_eCurrentTime == REnvironment::TIME_NUM )
		return;

	float fAlpha = GetBlendingFactor();
	_ASSERT( fAlpha >= 0.0f && fAlpha <= 1.0f );

	// (cosine interpolation) (1.0f - cos(fAlpha*MMath::PI))/2.0f;
	float blend = 1.0f - fAlpha;


	CommitWeatherEnv(blend);

	CommitWindEnv(blend);

	CommitFogEnv(blend);

	CommitSkyEnv(blend);

	CommitLightEnv(blend);

	CommitHDREnv(blend);

	CommitCloudShadow(blend);
}

void REnvironmentSet::CommitWeatherEnv(float fBlendFactor)
{
	if(IsBlending())
	{
		if(m_currentSet.pWeather)
			m_currentSet.pWeather->SetVisibility(fBlendFactor);

		if(m_blendingSet.pWeather)
			m_blendingSet.pWeather->SetVisibility(1.0f-fBlendFactor);
	}
	else
	{
		if(m_currentSet.pWeather)
			m_currentSet.pWeather->SetVisibility(1.0f);
	}
}

void REnvironmentSet::CommitWindEnv(float fBlendFactor)
{
	_ASSERT(m_currentSet.pEnvSet);

	float fWindStrength = m_currentSet.pEnvSet->fWindStrength;
	RVector3 vWindDirection = m_currentSet.pEnvSet->vWindDirection;

	float		fGrassJigglingPeriod		= m_currentSet.pEnvSet->m_fGrassJigglingPeriod;
	float		fGrassJigglingStrength		= m_currentSet.pEnvSet->m_fGrassJigglingStrength;
	float		fGrassIncliningStrength		= m_currentSet.pEnvSet->m_fGrassIncliningStrength;


	if (IsBlending())
	{
		vWindDirection = MMath::LinearInterpolation(m_blendingSet.pEnvSet->vWindDirection, m_currentSet.pEnvSet->vWindDirection, fBlendFactor);
		fWindStrength = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fWindStrength, m_currentSet.pEnvSet->fWindStrength, fBlendFactor);

		fGrassJigglingPeriod = MMath::LinearInterpolation(m_blendingSet.pEnvSet->m_fGrassJigglingPeriod, m_currentSet.pEnvSet->m_fGrassJigglingPeriod, fBlendFactor);
		fGrassJigglingStrength = MMath::LinearInterpolation(m_blendingSet.pEnvSet->m_fGrassJigglingStrength, m_currentSet.pEnvSet->m_fGrassJigglingStrength, fBlendFactor);
		fGrassIncliningStrength = MMath::LinearInterpolation(m_blendingSet.pEnvSet->m_fGrassIncliningStrength, m_currentSet.pEnvSet->m_fGrassIncliningStrength, fBlendFactor);
	}


	//나무
	REngine::GetSceneManager().GetResourceMgr<RTreeResourceMgr>()->SetWindDirection(vWindDirection);
	REngine::GetSceneManager().GetResourceMgr<RTreeResourceMgr>()->SetWindStrength(fWindStrength);


	//풀
	if( GetRootSceneNode() && GetRootSceneNode()->GetTerrain() && GetRootSceneNode()->GetTerrain()->GetGrassSystem() )
	{
		GetRootSceneNode()->GetTerrain()->GetGrassSystem()->SetWindDirection(m_currentSet.pEnvSet->vWindDirection);//vWindDirection);
		GetRootSceneNode()->GetTerrain()->GetGrassSystem()->SetWindStrength(fWindStrength);

		GetRootSceneNode()->GetTerrain()->GetGrassSystem()->SetGrassJigglingPeriod(		m_currentSet.pEnvSet->m_fGrassJigglingPeriod);//fGrassJigglingPeriod	);
		GetRootSceneNode()->GetTerrain()->GetGrassSystem()->SetGrassJigglingStrength(		fGrassJigglingStrength	);
		GetRootSceneNode()->GetTerrain()->GetGrassSystem()->SetGrassIncliningStrength(	fGrassIncliningStrength	);
	}
}

void REnvironmentSet::CommitFogEnv(float fBlendFactor)
{
	_ASSERT(m_currentSet.pEnvSet);

	bool bFogEnable		= !m_currentSet.pEnvSet->bFogDisable;
	RVector4 vFogColor	= m_currentSet.pEnvSet->vFogColor;
	float fFogNear		= m_currentSet.pEnvSet->fNear;
	float fFogFar		= m_currentSet.pEnvSet->fFar;

	if (IsBlending())
	{
		bFogEnable |= !m_blendingSet.pEnvSet->bFogDisable;
		vFogColor = MMath::LinearInterpolation(m_blendingSet.pEnvSet->vFogColor, m_currentSet.pEnvSet->vFogColor, fBlendFactor);
		fFogNear = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fNear, m_currentSet.pEnvSet->fNear, fBlendFactor);
		fFogFar = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fFar, m_currentSet.pEnvSet->fFar, fBlendFactor);
	}


	DWORD dwRed, dwGreen, dwBlue;
	dwRed	= (DWORD)(vFogColor.x * 255);
	dwGreen = (DWORD)(vFogColor.y * 255);
	dwBlue	= (DWORD)(vFogColor.z * 255);

	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	pCamera->SetFog(fFogNear, fFogFar, D3DCOLOR_XRGB(dwRed, dwGreen, dwBlue));
	pCamera->SetFogEnable(m_bApplyFog && bFogEnable);
}

void REnvironmentSet::CommitSkyEnv(float fBlendFactor)
{
	_ASSERT(m_currentSet.pEnvSet);

	if (m_currentSet.pEnvSet->pSkyManager)
	{//블렌딩후에 최종적으로 그려져야 되는 것이라서, 나중에 그려준다.
		m_currentSet.pEnvSet->pSkyManager->SetRenderOrder(RSkyManager::eRENDERORDER_SECOND);
		m_currentSet.pEnvSet->pSkyManager->SetMultiplyColor(RVector4(1, 1, 1, fBlendFactor));
	}

	if(IsBlending())
	{
		if (m_blendingSet.pEnvSet->pSkyManager)
		{
			// 배경으로 쓰이는 클리어버퍼의 색깔값이 스카이색과 블렌딩 되면 안되기때문에,
			// 처음에 그리는 스카이의 색깔값은 클리어버퍼의 색깔값과 블렌딩이 안되게 하기 위해,
			// 블렌딩값을 1로 준다. 블렌딩값 1의 의미는 스카이의 색깔을 배경과 전혀 블렌딩 안한다는 의미이다.
			m_blendingSet.pEnvSet->pSkyManager->SetRenderOrder(RSkyManager::eRENDERORDER_FIRST);
			m_blendingSet.pEnvSet->pSkyManager->SetMultiplyColor(RVector4(1, 1, 1, 1));
		}
	}
}

void REnvironmentSet::CommitLightEnv(float fBlendFactor)
{
	RDirectionalLightSceneNode *pLight = REngine::GetSceneManager().GetDefaultLight();
	if (pLight)
	{
		_ASSERT(m_currentSet.pEnvSet);

		RVector4 vAmbient = m_currentSet.pEnvSet->vAmbient;
		RVector4 vDiffuse = m_currentSet.pEnvSet->vDiffuse;
		float fPower = m_currentSet.pEnvSet->fPower;
		float fShadowLuminosity = m_currentSet.pEnvSet->fShadowLuminosity;

		if (IsBlending())
		{
			vAmbient = MMath::LinearInterpolation(m_blendingSet.pEnvSet->vAmbient, m_currentSet.pEnvSet->vAmbient, fBlendFactor);
			vDiffuse = MMath::LinearInterpolation(m_blendingSet.pEnvSet->vDiffuse, m_currentSet.pEnvSet->vDiffuse, fBlendFactor);
			fPower = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fPower, m_currentSet.pEnvSet->fPower, fBlendFactor);
			fShadowLuminosity = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fShadowLuminosity, m_currentSet.pEnvSet->fShadowLuminosity, fBlendFactor);
		}

		pLight->SetLightAmbient(vAmbient);
		pLight->SetLightDiffuse(vDiffuse);
		pLight->SetPower(fPower);
		pLight->SetShadowLuminosity(fShadowLuminosity);
	}


	if (IsBlending())
		m_fSubLightIntensity = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fSubLightIntensity, m_currentSet.pEnvSet->fSubLightIntensity, fBlendFactor);
	else
		m_fSubLightIntensity = m_currentSet.pEnvSet->fSubLightIntensity;
}

void REnvironmentSet::CommitHDREnv(float fBlendFactor)
{
	_ASSERT(m_currentSet.pEnvSet);

	if (IsBlending())
	{
		m_vHDRBrightPassThreshold = MMath::LinearInterpolation(m_blendingSet.pEnvSet->vHDRBrightPassThreshold, m_currentSet.pEnvSet->vHDRBrightPassThreshold, fBlendFactor);
		m_fHDRBloomBlurDeviation = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fHDRBloomBlurDeviation, m_currentSet.pEnvSet->fHDRBloomBlurDeviation, fBlendFactor);
		m_fHDRBloomBlurMultiplier = MMath::LinearInterpolation(m_blendingSet.pEnvSet->fHDRBloomBlurMultiplier, m_currentSet.pEnvSet->fHDRBloomBlurMultiplier, fBlendFactor);
	}
	else
	{
		m_vHDRBrightPassThreshold = m_currentSet.pEnvSet->vHDRBrightPassThreshold;
		m_fHDRBloomBlurDeviation = m_currentSet.pEnvSet->fHDRBloomBlurDeviation;
		m_fHDRBloomBlurMultiplier = m_currentSet.pEnvSet->fHDRBloomBlurMultiplier;
	}
}

void REnvironmentSet::CommitCloudShadow(float fBlendFactor)
{
	_ASSERT(m_currentSet.pEnvSet);

	m_vecTilling.clear();
	if (hTillingTexture!=R_NONE)
	{
		// 선형으로 보간하면 블렌딩 중간이 너무 비어보여서 곡선을 만들었다
		const float fBlendingExp = 0.7f;
		if (m_currentSet.pEnvSet->m_bCloudVisible )
		{
			STillingInfo layer1;
			layer1.fVisibility	= pow( fBlendFactor, fBlendingExp );
			layer1.matTransform.MakeIdentity();

			RVector &vScale = m_currentSet.pEnvSet->m_vCloudShadowScale;
			layer1.vecScale = RVector(1.0f/vScale.x, 1.0f/vScale.y, 1.f);
			layer1.vecTranslate = m_currentSet.pEnvSet->m_vCloudShadowTranslate;
			layer1.matTransform.SetScale(layer1.vecScale);

			m_vecTilling.push_back(layer1);
		}

		if (IsBlending() && m_blendingSet.pEnvSet->m_bCloudVisible)
		{
			STillingInfo layer2;
			layer2.fVisibility = pow( (1.0f-fBlendFactor), fBlendingExp );
			layer2.matTransform.MakeIdentity();

			RVector &vScale = m_blendingSet.pEnvSet->m_vCloudShadowScale;
			layer2.vecTranslate = m_blendingSet.pEnvSet->m_vCloudShadowTranslate;
			layer2.vecScale = RVector(1.0f/vScale.x, 1.0f/vScale.y, 1.f);
			layer2.matTransform.SetScale(layer2.vecScale);
			m_vecTilling.push_back(layer2);
		}
	}
}

bool REnvironmentSet::EnvironmentFadingInfo::Create(RSceneNode* pSceneNode)
{
	if (pEnvSet->pSkyManager->GetParent()==NULL)
	{
		pSceneNode->AddChild(pEnvSet->pSkyManager);
	}
	if (pWeather && pWeather->GetParent()==NULL)
	{
		pSceneNode->AddChild(pWeather);
		pWeather->SetFill(true);
	}
	return true;
}

void REnvironmentSet::EnvironmentFadingInfo::Destroy()
{
	if (pEnvSet)
	{
		RSkyManager *pSky = pEnvSet->pSkyManager;
		if (pSky)
		{
			if (pSky->GetParent())
				pSky->RemoveFromParent();

			pSky->ReleaseScenes();
		}

		pEnvSet = NULL;
	}
	if (pWeather)
	{
		if (pWeather->GetParent())
			pWeather->RemoveFromParent();

		pWeather = NULL;
	}
}


bool REnvironmentSet::SetCurrentEnvSet(REnvironment::ETime eTime, REnvironment::EWeather eWeather, bool bFade, float fFadingTime, bool bBackgroundLoading, RCollisionTree* pCollisionTree)
{
	_ASSERT(m_pCurrentEnvironment);

	
	/*// 무시하는 것들 <= 이거 때문에 environmentSet이 바뀔때 해당 셋의 날씨값으로 적용이 안된다
	if (m_eCurrentTime == eTime && m_eCurrentWeather == eWeather && m_currentSet.pEnvSet != NULL)
		return true;
	*/

	REnvironment::SEnvSet* pEnvSet = m_pCurrentEnvironment->GetEnv(eTime, eWeather);
	if (pEnvSet == NULL) return false;

	if( bFade )
	{
		// 이미 블렌딩 중이면 안된다
		//_ASSERT( m_bBlendingEnabled == false );
		
		// 블렌딩 시작
		m_blendingSet = m_currentSet;

		m_bBlendingEnabled = true;
		m_fFadingTime = fFadingTime;
		m_fLifeTime = fFadingTime;
	}

	EnvironmentFadingInfo& env = m_currentSet;
	env.pEnvSet = pEnvSet;
	env.pWeather = m_pWeatherEffectSet->GetWeatherEffect(eWeather);
	env.eWeather = eWeather;
	env.eTime = eTime;

	// Fade가 아니면, 큐에 대기열을 모두 지우고 하나만을 넣는다.
	m_eCurrentTime = eTime;
	m_eCurrentWeather = eWeather;

	_ASSERT(env.pEnvSet);
	RSkyManager *pSky = env.pEnvSet->pSkyManager;
	_ASSERT(pSky);

	// 백그라운드 로딩을 요청
	bool bCreateBackground = pSky->CreateScenes(bBackgroundLoading);
	_ASSERT(bCreateBackground);

	Update( pCollisionTree );

	if(	!bFade )
		CommitEnvSet();

	// environment 가 바뀌었으므로 통지한다
	REngine::BroadcastEvent(RE_ENVIRONMENT_CHANGE);

	return true;
}

bool REnvironmentSet::IsBlending()
{
	return m_bBlendingEnabled && m_blendingSet.pEnvSet && m_blendingSet.pWeather;
}

void REnvironmentSet::EndBlending()
{
	if(!m_bBlendingEnabled) return;

	m_blendingSet.Destroy();
	m_bBlendingEnabled = false;

	CommitEnvSet();
	REngine::BroadcastEvent(RE_ENVIRONMENT_CHANGE);
}

void REnvironmentSet::UpdateBlending()
{
	if ( !m_bBlendingEnabled ) return;

	float fElapsedTime = REngine::GetSceneManager().GetTimer().GetElapsedTime() / 1000.f;

	m_fLifeTime -= fElapsedTime;
	m_fLifeTime = max( m_fLifeTime, 0 );

	// 계속 블렌딩 중
	REngine::BroadcastEvent(RE_ENVIRONMENT_BLENDING);
	CommitEnvSet();

	// 블렌딩 끝
	if (m_fLifeTime <= 0.0f)
		EndBlending();
}

void REnvironmentSet::UpdateLoading()
{
	if(!m_currentSet.pEnvSet) return;
	if(!m_currentSet.pEnvSet->pSkyManager) return;

	// 전부 Scene이 로드 되지 않았으면
	RRSTATE eState = m_currentSet.pEnvSet->pSkyManager->GetScenesState();
	if ( eState == RRS_EMPTY )
		return;

	if ( eState == RRS_LOADED )
	{
		m_currentSet.Create(this);
	}
	else
	{
		m_fLifeTime = -0.001f;
		mlog("Failed to create sky.\n");
	}
}

void REnvironmentSet::UpdateWeather( float fTime, RCollisionTree* pCollisionTree )
{
	RWeatherEffect* pCurrent = m_currentSet.pWeather;
	RWeatherEffect* pOld = m_bBlendingEnabled ? m_blendingSet.pWeather : NULL;

	if( pCurrent )
		pCurrent->Update( fTime, pCollisionTree );

	if( pOld )
		pOld->Update( fTime, pCollisionTree );
}

void REnvironmentSet::UpdateCloud( float fTime )
{
	for ( size_t i = 0; i<m_vecTilling.size(); ++i )
	{
		RMatrix matScale;
		matScale.MakeIdentity();
		matScale.SetScale(m_vecTilling[i].vecScale);

		RMatrix matTranslate;
		matTranslate.MakeIdentity();
		matTranslate.SetTranslation(m_vecTilling[i].vecTranslate*fTime);

		// 광원 방향에 정렬되게 회전매트릭스를 생성한다. - 090609, OZ
		RMatrix matRotate;
		matRotate.MakeIdentity();
		RVector lightDir = RVector::AXISZ;
		if( REngine::GetSceneManagerPtr())
		{
			RDirectionalLightSceneNode* pDirLight = REngine::GetSceneManagerPtr()->GetDefaultLight();
			if( pDirLight)
			{
				lightDir = -pDirLight->GetDirection();
			}
		}

		// 우방벡터와 상방벡터로 전방벡터구함. 
		RVector _dir = RVector::AXISY;		// Y가 전방향
		RVector _up = Normalize(lightDir);
		RVector _right = Normalize(_dir.CrossProduct(_up));
		_dir = Normalize( CrossProduct( _up, _right));
		matRotate._11 = _right.x;	matRotate._12 = _right.y;	matRotate._13 = _right.z;
		matRotate._21 = _dir.x;		matRotate._22 = _dir.y;		matRotate._23 = _dir.z;
		matRotate._31 = _up.x;		matRotate._32 = _up.y;		matRotate._33 = _up.z;

		m_vecTilling[i].matTransform = matRotate * matTranslate * matScale;
	}
}

void REnvironmentSet::Update( RCollisionTree* pCollisionTree )
{
	RPFC_THISFUNC;

	UpdateLoading();

	UpdateBlending();

	float fCurrentTime = REngine::GetSceneManager().GetTimer().GetTime() / 1000.f;

	UpdateWeather( fCurrentTime, pCollisionTree );

	// 구름 업데이트
	UpdateCloud( fCurrentTime );
}

void REnvironmentSet::SetCurrent()
{
	m_pCurrentSet = this;
	// 외부에서 지정이 되었다면 OwnSet은 더이상 필요가 없다.
	DeleteOwnSet();
}

REnvironmentSet* REnvironmentSet::GetInstancePtr()
{
	if(!m_pCurrentSet)
	{
		if(!m_pCurrentSetOwn)
			m_pCurrentSetOwn = new REnvironmentSet;
		m_pCurrentSet = m_pCurrentSetOwn;
	}

	return m_pCurrentSet;
}

void REnvironmentSet::DeleteOwnSet()
{
	if( m_pCurrentSet == m_pCurrentSetOwn)
		m_pCurrentSet = NULL;
	if( bCallDel)
	{
		bCallDel = false;
		SAFE_DELETE( m_pCurrentSetOwn);
		bCallDel = true;
	}
}

bool REnvironmentSet::LoadWeatherFromXML()
{
	return m_pWeatherEffectSet->LoadXML();
}

void REnvironmentSet::OnAddScene()
{
	_ASSERT( NULL != GetRootSceneNode() );

	GetRootSceneNode()->SetEnvironmentSet( this);

	// 실제 적용도 되어야 한다.
	CommitEnvSet();
	SetCurrent();
}

} // end of rs3