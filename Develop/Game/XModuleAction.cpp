#include "stdafx.h"
#include "XModuleAction.h"
#include "XModuleMovable.h"
#include "XModuleEntity.h"
#include "XModuleEffect.h"
#include "XModuleBuff.h"
#include "XModuleTalent.h"
#include "XEventID.h"
#include "XObject.h"
#include "XPlayer.h"
#include "XTalentInfo.h"
#include "XConst.h"
#include "XNonPlayer.h"
#include "XGameTransformControllerManager.h"
#include "XTalentInfoMgr.h"
#include "XTalentHelper.h"
#include "XMotionLogic.h"
#include "XMotionHelper.h"
#include "XMotionFactorHelper.h"
#include "XAnim.h"
#include "XNaming.h"
#include "CSMotionFactorHelper.h"
#include "CSDef.h"

#define KB_MAX_VELOCITY						2000.0f
#define FAKE_KNOCKBACK_VELOCITY				5000.0f


MDIRECTION GetMFBeatenDirection(XActor* pAttacker, XActor* pVictim, int nTalentID)
{
	if ((!pAttacker) || (!pVictim)) return MDIR_BACK;
	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pAttacker);
	if (!pTalentInfo) return MDIR_BACK;

	float fAngle = pAttacker->GetDirection().DotProduct(pVictim->GetDirection());
	// 뒤에서 공격시
	if (fAngle > 0.f)
	{
		if (pVictim->GetEntityType() == ETID_NPC)
		{
			return MDIR_FRONT;
		}
		else
		{
			return pTalentInfo->m_nAttackDirection;
		}
	}
	else	// 앞에서 공격시
	{
		if (pVictim->GetEntityType() == ETID_NPC)
		{
			return MDIR_BACK;
		}
		else
		{
			switch (pTalentInfo->m_nAttackDirection)
			{
			case MDIR_FRONT: return MDIR_BACK;
			case MDIR_BACK: return MDIR_FRONT;
			case MDIR_LEFT: return MDIR_RIGHT;
			case MDIR_RIGHT: return MDIR_LEFT;
			}
		}
	}

	return MDIR_BACK;
}

///////////////////////////////////////////////////////////////////////////////////////////////

XModuleAction::XModuleAction(XObject* pOwner)
: XModule(pOwner)
, m_pModuleMotion(NULL), m_pModuleMovable(NULL), m_bMFStunNow(false), m_bHoldMotion(false), m_fHoldMotionCounter(0.0f)
{
	_ASSERT(MIsDerivedFromClass(XActor, pOwner));
	m_ActionDie.Init(pOwner);
	m_MotionStopChecker.Reset();
	m_vPrevPos = vec3::ZERO;
	m_vPrevDir = vec3::ZERO;
}

XModuleAction::~XModuleAction(void)
{

}


void XModuleAction::OnInitialized()
{
	m_pModuleMotion = m_pOwner->GetModuleMotion();
	_ASSERT(m_pModuleMotion);

	m_pModuleMovable = m_pOwner->GetModuleMovable();
	_ASSERT(m_pModuleMovable);
}

void XModuleAction::OnSubscribeEvent()
{
	XModule::OnSubscribeEvent();

	Subscribe(XEVTD_END_MF_STUN);
	Subscribe(XEVTD_END_MF_KNOCKDOWN);
}

XEventResult XModuleAction::OnEvent(XEvent& msg)
{
	switch (msg.m_nID)
	{
	case XEVTD_END_MF_STUN:
		{
			EndStun();
		}
		return MR_TRUE;
	case XEVTD_END_MF_KNOCKDOWN:
		{
			XEvent msg;
			msg.m_nID = XEVTL_ON_MOTION_EVENT; 
			msg.m_pData = MOTION_EVENT_MF_KNOCKDOWN_EVENT;
			m_pOwner->Event(msg);
		}
		return MR_TRUE;
	};

	return MR_FALSE;
}

void XModuleAction::ActionGuard()
{
	StopMove();
	Guard();
}

bool XModuleAction::Jump( XMotionTypes nMotionType, bool bForced /*=false*/, vec3* vResultDir /*= NULL*/ )	
{
	if (m_pModuleMotion == NULL)
		return false;


	float fMoveSpeedRatio = 1.0f;
	XModuleBuff* pModuleBuff = m_pOwner->GetModuleBuff();
	if ( pModuleBuff)
	{
		XBuffAttribute attrBuff = pModuleBuff->GetBuffAttribute();
		fMoveSpeedRatio = attrBuff.m_fMoveSpeedRatio;
	}


	// Jump
	bool bValidJump = true;
	if ( bForced == true)
		m_pModuleMovable->ForceJump( vResultDir, fMoveSpeedRatio);
	else
		bValidJump = m_pModuleMovable->TriggerJump( vResultDir, fMoveSpeedRatio);


	if ( bValidJump == false)
		return false;


	bool ret = m_pModuleMotion->ChangeMotion( MOTION_NAME_JUMP, nMotionType, bForced);
	m_pModuleMotion->PushMotion( MOTION_NAME_IDLE);

	return ret;
}

bool XModuleAction::Tumble(XMotionTypes nMotionType)
{
	if (m_pModuleMotion == NULL) return false;

	// 텀블링 없어졌지만 혹시 다시 생길지 몰라 완전히 없애지는 않음. birdkr
	return m_pModuleMotion->ChangeMotion(MOTION_NAME_JUMP, nMotionType);
	//return m_pModuleMotion->ChangeMotion("tumble", nMotionType);
}


void XModuleAction::ChangeStance(CHAR_STANCE nStanceTo, bool bQuick)
{
	bool bShield = false;
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
		if (pPlayer->IsEquipShield())
		{
			bShield = true;
		}
	}

	wstring strMotion = m_pModuleMotion->GetCurrMotion();

	if (nStanceTo == CS_NORMAL)
	{
		if(strMotion == MOTION_NAME_RUN)
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL);
			}
		}
		else
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL, MOTION_NAME_IDLE);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL, MOTION_NAME_IDLE);
			}
		}
	}
	else if (nStanceTo == CS_BATTLE)
	{
		if(strMotion == MOTION_NAME_RUN)
		{
			if (bShield)
			{
				if(bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE);
			}
			else
			{
				if(bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE);
			}
		}
		else
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE, MOTION_NAME_IDLE);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE, MOTION_NAME_IDLE);
			}
		}
	}
}

void XModuleAction::OnUpdate(float fDelta)
{
	PFC_THISFUNC;

	PFI_BLOCK_THISFUNC(301);

	m_ActionDie.Update(fDelta);

	XModule::OnUpdate(fDelta);
}

void XModuleAction::Stop( bool bForced)
{
	ChangeMotionToIdleMotion();

	StopMove( bForced);
}

void XModuleAction::StopMove(bool bForced/* =false */)
{
	if (bForced)
	{
		m_pModuleMovable->ForceStop();
	}
	else
	{
		m_pModuleMovable->TriggerStop();
	}
}

void XModuleAction::StopUpperAni()
{
	m_pModuleMotion->StopUpperMotion();
}

bool XModuleAction::IsCurAiming()
{
	if( m_pModuleMotion && (m_pModuleMotion->GetCurrMotion() == MOTION_NAME_AIMING))
		return true;

	return false;
}

bool XModuleAction::ActionAiming()
{
	if (m_pModuleMotion == NULL) return false;
		
	if(m_pModuleMotion->ChangeMotion(MOTION_NAME_AIMING))
	{
		m_vAimingDir = m_pOwner->GetDirection();	//Z가 0이당...
		return true;
	}

	m_vAimingDir = vec3(0.f, 0.f, 0.f);
	return false;
}

