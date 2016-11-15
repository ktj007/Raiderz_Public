#include "stdafx.h"
#include "GTalentEffector.h"
#include "GTalentInfo.h"
#include "GModuleAI.h"	// for 어그로
#include "GEntityPlayer.h"
#include "CCommandTable.h"	// for RoutePacket
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GGlueNPC.h"
#include "GEntityNPC.h"
#include "GNPCSwallow.h"
#include "GNPCLoot.h"
#include "GTalentHitRouter.h"
#include "GActorHateTracker.h"
#include "GDeadMotionFactorImpactor.h"
#include "GItemDurabilityUpdater.h"
#include "GBuffInfo.h"
#include "GModuleBuff.h"
#include "GTalentRouter.h"

#include "GEntityActor.h"
#include "GConst.h"

void GTalentEffector::ApplyTalent(Info& info)
{
	VALID(info.pUser);
	VALID(info.pTalentInfo);	
	info.pField = info.pUser->GetField();

	{
		if (GConst::RUN_FOR_TEST &&
			!info.pField)
		{
			if (info.pUser->IsActor())
			{
				mlog3("effector: m_pField is null. (is_npc: %d, name = %s, uid = %I64d)\n", info.pUser->IsNPC(), MLocale::ConvUTF16ToAnsi(ToEntityActor(info.pUser)->GetName()).c_str(), info.pUser->GetUID())	;
			}

		}
	}

	VALID(info.pField);	



	GTalentInfo* pModifiedTalentInfo = ModifyBackHitTalentInfo(info.pTalentInfo, info.pTarget);
	bool bTalentInfoModified = pModifiedTalentInfo != info.pTalentInfo;

	// 탤런트를 사용할때만 즉시 효과를 얻는다.
	if (info.nTalentCondition == pModifiedTalentInfo->m_nTiming)
	{
		PFI_B(9016, "GTalentEffector::ApplyTalent-GEffectTargetSelector::Select");
		GEffectTargetSelector::Desc desc(pModifiedTalentInfo->m_EffectInfo);
		desc.pSelf = info.pUser;
		desc.pTarget = info.pTarget;
		desc.vCenterPoint = info.vGroundPoint;
		set<GEntityActor*> qEffecteeList;
		m_EffectTargetSelector.Select(desc, qEffecteeList);
		PFI_E(9016);

		for each (GEntityActor* pActor in qEffecteeList)
		{
			PFI_B(9017, "GTalentEffector::ApplyTalent-GTalentEffector::GainTalentEffect");
			GainTalentEffect(info.pUser, pModifiedTalentInfo, pActor, info.bIgnoreMF, bTalentInfoModified);
			PFI_E(9017);
		}	
	}

	PFI_B(9018, "GTalentEffector::ApplyTalent-GTalentEffector::GainTalentBuff");
	GainTalentBuff(info, pModifiedTalentInfo);
	PFI_E(9018);
}

