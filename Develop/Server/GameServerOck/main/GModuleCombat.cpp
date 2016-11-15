#include "stdafx.h"
#include "GModuleCombat.h"
#include "GMessageID.h"
#include "GEntitySync.h"
#include "GTalentInfo.h"
#include "GTalent.h"
#include "GEntityActor.h"
#include "CCommandResultTable.h"
#include "GTalentFactory.h"
#include "GEntityPlayer.h"
#include "GCommandTable.h"
#include "GTalentFocusMgr.h"
#include "GGlobal.h"
#include "GTalentHelper.h"
#include "GMath.h"
#include "GCollision.h"
#include "GUseCostMgr.h"
#include "GPlayerTalent.h"
#include "GActorCooltimeChecker.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"
#include "GOverlappedEquipmentSlot.h"
#include "GPalette.h"

////////////////////////////////////////////////////////////////
GInvincibleProcessor::GInvincibleProcessor() 
: m_bInvincible(false)
{

}

bool GInvincibleProcessor::IsInvincible()
{
	return m_bInvincible;
}

void GInvincibleProcessor::DoInvincible( float fDurationTime )
{
	if (0.0f >= fDurationTime) return;

	m_Regulator.SetTickTime(fDurationTime);
	m_Regulator.Start();
	m_bInvincible = true;
}

void GInvincibleProcessor::Update( float fDelta )
{
	PFI_BLOCK_THISFUNC(9055);

	if (!m_Regulator.IsReady(fDelta)) return;

	m_Regulator.Stop();
	m_bInvincible = false;
}


////////////////////////////////////////////////////////////////

GModuleCombat::GModuleCombat(GEntity* pOwner) 
: GModule(pOwner)
, m_pTalent(NULL)
, m_pTalentFocusMgr(NULL)
{
	_ASSERT(m_pOwner->GetTypeID() == ETID_PLAYER || 
			m_pOwner->GetTypeID() == ETID_NPC ||
			m_pOwner->GetTypeID() == ETID_ACTOR);
	m_pOwnerActor = static_cast<GEntityActor*>(pOwner);
}

GModuleCombat::~GModuleCombat(void)
{
	SAFE_DELETE(m_pTalent);
}

void GModuleCombat::OnInitialized()
{
	if (m_pOwner->IsPlayer())
	{
		m_pTalentFocusMgr = new GTalentFocusMgr(static_cast<GEntityPlayer*>(m_pOwner));
		m_pTalentFocusMgr->Init();
	}
}

void GModuleCombat::OnFinalized()
{
	if (m_pTalentFocusMgr)
	{
		m_pTalentFocusMgr->Fini();
		delete m_pTalentFocusMgr;
	}
}

void GModuleCombat::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(711);

	GModule::OnUpdate(fDelta);
	
	UpdateActiveTalents(fDelta);

	m_Invincible.Update(fDelta);

	m_TargetHitterList.Update(fDelta, m_pOwner->GetField());

	// 히터 처리 후에 탤런트 종료
	if (m_pTalent)
	{
		m_pTalent->UpdateExpired(fDelta);
	}

	// 히터 처리 후에 포커스 처리를 함
	if (m_pTalentFocusMgr)
	{
		m_pTalentFocusMgr->Update(fDelta);
	}
}