void XModuleAction::Guard()
{
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
		if (pPlayer->IsEquipShield())
		{
			if (!m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_GUARD, MT_DEFAULT, true)) return;
		}
		else
		{
			if (!m_pModuleMotion->ChangeMotion(MOTION_NAME_GUARD, MT_DEFAULT, true)) return;
		}

		// 방어를 취할때 나는 사운드
		if ( global.sound)
			global.sound->PlayDynamic3D( "pc_guard", NULL, 0, m_pOwner->GetUID());

	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNpc = static_cast<XNonPlayer*>(m_pOwner);
		if(pNpc->IsEquipShield())
		{
			if (!m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_SHIELD_GUARD, MT_DEFAULT, true)) return;
		}
		else
		{
			if (!m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_GUARD, MT_DEFAULT, true)) return;
		}
	}
}

void XModuleAction::ActionGuardRelease()
{
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
		if (pPlayer->IsEquipShield())
		{
			m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_GUARD_RETURN);
			m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
		}
		else
		{
			m_pModuleMotion->ChangeMotion(MOTION_NAME_GUARD_RETURN);
			m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
		}

		// 내꺼에만 적용한다.
		// 가드 끝나고 튀는 현상이 있어서 막기 위해서이다.(로테이션 애니가 들어가서 그렇다.)
		if(m_pOwner->GetUID() == XGetMyUID())
			m_vPrevDir = m_pOwner->GetDirection();
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNpc = static_cast<XNonPlayer*>(m_pOwner);
		if(pNpc->IsEquipShield())
			m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_SHIELD_GUARD_RETURN);
		else
			m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_GUARD_RETURN);

		m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);
	}
}

void XModuleAction::ActionFalling()
{
	if (m_pModuleMotion == NULL) return; 

	if (m_pModuleMotion->ChangeMotion(MOTION_NAME_FALLING))
	{
		m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
		m_pModuleMovable->TriggerFall();
	}
	else
	{
		m_pModuleMotion->ChangeMotion(MOTION_NAME_IDLE, MT_DEFAULT, true);
	}
}

void XModuleAction::Shoot()
{
	if (m_pModuleMotion == NULL) return; 

	m_pModuleMotion->ChangeMotion(MOTION_NAME_SHOOT);
	m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
}


void XModuleAction::NPC_Walk()
{
	if (m_pModuleMotion == NULL) return; 
	
	m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_WALK);
}


void XModuleAction::Die( MF_STATE nMFState /*= MF_NONE*/, bool bSetAni /*= true*/ )
{
	//const wchar_t* szDieMotionName = MOTION_NAME_DIE;
	//if (m_pOwner->GetEntityType() == ETID_NPC) szDieMotionName = MOTION_NAME_NPC_DIE;

	//if(m_pModuleMotion->GetCurrMotion() != szDieMotionName)
	//{
	//	// grapple 중에 죽거나, 현재 애니메이션이 hold된 상태면 죽는 모션을 취하지 않음
	//	if ( m_pModuleMovable->GetMovableFactor().IsGrappled() == false  &&
	//		m_pModuleMotion->IsHoldingSequence() == false)
	//	{
	//		// 애니메이션 및 탤런트... 기타등등 모두 멈춘다.
	//		Stop();

	//		bool bSetAni = m_pModuleMotion->ChangeMotion(szDieMotionName, MT_DEFAULT, true);

	//		// 죽는 애니메이션에 대한 보간 시작
	//		m_ActionDie.StartDie(bSetAni);
	//	}
	//}
	//else if(m_ActionDie.IsDoingActionDie() == false)
	//{
	//	// 액션 다이를 시작도 안했다.
	//	m_ActionDie.StartDie(true);
	//}

	// 1. 피격중에 다이가 왔다.
	if(nMFState == MF_NONE)
	{
		// 현재 무슨 모션중입니까?
		// 혹시 MF 중이라면...
		if(CheckUsableDieAnimation() == false)
		{
			// 다이 애니메이션을 하면 되는 경우는 일단 KNOCKDOWN으로 인식
			m_ActionDie.OnStartDie(ADS_KNOCKDOWN);
			return;
		}
	}

	m_ActionDie.OnStartDie(nMFState, bSetAni);
}

void XModuleAction::Idle()
{
	const wchar_t* motion = GetIdelMotionName();

	m_pModuleMotion->ChangeMotion(motion, MT_DEFAULT, true);
}

void XModuleAction::ActionRebirth()
{
	m_pModuleMovable->TriggerDie(false);

	const wchar_t* motion = MOTION_NAME_REBIRTH;
	m_pModuleMotion->ChangeMotion(motion, MT_DEFAULT, true);
	m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);

	// 죽는 애니메이션 종료
	EndDie();
}

void XModuleAction::EndDie()
{
	// 죽는 애니메이션 종료
	m_ActionDie.OnEndDie();
}

//bool XModuleAction::ActMF_BeatenFake2( int nWeight, MDIRECTION nDirection )
//{
//	if(CheckUsableBeatenFake() == false)
//	{
//		return false;
//	}
//
//	bool bSucceed = false;
//
//	bool bNotIdleAni = false;
//
//	if (m_pOwner->GetEntityType() == ETID_NPC)
//	{
//		if (m_pModuleMotion->GetCurrMotion() != MOTION_NAME_NPC_IDLE)
//			bNotIdleAni = true;
//	}
//	else if (m_pOwner->GetEntityType() == ETID_PLAYER)
//	{
//		if (m_pModuleMotion->GetCurrMotion() != MOTION_NAME_IDLE)
//			bNotIdleAni = true;
//	}
//
//	if (m_pOwner->GetEntityType() == ETID_NPC)
//	{
//		if(bNotIdleAni)
//			return false;
//
//		bSucceed = m_pModuleMotion->ChangeMotion(GetBeatenMotionName(ETID_NPC, nDirection));
//		if (bSucceed)
//		{
//			m_pModuleMotion->SetAnimationTime(0, nWeight);
//			m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);
//		}
//	}
//	else if (m_pOwner->GetEntityType() == ETID_PLAYER)
//	{
//		if(bNotIdleAni == false)
//		{
//			bSucceed = m_pModuleMotion->ChangeMotion(GetBeatenMotionName(ETID_PLAYER, nDirection));
//			if (bSucceed)
//			{
//				m_pModuleMotion->SetAnimationTime(0, nWeight);
//				m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
//			}
//		}
//		else
//		{
//			// 이동시 상체 애니메이션으로 합니다.
//			bSucceed = m_pModuleMotion->ChangeUpperMotion(GetBeatenMotionName(ETID_PLAYER, nDirection));
//			if (bSucceed)
//			{
//				m_pModuleMotion->SetUpperAnimationTime(0, nWeight);
//			}
//		}
//	}
//
//	return true;
//}
//
//void XModuleAction::ActMF_BeatenFake( MDIRECTION nDirection )
//{
//	const float fBeatenBlendFactor = 0.2f;
//	const float fBeatenAniSpeed = 2.0f;
//	const float fBeatenAniDuration = 0.5f;
//
//	if(CheckUsableBeatenFake() == false)
//		return;
//
//	if (m_pOwner->GetEntityType() == ETID_PLAYER)
//	{
//		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
//		WEAPON_TYPE nWeaponType = pPlayer->GetCurrWeaponType();
//		CHAR_STANCE nStance = pPlayer->GetStance();
//
//		wstring strMixAniName;
//
//		XMotion* pMotion = info.motion->GetMotion(GetBeatenMotionName(ETID_PLAYER, nDirection));
//		if (pMotion)
//		{
//			XMotionAniNameMaker aniNameMaker;
//			aniNameMaker.MakePlayerAniName(strMixAniName, pMotion, MT_DEFAULT, 0, nWeaponType, nStance);
//		}
//
//		// 믹스 변경이 되어 주석처리. 다시 만들거나 XFakeBeatenAnimationTC를 쓰셔야 합니다.
//		//XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();
//		//pModuleEntity->GetAnimationController()->SetMixAnimation(strMixAniName.c_str(), fBeatenBlendFactor, fBeatenAniSpeed, fBeatenAniDuration);
//	}
//	else if (m_pOwner->GetEntityType() == ETID_NPC)
//	{
//		XNonPlayer* pNPC = m_pOwner->AsNPC();
//		if (pNPC->GetMode() != 0) return;
//		if (pNPC->GetInfo()->bHuge) return;
//
//		wstring strMixAniName;
//
//		XMotion* pMotion = info.motion->GetMotion(GetBeatenMotionName(ETID_NPC, nDirection));
//		if (pMotion)
//		{
//			XMotionAniNameMaker aniNameMaker;
//			aniNameMaker.MakeNPCAniName(strMixAniName, pMotion, MT_DEFAULT, 0, 0, pNPC->GetInfo());
//		}
//
//		// 믹스 변경이 되어 주석처리. 다시 만들거나 XFakeBeatenAnimationTC를 쓰셔야 합니다.
//		//XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();
//		//pModuleEntity->GetAnimationController()->SetMixAnimation(strMixAniName.c_str(), fBeatenBlendFactor, fBeatenAniSpeed, fBeatenAniDuration);
//	}
//}

