#include "stdafx.h"
#include "REnvironment.h"

SUITE(RS3)
{
	TEST(EnvironmentString)
	{
		CHECK_EQUAL(REnvironment::StringToETime(NULL), REnvironment::TIME_NUM);
		CHECK_EQUAL(REnvironment::StringToEWeather(NULL), REnvironment::WEATHER_NUM);

		for (int i = 0; i < REnvironment::TIME_NUM; i++)
		{
			string strTime = REnvironment::ETimeToString(REnvironment::ETime(i));
			CHECK_EQUAL(REnvironment::ETime(i), REnvironment::StringToETime(strTime.c_str()));
		}

		for (int i = 0; i < REnvironment::WEATHER_NUM; i++)
		{
			string strWeather = REnvironment::EWeatherToString(REnvironment::EWeather(i));
			CHECK_EQUAL(REnvironment::EWeather(i), REnvironment::StringToEWeather(strWeather.c_str()));
		}

	}
}