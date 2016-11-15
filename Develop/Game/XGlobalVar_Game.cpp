#include "stdafx.h"
#include "XGlobalVar_Game.h"
#include "XInteractionInfo.h"

GlobalVar_Game::GlobalVar_Game() : m_pInteractionInfo(new XInteractionInfo())
{
	pMyPlayer						= NULL;

	Init();

	bGameStart						= false;

	bDisconnectByDuplicatedLogin	= false;
	strLastErrorMsg.clear();
}

GlobalVar_Game::~GlobalVar_Game()
{
	SAFE_DELETE(m_pInteractionInfo);
}

void GlobalVar_Game::Init()
{
	nLocalTime				= TIME_INVALID;
	nLocalWeather			= WEATHER_INVALID;

	bTradeReq				= false;
	bPartyReq				= false;
	bGuildReq				= false;
	bDuelReq				= false;

	EnemyInfo.Clear();
	vecNPCIcon.clear();
}

void GlobalVar_Game::Init_ForChangeField()
{
	Init();

	m_pInteractionInfo->Init_ForChangeField();
}

bool GlobalVar_Game::IsCombatingNow()
{
	if(EnemyInfo.GetEnemyCount() > 0) return true;

	return false;
}
