#include "StdAfx.h"
#include "GuardEffector.h"
#include "GTalentHelper.h"
#include "GCombatCalculator.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GModuleBuff.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GCalculator.h"
#include "GGuardCalculator.h"
#include "GTestSystem.h"

GuardEffector::Result GuardEffector::CheckGuard(GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)
{
	VALID_RET(pGuarder, GUARD_FAILED);
	VALID_RET(pAttacker, GUARD_FAILED);
	VALID_RET(pAttackTalentInfo, GUARD_FAILED);

	if (!pGuarder->IsGuarding())
		return GUARD_FAILED;	// 가드중이지 않음

	GTalentInfo* pGuardTalentInfo = GTalentHelper::FindTalentInfo(pGuarder->GetGuardTalentID(), pGuarder);
	if (!pGuardTalentInfo)
		return GUARD_FAILED;	// 가드 탤런트 존재하지 않음

	if (!pGuardTalentInfo->IsGuard())
		return GUARD_FAILED;	// 가드 탤런트가 아님

	const int nGuardCrashLevel = pAttackTalentInfo->m_nGuardCrashLevel;
	if (nGuardCrashLevel > GetGuardLevel(pGuarder, pAttacker, pAttackTalentInfo))
		return GUARD_FAILED;	// 가드 할 수 없는 공격

	if (!pGuarder->IsGuardableDirection(pAttacker))
		return GUARD_FAILED;	// 방어할 수 없는 방향

	return GUARD_SUCCESS;
}

GUARD_TYPE GuardEffector::GetGuardLevel( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID_RET(pGuarder, GT_INVALID);
	VALID_RET(pAttacker, GT_INVALID);
	VALID_RET(pAttackTalentInfo, GT_INVALID);

	GUARD_TYPE nGuardType = GT_INVALID;
	if (pGuarder->IsNPC())
	{
		GEntityNPC* pThisNPC = static_cast<GEntityNPC*>(pGuarder);
		VALID_RET(pThisNPC->GetNPCInfo(), GT_INVALID);
		nGuardType = pThisNPC->GetNPCInfo()->nDefaultGuardType;
	}
	else
	{
		if (pGuarder->IsEquipShield())	nGuardType = GT_PERFECT;
		else							nGuardType = GT_PARTIAL;
	}

	// 절대방어 가능버프가 있는지 여부
	if (IsAbsoluteGuard(pGuarder))
	{
		nGuardType = GT_ABSOLUTE;
	}

	// 방어 세부 조건
	switch(nGuardType)
	{
	case GT_PARTIAL:
		if (pAttackTalentInfo->m_bPerfectGuardOnly)
			return GT_INVALID;	// 완전방어가 아니면 실패로 처리

		return GT_PARTIAL;	// 부분방어는 처리완료

	case GT_PERFECT:
		if (pAttackTalentInfo->m_bPartialGuardHit)
			return GT_PARTIAL;	// 완전방어 성공하더라도 부분방어로 처리

		if (!CheckPerfectGuard(pGuarder, pAttacker))
			return GT_PARTIAL;	// 완전방어가 실패되어 부분방어 처리

		return GT_PERFECT;

	case GT_ABSOLUTE:
		return GT_ABSOLUTE;
	}

	return GT_INVALID;
}

bool GuardEffector::IsTryGuard( GEntityActor* pGuarder )
{
	VALID_RET(pGuarder, false);

	return pGuarder->IsGuarding();
}

void GuardEffector::ApplyEffect_Guard( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, GUARD_TYPE nGuardType, uint16 nCheckTime, int nDamage)
{
	VALID(pGuarder);
	VALID(pAttacker);
	VALID(pAttackTalentInfo);

	pGuarder->RouteActionCancelNextAttack(pGuarder->GetUIID(), CANCEL_NEXT_ATTACK_PARAM_ALL);

	int nGuardedDamage = CalcGuardedDamage(pGuarder, pAttacker, nGuardType, nDamage);
	RouteGuardDefense(pGuarder, nGuardType, pAttacker->GetUIID(), pAttackTalentInfo->m_nID, nCheckTime, nGuardedDamage);
}

void GuardEffector::ApplyEffect_PerfectGuard( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, int nDamage )
{
	ApplyEffect_SpecificGuardImpl(pGuarder, pAttacker, pAttackTalentInfo, GT_PERFECT, nDamage);
}

void GuardEffector::ApplyEffect_PartialGuard( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, int nDamage )
{
	ApplyEffect_SpecificGuardImpl(pGuarder, pAttacker, pAttackTalentInfo, GT_PARTIAL, nDamage);
}

