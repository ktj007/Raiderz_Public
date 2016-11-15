#pragma once

#include "REnvironment.h"
#include "RWeatherEffect.h"

namespace rs3 {

class RS_API RWeatherEffectSet
{
private:
	RWeatherEffect* m_pWeatherEffectTable[REnvironment::WEATHER_NUM];
public:
	RWeatherEffectSet();
	~RWeatherEffectSet();
	void Release();

	bool LoadXML();
	void SaveXML();

	RWeatherEffect* GetWeatherEffect(REnvironment::EWeather eWeather)
	{
		return m_pWeatherEffectTable[eWeather];
	}
};
















} // namespace rs3