bool XModuleAction::ActMF_Beaten(int nWeight, MDIRECTION nDirection)
{
	if (nWeight == 0) return false;
	bool bSucceed = false;

	if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		if (m_pModuleMotion->GetCurrMotion() == MOTION_NAME_NPC_MF_KNOCKDOWN)
			return true;
	}
	else if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		if (m_pModuleMotion->GetCurrMotion() == MOTION_NAME_MF_KNOCKDOWN)
			return true;
	}
	

	if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		bSucceed = m_pModuleMotion->ChangeMotion(GetBeatenMotionName(ETID_NPC, nDirection), MT_DEFAULT, true);
		m_pModuleMotion->SetAnimationTime(0, nWeight);
		m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);

	}
	else if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		bSucceed = m_pModuleMotion->ChangeMotion(GetBeatenMotionName(ETID_PLAYER, nDirection), MT_DEFAULT, true);
		m_pModuleMotion->SetAnimationTime(0, nWeight);
		m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
	}

	if (!bSucceed)
	{
		m_pModuleMotion->ChangeMotion(MOTION_NAME_IDLE);
		return false;
	}

	return true;
}

bool XModuleAction::ActMF_Deflect(int nWeight)
{
	bool bSetAniResult = true;
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_DEFLECT);
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_MF_DEFLECT);
	}

	if(bSetAniResult)
	{
		m_pModuleMotion->SetAnimationTime(0, nWeight);
		m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
	}

	return bSetAniResult;
}

bool XModuleAction::ActMF_Stun(int nWeight)
{
	m_bMFStunNow = true;
	bool bSetAniResult = true;

	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		bool bSuccess = m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_STUN);
		if (!bSuccess)
		{
			bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_SLEEP);
		}
		m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNonPlayer = m_pOwner->AsNPC();
		wstring strStunMotion = pNonPlayer->GetStunMotionAnimation();
		bool bSuccess = m_pModuleMotion->ChangeMotion(strStunMotion.c_str());
		if (!bSuccess)
		{
			bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_SLEEP);
		}
		m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);
	}

	m_pModuleMovable->TriggerStunActive();

	XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect)
	{
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_MF_STUN);
	}

	XEvent msg(XEVTD_END_MF_STUN);
	m_pOwner->PostDelayEvent(msg, unsigned int(nWeight));

	return bSetAniResult;
}

bool XModuleAction::ActMF_Knockback( vec3& dir, vec3& pos, vec3& targetPos, int nWeight, bool bChangeMotion /*= true*/ )
{
	dir.Normalize();
	float fPower = nWeight;

	m_pModuleMovable->TriggerKnockback(dir, pos, targetPos, fPower, KB_MAX_VELOCITY, XCONST::MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST);

	XActor* pActor = static_cast<XActor*>(m_pOwner);
	bool bDead = pActor->IsDead();
	
	bool bSetAniResult = true;
	if (bChangeMotion)
	{
		if (m_pOwner->GetEntityType() == ETID_PLAYER)
		{
			//if(bDead == true)
			//{
			//	m_pModuleMotion->ChangeMotion(MOTION_NAME_DIE, MT_DEFAULT, true);
			//}
			//else
			{
				bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_KNOCKBACK, MT_DEFAULT, true);
				m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
			}
		}
		else if (m_pOwner->GetEntityType() == ETID_NPC)
		{
			//XNonPlayer* pNonPlayer = m_pOwner->AsNPC();
			//if(bDead == true && pNonPlayer->GetUseDieAni())
			//{
			//	m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_DIE, MT_DEFAULT, true);
			//}
			//else
			{
				if(nWeight < 100)
					bSetAniResult = m_pModuleMotion->ChangeMotion(GetBeatenMotionName(ETID_NPC, MDIR_BACK), MT_DEFAULT, true);
				else
					bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_MF_KNOCKBACK, MT_DEFAULT, true);
				
				m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);
			}
		}
	}

	return bSetAniResult;
}

bool XModuleAction::ActMF_FakeKnockback( vec3& dir, vec3& pos, vec3& targetPos, int nWeight )
{
	dir.Normalize();
	float fPower = nWeight;

	m_pModuleMovable->TriggerKnockback(dir, pos, targetPos, fPower, FAKE_KNOCKBACK_VELOCITY, XCONST::MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST);

	return true;
}


bool XModuleAction::ActMF_Drag( vec3& dir, vec3& pos, vec3& targetPos, int nWeight )
{
	dir.Normalize();
	float fPower = nWeight;

	m_pModuleMovable->TriggerDrag(dir, pos, targetPos, fPower, XCONST::MOTION_FACTOR_KNOCKBACK_MOTION_EVENT_DIST);

	return true;
}

bool XModuleAction::ActMF_Throwup(vec3& dir, vec3& pos, vec3& targetPos, int nWeight)
{
	// test
	dir.Normalize();
	float fPower = nWeight;

	m_pModuleMovable->TriggerKnockback(dir, pos, targetPos, fPower, KB_MAX_VELOCITY, XCONST::MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST);

	XActor* pActor = static_cast<XActor*>(m_pOwner);
	bool bDead = pActor->IsDead();

	bool bSetAniResult = true;
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		//if(bDead == true)
		//{
		//	m_pModuleMotion->ChangeMotion(MOTION_NAME_DIE, MT_DEFAULT, true);
		//}
		//else
		{
			// speed 설정
			float fMotionSpeed = XMotionFactorHelper::CalcThrowupMotionSpeed(fPower, 
				XCONST::MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST,
				ANIM_PLAYER_MF_TU1_TIME + 0.6f);

			m_pModuleMotion->ReserveNextMotionSpeed(fMotionSpeed);

			bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_THROWUP, MT_DEFAULT, true);
			m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
		}
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNonPlayer = m_pOwner->AsNPC();
		//if(bDead == true && pNonPlayer->GetUseDieAni())
		//{
		//	m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_DIE, MT_DEFAULT, true);
		//}
		//else
		{
			// speed 설정
			float fMotionSpeed = XMotionFactorHelper::CalcThrowupMotionSpeed(fPower, 
				XCONST::MOTION_FACTOR_THROWUP_MOTION_EVENT_DIST,
				ANIM_NPC_MF_TU1_TIME + 0.6f);

			m_pModuleMotion->ReserveNextMotionSpeed(fMotionSpeed);

			bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_MF_THROWUP, MT_DEFAULT, true);
			m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);
		}
	}

	return bSetAniResult;
}

