#pragma once

#include "CSWorldInfo.h"

class GEntityPlayer;
class GField;

class GClientFieldRouter
{
public:
	void PrepareChangeChannel(const GEntityPlayer* pTarget, int nResult);
	void ChangeField(MUID uidPlayer, GField* pField, vec3& vPos, vec3& vDir, int nIntroScene, GAME_WEATHER_TYPE nInitWeather, GAME_TIME_TYPE nInitTime);
	void ObjectLoading(MUID uidPlayer);
	void ChangeTimeWeather(MUID uidPlayer, GAME_TIME_TYPE nTimeType, GAME_WEATHER_TYPE nWeatherType);
	void StartGame(MUID uidPlayer);

	void CancelChangeField(MUID uidPlayer);

};
