#include "stdafx.h"
#include "GFieldInfo.h"
#include "GWeatherMgr.h"

SUITE(Weather)
{
	class TestFieldInfo : public GFieldInfo
	{
	public:
		using GFieldInfo::InitWeather;
	};

	class FWeather
	{
	public:
		FWeather()
		{

		}
		~FWeather()
		{

		}
		void AddWeatherNode(GAME_WEATHER_TYPE nWeatherType, int nPercent)
		{
			GFieldWeatherInfoNode node;
			node.nWeather = nWeatherType;
			node.nPercent = nPercent;

			m_FieldInfo.m_WeatherInfo.vecNodes.push_back(node);
		}
		void InitWeather()
		{
			m_FieldInfo.InitWeather();
		}

		TestFieldInfo	m_FieldInfo;
		GWeatherMgr		m_WeatherMgr;
	};

	TEST_FIXTURE(FWeather, TestFieldInfo_WeatherInfo_Default)
	{
		InitWeather();

		CHECK_EQUAL(WEATHER_SUNNY, m_FieldInfo.m_WeatherInfo.nDefault);
	}

	TEST_FIXTURE(FWeather, TestFieldInfo_IsWeatherNotChanged)
	{
		AddWeatherNode(WEATHER_RAINY, 10);
		InitWeather();

		CHECK_EQUAL(false, m_FieldInfo.IsWeatherNotChanged());
	}

	TEST_FIXTURE(FWeather, TestFieldInfo_IsWeatherNotChanged_Negative)
	{
		m_FieldInfo.m_WeatherInfo.vecNodes.clear();
		InitWeather();

		CHECK_EQUAL(true, m_FieldInfo.IsWeatherNotChanged());
	}

	TEST_FIXTURE(FWeather, TestFieldInfo_InitWeather)
	{
		AddWeatherNode(WEATHER_HEAVY_SNOWY, 10);
		AddWeatherNode(WEATHER_CLOUDY, 50);
		InitWeather();

		CHECK_EQUAL(100, m_FieldInfo.m_WeatherInfo.vecRandomData.size());

		int nSunny = 0;
		int nHeavySnowy = 0;
		int nCloudy = 0;

		for (size_t i = 0; i < m_FieldInfo.m_WeatherInfo.vecRandomData.size(); i++)
		{
			GAME_WEATHER_TYPE nWeather = m_FieldInfo.m_WeatherInfo.vecRandomData[i];
			if (nWeather == WEATHER_SUNNY) ++nSunny;
			else if (nWeather == WEATHER_CLOUDY) ++nCloudy;
			else if (nWeather == WEATHER_HEAVY_SNOWY) ++nHeavySnowy;
		}

		CHECK_EQUAL(10, nHeavySnowy);
		CHECK_EQUAL(50, nCloudy);
		CHECK_EQUAL(40, nSunny);
	}

	TEST_FIXTURE(FWeather, TestWeatherMgr_Init_NoWeatherChange)
	{
		m_FieldInfo.m_WeatherInfo.nDefault = WEATHER_HEAVY_SNOWY;
		InitWeather();

		m_WeatherMgr.Init(&m_FieldInfo, 1);

		CHECK_EQUAL(WEATHER_HEAVY_SNOWY, m_WeatherMgr.GetCurrentWeather());
	}

}