CCommandResultTable GModuleCombat::IsUsableTalent(GTalentInfo* pTalentInfo)
{
	if (pTalentInfo == NULL)			return CR_FAIL_SYSTEM_INVALID_TALENT_ID;

	// 패시브면 무시
	if (pTalentInfo->IsPassiveSkill())	return CR_FAIL_USE_TALENT_PASSIVE;

	// 탤런트를 사용할 수 없는 상태면 무시
	if (m_pOwner->IsActor())
	{
		GEntityActor* pEntityActor = static_cast<GEntityActor*>(m_pOwner);
		if (pEntityActor->IsDisableTalent(pTalentInfo))	
		{
			return CR_FAIL_USE_TALENT_DISABLED;
		}

		if (pTalentInfo->IsRequireMoveSpeed())
		{
			float fPlayerMoveSpeed = 1.0f + pEntityActor->GetChrStatus()->ActorModifier.fMoveSpeed;
			float fRequiredMoveSpeed = pTalentInfo->m_fRequireMoveSpeed;

			if (fPlayerMoveSpeed - fRequiredMoveSpeed + TALENT_REQUIRED_MOVE_TRUNCATED < 0.0f ||
				!pEntityActor->IsMovable())
			{
				return CR_FAIL_USE_TALENT_NOTENOUGH_SPEED;
			}
		}

		if (pTalentInfo->m_bNonCombatOnly &&
			pEntityActor->IsNowCombat())
			return CR_FAIL_USE_TALENT_NONCOMBAT_ONLY;

		if (pTalentInfo->IsPortalTalent())
		{
			const GFieldInfo* pFieldInfo = m_pOwner->GetFieldInfo();
			if (NULL != pFieldInfo)
			{
				if (!pFieldInfo->IsPortalTalentUsable())
				{
					return CR_FAIL_USE_TALENT_NOTUSE_IN_THISFIELD;
				}
			}
		}
	}


	

	// 쿨타임 체크
	if (m_pOwner->IsActor() &&
		ToEntityActor(m_pOwner)->GetActorCooltimeChecker().IsCooldown(pTalentInfo->m_nID))
		return CR_FAIL_USE_TALENT_NOTENOUGH_COOLDOWN;

	// 사용비용 체크
	GUseCostMgr costMgr;
	if (m_pOwner->IsActor() &&
		!costMgr.IsUseTalent(ToEntityActor(m_pOwner), pTalentInfo))
	{
		return CR_FAIL_USE_TALENT_NOTENOUGH_EN;
	}

	return CR_SUCCESS;
}

CCommandResultTable GModuleCombat::UseTalent(GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bCheckEnabled, bool bGainStress)
{
	CCommandResultTable crRet =
		IsUsableTalent(pTalentInfo);

	// 사용 가능한지 체크
	if (bCheckEnabled)
	{
		if (m_pOwner->IsPlayer())
		{
			GEntityPlayer* pEntityPlayer = static_cast<GEntityPlayer*>(m_pOwner);
			CCommandResultTable rstCheck = CheckPlayerTalentEnable(pEntityPlayer, pTalentInfo);
			if (rstCheck != CR_SUCCESS) return rstCheck;
		}
	}

	if (crRet != CR_SUCCESS)
		return crRet;

	if (m_pTalent)
	{
		if (m_pOwner->IsPlayer() &&
			!CSTalentInfoHelper::IsNormalAttackTalent(m_pTalent->GetID()) &&
			!m_pTalent->IsExpired())
		{
			return CR_FAIL_USE_TALENT_ALREADY_RUNNING;
		}
		
		if (!m_pTalent->IsExpired())
		{
			CancelTalent(false);
		}
		
		SAFE_DELETE(m_pTalent);
	}

	// 즉시 탤런트면 사용비용 바로 처리
	GUseCostMgr costMgr;
	if (CSTalentInfoHelper::IsImmediateTalent(pTalentInfo) &&
		m_pOwner->IsActor() &&
		!costMgr.Pay_TalentCost(ToEntityActor(m_pOwner), pTalentInfo))
	{
		return CR_FAIL_USE_TALENT_NOTENOUGH_EN;
	}

	m_pTalent = gsys.pTalentFactory->NewTalent(m_pOwnerActor, pTalentInfo, Target, bGainStress);

	if (m_pOwner->IsActor())
	{
		m_pTalent->AttachObserver(&ToEntityActor(m_pOwner)->GetActorCooltimeChecker());
	}
	PFI_B(3001, "GTalent::Start");
	m_pTalent->Start();
	PFI_E(3001);

	return CR_SUCCESS;
}

void GModuleCombat::UpdateActiveTalents( float fDelta ) 
{
	PFI_BLOCK_THISFUNC(9006);

	if (m_pTalent)
	{
		PFI_B(9051, "GModuleCombat::UpdateActiveTalents-SAFE_DELETE(m_pTalent)1");
		if (IsDeletable())
		{
			SAFE_DELETE(m_pTalent);
			return;
		}
		PFI_E(9051);

		bool bFinished = !m_pTalent->Update(fDelta);

		PFI_B(9052, "GModuleCombat::UpdateActiveTalents-SAFE_DELETE(m_pTalent)2");
		if (bFinished &&
			m_pOwner->IsPlayer())
		{
			SAFE_DELETE(m_pTalent);
		}
		PFI_E(9052);
	}
}

void GModuleCombat::CancelTalent(bool bPostCommand)
{
	if (!m_pTalent)	return;

	if (m_pTalent->IsCancelable())
	{
		// 모션팩토로 인해 캔슬됐다면 비용을 지불한다.
		if (GetOwner()->IsActor() &&
			ToEntityActor(GetOwner())->HasMotionFactor())
		{
			// 사용비용 지불
			GUseCostMgr costMgr;
			costMgr.Pay_TalentCostForce(ToEntityActor(m_pOwner), m_pTalent->GetInfo());
		}

		m_pTalent->Cancel(bPostCommand);
	}
}

