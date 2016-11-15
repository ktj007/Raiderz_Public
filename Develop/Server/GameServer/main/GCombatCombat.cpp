#include "stdafx.h"
#include "GCombatCombat.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GJob_MoveToEntity.h"
#include "GJob_Talent.h"
#include "GMessageID.h"
#include "GJob_Broker.h"
#include "GConst.h"
#include "GColtMgr.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GAIProfiler.h"
#include "GJobMgr.h"
#include "GHateTable.h"
#include "GNPCStress.h"
#include "GFieldNPCSession.h"

/////////////////////////////////////////////////////////////////////////////////////////
// GCombatCombat ////////////////////////////////////////////////////////////////////////
GCombatCombat::GCombatCombat(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM) 
: GCombatState(pOwner, pFSM)
, m_pNPCInfo(NULL)
, m_bAlreadyCallAggro(false)
{
	m_pNPCInfo = static_cast<GEntityNPC*>(pOwner->GetOwner())->GetNPCInfo();
	m_ChaseStressRegulator.Stop();
}

GCombatCombat::~GCombatCombat()
{
	;
}

void GCombatCombat::OnRegisterTrainstionRule()
{
	AddTransitionRule(GMSG_AI_EVT_FLEE,				AI_COMBAT_FLEE);
	AddTransitionRule(GMSG_AI_EVT_START_VICTORY,	AI_COMBAT_VICTORY);
	AddTransitionRule(GMSG_AI_EVT_SESSION,			AI_BEHAVIOR_SESSION);
	AddTransitionRule(GMSG_AI_EVT_SESSION_PUSHJOB,	AI_BEHAVIOR_SESSION);
}

void GCombatCombat::Enter()
{
	m_uidOldTarget = MUID::Invalid();
	m_bAlreadyCallAggro = false;

	m_ChaseStressRegulator.SetTickTime(GConst::STRESS_CHASE_INTERVAL);
	m_ChaseStressRegulator.Start();

	// 현재 위치를 홈포인트로 저장
	mmlog("%s - %.1f %.1f\n", __FUNCTION__, GetNPC()->GetDir().x, GetNPC()->GetDir().y);
	GetNPC()->SetHomePoint(GetNPC()->GetPos(), GetNPC()->GetDir());
}

void GCombatCombat::Exit(int nNextStateID)
{
	GetNPC()->doGuardReleased();
}

GMessageResult GCombatCombat::Message(GMessage& msg)
{
	if (__super::Message(msg) == MR_TRUE)		return MR_TRUE;

	switch (msg.m_nID)
	{
	case GMSG_AI_EVT_ATTACK:
		m_ChaseStressRegulator.Reset(); // 때리면 추적 스트레스는 초기화
		break;
	}

	return MR_FALSE;
}

void GCombatCombat::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_COMBAT_COMBAT, L"CombatCombat");

	OnAggro();

	if (!GetAI()->GetOwner()->IsNPC())	return;

	_UpdateChase(fDelta);

	if(IsEndCombat())
	{
		// 탤런트를 다 사용할때까지, combatcombat 스테이트에서 다른 스테이트로 바뀌지 않는다.
		if (_IsRunnginTalentJob()) return;

		_EndCombat();
		return;
	}

	if (!GetNPC()->GetTargetActor())
	{
		TargetNewEnemy();
	}

	_UpdateCombat();

	if (CheckCombatRadius())
	{
		// 전투 범위 벗어남
		GetNPC()->SetHomePoint(GetNPC()->GetSpawnPos(), GetNPC()->GetDir()); // 스폰위치로 이동되도록 유도
		_EndCombatForced();
		return;
	}
}

bool GCombatCombat::CheckCombatRadius()
{
	vec3 vSpawnPos = GetNPC()->GetSpawnPos();
	vec3 vCurPos = GetNPC()->GetPos();
	float fCombatRadius = GetNPC()->GetCombatRadius();
	if (fCombatRadius == 0.0f)
		return false; // 설정되지 않음
	
	return (vCurPos.DistanceTo(vSpawnPos) > fCombatRadius);
}