bool XModuleAction::ActMF_Uppered(vec3& dir, vec3& pos, vec3& targetPos, int nWeight)
{
	// test
	dir.Normalize();
	float fPower = nWeight;

	XMotionFactorHelper::UPPERED_FACTOR upperedFactor = XMotionFactorHelper::CalcUpperedFactor(fPower);
	m_pModuleMovable->TriggerUppered(dir, pos, targetPos, fPower, upperedFactor.fDownTime, upperedFactor.fUpVelocity);

	XActor* pActor = static_cast<XActor*>(m_pOwner);
	bool bDead = pActor->IsDead();

	bool bSetAniResult = true;
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		//if(bDead == true)
		//{
		//	m_pModuleMotion->ChangeMotion(MOTION_NAME_DIE, MT_DEFAULT, true);
		//}
		//else
		{
			if (fPower <= 900.0f)
			{
				bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_UPPERED_SHORT, MT_DEFAULT, true);
			}
			else
			{
				bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_UPPERED, MT_DEFAULT, true);
			}
			
			m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
		}
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_MF_UPPERED, MT_DEFAULT, true);
		m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);
	}

	return bSetAniResult;
}

bool XModuleAction::ActMF_Knockdown(int nWeight)
{
	bool bSetAniResult = true;
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_KNOCKDOWN, MT_DEFAULT, true);
		m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		bSetAniResult = m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_MF_KNOCKDOWN, MT_DEFAULT, true);
		m_pModuleMotion->PushMotion(MOTION_NAME_NPC_IDLE);
	}

	XEvent msg(XEVTD_END_MF_KNOCKDOWN);
	m_pOwner->PostDelayEvent(msg, unsigned int(nWeight));

	return bSetAniResult;
}

bool XModuleAction::ActMF_Grappled( const vec3& targetpos, const vec3& targetdir, int nWeight )
{
//	if(m_pOwner->GetTypeID() == ETID_PLAYER)
//	{
//		m_pModuleMovable->TriggerGrappleStart(targetpos, 0.2f, nWeight);
//		m_pModuleMotion->ChangeMotion(MOTION_NAME_MF_GRAPPLED);
//		m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
//	}

	return true;
}

void XModuleAction::ActMF( MF_STATE nState, int nWeight, UIID nAttackerUIID, stServerKnockbackVictimInfo stServerKnockbackInfo, int nAttackedTalentID, stMotionFactorResult& outResult )
{
	XActor* pOwnerActor = static_cast<XActor*>(m_pOwner);
	if(pOwnerActor->IsDead() &&
		nState != MF_KNOCKBACK &&
		nState != MF_THROWUP &&
		nState != MF_DRAG &&
		nState != MF_UPPERED) 
		return;

	if (nState == MF_NONE) return;
	if (nWeight <= 0) return;

	if (CSMotionFactorHelper::IsFakeMotionFactor(nState) == false)
	{
		StopMove();
	}

	XActor* pAttacker = gg.omgr->FindActor_UIID(nAttackerUIID);

	//if(m_pOwner->GetUID() == XGetMyUID())
	//	mlog("모션 팩터 %s\n", MLocale::ConvTCHARToAnsi(MF_STATE_NAME[nState]).c_str());

	bool bSetAni = false;
	switch (nState)
	{
	//case MF_FAKE_BEATEN:
	//	{
	//		MDIRECTION nBeatenDirection = MDIR_BACK;
	//		nBeatenDirection = GetMFBeatenDirection(pAttacker, pOwnerActor, nAttackedTalentID);

	//		outResult.bActFackBeaten = ActMF_BeatenFake2(nWeight, nBeatenDirection);
	//	}
	//	break;
	case MF_BEATEN:
		{
			MDIRECTION nBeatenDirection = MDIR_BACK;
			nBeatenDirection = GetMFBeatenDirection(pAttacker, pOwnerActor, nAttackedTalentID);

			bSetAni = ActMF_Beaten(nWeight, nBeatenDirection);
		}
		break;
	case MF_DEFLECT:
		{
			bSetAni = ActMF_Deflect(nWeight);
		}
		break;
	case MF_STUN:
		{
			bSetAni = ActMF_Stun(nWeight);
		}
		break;
	case MF_KNOCKBACK:
		{
			if (m_bMFStunNow)
			{
				EndStun();
			}
			bSetAni = ActMF_Knockback(stServerKnockbackInfo.vKnockbackDir, stServerKnockbackInfo.vKnockbackVictimPos, stServerKnockbackInfo.vKnockbackVictimTargetPos, nWeight);
		}
		break;
	case MF_FAKE_KNOCKBACK:
		{
			bSetAni = ActMF_FakeKnockback(stServerKnockbackInfo.vKnockbackDir, stServerKnockbackInfo.vKnockbackVictimPos, stServerKnockbackInfo.vKnockbackVictimTargetPos, nWeight);
		}
		break;
	case MF_DRAG:
		{
			if (m_bMFStunNow)
			{
				EndStun();
			}
			bSetAni = ActMF_Drag(stServerKnockbackInfo.vKnockbackDir, stServerKnockbackInfo.vKnockbackVictimPos, stServerKnockbackInfo.vKnockbackVictimTargetPos, nWeight);
		}
		break;
	case MF_THROWUP:
		{
			if (m_bMFStunNow)
			{
				EndStun();
			}

			bSetAni = ActMF_Throwup(stServerKnockbackInfo.vKnockbackDir, stServerKnockbackInfo.vKnockbackVictimPos, stServerKnockbackInfo.vKnockbackVictimTargetPos, nWeight);
		}
		break;
	case MF_KNOCKDOWN:
		{
			if (m_bMFStunNow)
			{
				EndStun();
			}

			bSetAni = ActMF_Knockdown(nWeight);
		}
		break;
	case MF_UPPERED:
		{
			bSetAni = ActMF_Uppered(stServerKnockbackInfo.vKnockbackDir, stServerKnockbackInfo.vKnockbackVictimPos, stServerKnockbackInfo.vKnockbackVictimTargetPos, nWeight);
		}
		break;
	case MF_GRAPPLED:
	case MF_SWALLOWED:
		{
			if (pAttacker)
			{
				vec3 vGrappledPos	= m_pOwner->GetPosition();
				vec3 vGrappledDir	= m_pOwner->GetDirection();
				XCharacter* pActor	= pAttacker->GetActor();
				if(pActor)
				{
					pActor->GetActorNodePos(GRAPPLED_NPC_GRIP_BONE_NAME, vGrappledPos, E_TS_WORLD);
				}

				bSetAni = ActMF_Grappled(vGrappledPos, vGrappledDir, nWeight);
			}
		}
		break;
	}

	// 모션팩터 애니메이션 체크
	CheckPostMFAnimation(bSetAni);

	outResult.bUseMFAnimation = bSetAni;
}

void XModuleAction::Guarded(bool bKeepGuard)
{
	// PLAYER만 밀리게 한다.
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		const int WEIGHT = 30;
		vec3 vKnockbackDir = -m_pOwner->GetDirection();
		vec3 vKnockbackTarget = m_pOwner->GetPosition() + (vKnockbackDir * WEIGHT);
		ActMF_Knockback(vKnockbackDir, m_pOwner->GetPosition(), vKnockbackTarget, WEIGHT, false);

		// 가드를 받았는데... 현재 탤런트 사용중이라면... 모션을 Guarded로 변경하지 않는다. 
		// 변경해버리면 더 이상하다. 그냥 탤런트 애니메이션이 나오게 한다.
		if(m_pModuleMovable->GetMovableFactor().GENERAL_STATE.m_bTalentActive)
			return;

		// 가드 성공 후에는 idle로 상태 이동하는게 자연스러움
		// 계속 가드 유지시에는 guard_idle로 변경
		wstring strNextMotionName = MOTION_NAME_IDLE;
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
		if (pPlayer->IsEquipShield())
		{
			if(bKeepGuard)
				strNextMotionName = MOTION_NAME_SHIELD_GUARD;

			m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_GUARD_BLOCK);
		}
		else
		{
			if(bKeepGuard)
				strNextMotionName = MOTION_NAME_GUARD;

			m_pModuleMotion->ChangeMotion(MOTION_NAME_GUARD_BLOCK);
		}

		m_pModuleMotion->PushMotion(strNextMotionName.c_str());
	}
}

