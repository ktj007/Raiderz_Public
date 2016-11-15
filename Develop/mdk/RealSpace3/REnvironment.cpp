#include "stdafx.h"
#include "REnvironment.h"
#include "RSkyManager.h"
//#include "MZFileSystem.h"
#include "RXMLTemplate.h"
#include "RConfiguration.h"
#include "RTerrain.h"

#include "RApplication.h"

namespace rs3 {

using namespace std;

REnvironment::REnvironment()
{
	m_kEnvTable.clear();
//	mlog("REnvironment 생성자\n");
}

REnvironment::~REnvironment()
{
	Destroy();
//	mlog("REnvironment 소멸자\n");
}

char *REnvironment::m_pcTimeTable[TIME_NUM] =
{
	"dawn",
	"daytime",
	"sunset",
	"night"
};

char *REnvironment::m_pcWeatherTable[WEATHER_NUM] =
{
	"sunny",
	"cloud",
	"rainy",
	"heavy_rainy",
	"snow",
	"heavy_snow"
};

const char *REnvironment::ETimeToString(ETime eTime)
{
	if ( eTime >= TIME_NUM )	return NULL;
	if ( eTime < 0 )			return NULL;

	return m_pcTimeTable[eTime];
}

const char *REnvironment::EWeatherToString(EWeather eWeather)
{
	if ( eWeather >= WEATHER_NUM )	return NULL;
	if ( eWeather < 0 )				return NULL;

	return m_pcWeatherTable[eWeather];
}

REnvironment::ETime REnvironment::StringToETime(const char *szName)
{
	if (szName == NULL) return TIME_NUM;

	for ( int i = 0; i<TIME_NUM; ++i )
	{
		ETime ei = (ETime)i;
		if ( stricmp(szName,m_pcTimeTable[ei]) == 0 )
			return ei;
	}

	return TIME_NUM;
}

REnvironment::EWeather REnvironment::StringToEWeather(const char *szName)
{
	if (szName == NULL) return WEATHER_NUM;

	for ( int i = 0; i<WEATHER_NUM; ++i )
	{
		EWeather ei = (EWeather)i;
		if ( stricmp(szName,m_pcWeatherTable[ei]) == 0 )
			return ei;
	}

	return WEATHER_NUM;
}

void REnvironment::Destroy()
{
	for ( ENVTABLE::iterator it = m_kEnvTable.begin(); it != m_kEnvTable.end(); ++it )
	{
		SEnvSet *pSet = (SEnvSet*)it->second;
		if ( pSet )
		{
			pSet->pSkyManager->RemoveFromParent();
#if 0
			if ( pSet->pWeather )
				pSet->pWeather->RemoveFromParent();

			SAFE_DELETE(pSet->pWeather);
#endif
			SAFE_DELETE(pSet->pSkyManager);

			delete pSet;
			pSet = NULL;
		}
	}

	m_kEnvTable.clear();
}

void REnvironment::DelEnv(ETime eTime, EWeather eWeather)
{
	ENVKEY key(eTime, eWeather);
	ENVTABLE::iterator it = m_kEnvTable.find(key);
	if ( it != m_kEnvTable.end())
	{
		m_kEnvTable[key]->pSkyManager->RemoveFromParent();
#if 0
		m_kEnvTable[key]->pWeather->RemoveFromParent();
		SAFE_DELETE(m_kEnvTable[key]->pWeather);
#endif

		SAFE_DELETE(m_kEnvTable[key]->pSkyManager);
		SAFE_DELETE(m_kEnvTable[key]);

		m_kEnvTable.erase(it);
	}
}

REnvironment::SEnvSet* REnvironment::CreateEnv(ETime eTime, EWeather eWeather)
{
	SEnvSet *pSet = new SEnvSet;
	pSet->pSkyManager = new RSkyManager;

	// by pok. 씬매니저 변경중
	//pSet->pSkyManager->SetManager( RGetSceneManager() );
#if 0
	pSet->pWeather = new RWeatherEffect;
#endif

	ENVKEY key(eTime, eWeather);
	ENVTABLE::iterator itr = m_kEnvTable.find(key);

	if ( itr != m_kEnvTable.end())
	{
		RSkyManager *pSky = itr->second->pSkyManager;
		pSky->RemoveFromParent();

#if 0 
		RWeatherEffect *pWeather = itr->second->pWeather;
		if ( pWeather )
			pWeather->RemoveFromParent();
		SAFE_DELETE(pWeather);
#endif

		SAFE_DELETE(pSky);
		SAFE_DELETE(itr->second);
	}

	m_kEnvTable[ENVKEY((int)eTime,(int)eWeather)] = pSet;

	return pSet;
}

REnvironment::SEnvSet* REnvironment::GetEnv(ETime eTime, EWeather eWeather)
{
	ENVKEY key(eTime, eWeather);

	if ( m_kEnvTable.find(key) != m_kEnvTable.end())
		return m_kEnvTable[key];

	return NULL;
}

void REnvironment::FillEnvFromDefault()
{
	for ( int nTime = 0; nTime<TIME_NUM; ++nTime)
		for ( int nWeather = 0; nWeather<WEATHER_NUM; ++nWeather )
		{
			ETime eTime = (ETime)nTime;
			EWeather eWeather = (EWeather)nWeather;

			if ( GetEnv(eTime, eWeather) == NULL )
				CreateEnv(eTime, eWeather);
		}
}


void REnvironment::LoadEnvironmentTexture( SEnvSet* _pEnvSet, MXmlElement* _pChildElement )
{
	MXmlElement *pElement = _pChildElement->FirstChildElement("ENVIRONMENT_MAP");
	if( pElement )
	{
		MXmlElement *pChild = pElement->FirstChildElement("FILENAME");
		char strFilename[32] = "";
		_Contents(strFilename, pChild);

		_pEnvSet->pEnv_Texture = REngine::GetDevice().CreateTexture(strFilename, RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE);
		if ( R_NONE == _pEnvSet->pEnv_Texture )
			mlog("Environment texture file not found...\n");
	}
}

bool REnvironment::ReadEnvSetContents(SEnvSet* pEnvSet, MXmlElement* pChildElement)
{
	// by pok_background, REnvironment는 백그라운드 로딩을 하지 않는다.
	if( !pEnvSet->pSkyManager->CreateFromXML(*pChildElement, false) )
		return false;

	using namespace TXML;

	//LoadEnvironmentTexture( pEnvSet, pChildElement );

	{
		MXmlElement *pElement = pChildElement->FirstChildElement("ENVIRONMENT_MAP");
		if( pElement )
		{
			MXmlElement *pChild = pElement->FirstChildElement("FILENAME");
			char strFilename[32] = "";
			_Contents(strFilename, pChild);

			pEnvSet->pEnv_Texture = REngine::GetDevice().CreateTexture(strFilename, RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE);
			if ( R_NONE == pEnvSet->pEnv_Texture )
				mlog("Environment texture file not found...\n");
		}
	}

	_GetElementValue(&pEnvSet->vAmbient, pChildElement, "AMBIENT","vec4");
	_GetElementValue(&pEnvSet->vDiffuse, pChildElement, "DIFFUSE","vec4");

	// POWER 항목이 없을경우 기본값을 1로 주고싶은데 방법이 없어서,,, dubble
	if(NULL!=pChildElement->FirstChild("POWER"))
		pEnvSet->fPower = _GETElement<float>::Value(pChildElement,"POWER" , "float");

	if(NULL!=pChildElement->FirstChild("SHADOW_LUMINOSITY"))
		pEnvSet->fShadowLuminosity = _GETElement<float>::Value(pChildElement,"SHADOW_LUMINOSITY", "float");

	_GetElementValue(&pEnvSet->vHDRBrightPassThreshold, pChildElement, "HDR_BRIGHTPASS_THRESHOLD", "vec3");

	if(NULL!=pChildElement->FirstChild("HDR_BLOOM_BLUR_DEVIATION"))
		pEnvSet->fHDRBloomBlurDeviation = _GETElement<float>::Value(pChildElement,"HDR_BLOOM_BLUR_DEVIATION", "float");

	if(NULL!=pChildElement->FirstChild("HDR_BLOOM_BLUR_MULTIPLIER"))
		pEnvSet->fHDRBloomBlurMultiplier = _GETElement<float>::Value(pChildElement,"HDR_BLOOM_BLUR_MULTIPLIER", "float");

	_GetElementValue(&pEnvSet->vFogColor, pChildElement, "FOG_COLOR","vec4");
	pEnvSet->fNear = _GETElement<float>::Value(pChildElement,"FOG_NEAR" , "float");
	pEnvSet->fFar = _GETElement<float>::Value(pChildElement, "FOG_FAR", "float");
	pEnvSet->bFogDisable = ( _GETElement<int>::Value(pChildElement, "FOG_DISABLE", "int") ? true : false);

	_GetElementValue(&pEnvSet->vWindDirection, pChildElement, "WIND_DIRECTION", "vec3");
	pEnvSet->fWindStrength = _GETElement<float>::Value(pChildElement,"WIND_STRENGTH", "float");

	// 풀
	pEnvSet->m_fGrassJigglingPeriod		= _GETElement<float>::Value(pChildElement,"GRASS_JIGGLING_PERIOD",		"float");
	pEnvSet->m_fGrassJigglingStrength	= _GETElement<float>::Value(pChildElement,"GRASS_JIGGLING_STRENGTH",	"float");
	pEnvSet->m_fGrassIncliningStrength	= _GETElement<float>::Value(pChildElement,"GRASS_INCLINGING_STRENGTH",	"float");

	if(NULL!=pChildElement->FirstChild("SUBLIGHT_INTENSITY"))
		pEnvSet->fSubLightIntensity = _GETElement<float>::Value(pChildElement,"SUBLIGHT_INTENSITY" , "float");

	// 구름 그림자 읽어들이는 부분	
	MXmlElement *pCloudElement = pChildElement->FirstChildElement("CLOUDSHADOW");
	if ( pCloudElement )
	{
		int num = 0;
		pCloudElement->Attribute("size",&num);
		_ASSERT(num==1);

		MXmlElement *pElement = pCloudElement->FirstChildElement();
		_ASSERT(_stricmp(pElement->Value(),"LAYER")==0);
		{
			_GetElementValue(&pEnvSet->m_vCloudShadowTranslate, pElement, "TRANSLATE","vec3");
			_GetElementValue(&pEnvSet->m_vCloudShadowScale, pElement, "SCALE", "vec3");
		}
		pEnvSet->m_bCloudVisible = ( _GETElement<int>::Value(pElement,"VISIBLE","int") ? true : false );
	}

	return true;
}

bool REnvironment::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	Destroy();

