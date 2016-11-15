#include "stdafx.h"
#include "GPlayerEnemyInfoSender.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GDuel.h"
#include "GDuelMgr.h"

GPlayerEnemyInfoSender::GPlayerEnemyInfoSender(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_nOldCheckSum(0)
, m_uidLastHittedEnemy(MUID::ZERO)
, m_nLastHitEnemy(UIID_INVALID)
{
	m_pOwner->AttachObserver(this);

	const float UPDATE_TICK = 1.0f;
	m_rgrTick.SetTickTime(UPDATE_TICK);
	m_rgrTick.Start();
}

GPlayerEnemyInfoSender::~GPlayerEnemyInfoSender()
{
	m_pOwner->DetachObserver(this);
}

void GPlayerEnemyInfoSender::Clear()
{
	m_uidLastHittedEnemy  = MUID::ZERO;
	m_nLastHitEnemy = UIID_INVALID;
	m_setEnemies.clear();
	m_nOldCheckSum = 0;
}

void GPlayerEnemyInfoSender::Update( float fDelta)
{	
	PFC_THISFUNC;
	if (!m_rgrTick.IsReady(fDelta)) 
		return;	

	int nNewCheckSum = CalcCheckSum();
	if (nNewCheckSum == m_nOldCheckSum)	
		return; // 정보의 변화가 없음

	UpdateEnemyEntry();
}

bool GPlayerEnemyInfoSender::HasEnemy() const
{
	if (m_pOwner->HasDuel())
		return true; // 결투중

	if (IsObserveDuel())
		return true; // 결투 관람중

	return !m_setEnemies.empty();
}

void GPlayerEnemyInfoSender::RouteInfo()
{
	vector<MUID> vecEnemies;
	for each (const MUID& each in m_setEnemies)
	{
		GEntityActor* pEachActor = FindActor(each);
		if (!pEachActor)
			continue;

		if (pEachActor->IsNPC())
		{
			if (ToEntityNPC(pEachActor)->IsNowPuppet())
				continue; // 조정 상태인 녀석은 보여주지 않음
		}
				
		vecEnemies.push_back(each);	
	}

	// 결투중 추가 처리
	if (m_pOwner->HasDuel())
	{
		GDuel* pDuel = m_pOwner->GetDuel();
		vector<MUID>& vecOpponentUIDs = 
			pDuel->GetOpponentTeamUIDList(pDuel->GetTeam(m_pOwner->GetUID()));

		for each (MUID uid in vecOpponentUIDs)
		{
			if (pDuel->IsLoser(uid))
				continue; // 이미 전투에서 탈락한 플레이어

			vecEnemies.push_back(uid);	
		}
	}
	
	// 관람중인 결투 처리
	if (IsObserveDuel())
	{
		VALID(m_pOwner->GetField());
		GDuel* pDuel = m_pOwner->GetField()->GetDuelMgr()->FindDuel(m_uidObserveDuel);
		if (!pDuel)
		{
			DetachObserveDuel();
			
			if (!HasEnemy())
			{
				RouteClearInfo();
				Clear();
				return;
			}
		}
		else
		{
			vecEnemies.insert(vecEnemies.end(), pDuel->GetAllFightersUIDList().begin(), pDuel->GetAllFightersUIDList().end());	
		}
		
	}

	vector<TD_ENEMY_INFO> vecTDEnemyInfo;
	for each (MUID each in vecEnemies)
	{
		TD_ENEMY_INFO tdEnemyInfo
			= MakeTD(m_pOwner->FindActor(each));

		vecTDEnemyInfo.push_back(tdEnemyInfo);
	}

	MCommand* pNewCommand = MakeNewCommand(MC_CHAR_ENEMY_INFO, 1, 
		NEW_BLOB(vecTDEnemyInfo));
	m_pOwner->RouteToMe(pNewCommand);
}

void GPlayerEnemyInfoSender::RouteClearInfo()
{
	MCommand* pNewCommand = gsys.pCommandCenter->NewCommand(MC_CHAR_ENEMY_INFO_CLEAR);
	m_pOwner->RouteToMe(pNewCommand);
}

GEntityActor* GPlayerEnemyInfoSender::FindActor( const MUID& id )
{
	GField* pField = m_pOwner->GetField();
	if (!pField)
		return NULL;
	
	return pField->FindActor(id);
}

int GPlayerEnemyInfoSender::CalcCheckSum()
{
	int nCheckSum = 0;
	for each (const MUID& each in m_setEnemies)
	{
		GEntityActor* pEachActor = FindActor(each);
		if (!pEachActor)
			continue;

		nCheckSum += each.Low;
		nCheckSum += pEachActor->GetHPPercent();
	}

	VALID_RET(m_pOwner->GetField(), 0);
	GDuel* pDuel = m_pOwner->GetField()->GetDuelMgr()->FindDuel(m_uidObserveDuel);
	if (pDuel)
	{
		for each (const MUID& each in pDuel->GetAllFightersUIDList())
		{
			GEntityActor* pEachActor = FindActor(each);
			if (!pEachActor)
				continue;

			nCheckSum += each.Low;
			nCheckSum += pEachActor->GetHPPercent();
		}
	}
	

	return nCheckSum;
}

void GPlayerEnemyInfoSender::UpdateEnemyEntry()
{
	// 유효하지 않은 적 제거
	for (set<MUID>::iterator it = m_setEnemies.begin();
		 it != m_setEnemies.end();)
	{
		GEntityActor* pActor = FindActor(*it);
		if (!pActor ||						// 찾을 수 없는 적이거나
			(pActor && pActor->IsDead()))	// 이미 죽은 적일 경우
		{
			it = m_setEnemies.erase(it);
		}
		else
		{
			++it;
		}
	}
		

	if (!HasEnemy())
	{
		RouteClearInfo();
		Clear();
		return;
	}

	m_nOldCheckSum = CalcCheckSum();
	RouteInfo();
}

TD_ENEMY_INFO GPlayerEnemyInfoSender::MakeTD( GEntityActor* pEachActor )
{
	VALID_RET(pEachActor, TD_ENEMY_INFO());

	TD_ENEMY_INFO tdEnemyInfo;
	tdEnemyInfo.nUIID = pEachActor->GetUIID();
	tdEnemyInfo.nMaxHP = pEachActor->GetMaxHP();
	tdEnemyInfo.nHPPercent = pEachActor->GetHPPercent();
	tdEnemyInfo.nLevel = (uint8)pEachActor->GetLevel();
	return tdEnemyInfo;
}

void GPlayerEnemyInfoSender::OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	if (!pAttacker->IsPlayer())
		return; // 플레이어가 아님

	m_setEnemies.insert(pAttacker->GetUID());
	UpdateEnemyEntry();
}