void XModuleAction::ChangeWeapon( CHAR_STANCE nSwitching, bool bQuick )
{
	bool bShield = false;
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
		if (pPlayer->IsEquipShield())
		{
			bShield = true;
		}
	}

	wstring strMotion = m_pModuleMotion->GetCurrMotion();

	if (nSwitching == CS_NORMAL)
	{
		if(strMotion == MOTION_NAME_RUN)
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL);
			}
		}
		else
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL);
			}
		}
	}
	else if (nSwitching == CS_BATTLE)
	{
		if(strMotion == MOTION_NAME_RUN)
		{
			if (bShield)
			{
				if(bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE);
			}
			else
			{
				if(bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE);
			}
		}
		else
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE, MOTION_NAME_IDLE);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE, MOTION_NAME_IDLE);
			}
		}
	}
}

const wchar_t* XModuleAction::GetBeatenMotionName( ENTITY_TYPEID nEntityTypeID, MDIRECTION nDirection )
{
	if (nEntityTypeID == ETID_PLAYER)
	{
		switch (nDirection)
		{
		case MDIR_FRONT:	return MOTION_NAME_MF_BEATEN_FRONT;
		case MDIR_BACK:		return MOTION_NAME_MF_BEATEN_BACK;
		case MDIR_LEFT:		return MOTION_NAME_MF_BEATEN_LEFT;
		case MDIR_RIGHT:	return MOTION_NAME_MF_BEATEN_RIGHT;
		};

		return MOTION_NAME_MF_BEATEN_BACK;
	} 
	else if (nEntityTypeID == ETID_NPC)
	{
		if (nDirection == MDIR_FRONT) return MOTION_NAME_NPC_MF_BEATEN_FRONT;
		else return MOTION_NAME_NPC_MF_BEATEN_BACK;
	}

	return L"";
}

void XModuleAction::StartBuffAnimation( int nBuffID, wstring& strAniName )
{
	if(m_pModuleMotion == NULL ||
		strAniName.empty())
	{
		return;
	}

	// 애니메이션 이전에 일단 정지시킨다.
	Stop();

	if(XMotionMgr::IsTalentMotionName(strAniName.c_str()))
	{
		m_pModuleMotion->ChangeMotion(strAniName.c_str());
		m_pModuleMotion->SetIdleMemoryMotion(nBuffID, strAniName);
	}
	else
	{
		// 버프는 무기별 타입이 없다.
		wstring strBuffPreFix;

		if(m_pOwner->GetEntityType() == ETID_PLAYER)
		{
			strBuffPreFix = BUFF_ANIMATION_START_NAME;
		}
		// 그러나 NPC는 PreFix 참조(인간형 때문에...)
		else if(m_pOwner->GetEntityType() == ETID_NPC)
		{
			XNonPlayer* pNPC = m_pOwner->AsNPC();
			int nMode = pNPC->GetMode();
			strBuffPreFix = XNaming::ModeAnimationNameHeader(nMode, pNPC->GetInfo(true));
		}

		wstring strCurrAni = strBuffPreFix + strAniName;

		if (!m_pModuleMotion->ChangeToCustomMotion(MOTION_NAME_TALENT_USE, strCurrAni.c_str(), true))
		{
			Stop();
			return;
		}

		m_pModuleMotion->SetIdleMemoryCustomMotion(nBuffID, MOTION_NAME_TALENT_USE, strCurrAni.c_str(), true);

		wchar_t* szMotionName = MOTION_NAME_IDLE;
		if (m_pOwner->GetEntityType() == ETID_NPC) szMotionName = MOTION_NAME_NPC_IDLE;
		{
			m_pModuleMotion->PushMotion(szMotionName);
		}
	}
}

void XModuleAction::DoneDIe()
{
	// 이미 죽어있다.
	m_ActionDie.DoneDie();
}

wstring XModuleAction::GetCurrentMotionName()
{
	return m_pModuleMotion->GetCurrMotion();
}

float XModuleAction::GetMotionTime()
{
	return m_pModuleMotion->GetMotionTime();
}

bool XModuleAction::IsCurrMotionPlayDone()
{
	return m_pModuleMotion->IsCurrMotionPlayDone();
}

bool XModuleAction::IsCameraLook()
{
	return m_pModuleMotion->IsCameraLook();
}

const wchar_t* XModuleAction::GetMoveMotionName()
{
	if(IsSwim())
	{
		return MOTION_NAME_SWIM;
	}

	return MOTION_NAME_RUN;
}

bool XModuleAction::IsSwim()
{
	// 플레이어들만 수영을 한다.
	if(m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer * pPlayer = static_cast<XPlayer*>(m_pOwner);
		if(pPlayer &&
			pPlayer->IsSwim())
		{
			return true;
		}
	}

	return false;
}

const wchar_t* XModuleAction::GetIdelMotionName()
{
	if (m_pOwner->GetEntityType() == ETID_NPC) 
	{
		return MOTION_NAME_NPC_IDLE;
	}

	if(IsSwim())
	{
		return MOTION_NAME_SWIM_IDLE;
	}

	return MOTION_NAME_IDLE;
}

bool XModuleAction::ChangeMotionQuestInteraction()
{
	if(false == m_pModuleMotion->ChangeMotion(XCONST::INTERACT_QUEST_OBJECTIVE_ANI_NAME, MOTION_NAME_IDLE ))
	{
		return false;
	}

	return true;
}

bool XModuleAction::ChangeLootingMotion( const wchar_t* szAniName/*=NULL*/ )
{
	wstring szName = szAniName;

	if (szName.empty())
	{
		szName = MOTION_NAME_LOOTING_ITEM;
	}

	XMotion* pMotion = info.motion->GetMotion(szName.c_str());
	if (pMotion == NULL)
	{
		pMotion = info.motion->GetMotion(MOTION_NAME_LOOTING_ITEM);
		if (pMotion == NULL)
		{
			return false;
		}
		szName = MOTION_NAME_LOOTING_ITEM;
	}

	if(false == m_pModuleMotion->ChangeMotion(szName.c_str(), MOTION_NAME_IDLE ))
	{
		return false;
	}

	return true;
}

void XModuleAction::ActionBedSleep()
{
	StopMove();

	m_pModuleMotion->ChangeMotion(MOTION_NAME_BED_SLEEP, MT_DEFAULT, true);	
}

void XModuleAction::EndStun( bool bSendMotionEvent /*= true*/ )
{
	m_bMFStunNow = false;

	if(bSendMotionEvent)
	{
		XEvent msg;
		msg.m_nID = XEVTL_ON_MOTION_EVENT; 
		msg.m_pData = MOTION_EVENT_MF_STUN_EVENT;
		m_pOwner->Event(msg);
	}

	m_pModuleMovable->TriggerStunDeActive();

	XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect)
	{
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_END_MF_STUN);
	}
}

void XModuleAction::SetPause( bool bPause)
{
	m_pModuleMotion->SetPause( bPause);
}

void XModuleAction::ChangeMotionToIdleMotion()
{
	const wchar_t* motion = GetIdelMotionName();
	if (m_pOwner->GetEntityType() == ETID_NPC) motion = MOTION_NAME_NPC_IDLE;

	// 이동중이지 않으면 idle 모션으로 바꿈
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	if ( pActor->IsDead() == false)
		m_pModuleMotion->ChangeMotion( motion);
}

void XModuleAction::SitDown()
{
	m_pModuleMotion->ChangeMotion( MOTION_NAME_SIT_DOWN, MT_DEFAULT, false);
	m_pModuleMotion->PushMotion( MOTION_NAME_SIT_IDLE);
}

