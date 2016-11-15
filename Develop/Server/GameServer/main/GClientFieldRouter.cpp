#include "StdAfx.h"
#include "GClientFieldRouter.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GSharedField.h"
#include "GEnvManager.h"
#include "CTransData.h"


void GClientFieldRouter::PrepareChangeChannel(const GEntityPlayer* pTarget, int nResult)
{
	VALID(NULL != pTarget);

	MCommand* pNewCommand = MakeNewCommand(MC_FIELD_PREPARE_CHANGE_CHANNEL_RES
		, pTarget->GetUID()
		, 1
		, NEW_INT(nResult));
	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GClientFieldRouter::ChangeField(MUID uidPlayer, GField* pField, vec3& vPos, vec3& vDir, int nIntroScene)
{
	VALID(NULL != pField);

	MCommand* pNewCommand = MakeNewCommand(MC_FIELD_CHANGE_FIELD, uidPlayer, 
		5,
		NEW_INT(pField->GetID()),
		NEW_INT(GetChannelID(pField)),
		NEW_VEC(vPos),
		NEW_VEC(vDir),
		NEW_INT(nIntroScene)
		);
	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GClientFieldRouter::StartGame(MUID uidPlayer)
{
	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_START_GAME, uidPlayer, 0, NULL);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GClientFieldRouter::ChangeTimeWeather(MUID uidPlayer, GAME_TIME_TYPE nTimeType, GAME_WEATHER_TYPE nWeatherType)
{
	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_SET_TIME_WEATHER,										
										uidPlayer,
										2,
										NEW_CHAR(nTimeType),
										NEW_CHAR(nWeatherType)
										);

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GClientFieldRouter::CancelChangeField(MUID uidPlayer)
{
	MCommand* pNewCommand = gsys.pCommandCenter->NewCommand(MC_FIELD_CANCEL_CHANGE_FIELD, uidPlayer);	
	gsys.pCommandCenter->PostCommand(pNewCommand);
}