bool GTalentEffector::GainTalentEffect(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pTarget, bool bIgnoreMF, bool bTalentInfoModified)
{
	VALID_RET(pUser, false);
	VALID_RET(pTarget, false);	

	if (!pTalentInfo->HasExtraActiveEvent())
	{
		// ExtraActive Event가 없을 때만 즉시 효과를 얻음		
		m_ExtraActiveTalentRunner.Apply(pUser, vec3::ZERO, pTalentInfo, pTarget);
	}
	
	if (pTarget->IsDead())
		return false;	// 죽은 목표에게는 효과 없음

	bool bRet = true;
	if (IsNeedEffect(pTalentInfo))
	{
		bRet = 
			GainDamageAndHeal(  pUser, 
								pTarget, 
								pTalentInfo, 
								GDamageRangedInfo(pTalentInfo->m_nMaxDamage, pTalentInfo->m_nMinDamage), 
								GHealRangedInfo(pTalentInfo->m_nMaxHeal, pTalentInfo->m_nMinHeal), 
								bIgnoreMF, 
								bTalentInfoModified);
	}
	
	// 모디파이어
	if (pTalentInfo->IsPassiveSkill())
	{
		m_ModEffector.AddActorModifier(pTarget->GetChrStatus()->ActorModifier, pTalentInfo->m_ActorModifier);

		if (pTarget->IsPlayer())
		{
			GEntityPlayer* pTargetPlayer = ToEntityPlayer(pTarget);
			m_ModEffector.AddPlayerModifier(pTargetPlayer->GetPlayerModifier(), pTalentInfo->m_PlayerModifier);
		}

		m_ModEffector.RefreshStatus(pTarget);
	}
	else
	{
		int nBeforeHP = pTarget->GetHP();
		m_ModEffector.ApplyInstantModifier(pTarget, pTalentInfo->m_InstantModifier);
		if (IsInstantEffect(pTalentInfo))
		{
			RouteGainInstantEffect(pTarget, pTalentInfo);
		}
		
		int nHealHPAmount = pTarget->GetHP() - nBeforeHP;
		if (nHealHPAmount > 0)
		{
			OnHealed(pTarget, pTalentInfo, nHealHPAmount);
		}
	}	

	// 사자후 특화 처리
	if (pTalentInfo->m_strSpecialization == TALENT_PROPERTY_SAZAHU &&
		pUser->IsPlayer())
	{
		int nSazahuTalentID = static_cast<GEntityPlayer*>(pUser)->GetPassiveValue(TEPT_TALENT_SAZAHU_SPECIALIZATION);
		GTalentInfo* pSazahuTalent = gmgr.pTalentInfoMgr->Find(nSazahuTalentID);
		if (pSazahuTalent)
			m_ExtraActiveTalentRunner.Apply(pUser, vec3::ZERO, pSazahuTalent, pTarget);
	}

	return bRet;
}

GTalentInfo* GTalentEffector::ModifyBackHitTalentInfo(GTalentInfo* pTalentInfo, GEntityActor* pTarget)
{
	if (NULL == pTalentInfo) return pTalentInfo;
	if (NULL == pTarget) return pTalentInfo;

	if (0 == pTalentInfo->m_nBackHitTalent) return pTalentInfo;

	GHitInfo hitInfo = pTarget->GetLastHitInfo();	
	GTalentInfo* pBackHItTalentInfo = gmgr.pTalentInfoMgr->Find(pTalentInfo->m_nBackHitTalent);	

	if (pTalentInfo != hitInfo.pTalentInfo || false == hitInfo.bBackHit || NULL == pBackHItTalentInfo)
	{
		return pTalentInfo;
	}

	return pBackHItTalentInfo;	
}

void GTalentEffector::LostTalentEffect(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pTarget)
{
	VALID(pUser);
	VALID(pTarget);

	// 모디파이어
	if (pTalentInfo->IsPassiveSkill())
	{
		m_ModEffector.EraseActorModifier(pTarget->GetChrStatus()->ActorModifier, pTalentInfo->m_ActorModifier);

		if (pTarget->IsPlayer())
		{
			GEntityPlayer* pPlayer = ToEntityPlayer(pTarget);
			m_ModEffector.ErasePlayerModifier(pPlayer->GetPlayerModifier(), pTalentInfo->m_PlayerModifier);
		}

		m_ModEffector.RefreshStatus(pTarget);
	}
}