void GuardEffector::ApplyEffect_SpecificGuardImpl( GEntityActor* pGuarder, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo, GUARD_TYPE nGuardType, int nDamage )
{
	VALID(pGuarder);
	VALID(pAttacker);
	VALID(pAttackTalentInfo);

	float fReduce = CalcReducedDamage(pGuarder, pAttacker, nGuardType);
	gsys.pTestSystem->ShowGuardFormula(pGuarder, nDamage, fReduce, nGuardType);

	nDamage = CalcGuardedDamage(pGuarder, pAttacker, nGuardType, nDamage);
	pGuarder->doDamage(pAttacker->GetUID(), pAttackTalentInfo->m_nDamageAttrib, nDamage);

	// 죽음 처리
	if (pGuarder->IsDie()) 
	{
		pGuarder->SetKiller(pAttacker->GetUID());
		if (pGuarder->IsPlayer())
		{
			ToEntityPlayer(pGuarder)->SetKillTalent(pAttackTalentInfo);
		}
		pGuarder->doDie();
	}
}


bool GuardEffector::CheckPerfectGuard(GEntityActor* pGuarder, GEntityActor* pAttacker)
{
	GPercentDice dice;
	return dice.Dice(gsys.pCombatCalculator->CalcPerfectGuardRate(pAttacker, pGuarder));
}

int GuardEffector::CalcGuardedDamage(GEntityActor* pGuarder, GEntityActor* pAttacker, GUARD_TYPE nGuardType, int nDamage)
{	
	return (int)(nDamage * CalcReducedDamage(pGuarder, pAttacker, nGuardType));
}

float GuardEffector::CalcReducedDamage(GEntityActor* pGuarder, GEntityActor* pAttacker, GUARD_TYPE nGuardType)
{
	int nLF = GCalculator::CalcLevelFactor(pGuarder->GetLevel(), pAttacker->GetLevel());

	switch (nGuardType)
	{
	case GT_PARTIAL:
		return GGuardCalculator::CalcPartialGuardDamageMod(pGuarder, pAttacker);
	case GT_PERFECT:
		return 0.0f;
	case GT_ABSOLUTE:
		return 0.0f;
	}

	_ASSERT(FALSE && L"처리되지 않은 방어타입");
	return 0.0f;
}


void GuardEffector::RouteGuardDefense(GEntityActor* pGuarder, GUARD_TYPE nType, UIID nAttackerUIID, int nTalentID, uint32 nHitTime, int nGuardedDamage)
{
	VALID(pGuarder);

	if (nType == GT_PARTIAL)
	{
		TD_TALENT_PARTIAL_GUARD td_talent_guard;
		td_talent_guard.nType = nType;
		td_talent_guard.nAttackerUIID = nAttackerUIID;
		td_talent_guard.nGuardUIID = pGuarder->GetUIID();
		td_talent_guard.nTalentID = nTalentID;
		td_talent_guard.nHitTime = nHitTime;
		td_talent_guard.nGuardedDamage = nGuardedDamage;	
		td_talent_guard.vGuarderDir = pGuarder->GetDir();

		MCommand* pNewCommand = MakeNewCommand(MC_ACTION_GUARD_DEFENSE_PARTIAL, 1, NEW_SINGLE_BLOB( &td_talent_guard, sizeof( TD_TALENT_PARTIAL_GUARD)));
		pGuarder->RouteToThisCell(pNewCommand);
	}
	else
	{
		TD_TALENT_GUARD td_talent_guard;
		td_talent_guard.nType = nType;
		td_talent_guard.nAttackerUIID = nAttackerUIID;
		td_talent_guard.nGuardUIID = pGuarder->GetUIID();
		td_talent_guard.nTalentID = nTalentID;
		td_talent_guard.nHitTime = nHitTime;

		MCommand* pNewCommand = MakeNewCommand(MC_ACTION_GUARD_DEFENSE, 1, NEW_SINGLE_BLOB( &td_talent_guard, sizeof( TD_TALENT_GUARD)));
		pGuarder->RouteToThisCell(pNewCommand);
	}
}

bool GuardEffector::IsAbsoluteGuard( GEntityActor* pGuarder )
{
	if (!pGuarder->GetModuleBuff()->IsNowAbsoluteGuard())
		return false; // 절대방어 버프가 없음

	if (pGuarder->IsPlayer())
	{
		if (!pGuarder->IsEquipShield())
			return false; // 절대방어는 방패가 필요함

		return true;
	}
	else if (pGuarder->IsNPC())
	{
		GEntityNPC* pThisNPC = static_cast<GEntityNPC*>(pGuarder);
		VALID_RET(pThisNPC->GetNPCInfo(), false);
		if (GT_PERFECT == pThisNPC->GetNPCInfo()->nDefaultGuardType)
			return true; // 완벽 방어일 때만 절대 방어 가능

		return false;
	}
	
	return false;
}
