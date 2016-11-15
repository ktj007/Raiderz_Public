#include "stdafx.h"
#include "GTalentRouter.h"
#include "GTalentInfo.h"
#include "GEntityActor.h"
#include "GTalent.h"
#include "GCommandTable.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GGlobal.h"

void GTalentRouter::RouteUseTalent(GTalentInfo* pTalentInfo, vec3 vPos, vec3 vDir)
{
	// 기본 공격은 따로 패킷을 보낸다.
	if (CSTalentInfoHelper::IsPlayerNormalTalentID(pTalentInfo->m_nID)) return;

	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_USE_TALENT, 4, 
		NEW_USHORT(pOwner->GetUIID()), 
		NEW_INT(pTalentInfo->m_nID), 
		NEW_SVEC(vDir),
		NEW_VEC(vPos));
	pOwner->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteCancelTalent(GTalentInfo* pTalentInfo)
{
	// 기본 공격은 따로 패킷을 보낸다.
	if (CSTalentInfoHelper::IsPlayerNormalTalentID(pTalentInfo->m_nID)) return;

	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_CANCEL_TALENT, 2, NEW_USHORT(pOwner->GetUIID()), NEW_INT(pTalentInfo->m_nID));
	pOwner->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteActTalent( GTalentInfo* pTalentInfo, vec3 vDir )
{
	// 기본 공격은 따로 패킷을 보낸다.
	if (CSTalentInfoHelper::IsPlayerNormalTalentID(pTalentInfo->m_nID)) return;

	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ACT_TALENT, 3, 
		NEW_USHORT(pOwner->GetUIID()), 
		NEW_INT(pTalentInfo->m_nID), 
		NEW_SVEC(vDir)
		);
	pOwner->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteActTalentWithGround( vec3 vGroundPos, GTalentInfo* pTalentInfo, vec3 vDir )
{
	// 기본 공격은 따로 패킷을 보낸다.
	if (CSTalentInfoHelper::IsPlayerNormalTalentID(pTalentInfo->m_nID)) return;

	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ACT_TALENT_WITH_GROUND, 4, 
		NEW_USHORT(pOwner->GetUIID()), 
		NEW_INT(pTalentInfo->m_nID), 
		NEW_SVEC(vDir),
		NEW_VEC(vGroundPos)
		);
	pOwner->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteAttackArchery(  )
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ATTACK_ARCHERY, 5, 
		NEW_UID(m_pTalent->GetOwner()->GetUID()), 
		NEW_INT(m_pTalent->GetInfo()->m_nID), 
		NEW_VEC(m_pTalent->GetOwner()->GetPos()),
		NEW_VEC(m_pTalent->GetOwner()->GetFacingDir()),
		NEW_UID(m_pTalent->GetTargetUID())
		);

	m_pTalent->GetOwner()->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteAttackArcheryDetail()
{
	if (m_pTalent->GetTarget().IsInvalid())	return;

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ATTACK_ARCHERY_DETAIL, 7, 
		NEW_UID(m_pTalent->GetOwner()->GetUID()), 
		NEW_INT(m_pTalent->GetInfo()->m_nID), 
		NEW_VEC(m_pTalent->GetOwner()->GetPos()),
		NEW_VEC(m_pTalent->GetOwner()->GetFacingDir()),
		NEW_UID(m_pTalent->GetTarget().uidTarget),
		NEW_CHAR(m_pTalent->GetTarget().nCapsuleGroupIndex),
		NEW_CHAR(m_pTalent->GetTarget().nCapsuleIndex)
		);

	m_pTalent->GetOwner()->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteActRecastingSpell( int nSpellPhase, int nRet )
{
	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_RECASTING_SPELL, pOwner->GetUID(), 3, NEW_USHORT(pOwner->GetUIID()), NEW_INT(nRet), NEW_INT(nSpellPhase));
	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GTalentRouter::RouteActTalentProjectileMissile(int nTalentID, 
													vec3 vTargetPos, 
													MUID uidTarget, 
													int8 nCapsuleGroupIndex, 
													int8 nCapsuleIndex )
{
	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ACT_TALENT_PROJECTILE, 7, 
		NEW_UID(pOwner->GetUID()), 
		NEW_INT(nTalentID), 
		NEW_VEC(pOwner->GetFacingDir()), 
		NEW_VEC(vTargetPos),
		NEW_UID(uidTarget),
		NEW_CHAR(nCapsuleGroupIndex),
		NEW_CHAR(nCapsuleIndex));

	pOwner->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteActSpellMagicArea( int nTalentID, vec3 vTargetPos )
{
	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ACT_SPELL_MAGICAREA, 3, 
		NEW_USHORT(pOwner->GetUIID()), 
		NEW_INT(nTalentID), 
		NEW_VEC(vTargetPos));

	pOwner->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteExtraActTalent( GTalentInfo* pTalentInfo )
{
	// 기본 공격은 따로 패킷을 보낸다.
	if (CSTalentInfoHelper::IsPlayerNormalTalentID(pTalentInfo->m_nID)) return;

	GEntityActor* pOwner = m_pTalent->GetOwner();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_EXTRA_ACT_TALENT, 4, 
								NEW_USHORT(pOwner->GetUIID()), 
								NEW_INT(pTalentInfo->m_nID), 
								NEW_VEC(pOwner->GetPos()), 
								NEW_SVEC(pOwner->GetDir()));
	pOwner->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteTalentHeal( GEntityActor* pUser, GEntityActor* pTarget, int nTalentID, int nHealAmount )
{
	VALID(pUser);
	VALID(pTarget);
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_TALENT_HEAL, 4, 
		NEW_USHORT(pUser->GetUIID()), 
		NEW_USHORT(pTarget->GetUIID()), 
		NEW_INT(nTalentID),
		NEW_INT(nHealAmount)
		);
	pTarget->RouteToThisCell(pNewCommand);
}

void GTalentRouter::RouteBuffHeal( GEntityActor* pUser, GEntityActor* pTarget, int nBuffID, int nHealAmount )
{
	VALID(pTarget);
	MCommand* pNewCommand = MakeNewCommand(MC_BUFF_HEAL, 4, 
		NEW_USHORT(pUser?pUser->GetUIID():UIID_INVALID), 
		NEW_USHORT(pTarget->GetUIID()), 
		NEW_INT(nBuffID),
		NEW_INT(nHealAmount)
		);
	pTarget->RouteToThisCell(pNewCommand);
}