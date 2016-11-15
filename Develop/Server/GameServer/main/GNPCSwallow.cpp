#include "StdAfx.h"
#include "GNPCSwallow.h"
#include "GDef.h"
#include "GEntityNPC.h"
#include "GModuleBuff.h"
#include "GBuff.h"
#include "GField.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GScriptGroup_NPC.h"

GNPCSwallow::GNPCSwallow(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GNPCSwallow::~GNPCSwallow(void)
{
	m_pOwner->DetachObserver(this);
}

bool GNPCSwallow::IsSwallowable()
{
	return m_uidTarget.IsInvalid();	// 먹지 않았으면 먹을 수 있음
}


bool GNPCSwallow::IsSwallowing()
{
	return !IsSwallowable();	// 먹을 수 없으면 먹고 있는 상태
}

bool GNPCSwallow::Swallow( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	VALID_RET(pTarget, false);
	VALID_RET(m_pOwner, false);
	GNPCInfo* pNPCInfo = m_pOwner->GetNPCInfo();
	VALID_RET(pNPCInfo, false);
	VALID_RET(pNPCInfo->nEatOwnerBuffID != 0, false);
	VALID_RET(pNPCInfo->nEatTargetBuffID != 0, false);
	VALID_RET(pNPCInfo->fEatPeriod > 0.0f, false);

	if (!IsSwallowable())
		return false;	// 먹을 수 없는 상태

	// 먹기 효과 얻기
	m_uidTarget = pTarget->GetUID();
	GainBuff(pTarget, pNPCInfo, pTalentInfo);

	// 콜백 호출
	GetScriptNPC()->OnSwalloweBegin(m_pOwner->GetDelegator(), pTarget->GetDelegator());

	return true;
}

bool GNPCSwallow::Spewup(bool bInterrupt)
{
	if (IsSwallowable())
		return false;	// 먹지 않았음

	GField* pField = m_pOwner->GetField();
	VALID_RET(pField, false);
	GEntityActor* pTargetActor = pField->FindActor(m_uidTarget);
	if (pTargetActor)
	{
		pTargetActor->ReleaseMotionFactor();

		LostBuff(pTargetActor);
	}
	
	m_uidTarget.SetInvalid();

	RouteSpewup(bInterrupt);
	
	return true;
}

void GNPCSwallow::GainBuff(GEntityActor* pTarget, GNPCInfo* pNPCInfo, GTalentInfo* pTalentInfo )
{
	GainBuffImpl(m_pOwner, pTalentInfo, pNPCInfo->nEatOwnerBuffID);
	GainBuffImpl(pTarget, pTalentInfo, pNPCInfo->nEatTargetBuffID);
}

void GNPCSwallow::GainBuffImpl(GEntityActor* pTarget, GTalentInfo* pTalentInfo, int nBuffID )
{
	if (pTarget->HasBuff(nBuffID))
		return;

	pTarget->GainBuff(nBuffID, pTalentInfo, m_pOwner);
}

void GNPCSwallow::LostBuff(GEntityActor* pTargetActor)
{
	VALID(pTargetActor);
	GNPCInfo* pNPCInfo = m_pOwner->GetNPCInfo();
	VALID(pNPCInfo);
	GField* pField = m_pOwner->GetField();
	VALID(pField);
	pTargetActor->LostBuff(pNPCInfo->nEatTargetBuffID);
	m_pOwner->LostBuff(pNPCInfo->nEatOwnerBuffID);
}

void GNPCSwallow::RouteSpewup(bool bInterrupt)
{
	VALID(m_pOwner);

	MCommand* pNewCmd = MakeNewCommand(bInterrupt?MC_NPC_SPEWUP_INTERRUPT:MC_NPC_SPEWUP, 1,
		NEW_UID(m_pOwner->GetUID()));
	m_pOwner->RouteToThisCell(pNewCmd);
}

void GNPCSwallow::OnDie()
{
	Spewup(true);
}