bool GTalentEffector::GainDamageAndHeal(GEntityActor* pUser, GEntityActor* pTarget, GTalentInfo* pTalentInfo, GDamageRangedInfo infoDamage, GHealRangedInfo infoHeal, bool bIgnoreMF, bool bNeedDamageRecalc, float fDamageApplyRate)
{
	PFI_BLOCK_THISFUNC(9022);

	VALID_RET(pTarget, false);

	PFI_B(9028, "GEntityActor::IsHittable");
	if (!pTarget->IsHittable(pUser, pTalentInfo)) 	
		return false;
	PFI_E(9028);

	// 면역 처리
	PFI_B(9029, "GEntityActor::HandleImmune");
	if (pTarget->HandleImmune(pUser, pTalentInfo))	
		return false;
	PFI_E(9029);

	GHitInfo infoHit = pTarget->GetLastHitInfo();
	DAMAGE_ATTRIB nDamageAttrib = DA_NONE;
	MOTION_FACTOR_GROUP mf = pTalentInfo->m_MotionFactorGroup;

	// 상태보정 모션팩터 적용
	float fStateMotionFactor = 1.0f;
	nDamageAttrib = pUser->GetDamageType(pTalentInfo);
	infoHit.nDamageType = nDamageAttrib;

	// 기존에 히트된 기록이 없거나 데미지 재계산이 필요한 경우
	if (false == infoHit.IsValidInfo() || true == bNeedDamageRecalc)
	{
		PFI_B(9030, "GActorDamageCalculator::CalcDamage");
		GActorDamageCalculator::Desc desc;
		desc.pAttacker = pUser;
		desc.pVictim = pTarget;
		desc.pTalentInfo = pTalentInfo;
		desc.DamageInfo = infoDamage;
		desc.HealInfo = infoHeal;
		desc.nHitCapGroup = infoHit.nCapsuleGroup;
		desc.nHitCapIndex = infoHit.nCapsuleIndex;
		GCombatTurnResult combat_result = 
			m_DamageCalculator.CalcDamage(desc, &fStateMotionFactor);
		PFI_E(9030);

		infoHit.nDamage = combat_result.nDamage;
		infoHit.nHealAmount = combat_result.nHealAmount;
		infoHit.nCombatResultFlags = combat_result.nResultFlags;
		infoHit.pTalentInfo			= pTalentInfo;		
	}	
	else
	{
		bool bGuarded = infoHit.bGuarded;
		pTarget->ClearLastHitInfo(); // 히트정보 삭제

		if (bGuarded)
			return false; // 가드했다면 무시
	}

	infoHit.nDamage = static_cast<int32>(infoHit.nDamage * fDamageApplyRate);

	bool bRunCallback = false;

	if (pTarget->IsNPC())
	{
		GEntityNPC* pNPC = ToEntityNPC(pTarget);
		infoHit.pMotionFactorGroup = &mf;
		infoHit.pAttacker = pUser;
		infoHit.pVictim = pNPC;

		PFI_B(9031, "GTalentEffector::GainDamageAndHeal-CallBack");

		// 먹기 공격 콜백
		if (pNPC->GetNPCSwallow().IsSwallowing())
		{
			bRunCallback = true;
			GetScriptNPC()->OnHitSwallowed(pNPC->GetDelegator(), infoHit);
			nDamageAttrib = (DAMAGE_ATTRIB)infoHit.nDamageType;
		}
		
		// 부분 판정 콜백
		if (infoHit.nCapsuleGroup >=0 &&
			infoHit.nCapsuleIndex >=0)
		{
			bRunCallback = true;
			GetScriptNPC()->OnHitCapsule(pNPC->GetDelegator(), infoHit);
			nDamageAttrib = (DAMAGE_ATTRIB)infoHit.nDamageType;
		}

		PFI_E(9031);
	}
	
	if (!bRunCallback)
	{
		// 콜백을 사용하지 않은 경우에는 기본값을 적용
		infoHit.pMotionFactorGroup = &pTalentInfo->m_MotionFactorGroup;
	}

	PFI_B(9032, "GTalentEffector::GainDamageAndHeal-Resist");

	// 완전 저항
	if (CheckBitSet(infoHit.nCombatResultFlags, CTR_RESIST_PERFECT))
	{
		// 완전 저항도 패킷 날림
		GTalentHitRouter talentHitRouter;
		talentHitRouter.Route_Resist(pUser, pTarget, pTalentInfo, infoHit.nCheckTime, infoHit.nCombatResultFlags);

		return false;
	}

	PFI_E(9032);

	PFI_B(9033, "GTalentEffector::GainDamageAndHeal-MotionFactor");

	// 모션팩터 처리
	GMFApplyReturnValue mfApplyRet(MF_NONE, 0);

	if (!bIgnoreMF)
	{
		GMFApplyArgs mfApplyArgs;
		mfApplyArgs.pMotionFactorGroup = infoHit.pMotionFactorGroup;
		mfApplyArgs.pOpponent = pUser;
		mfApplyArgs.nNotApplyMF = MF_NONE;
		mfApplyArgs.nForceMF = pTalentInfo->m_nForceMF;
		mfApplyArgs.nForceMFWeight = pTalentInfo->m_nForceMFWeight;
		mfApplyArgs.bKnockbackDirSyncToOpponentDir = pTalentInfo->IsKnockbackDirSyncToAttackerDir();	// 이동하는 탤런트는 넉백시 공격자 방향으로 넉백되도록 설정함
		mfApplyArgs.fStateMotionFactor = fStateMotionFactor;											// 상태보정에 따른 모션팩터

		// 죽었을 때에는 임팩트 있는 효과를 위해 가끔씩 넉백으로 죽는다.
		GDeadMotionFactorImpactor deadMotionFactorImpactor;
		deadMotionFactorImpactor.CheckUp(mfApplyArgs, pTarget, pTalentInfo, infoHit.nDamage);

		// drag일 때 Weight를 따로 계산
		if (mfApplyArgs.nForceMF == MF_DRAG)
		{
			float fActorDistance = pUser->GetPos().DistanceTo(pTarget->GetPos());
			float fOpponentRadius = pUser->GetColRadius();
			float fVictimRadius = pTarget->GetColRadius();

			float fDistance;
			if (fActorDistance > fVictimRadius + fOpponentRadius)
			{
				fDistance = fActorDistance - (fVictimRadius + fOpponentRadius);
			}
			else
			{
				fDistance = 0;
			}
			
			mfApplyArgs.nForceMFWeight = (int)fDistance;
		}


		mfApplyRet = pTarget->ApplyMotionFactor(mfApplyArgs);
	}

	PFI_E(9033);

	PFI_B(9034, "GTalentEffector::GainDamageAndHeal-Heal");

	// 회복 처리
	int nHealAmount=infoHit.nHealAmount;
	if (nHealAmount)
	{
		pTarget->doHeal(pUser->GetUID(), nHealAmount);

		GTalentRouter router;
		router.RouteTalentHeal(pUser, pTarget, pTalentInfo->m_nID, nHealAmount);

		OnHealed(pTarget, pTalentInfo, nHealAmount);
	}

	PFI_E(9034);

	PFI_B(9035, "GTalentEffector::GainDamageAndHeal-Damage");

	// 피해 처리
	int nDamage=infoHit.nDamage;
	if (nDamage > 0)
	{
		pTarget->doDamage(pUser->GetUID(), nDamageAttrib, nDamage, EST_TALENT, pTalentInfo->m_nID);
	}

	PFI_E(9035);


	PFI_B(9036, "GTalentEffector::GainDamageAndHeal-HitPackit");

	// 히트 패킷 날림
	if (pTalentInfo->HasAttack())
	{
		GTalentHitRouter talentHitRouter;
		talentHitRouter.RouteTalentHit(pUser, pTarget, pTalentInfo, nDamage, mfApplyRet.nMotionFactor, mfApplyRet.nWeight, infoHit.nCheckTime, infoHit.nCombatResultFlags);
	}

	PFI_E(9036);

	PFI_B(9037, "GTalentEffector::GainDamageAndHeal-Attacker");
	
	// 공격자 처리
	if (infoHit.pAttacker)
	{
		if (mfApplyRet.nMotionFactor == MF_UPPERED)
		{
			pTarget->RouteActionCancelNextAttack(infoHit.pAttacker->GetUIID(), CANCEL_NEXT_ATTACK_PARAM_UPPERED);
		}
	}

	PFI_E(9037);

	PFI_B(9038, "GTalentEffector::GainDamageAndHeal-Die");

	// 죽음 처리
	if (pTarget->IsDie()) 
	{
		pTarget->SetKiller(pUser->GetUID());
		if (pTarget->IsPlayer())
		{
			ToEntityPlayer(pTarget)->SetKillTalent(pTalentInfo);
		}
		pTarget->doDie();
	}

	PFI_E(9038);

	PFI_B(9039, "GTalentEffector::GainDamageAndHeal-Durability");

	gsys.pItemSystem->GetDurabilityUpdater().LostDurabilityByCombat(pUser, pTarget, infoHit.bCriticalHit, infoHit.bTryGuard);

	PFI_E(9039);

	return true;
}