void GPlayerEnemyInfoSender::OnHitEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	if (!pTarget->IsPlayer())
		return; // 플레이어가 아님

	m_setEnemies.insert(pTarget->GetUID());
	UpdateEnemyEntry();
}

void GPlayerEnemyInfoSender::OnAddHateTarget( GEntityActor* pEnemy )
{
	m_setEnemies.insert(pEnemy->GetUID());
	UpdateEnemyEntry();
}

void GPlayerEnemyInfoSender::OnLostHateTarget( MUID uidEnemy )
{
	m_setEnemies.erase(uidEnemy);
	UpdateEnemyEntry();
}

void GPlayerEnemyInfoSender::OnCombatEnd(GEntityActor* pOwner)
{
	Clear();
	UpdateEnemyEntry();
}

void GPlayerEnemyInfoSender::OnDuelReady( const vector<MUID>& pEnemy )
{
	UpdateEnemyEntry();
}

void GPlayerEnemyInfoSender::OnDuelFinish( const vector<MUID>& pEnemy )
{
	// 결투중인 상태는 빼기
	for each (MUID uid in pEnemy)
	{
		m_setEnemies.erase(uid);
	}
	
	UpdateEnemyEntry();
}

bool GPlayerEnemyInfoSender::IsObserveDuel() const
{
	return m_uidObserveDuel.IsValid();
}

bool GPlayerEnemyInfoSender::AttachObserveDuel(MUID uidDuel)
{
	if (IsObserveDuel())
		return false; // 이미 관람 중

	VALID_RET(m_pOwner->GetField(), false);
	GDuel* pDuel = m_pOwner->GetField()->GetDuelMgr()->FindDuel(uidDuel);
	if (!pDuel)
		return false; // 존재하지 않는 파티

	if (!pDuel->IsObservable())
		return false; // 더 관람할 수 없는 상태

	pDuel->IncreaseObserveCount();
	m_uidObserveDuel = uidDuel;

	return true;
}

bool GPlayerEnemyInfoSender::DetachObserveDuel()
{
	if (!IsObserveDuel())
		return false; // 관람중이지 않음

	MUID uidDuel = m_uidObserveDuel;
	m_uidObserveDuel.SetInvalid();

	VALID_RET(m_pOwner->GetField(), false);
	GDuel* pDuel = m_pOwner->GetField()->GetDuelMgr()->FindDuel(uidDuel);
	if (!pDuel)
		return false; // 존재하지 않는 파티

	pDuel->DecreaseObserveCount();
	
	return true;
}