	MXmlElement *pRootElement = &element;
	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement("ENVSET");
		pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
	{
		int nTime, nWeather;

		nTime = StringToETime( pChildElement->Attribute("time"));
		if ( nTime == TIME_NUM )
		{
			continue;
		}

		nWeather = StringToEWeather( pChildElement->Attribute("weather"));
		if ( nWeather == WEATHER_NUM )
		{
			continue;
		}

		SEnvSet *pEnvSet = CreateEnv((ETime)nTime,(EWeather)nWeather);
		if( !ReadEnvSetContents(pEnvSet, pChildElement) )
		{
			mlog("REnvironment: failed loading ENVSet %d %d\n", nTime, nWeather);
			Destroy();
			return false;
		}
	}

	return true;
}

void REnvironment::SaveToXML(MXmlElement &element)
{
	MXmlElement *pRootElement = &element;

	for ( ENVTABLE::iterator it = m_kEnvTable.begin(); it != m_kEnvTable.end(); ++it )
	{
		MXmlElement *pChildElement = new MXmlElement("ENVSET");
		ETime eTime = (ETime)(it->first.first);
		EWeather eWeather = (EWeather)(it->first.second);
		pChildElement->SetAttribute("time", ETimeToString(eTime));
		pChildElement->SetAttribute("weather", EWeatherToString(eWeather));

		SEnvSet *pEnvSet = (SEnvSet*)(it->second);
		_ASSERT(pEnvSet!=NULL);

		pEnvSet->pSkyManager->SaveToXML(*pChildElement);

		using namespace TXML;

		_SetElementValue(pChildElement,"AMBIENT","vec4", pEnvSet->vAmbient);
		_SetElementValue(pChildElement,"DIFFUSE","vec4", pEnvSet->vDiffuse);
		_SetElementValue(pChildElement,"POWER","float", pEnvSet->fPower);
		_SetElementValue(pChildElement,"SHADOW_LUMINOSITY", "float", pEnvSet->fShadowLuminosity);
		_SetElementValue(pChildElement,"HDR_BRIGHTPASS_THRESHOLD", "vec3", pEnvSet->vHDRBrightPassThreshold);
		_SetElementValue(pChildElement,"HDR_BLOOM_BLUR_DEVIATION", "float", pEnvSet->fHDRBloomBlurDeviation);
		_SetElementValue(pChildElement,"HDR_BLOOM_BLUR_MULTIPLIER", "float", pEnvSet->fHDRBloomBlurMultiplier);
		_SetElementValue(pChildElement,"FOG_COLOR","vec4", pEnvSet->vFogColor);
		_SetElementValue(pChildElement,"FOG_NEAR" ,"float",pEnvSet->fNear);
		_SetElementValue(pChildElement,"FOG_FAR" ,"float", pEnvSet->fFar);
		_SetElementValue(pChildElement,"FOG_DISABLE","int", ( pEnvSet->bFogDisable ? 1 : 0 ));

		RVector vDirection(pEnvSet->vWindDirection);
		_SetElementValue(pChildElement,"WIND_DIRECTION","vec3", vDirection);
		_SetElementValue(pChildElement,"WIND_STRENGTH" ,"float", pEnvSet->fWindStrength);

		_SetElementValue(pChildElement,"GRASS_JIGGLING_PERIOD" ,	"float", pEnvSet->m_fGrassJigglingPeriod);
		_SetElementValue(pChildElement,"GRASS_JIGGLING_STRENGTH" ,	"float", pEnvSet->m_fGrassJigglingStrength);
		_SetElementValue(pChildElement,"GRASS_INCLINGING_STRENGTH" ,"float", pEnvSet->m_fGrassIncliningStrength);

		_SetElementValue(pChildElement,"SUBLIGHT_INTENSITY","float", pEnvSet->fSubLightIntensity);

		if ( pEnvSet->m_bCloudVisible )
		{
			// cloud save
			MXmlElement *pCloudElement = new MXmlElement("CLOUDSHADOW");
			pChildElement->LinkEndChild(pCloudElement);

			const int LAYER_NUM = 1;
			pCloudElement->SetAttribute("size", LAYER_NUM);
			MXmlElement *pElement = new MXmlElement("LAYER");
			pCloudElement->LinkEndChild(pElement);
			{
				_SetElementValue(pElement,"TRANSLATE","vec3", pEnvSet->m_vCloudShadowTranslate);
				_SetElementValue(pElement,"SCALE","vec3", pEnvSet->m_vCloudShadowScale);
			}
			_SetElementValue(pElement,"VISIBLE","int", (int)pEnvSet->m_bCloudVisible);
		}

		pRootElement->LinkEndChild(pChildElement);
	}
}


};