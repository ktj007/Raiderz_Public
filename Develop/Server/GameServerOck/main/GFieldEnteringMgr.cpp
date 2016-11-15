#include "StdAfx.h"
#include "GFieldEnteringMgr.h"
#include "GPlayerObject.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GTDMaker.h"
#include "GPlayerBattleArena.h"
#include "GModuleBuff.h"
#include "GConst.h"
#include "GClientFieldRouter.h"
#include "GClientCharRouter.h"

const float GFieldEnteringMgr::TIME_UPDATE_SEC = 1.0f;
const int GFieldEnteringMgr::LIMIT_COUNT_ENTER_PLAYER_A_UPDATE = 50;

GFieldEnteringMgr::GFieldEnteringMgr()
{
	m_rgltUpdate.SetTickTime((float)TIME_UPDATE_SEC);
	m_rgltUpdate.Start();
}

GFieldEnteringMgr::~GFieldEnteringMgr()
{
}

void GFieldEnteringMgr::Update(float fDelta)
{
	if (!m_rgltUpdate.IsReady(fDelta)) return;
	if (m_qEnteringPlayer.empty()) return;

	for (int i = 0; i < LIMIT_COUNT_ENTER_PLAYER_A_UPDATE; ++i)
	{
		MUID uidPlayer = m_qEnteringPlayer.front();

		EnterPlayerWhoFieldLoadingComplete(uidPlayer);

		m_qEnteringPlayer.pop();
		if (m_qEnteringPlayer.empty()) break;
	}
}

void GFieldEnteringMgr::Add(MUID uidPlayer)
{
	m_qEnteringPlayer.push(uidPlayer);
}

void GFieldEnteringMgr::EnterPlayerWhoFieldLoadingComplete(MUID uidPlayer)
{
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPlayer)
		return;

	if (POS_LOGOUT == pPlayer->GetState())
		return;

	GEntityPlayer* pEntityPlayer = pPlayer->GetEntity();
	if (NULL == pEntityPlayer)
		return;

	pEntityPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();


	// 캐릭터의 시간 정보 전송
	GTDMaker tdMaker;
	vector<TD_BUFF_REMAIN_TIME> vecTDBuffRemainTime;
	tdMaker.MakeTD_BUFF_REMAIN_TIME(pEntityPlayer, vecTDBuffRemainTime);

	vector<TD_TALENT_REMAIN_COOLTIME> vecTDTalentRemainCoolTime;
	tdMaker.MakeTD_TALENT_REMAIN_COOLTIME(pEntityPlayer, vecTDTalentRemainCoolTime);

	float fAutoRebirthRemainTime = 0.0f;
	if (true == pEntityPlayer->IsDead())
	{
		bool bEnteredBattleArena = pEntityPlayer->GetPlayerBattleArena().IsEntered();
		if (bEnteredBattleArena)
		{
			fAutoRebirthRemainTime = GConst::ARENA_DEATHMATCH_REBIRTH_TIME;
		}
		else
		{
			fAutoRebirthRemainTime = pEntityPlayer->GetPlayerInfo()->CalcAutoRebirthRemainTime();
		}
	}

	GClientCharRouter routerChar;
	routerChar.CharMyTimeInfo(uidPlayer, vecTDBuffRemainTime, vecTDTalentRemainCoolTime, fAutoRebirthRemainTime);


	GClientFieldRouter routerField;

	// 필드 이동 중 날씨, 시간 변경 있을 수 있으니 적용한다.
	GField* pField = pEntityPlayer->GetField();
	if (pField != NULL)
	{
		GAME_TIME_TYPE nTimeType = pField->GetCurrentTime();
		GAME_WEATHER_TYPE nWeatherType = pField->GetCurrentWeather();		

		routerField.ChangeTimeWeather(uidPlayer, nTimeType, nWeatherType);
	}

	// 게임 시작
	routerField.StartGame(uidPlayer);

	// 필드 입장 무적 버프
	pEntityPlayer->GetModuleBuff()->GainInvincibleBuff_EnterField();
}
