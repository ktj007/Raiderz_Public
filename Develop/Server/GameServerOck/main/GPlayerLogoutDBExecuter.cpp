#include "StdAfx.h"
#include "GPlayerLogoutDBExecuter.h"
#include "GPlayerObject.h"
#include "GEntityPlayer.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GActorCooltimeChecker.h"
#include "GDBCacheSystem.h"
#include "GPlayerDBCache.h"
#include "GDBCacheFlush.h"
#include "GGMExpoHandler.h"
#include "GConfig.h"



GPlayerLogoutDBExecuter::GPlayerLogoutDBExecuter(GPlayerObject* pPlayerObject)
: m_state(FlushDBChachState)
, m_pPlayerObject(pPlayerObject)
{
	if (pPlayerObject == NULL)
	{
		SetState(FinalState);
		return;
	}

	m_pEntityPlayer = m_pPlayerObject->GetEntity();
	if (m_pEntityPlayer == NULL)
	{
		SetState(FinalState);
		return;
	}

	// Expo Mode 이거나 Expo 용 계정일 경우, MasterServe 에서 플레이어를 지우고 DB 작업을 하지 않는다.
	GGMExpoHandler expoHandler;
	if (GConfig::IsExpoMode() ||
		expoHandler.IsExpoAccount(pPlayerObject->GetAccountInfo().szUserID)||
		expoHandler.IsExpoPublicBoothAccount(pPlayerObject->GetAccountInfo().szUserID))
	{
		SetState(FinalState);
		return;
	}
}

GPlayerLogoutDBExecuter::~GPlayerLogoutDBExecuter()
{
}

void GPlayerLogoutDBExecuter::Update()
{
	switch(GetState())
	{
	case FlushDBChachState:			if (OnFlushDBChachState() == false) break;
	case LogoutDBTaskState:			if (OnLogoutDBTaskState() == false) break;
	case WaitingLogoutDBTaskState:	if (OnWaitingLogoutDBTaskState() == false) break;
	}
}

bool GPlayerLogoutDBExecuter::OnFlushDBChachState()
{
	if (GetState() != FlushDBChachState)
		return true;

	if (!FlushCache())
		return false;

	GEntityPlayerDBAsyncMonitor& DBAsyncMonitor = m_pEntityPlayer->GetDBAsyncMonitor();

	if (0 != DBAsyncMonitor.GetRefCount())
		return false;

	gsys.pDBCacheSystem->DeletePlayerCache(m_pEntityPlayer->GetUID());

	SetState(LogoutDBTaskState);
	return true;
}

bool GPlayerLogoutDBExecuter::OnLogoutDBTaskState()
{
	if (GetState() != LogoutDBTaskState)
		return true;

	// 현재 상태 저장
	vector<pair<int, float>> vecTalentCoolTime;
	m_pEntityPlayer->GetActorCooltimeChecker().GetTalentRemainCoolTime(vecTalentCoolTime);

	vector<REMAIN_BUFF_TIME> vecBuffRemainTime;
	m_pEntityPlayer->GetBuffRemainTime(vecBuffRemainTime, true, true);

	gsys.pDBManager->Logout(m_pEntityPlayer->GetAID(), m_pEntityPlayer->GetCID(), m_pEntityPlayer, vecBuffRemainTime, vecTalentCoolTime);

	// 로그아웃 DB 처리 대기 상태로 변경
	SetState(WaitingLogoutDBTaskState);
	return true;
}

bool GPlayerLogoutDBExecuter::OnWaitingLogoutDBTaskState()
{
	if (GetState() != WaitingLogoutDBTaskState)
		return true;

	GEntityPlayerDBAsyncMonitor& DBAsyncMonitor = m_pEntityPlayer->GetDBAsyncMonitor();

	if (0 != DBAsyncMonitor.GetRefCount())
		return false;	

	SetState(FinalState);
	return true;
}


bool GPlayerLogoutDBExecuter::FlushCache()
{
	GPlayerDBCache* pc = gsys.pDBCacheSystem->FindPlayerCache(m_pPlayerObject->GetUID());
	if (NULL == pc)
		return true;

	if (pc->IsFlushing())
		return false;

	GDBCacheFlush poster(gmgr.pPlayerDBCacheMgr->Find(m_pPlayerObject->GetUID()));
	if (!poster.Post())
	{
		SetState(FinalState);
		return false;
	}

	return true;
}