void GModuleCombat::CancelTalentForce(bool bPostCommand)
{
	if (!m_pTalent)	return;
	m_pTalent->CancelForce(bPostCommand);
}

bool GModuleCombat::IsUsingMovingTalent()
{
	if (!m_pTalent)		return false;

	return m_pTalent->IsMovable();
}

bool GModuleCombat::IsUsingNotCancelableTalent()
{
	if (!m_pTalent)		return false;

	return (!m_pTalent->IsCancelable());
}

bool GModuleCombat::IsUsingTalent()
{
	if (!m_pTalent)		
		return false;

	if (m_pTalent->IsExpired())
		return false;

	return true;
}

int GModuleCombat::GetUsingTalentID()
{
	if (!m_pTalent)		
		return INVALID_TALENT_ID;

	if (m_pTalent->IsExpired())
		return INVALID_TALENT_ID;

	return m_pTalent->GetID();
}

bool GModuleCombat::IsNowInvincibleTime()
{
	if (m_Invincible.IsInvincible())		return true;
	if (!m_pTalent)							return false;
	if (m_pTalent->IsNowInvincibleTime())	return true;

	return false;
}

bool GModuleCombat::IsNowSuperarmorTime()
{
	if (!m_pTalent)						return false;
	return m_pTalent->IsNowSuperarmorTime();
}

bool GModuleCombat::IsNowAvoidTime()
{
	if (!m_pTalent)						return false;
	return m_pTalent->IsNowAvoidTime();
}

CCommandResultTable GModuleCombat::CheckPlayerTalentEnable(GEntityPlayer* pOwnerPlayer, GTalentInfo* pTalentInfo)
{
	VALID_RET(pOwnerPlayer, CR_FAIL);

	if (pOwnerPlayer->GetItemHolder()->GetEquipment().GetOverlapped().IsOverlapped())
	{
		// 장비 오버랩 된 경우
		const VEC_PALETTE_ITEM& vecPalettes =
			pOwnerPlayer->GetPalette().GetContainer();

		if (pTalentInfo->m_bIsDodge)
			return CR_SUCCESS;

		for each (const GPaletteItem& each in vecPalettes)
		{
			if (each.m_nType != PIT_TALENT)
				continue;

			if (each.m_nItemIDorTalentID == pTalentInfo->m_nID)
				return CR_SUCCESS;
		}

		return CR_FAIL_USE_TALENT;
	}

	// 배우지 않고 사용가능한지
	// 배웠는지
	// 아이템 사용해서 쓸수 있는 탤런트인지 체크
	if (false == pOwnerPlayer->GetTalent().IsTrainedTalent(pTalentInfo) &&
		false == pOwnerPlayer->GetTalent().IsEnableUseItemTalent(pTalentInfo->m_nID))
	{
		return CR_FAIL_USE_TALENT;
	}

	// 무기 체크
	if (pTalentInfo->m_WeaponAllowedInfo.m_bAllAllowed != false)
	{
		GItemHolder* pItemHolder = pOwnerPlayer->GetItemHolder();
		WEAPON_TYPE nWeaponType = pItemHolder->GetEquipment().GetWeaponType();
		bool bEquipShield = pOwnerPlayer->IsEquipShield();

		if (pTalentInfo->m_WeaponAllowedInfo.IsAllowed(nWeaponType, bEquipShield) == false)
		{
			return CR_FAIL_USE_TALENT;
		}
	}

	// 포커스 체크
	if (pTalentInfo->HasRequireFocus() &&
		!m_pTalentFocusMgr->HasFocus(pTalentInfo->m_nRequireFocus))
		return CR_FAIL_USE_TALENT_NOFOCUS;

	return CR_SUCCESS;
}

void GModuleCombat::Clear()
{
	SAFE_DELETE(m_pTalent);
	m_TargetHitterList.Clear();
}

bool GModuleCombat::HasFocus(TALENT_FOCUS_TYPE nFocus)
{
	if (!m_pTalentFocusMgr)	
		return false;

	return m_pTalentFocusMgr->HasFocus(nFocus);
}

bool GModuleCombat::HasFocus()
{
	if (!m_pTalentFocusMgr)	
		return false;

	return m_pTalentFocusMgr->HasFocus();
}