void GTalentEffector::GainTalentBuff(const Info& info, GTalentInfo* pTalentInfo)
{
	GainSingleBuff(info, pTalentInfo, pTalentInfo->m_Buff1);
	GainSingleBuff(info, pTalentInfo, pTalentInfo->m_Buff2);
}

void GTalentEffector::GainSingleBuff( const Info& info, GTalentInfo* pTalentInfo, const CSBuffEnchantInfo& BuffInvokeInfo )
{
	VALID(info.pUser);
	VALID(pTalentInfo);

	if (BuffInvokeInfo.nID == INVALID_BUFF_ID)				return;
	if (BuffInvokeInfo.nEnchantEvent != info.nTalentCondition)	return;
	
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(BuffInvokeInfo.nID);
	_ASSERT(pBuffInfo);
	if (!pBuffInfo)									return;

	// 외치기 속성의 탤런트일 경우, 패시브 효과 추가
	float fAddEffecteeRadius = 0.0f;	
	if (pTalentInfo->m_strSpecialization == TALENT_PROPERTY_SHOUT && info.pUser->IsPlayer())
	{
		fAddEffecteeRadius = (float)ToEntityPlayer(info.pUser)->GetPassiveValue(TEPT_TALENT_SHOUT_RADIUS)*100;
	}

	GEffectTargetSelector::Desc desc(BuffInvokeInfo.EffectInfo);
	desc.pSelf = info.pUser;
	desc.pTarget = info.pTarget;
	desc.fAddEffectRadius = fAddEffecteeRadius;
	desc.vCenterPoint = info.vGroundPoint;
	set<GEntityActor*> qEffecteeList;
	m_EffectTargetSelector.Select(desc, qEffecteeList);

	for each (GEntityActor* pActor in qEffecteeList)
	{
		pActor->GetModuleBuff()->GainBuff(pBuffInfo, BuffInvokeInfo, pTalentInfo, info.pUser);
	}
}