void XModuleAction::SitRise()
{
	m_pModuleMotion->ChangeMotion( MOTION_NAME_SIT_RISE, MT_DEFAULT, false);
	m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);
}

void XModuleAction::Swim()
{
	// 수영에서는 강제로 Idle로 변경한다.
	Idle();
	StopMove();
}

void XModuleAction::UpdateMyMovementAction(float fDelta, XMovableGoParam* pGoParam)
{
	_ASSERT( pGoParam != NULL );
	_ASSERT( pGoParam->GetParamType() == XMovableGoParam::E_GO_DIRECTION);
	_ASSERT( m_pOwner->GetEntityType() != ETID_NPC);

	if ( pGoParam == NULL)
		return;

	bool bMove = pGoParam->IsKeyPressed();
	if ( bMove)
	{
		SetMyMovementAction(pGoParam, bMove);
	}
	else
	{
		m_pModuleMovable->TriggerStop();


		// 모션 업데이트
		wstring strMotion = m_pModuleMotion->GetCurrMotion();

		if ( m_vPrevDir == vec3::ZERO)	m_vPrevDir = m_pOwner->GetDirection();
		float fRotate = m_vPrevDir.AngleToXY( m_pOwner->GetDirection()) / fDelta;
		m_vPrevDir = m_pOwner->GetDirection();

		bool bRotate = false;
		if ( abs( fRotate) > ( (strMotion == MOTION_NAME_ROTATE) ? 1.0f : 5.0f))
			bRotate = true;


		// 정지
		if ( bRotate == false)
		{
			if ( strMotion == MOTION_NAME_ROTATE)
			{
				m_pModuleMotion->ChangeMotion( GetIdelMotionName());
				m_MotionStopChecker.Reset();
			}
			else if (strMotion == GetMoveMotionName())
			{
				if ( m_MotionStopChecker.IsValid() == true)
				{
					m_pModuleMotion->ChangeMotion( GetIdelMotionName());
				}
			}
		}

		// 제자리 회전
		else if ( bRotate == true  &&  strMotion == MOTION_NAME_IDLE)
		{
			// 탤런트 사용중에는 로테이션 하면 안된다.
			XModuleTalent* pModuleTalent = m_pOwner->GetModuleTalent();
			if(pModuleTalent->IsActive() == false)
			{
				m_pModuleMotion->ChangeMotion( MOTION_NAME_ROTATE);
				m_MotionStopChecker.Reset();
			}
		}

		else
		{
			CheckChangeStanceMotion(false, GetIdelMotionName(), MT_DEFAULT);
			m_MotionStopChecker.Reset();
		}
	}
}

void XModuleAction::UpdateNetMovementAction( float fDelta )
{
	XModuleEntity* pEntity = m_pOwner->GetModuleEntity();
	if ( pEntity == NULL  ||  pEntity->IsLoadingCompleted() == false)
		return;


	if ( m_vPrevPos == vec3::ZERO)	m_vPrevPos = m_pOwner->GetPosition();
	vec3 vMove = ( m_pOwner->GetPosition() - m_vPrevPos) / fDelta;
	m_vPrevPos = m_pOwner->GetPosition();

	bool bMove = false;
	if ( (abs( vMove.x) > 10.0f  ||  abs( vMove.y) > 10.0f) &&							// 위치가 이동하였다.
		(m_pModuleMovable->GetMovableFactor().MOVING_DUMMY.m_bHaveDummy == false ||		// 그러나 더미가 없을 경우이다.
		(m_pModuleMovable->GetMovableFactor().MOVING_DUMMY.m_bHaveDummy == true &&		// 그래두 더미가 있는데... 이동처리가 패킷이 왔다면 이동 애니메이션을 한다.
		m_pModuleMovable->GetMovableFactor().INTERPOLATION.m_bArrived == false)))
		bMove = true;


	if ( m_vPrevDir == vec3::ZERO)	m_vPrevDir = m_pOwner->GetDirection();
	float fRotate = m_vPrevDir.AngleToXY( m_pOwner->GetDirection()) / fDelta;
	m_vPrevDir = m_pOwner->GetDirection();

	bool bRotate = false;
	if ( abs( fRotate) > 3.0f)
		bRotate = true;


	wstring strMotion = m_pModuleMotion->GetCurrMotion();
	XMotionTypes motion_type = GetMotionType( (XMotionTypes)m_pModuleMovable->GetMovableFactor().INTERPOLATION.m_nMovementFlags);

	// 정지
	if ( bMove == false)
	{
		// 완전 정지
		if ( bRotate == false  &&  (strMotion == MOTION_NAME_ROTATE  ||  strMotion == MOTION_NAME_RUN  ||  strMotion == MOTION_NAME_SWIM))
		{
			if ( m_MotionStopChecker.IsValid() == true)
			{
				ChangeMotionToIdleMotion();
			}
		}

		// 제자리 회전
		else if ( bRotate == true  && (strMotion == MOTION_NAME_IDLE  ||  strMotion == MOTION_NAME_RUN))
		{
			m_pModuleMotion->ChangeMotion( MOTION_NAME_ROTATE);
			m_MotionStopChecker.Reset();
		}

		else
		{
			CheckChangeStanceMotion(false, GetIdelMotionName(), motion_type);
			m_MotionStopChecker.Reset();
		}
	}

	// 이동
	else
	{
		// 모션 속도 구하기
		float fMotionSpeedRatio = 1.0f;
		XModuleBuff* pModuleBuff = m_pOwner->GetModuleBuff();
		if ( pModuleBuff)
		{
			XBuffAttribute attrBuff = pModuleBuff->GetBuffAttribute();
			fMotionSpeedRatio = attrBuff.m_fMoveSpeedRatio;
		}

		fMotionSpeedRatio *= XCONST::MOVE_MOTION_SPEED_RATIO;

		// 현재 idle이나 rotate 및 소셜 상태이면 run으로 즉시 모션 변경
		if ( XMotionHelper::IsIdleMotion(strMotion)  ||  strMotion == MOTION_NAME_ROTATE)
		{
			const wchar_t* motion = GetMoveMotionName();

			m_pModuleMotion->ChangeMotion( motion, motion_type);
		}

		// run 상태에서 달리는 방향이 다른 경우 즉시 모션 변경
		else if ( (strMotion == MOTION_NAME_RUN  ||  strMotion == MOTION_NAME_SWIM)  &&  (motion_type != m_pModuleMotion->GetCurrMotionType()) )
		{
			const wchar_t* motion = GetMoveMotionName();
			m_pModuleMotion->ChangeMotion( motion, motion_type);
		}

		// 수영에서 일반 상태로 올때
		else if ( strMotion == MOTION_NAME_SWIM)
		{
			const wchar_t* motion = GetMoveMotionName();
			m_pModuleMotion->ChangeMotion( motion, motion_type);;
		}
		else if(m_pModuleMovable->GetMovableFactor().MOVING_DUMMY.m_bHaveDummy == false)
		{
			if(CheckChangeStanceMotion(true, GetMoveMotionName(), motion_type) == false &&
				CheckUsableUpperMotion(true, GetMoveMotionName(), motion_type) == false)
			{
				bool bMoveSet = false;
				XModuleTalent* pModuleTalent = m_pOwner->GetModuleTalent();
				if(pModuleTalent && pModuleTalent->IsActive())
				{
					XTalentInfo* pInfo = pModuleTalent->GetInfo();
					if(pInfo->m_nMovableType != MUT_NONE)
						bMoveSet = true;
				}

				if(bMoveSet)
				{
					const wchar_t* motion = GetMoveMotionName();
					m_pModuleMotion->ChangeMotion( motion, motion_type);
				}
			}
		}
		else
		{
			CheckUsableUpperMotion(true, GetMoveMotionName(), motion_type);
		}

		// 현재 이동 애니메이션이라면...
		if(m_pModuleMotion->GetCurrMotion() == MOTION_NAME_RUN ||
			m_pModuleMotion->GetCurrMotion() == MOTION_NAME_SWIM)
		{
			// 애니메이션 속도 오리지널 가져오기
			float fOrgSpeed = m_pModuleMotion->GetOrgSpeed();

			// 이건 이동속도가 아니라 애니메이션 속도이다.
			// 이동에 관련된것에만 적용한다.
			fMotionSpeedRatio *= fOrgSpeed;
			m_pModuleMotion->SetSpeed( fMotionSpeedRatio);
		}

		m_MotionStopChecker.Reset();
	}
}