int GModuleCombat::GetBerserkBubble()
{
	if (!m_pTalentFocusMgr)	
		return 0;

	return m_pTalentFocusMgr->GetBerserkBubble();
}

bool GModuleCombat::CheckTalentRange( GEntityActor* pTargetActor, int nTalentID )
{
	if (!pTargetActor) return false;

	GTalentInfo* pTalentInfo = GTalentHelper::FindTalentInfo(nTalentID, m_pOwnerActor);
	if (!pTalentInfo)
	{
		dlog("GModuleCombat::CheckTalentRange - invalid talentid. (UID: %I64d ,TalentID: %d)\n", m_pOwnerActor->GetUID(), nTalentID);
		return false;
	}

	GTalentRangeChecker talentRangeChecker;
	return talentRangeChecker.Check(pTalentInfo, 
									m_pOwnerActor->GetPos(), 
									m_pOwnerActor->GetDir(), 
									m_pOwnerActor->GetColRadius(), 
									pTargetActor->GetPos(), 
									pTargetActor->GetColRadius());
}

float GModuleCombat::GetTalentElapsedTime()
{
	if (m_pTalent)
	{
		return m_pTalent->GetElapsedTime();
	}
	return 0.0f;
}

float GModuleCombat::GetTalentActElapsedTime()
{
	if (m_pTalent)
	{
		return m_pTalent->GetActElapsedTime();
	}
	return 0.0f;
}

void GModuleCombat::ActTalent( TALENT_TARGET_INFO Target )
{
	if (!m_pTalent)	return;

	// 사용비용 체크
	GUseCostMgr costMgr;
	if (m_pOwner->IsActor() &&
		!costMgr.Pay_TalentCost(ToEntityActor(m_pOwner), m_pTalent->GetInfo()))
	{
		// 실패시 취소
		m_pTalent->Cancel(true);

		if (GetOwner()->IsPlayer())
		{
			ToEntityPlayer(GetOwner())->FailAndRouteSystemMsg(CR_FAIL_USE_TALENT_NOTENOUGH_EN);
		}
		return;
	}

	m_pTalent->ActTalent(Target);
}

void GModuleCombat::AddTargetHitter( GTalentHitter* pNewHitter )
{
	m_TargetHitterList.Add(pNewHitter);
}

vector<GTalentHitter*> GModuleCombat::GetTargetHitter( TARGEHITTER_TYPE nType )
{
	return m_TargetHitterList.Get(nType);
}

void GModuleCombat::DeleteCurTalent()
{
	if (m_pTalent)
	{
		SAFE_DELETE(m_pTalent);
	}
}

bool GModuleCombat::IsDeletable()
{
	return false == m_pTalent->GetInfo()->m_bUsableAtDead && m_pOwner->IsDead();
}

bool GTalentRangeChecker::Check( GTalentInfo* pAttackTalentInfo, const vec3& vAttackerPos, const vec3& vAttackerDir, float fAttackerRadius, const vec3& vTargetPos, float fTargetRadius )
{
	const float fDistanceToTar = vTargetPos.DistanceTo(vAttackerPos);

	if (pAttackTalentInfo->m_nSkillType == ST_SOCIAL)			return true;

	if (pAttackTalentInfo->m_nSkillType == ST_ARCHERY || pAttackTalentInfo->m_nSkillType == ST_MAGIC)	
	{
		return (fDistanceToTar <= pAttackTalentInfo->m_fRange)?true:false;
	}
	else if (pAttackTalentInfo->m_nSkillType == ST_GUARD)
	{
		return (fDistanceToTar > fTargetRadius + fAttackerRadius)?true:false;
	}

	vec3 vTarDir = vTargetPos - vAttackerPos;
	GMath::NormalizeZ0(vTarDir);

	bool bHitable = false;
	if (pAttackTalentInfo->m_nSkillType == ST_MELEE)
	{
		// 근거리이고 겹쳐져 있으면 충돌
		if (fDistanceToTar < fTargetRadius + fAttackerRadius)
		{
			bHitable = true;
		}
	}

	if (!bHitable)
	{
		if (fDistanceToTar < (pAttackTalentInfo->GetDistanceToLongestPoint()))
		{
			bHitable = true;
		}
	}

	if (bHitable)
	{
		vec3 vCurDir = vAttackerDir;

		float fAngle = vCurDir.DotProduct(vTarDir);
		if (0.9f < fAngle) return true;
	}

	return false;
}

