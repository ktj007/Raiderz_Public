#include "stdafx.h"
#include "RWeatherEffectSet.h"

namespace rs3 {

RWeatherEffectSet::RWeatherEffectSet()
{
	for ( int i = 0; i<REnvironment::WEATHER_NUM; ++i )
		m_pWeatherEffectTable[i] = new RWeatherEffect;
}

RWeatherEffectSet::~RWeatherEffectSet()
{
	for ( int i = 0; i<REnvironment::WEATHER_NUM; ++i )
	{
		if (m_pWeatherEffectTable[i] )
		{
			if (m_pWeatherEffectTable[i]->GetParent())
				m_pWeatherEffectTable[i]->RemoveFromParent();

			delete m_pWeatherEffectTable[i];
			m_pWeatherEffectTable[i] = NULL;
		}
	}
}

void RWeatherEffectSet::Release()
{
	for ( int i = 0; i<REnvironment::WEATHER_NUM; ++i )
	{
		if (m_pWeatherEffectTable[i] )
		{
			if (m_pWeatherEffectTable[i]->GetParent())
				m_pWeatherEffectTable[i]->RemoveFromParent();
		}
	}
}

bool RWeatherEffectSet::LoadXML()
{
	MXml kXml;
	bool bResult = kXml.LoadFile("weathereffect.xml");
	if (bResult==false)
	{
		mlog("REnvironment-set: weathereffect.xml not found\n");
		return false;
	}

	MXmlElement *pWeatherRoot = kXml.Doc()->RootElement();
	if (_stricmp(pWeatherRoot->Value(),"WEATHERSET") == 0)
	{
		// weather load
		for (MXmlElement *pWeather = pWeatherRoot->FirstChildElement()
			; pWeather != NULL; pWeather = pWeather->NextSiblingElement())
		{
			const char *type = pWeather->Value();
			REnvironment::EWeather eWeather = REnvironment::StringToEWeather(type);
			if (eWeather == REnvironment::WEATHER_NUM)
				continue;

			RWeatherEffect *pWeatherEffect = m_pWeatherEffectTable[eWeather];
			_ASSERT(pWeatherEffect!=NULL);
			{
				// by pok_background, 날씨효과는 백그라운드로 로딩하지 않는다.
				bool bResult = true;
				bResult = pWeatherEffect->CreateFromXML(*pWeather, NULL);
				if ( bResult )
					bResult = pWeatherEffect->Create();
			}
		}

		return true;
	}

	mlog("REnvironment-set: isn't WEATHERSET\n");
	return false;
}

void RWeatherEffectSet::SaveXML()
{
	MXml xml;
	std::string strPath = REngine::GetConfig().GetDataPath("system/weathereffect.xml");

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);
	MXmlElement *pRootElement = new MXmlElement("WEATHERSET");
	xml.LinkEndChild(pRootElement);

	for (UINT ui = 0; ui < REnvironment::WEATHER_NUM; ++ui)
	{
		if (m_pWeatherEffectTable[ui])
		{
			const char *pType = REnvironment::EWeatherToString((REnvironment::EWeather)ui);
			MXmlElement *pChildElement = new MXmlElement(pType);
			m_pWeatherEffectTable[ui]->SaveToXML(*pChildElement);

			pRootElement->LinkEndChild(pChildElement);
		}
	}

	xml.SaveFile(strPath.c_str());
}

} // namespace rs3