void XModuleAction::UpdateNonMovementAction( float fDelta )
{
	XModuleEntity* pEntity = m_pOwner->GetModuleEntity();
	if ( pEntity == NULL  ||  pEntity->IsLoadingCompleted() == false)
		return;


	if ( m_vPrevPos == vec3::ZERO)	m_vPrevPos = m_pOwner->GetPosition();
	vec3 vMove = ( m_pOwner->GetPosition() - m_vPrevPos) / fDelta;
	m_vPrevPos = m_pOwner->GetPosition();

	bool bMove = false;
	if ( abs( vMove.x) > 10.0f  ||  abs( vMove.y) > 10.0f)
		bMove = true;


	wstring strMotion = m_pModuleMotion->GetCurrMotion();

	// 정지
	if ( bMove == false)
	{
		if ( strMotion == MOTION_NAME_NPC_WALK  ||  strMotion == MOTION_NAME_NPC_RUN  ||  strMotion == MOTION_NAME_RUN)
		{
			if ( m_MotionStopChecker.IsValid() == true)
				m_pModuleMotion->ChangeMotion( MOTION_NAME_NPC_IDLE);
		}
		else
			m_MotionStopChecker.Reset();
	}

	// 이동
	else
	{
		// 모션 속도 구하기
		float fMotionSpeedRatio = 1.0f;
		XModuleBuff* pModuleBuff = m_pOwner->GetModuleBuff();
		if ( pModuleBuff)
		{
			XBuffAttribute attrBuff = pModuleBuff->GetBuffAttribute();
			fMotionSpeedRatio = attrBuff.m_fMoveSpeedRatio;
		}

		fMotionSpeedRatio *= XCONST::MOVE_MOTION_SPEED_RATIO;

		// 달리는 모션
		if ( m_pModuleMovable->GetMovableFactor().INTERPOLATION.m_bRunMotion == true)
		{
			if ( XMotionHelper::IsIdleMotion(strMotion)  ||  strMotion == MOTION_NAME_NPC_WALK)
			{
				m_pModuleMotion->ChangeMotion( MOTION_NAME_NPC_RUN);
			}
		}

		// 걷는 모션
		else
		{
			if ( XMotionHelper::IsIdleMotion(strMotion)  ||  strMotion == MOTION_NAME_NPC_RUN)
			{
				m_pModuleMotion->ChangeMotion( MOTION_NAME_NPC_WALK);
			}
		}

		// 현재 이동 애니메이션이라면...
		if(m_pModuleMotion->GetCurrMotion() == MOTION_NAME_NPC_RUN ||
			m_pModuleMotion->GetCurrMotion() == MOTION_NAME_NPC_WALK)
		{
			// 애니메이션 속도 오리지널 가져오기
			float fOrgSpeed = m_pModuleMotion->GetOrgSpeed();

			// 이건 이동속도가 아니라 애니메이션 속도이다.
			// 이동에 관련된것에만 적용한다.
			fMotionSpeedRatio *= fOrgSpeed;
			m_pModuleMotion->SetSpeed( fMotionSpeedRatio);
		}

		m_MotionStopChecker.Reset();
	}
}

void XModuleAction::GuardKnockBack()
{
	// PLAYER만 작동한다.
	// 가드 넉백은 가드가 풀린것이다. 그러므로 idle 상태로 돌아간다.
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);

		// 가드 성공 후에는 idle로 상태 이동하는게 자연스러움
		if (pPlayer->IsEquipShield())
		{
			m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_GUARD_KNOCKBACK);
		}
		else
		{
			m_pModuleMotion->ChangeMotion(MOTION_NAME_GUARD_KNOCKBACK);
		}	

		m_pModuleMotion->PushMotion(MOTION_NAME_IDLE);

		// 내꺼에만 적용한다.
		// 가드 끝나고 튀는 현상이 있어서 막기 위해서이다.(로테이션 애니가 들어가서 그렇다.)
		if(m_pOwner->GetUID() == XGetMyUID())
			m_vPrevDir = m_pOwner->GetDirection();
	}
}

bool XModuleAction::CheckUsableBeatenFake()
{
	// 모션팩터 MF_THROWUP, MF_KNOCKDOWN, MF_UPPERED 에서는 작동하지 않는다.
	// 애니메이션이 이상하게 보이기 때문이다.

	wstring strCurMotion = m_pModuleMotion->GetCurrMotion();
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		if(strCurMotion == MOTION_NAME_MF_THROWUP ||
			strCurMotion == MOTION_NAME_MF_KNOCKDOWN ||
			strCurMotion == MOTION_NAME_MF_UPPERED)
		{
			return false;
		}
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		if(strCurMotion == MOTION_NAME_NPC_MF_THROWUP ||
			strCurMotion == MOTION_NAME_NPC_MF_KNOCKDOWN ||
			strCurMotion == MOTION_NAME_NPC_MF_UPPERED)
		{
			return false;
		}
	}

	return true;
}

void XModuleAction::SetMyMovementActionByMouseMove( XMovableGoParam* pGoParam )
{
	// 여기에 오는 이동은 마우스에 의한 이동이다.
	_ASSERT( pGoParam != NULL );
	_ASSERT( pGoParam->GetParamType() == XMovableGoParam::E_GO_MOUSE_MOVE);
	_ASSERT( m_pOwner->GetEntityType() != ETID_NPC);

	if ( pGoParam == NULL)
		return;

	SetMyMovementAction(pGoParam, true);

}

void XModuleAction::SetMyMovementAction( XMovableGoParam* pGoParam, bool bMove )
{
	const XMotionTypes nMotionType = GetMotionType(pGoParam->m_nMovementFlags);
	const wchar_t* motion = GetMoveMotionName();

	// 모션 업데이트
	if(CheckChangeStanceMotion(bMove, motion, nMotionType) == false &&
		CheckUsableUpperMotion(bMove, motion, nMotionType) == false)
	{
		// 달리기 프레임 맞추기...
		// 이전 모션이 달리기였는데... 방향이 바뀌는 거라면 이전 모션프레임을 다음 모션프레임에 셋팅
		int nStartFrame = 0;
		XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();

		if(m_pModuleMotion->GetCurrMotion() == MOTION_NAME_RUN &&
			pModuleEntity && pModuleEntity->GetAnimationController())
		{
			nStartFrame = pModuleEntity->GetAnimationController()->GetAniFrame();
		}

		bMove = m_pModuleMotion->ChangeMotion( motion, nMotionType);

		if(nStartFrame > 0 && pModuleEntity && pModuleEntity->GetAnimationController())
		{
			pModuleEntity->GetAnimationController()->SetAnimationFrame(nStartFrame);
		}			
	}

	// 모션 속도 구하기
	XModuleBuff* pModuleBuff = m_pOwner->GetModuleBuff();
	if ( bMove  &&  pModuleBuff)
	{
		XBuffAttribute attrBuff = pModuleBuff->GetBuffAttribute();
		float fMotionSppedRatio = attrBuff.m_fMoveSpeedRatio;
		fMotionSppedRatio *= XCONST::MOVE_MOTION_SPEED_RATIO;
		m_pModuleMotion->SetSpeed( m_pModuleMotion->GetSpeed() * fMotionSppedRatio);
	}


	if (bMove)
		m_pModuleMovable->TriggerGo(pGoParam);


	m_MotionStopChecker.Reset();
}