bool GTalentEffector::IsInstantEffect( GTalentInfo* pTalentInfo )
{
	VALID_RET(pTalentInfo, false);		

	if (IsNeedEffect(pTalentInfo))
		return false; // 피해가 있으면 인스턴트 이펙트가 아님

	if (pTalentInfo->m_nHateAdj != 0)
		return true; // 헤이트가 바뀜

	if (pTalentInfo->IsExistModValue())
		return true; // 모디파이어가 바뀜

	return false;

}

void GTalentEffector::RouteGainInstantEffect( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	VALID(pUser);
	VALID(pTalentInfo);		

	MCommand* pNewCommand = MakeNewCommand(MC_TALENT_INSTANT_EFFECT_GAIN, 2, 
			NEW_USHORT(pUser->GetUIID()), 
			NEW_INT(pTalentInfo->m_nID));

	pUser->RouteToThisCell(pNewCommand);
}

bool GTalentEffector::IsNeedEffect( GTalentInfo* pTalentInfo )
{
	VALID_RET(pTalentInfo, false);

	if (pTalentInfo->HasDamage())			return true;
	if (pTalentInfo->HasMotionfactor())		return true;
	if (pTalentInfo->HasHealEffect())		return true;
	if (pTalentInfo->HasHeal())				return true;
	if (pTalentInfo->HasBuff())				return true;
	return false;
}

void GTalentEffector::OnHealed(GEntityActor* pTarget, GTalentInfo* pTalentInfo, int nHealHPAmount)
{
	PFI_BLOCK_THISFUNC(9023);
	pTarget->GetActorHateTracker().AddHate_RecoverHP(nHealHPAmount, pTalentInfo->m_fHateFactor);
}
