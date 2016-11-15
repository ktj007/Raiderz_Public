#include "stdafx.h"
#include "XGlueGameFieldFunction.h"
#include "XPost_Channel.h"
#include "XMyPlayer.h"
#include "XModuleMyControl.h"
#include "XGame.h"

void XGlueGameFieldFunction::ReqChannelList()
{
	XPostChannel_GetChannelList();
}

void XGlueGameFieldFunction::ChangeChannel(int nChannelID)
{
	XPostChannel_ChangeChannelPrepare(nChannelID);
}

void XGlueGameFieldFunction::ChangeChannelCancel()
{
	gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionIdleWithSpecialState();

	XPostChannel_ChangeChannelCancel();
}

//void XGlueGameFieldFunction::ChangeWeather(int nWeather)
//{
//	if ((gg.currentgamestate) && (gg.currentgamestate->GetWorld()) && (gvar.Game.bLocalWeather == false))
//	{
//		gvar.Game.bLocalWeather = true;
//
//		gg.currentgamestate->GetWorld()->OnCmdSetEnv((GAME_TIME_TYPE)gvar.World.nGameTime, (GAME_WEATHER_TYPE)nWeather, true, GAME_ENVIRONMENT_FADING_TIME);
//	}
//}

//void XGlueGameFieldFunction::ChangeCurrentWeather()
//{
//	if ((gg.currentgamestate) && (gg.currentgamestate->GetWorld()) && (gvar.Game.bLocalWeather == true))
//	{
//		gg.currentgamestate->GetWorld()->OnCmdSetEnv("default", (GAME_TIME_TYPE)gvar.World.nGameTime, (GAME_WEATHER_TYPE)gvar.World.nWeather, true, GAME_ENVIRONMENT_FADING_TIME);
//
//		gvar.Game.bLocalWeather = false;
//	}
//}
//
//void XGlueGameFieldFunction::ChangeCustomWeather( const char* szEnvname, int nTime, int nWeather)
//{
//	if ((gg.currentgamestate) && (gg.currentgamestate->GetWorld()) && (gvar.Game.bLocalWeather == false))
//	{
//		gvar.Game.bLocalWeather = true;
//
//		gg.currentgamestate->GetWorld()->OnCmdSetEnv(szEnvname, (GAME_TIME_TYPE)gvar.World.nGameTime, (GAME_WEATHER_TYPE)nWeather, true, GAME_ENVIRONMENT_FADING_TIME);
//	}
//}