bool XModuleAction::CheckChangeStanceMotion(bool bMove, const wchar_t* szMotionName, XMotionTypes nType)
{
	if(m_pOwner->GetType() != XOBJ_MY_PLAYER &&
		m_pOwner->GetType() != XOBJ_NET_PLAYER)
		return false;

	XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
	wstring strCurMotion = m_pModuleMotion->GetCurrMotion();
	CHAR_STANCE	nStance = pPlayer->GetStance();
	CHAR_STANCE nPreStance = (nStance == CS_NORMAL) ? CS_BATTLE : CS_NORMAL;

	DWORD dwSaveFrame = 0;
	if(bMove)
	{
		bool bQuick = false;
		bool bMotion = false;

		if(strCurMotion == MOTION_NAME_CHANGE_STANCE_TO_BATTLE ||
			strCurMotion == MOTION_NAME_CHANGE_STANCE_TO_NORMAL ||
			strCurMotion == MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE ||
			strCurMotion == MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL)
		{
			bMotion = true;
		}
		else if(strCurMotion == MOTION_NAME_CHANGE_STANCE_TO_BATTLE2 ||
			strCurMotion == MOTION_NAME_CHANGE_STANCE_TO_NORMAL2 ||
			strCurMotion == MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE2 ||
			strCurMotion == MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL2)
		{
			bQuick = true;
			bMotion = true;
		}

		if(bMotion)
		{
			XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();
			if(pModuleEntity == NULL)
				return false;

			dwSaveFrame = pModuleEntity->GetAnimationController()->GetAniFrame();
			ChangeStanceMoveOrStop(nStance, bQuick, true);
			pModuleEntity->GetAnimationController()->SetUpperFrame(dwSaveFrame);

			pPlayer->SetStance(nPreStance);
			m_pModuleMotion->ChangeMotion( szMotionName, nType);
			pPlayer->SetStance(nStance);

			return true;
		}
	}
	else
	{
		bool bQuick = false;
		bool bMotion = false;

		if(strCurMotion == MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE	||
			strCurMotion == MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL ||
			strCurMotion == MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE ||
			strCurMotion == MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL)
		{
			bMotion = true;
		}
		else if(strCurMotion == MOTION_NAME_RUN_CHANGE_STANCE_TO_BATTLE2	||
			strCurMotion == MOTION_NAME_RUN_CHANGE_STANCE_TO_NORMAL2 ||
			strCurMotion == MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_BATTLE2 ||
			strCurMotion == MOTION_NAME_RUN_SHIELD_CHANGE_STANCE_TO_NORMAL2)
		{
			bQuick = true;
			bMotion = true;
		}

		if(bMotion)
		{
			XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();
			if(pModuleEntity == NULL)
				return false;

			dwSaveFrame = pModuleEntity->GetAnimationController()->GetUpperFrame();

			ChangeStanceMoveOrStop(nStance, bQuick, false);

			pModuleEntity->GetAnimationController()->SetAnimationFrame(dwSaveFrame);
			return true;
		}
	}

	return false;
}

bool XModuleAction::CheckUsableUpperMotion( bool bMove, const wchar_t* szMotionName, XMotionTypes nType )
{
	if(m_pOwner->GetType() != XOBJ_MY_PLAYER &&
		m_pOwner->GetType() != XOBJ_NET_PLAYER)
		return false;

	XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
	wstring strCurMotion = m_pModuleMotion->GetCurrMotion();
	CHAR_STANCE	nStance = pPlayer->GetStance();
	CHAR_STANCE nPreStance = (nStance == CS_NORMAL) ? CS_BATTLE : CS_NORMAL;

	DWORD dwSaveFrame = 0;
	if(bMove)
	{
		bool bMotion = false;

		int nCurFrame = m_pOwner->GetActor()->GetCurFrame();

		if(strCurMotion == MOTION_NAME_ATTACK_1_RETURN ||
			strCurMotion == MOTION_NAME_ATTACK_2_RETURN ||
			strCurMotion == MOTION_NAME_ATTACK_3_RETURN ||
			strCurMotion == MOTION_NAME_ATTACK_4_RETURN ||
			strCurMotion == MOTION_NAME_ATTACK_5_RETURN ||
			strCurMotion == MOTION_NAME_ATTACK_1F_RETURN ||
			strCurMotion == MOTION_NAME_ATTACK_SWORDDRAWING)
		{
			bMotion = true;
		}

		if(bMotion)
		{
			XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();
			if(pModuleEntity == NULL)
				return false;

			dwSaveFrame = pModuleEntity->GetAnimationController()->GetAniFrame();

			m_pModuleMotion->ChangeMotion( szMotionName, nType);
			m_pModuleMotion->ChangeUpperMotion(strCurMotion.c_str());

			pModuleEntity->GetAnimationController()->SetUpperFrame(dwSaveFrame);
			return true;
		}
	}

	return false;
}

void XModuleAction::EndBuffAnimation(int nBuffID)
{
	m_pModuleMotion->RemoveIdleMemory(nBuffID);

	Stop();
}

bool XModuleAction::IsDieAniEnd()
{
	return m_ActionDie.IsDieAniEnd();
}

void XModuleAction::ChangeStanceMoveOrStop( CHAR_STANCE nStanceTo, bool bQuick, bool bMove )
{
	bool bShield = false;
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
		if (pPlayer->IsEquipShield())
		{
			bShield = true;
		}
	}

	if (nStanceTo == CS_NORMAL)
	{
		if(bMove)
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL);
			}
		}
		else
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_NORMAL, MOTION_NAME_IDLE);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_NORMAL, MOTION_NAME_IDLE);
			}
		}
	}
	else if (nStanceTo == CS_BATTLE)
	{
		if(bMove)
		{
			if (bShield)
			{
				if(bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE);
			}
			else
			{
				if(bQuick) m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE2);
				else m_pModuleMotion->ChangeUpperMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE);
			}
		}
		else
		{
			if (bShield)
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_SHIELD_CHANGE_STANCE_TO_BATTLE, MOTION_NAME_IDLE);
			}
			else
			{
				if (bQuick) m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE2, MOTION_NAME_IDLE);
				else m_pModuleMotion->ChangeMotion(MOTION_NAME_CHANGE_STANCE_TO_BATTLE, MOTION_NAME_IDLE);
			}
		}
	}
}

void XModuleAction::CheckPostMFAnimation( bool bSetAni )
{
	XActor* pOwnerActor = static_cast<XActor*>(m_pOwner);

	// 상체 애니메이션 삭제
	if(bSetAni)
	{
		StopUpperAni();
	}
}

bool XModuleAction::CheckUsableDieAnimation()
{
	// 다이 애니메이션을 바로 할 수 있는가?
	// 모션팩터가 있는 애니메이션은 확인해야 한다.

	wstring strCurMotion = m_pModuleMotion->GetCurrMotion();
	if (m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		if(strCurMotion == MOTION_NAME_MF_THROWUP ||
			strCurMotion == MOTION_NAME_MF_KNOCKDOWN ||
			strCurMotion == MOTION_NAME_MF_UPPERED ||
			strCurMotion == MOTION_NAME_MF_GRAPPLED ||
			strCurMotion == MOTION_NAME_MF_GRAPPLED_COLLISION)
		{
			return false;
		}
	}
	else if (m_pOwner->GetEntityType() == ETID_NPC)
	{
		if(strCurMotion == MOTION_NAME_NPC_MF_THROWUP ||
			strCurMotion == MOTION_NAME_NPC_MF_KNOCKDOWN ||
			strCurMotion == MOTION_NAME_NPC_MF_UPPERED)
		{
			return false;
		}
	}

	return true;
}
