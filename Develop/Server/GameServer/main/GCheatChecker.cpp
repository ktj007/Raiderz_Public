#include "StdAfx.h"
#include "GCheatChecker.h"
#include "GEntityPlayer.h"
#include "STypes.h"
#include "CSCharHelper.h"
#include "GGlobal.h"
#include "Gserver.h"
#include "GConst.h"

GCheatChecker::GCheatChecker(void)
{
	m_rgrTick.SetTickTime(1.0f);
	m_rgrTick.Start();
	m_bCheckable = true;
}

GCheatChecker::~GCheatChecker(void)
{
}

void GCheatChecker::Init( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	VALID(pPlayer->GetField());
	
	m_PositionChecker.Init(pPlayer->GetPos());
}

void GCheatChecker::CheckPosition( POSITION_CHECK_TYPE nType, GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	VALID(pPlayer->GetPlayerInfo());

	if (!GConst::CHEAT_CHECK_DEVELOPER_TOO &&
		(pPlayer->IsDeveloper() || pPlayer->IsGM()))
	{
		return; // 운영자와 개발자는 검사하지 않음
	}

	if (nType == PCT_WALK &&
		!m_bCheckable)
		return; // 일반 이동은 1초마다 체크

	m_bCheckable = false;
	m_rgrTick.Start();

	POSITION_CHEAT_TYPE nCheatSubType;
	CHEAT_TREAT_TYPE nCheatTreat;
	m_PositionChecker.Check(nType, pPlayer->GetPos(), pPlayer->GetMoveSpeed(), nCheatSubType, nCheatTreat);
	
	OnTreat(CT_POSITION, nCheatSubType, nCheatTreat, pPlayer);
}

void GCheatChecker::OnTreat( CHEAT_TYPE nCheatType, int nCheatSubType, CHEAT_TREAT_TYPE nTreat, GEntityPlayer* pPlayer )
{
	switch(nTreat)
	{
		case CHEATTREAT_TRY:
			break;

		case CHEATTREAT_WARNING:
			OnLog_Warning(nCheatType, nCheatSubType, pPlayer);
			break;

		case CHEATTREAT_DISCONNECT:
			OnLog_Disconnect(nCheatType, nCheatSubType, pPlayer);
			gsys.pServer->Disconnect(pPlayer->GetUID());
			gsys.pServer->GetServerInfo().nDisconnCountByCheat;
			break;
	}
}

void GCheatChecker::OnLog_Warning( CHEAT_TYPE nCheatType, int nCheatSubType, GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	CID cidPlayer = pPlayer->GetCID();
	vec3 vPos = pPlayer->GetPos();

	// TODO: DB에 기록
	mlog("cheat warning: type(%d/%d), player(%s[%u] %.1f %.1f %.1f), speed: %.2f\n", 
		nCheatType, nCheatSubType, MLocale::ConvUTF16ToAnsi(pPlayer->GetName()).c_str(), cidPlayer, vPos.x, vPos.y, vPos.z, pPlayer->GetWalkSpeed());
}

void GCheatChecker::OnLog_Disconnect( CHEAT_TYPE nCheatType, int nCheatSubType, GEntityPlayer* pPlayer )
{
	if (!GConst::ENABLE_CHEATER_DISCONNECT)
		return; // 접속종료 시키지 않음

	VALID(pPlayer);
	CID cidPlayer = pPlayer->GetCID();
	vec3 vPos = pPlayer->GetPos();

	// TODO: DB에 기록
	mlog("cheated disconnect: type(%d/%d), player(%s[%u] %.1f %.1f %.1f), speed: %.2f\n", 
		nCheatType, nCheatSubType, MLocale::ConvUTF16ToAnsi(pPlayer->GetName()).c_str(), cidPlayer, vPos.x, vPos.y, vPos.z, pPlayer->GetWalkSpeed());
}

void GCheatChecker::Update( float fDelta )
{
	if (m_rgrTick.IsReady(fDelta))
	{
		m_rgrTick.Stop();
		m_bCheckable = true;
	}
}