void GCombatCombat::TargetNewEnemy()
{
	GEnemyFinder enemy_finder;
	MUID uidEnemy = enemy_finder.Find(GetNPC());

	EVENT_FOUND_ENEMY param;
	param.uidTarget = uidEnemy;
	GMessage msg(GMSG_AI_EVT_FIND_TARGET, &param);
	m_pOwner->Message(msg);
}

bool GCombatCombat::IsEndCombat()
{
	if (IsExistEnemy())
		return false; // 상대하고 있는 적이 있음

	if (GetNPC()->GetNPCInfo()->m_bAssistAfterCombat)
	{
		GEnemyFinder enemy_finder;
		if (enemy_finder.Find(GetNPC()).IsValid())
			return false; // 주변에 적이 있음
	}
	
	return true; // 전투 종료됨
}

bool GCombatCombat::_IsRunnginTalentJob()
{
	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	GJob* pJob = pJobMgr->GetCurrJob();

	if (NULL == pJob) return false;
	if (GJOB_TALENT != pJob->GetID() && GJOB_GUARD != pJob->GetID()) return false;

	return true;
}

void GCombatCombat::_EndCombat()
{
	GetNPC()->OnEndCombat();

	if (IsObtainVictory())
	{
		SendMsg_StartVictory();
	}
	else
	{
		SendMsg_EndCombat();
	}
}

void GCombatCombat::_EndCombatForced()
{
	GetNPC()->OnEndCombat();

	SendMsg_EndCombat();
}

void GCombatCombat::OnAggro()
{
	VALID(GetNPC()->GetNPCInfo());
	if (!GetNPC()->GetNPCInfo()->bCounterAttack)
		return; // 반격 안하는 상태

	if (!GetNPC()->IsNPC())	
		return;
	if (GetAI()->IsAssist())	
		return;
	if (m_bAlreadyCallAggro)
		return;

	m_bAlreadyCallAggro = true;

	GEntityActor* pTargetActor = GetNPC()->GetTargetActor();
	if (!pTargetActor)			return;

	// LuaCall -> NPC:OnAggro()
	GetScriptNPC()->OnAggro(GetNPC()->GetDelegator(), pTargetActor->GetDelegator());

	// Colt
	GetAI()->GetColt()->RunColtAggro(GetNPC(), pTargetActor);
}

void GCombatCombat::_UpdateChase(float fDelta)
{
	if ((!GetNPC()->IsNowUsingTalent() || GetNPC()->IsNowUsingRotateTalent()) && // 회전 탤런트가 아니일 때만 스트레스
		m_ChaseStressRegulator.IsReady(fDelta))
	{
		GetNPC()->GetNPCStress().AddStress(GConst::STRESS_ADD_LIMITED_CHASE_TIME);
	}
}

void GCombatCombat::OnTargetChanged(MUID uidOld, MUID uidNew)
{
	GetNPC()->RouteChangeCombatTarget(uidNew);
}

void GCombatCombat::_UpdateCombat()
{
	GEntityNPC* pThisNPC			= GetNPC();
	GEntityActor* pOpponentActor	= GetNPC()->GetTargetActor();

	VALID(pThisNPC->GetNPCInfo());

	if (!pThisNPC->GetNPCInfo()->bCounterAttack)
		return; // 반격 안하는 상태

	MUID uidCurTarget = pThisNPC->GetTarget();
	if (uidCurTarget != m_uidOldTarget)
	{
		OnTargetChanged(m_uidOldTarget, uidCurTarget);
		m_uidOldTarget = uidCurTarget;
	}

	GFieldNPCSession* pSession = 
		pThisNPC->GetSession();
	if (pSession)
	{
		if (!pSession->IsThinkable())
		{
			GMessage msg(GMSG_AI_EVT_SESSION);
			m_pOwner->Message(msg);
			return; // NPC 세션으로 인해 불가능한 상태
		}
	}

	if (pThisNPC->IsGuarding()) 	
		return;	// 방어중

	if (!pOpponentActor)
		return;	// 전투 상대가 없음

	if (IsJobRunning())				
		return; // 잡이 진행중

	if (pThisNPC->HasMotionFactor())
		return;	// 모션 팩터가 걸려있음


	GetAI()->GetColt()->RunColtCombat(pThisNPC, pOpponentActor);

	m_ChaseStressRegulator.Reset();
}
