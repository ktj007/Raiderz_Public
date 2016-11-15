#include "stdafx.h"
#include "XCmdHandler_Action.h"
#include "XEventID.h"
#include "XGlueGameEvent.h"
#include "XNonPlayer.h"
#include "XPlayer.h"
#include "XGame.h"
#include "XMyPlayer.h"
#include "XModuleNetControl.h"
#include "XModuleMyControl.h"
#include "XModuleCollision.h"
#include "XModuleAction.h"
#include "XTalentInfo.h"
#include "XPost_GM.h" // for 'XPostGMAISetMonitorNPC'
#include "XModuleNonControl.h"
#include "XModuleEffect.h"
#include "XProfiler.h"
#include "XStrings.h"
#include "XSystem.h"
#include "XPost_Loot.h"
#include "XTalentInfoMgr.h"
#include "XTalentHelper.h"
#include "XTalentHitParam.h"
#include "XNetwork.h"
#include "XWhoIsRequester.h"
#include "XInteractionInfo.h"

XCmdHandler_Action::XCmdHandler_Action(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_ACTION_MOVE,				OnMove);
	SetCmdHandler(MC_ACTION_STOP,				OnStop);
	SetCmdHandler(MC_ACTION_STOP_MOVE,			OnMoveStop);
	SetCmdHandler(MC_ACTION_ROTATE,				OnRotate);

	SetCmdHandler(MC_ACTION_CHANGE_STANCE,		OnChangeStance);

	SetCmdHandler(MC_ACTION_PREPARE_ATTACK,		OnPrepareAttack);
	SetCmdHandler(MC_ACTION_ATTACK_CANCEL,		OnAttackCancel);
	SetCmdHandler(MC_ACTION_BREAK_PART,			OnBreakPart);
	SetCmdHandler(MC_ACTION_BPART_RECOVERY,		OnBreakPartRecovery);
	SetCmdHandler(MC_ACTION_BREAK_PART_EX,		OnBreakPartEx);

	SetCmdHandler(MC_ACTION_ACT_TALENT,				OnActTalent);
	SetCmdHandler(MC_ACTION_EXTRA_ACT_TALENT,		OnExtraActTalent);
	SetCmdHandler(MC_ACTION_ACT_TALENT_WITH_GROUND,	OnActTalentWithGround);

	SetCmdHandler(MC_ACTION_USE_TALENT,				OnUseTalent);

	SetCmdHandler(MC_ACTION_ACT_TALENT_PROJECTILE,	OnActSpellProjectile);
	SetCmdHandler(MC_ACTION_ACT_SPELL_MAGICAREA,	OnActSpellMagicArea);
	
	SetCmdHandler(MC_ACTION_REQUEST_FAIL_TALENT,	OnRequestFailTalent);
	SetCmdHandler(MC_ACTION_CANCEL_TALENT,			OnCancelTalent);
	SetCmdHandler(MC_ACTION_RECASTING_SPELL,		OnReCastingSpell);
	SetCmdHandler(MC_ACTION_END_TALENT_COOLDOWN,	OnEndTalentCooldown);	

	SetCmdHandler(MC_ACTION_TALENT_HIT,					OnTalentHit);
	SetCmdHandler(MC_ACTION_TALENT_HIT_NO_MF,			OnHitTalentNoMotionFactor);
	SetCmdHandler(MC_ACTION_TALENT_HIT_KNOCKBACK_MF,	OnHitTalentKnockbackMotionFactor);
	SetCmdHandler(MC_ACTION_TALENT_HEAL,				OnTalentHeal);

	SetCmdHandler(MC_ACTION_BUFF_HIT,				OnBuffHit);
	SetCmdHandler(MC_ACTION_BUFF_HIT_NO_MF,			OnHitBuffNoMotionFactor);
	SetCmdHandler(MC_ACTION_TALENT_HIT_MISS,		OnTalentHitDodge);
	
	SetCmdHandler(MC_ACTION_ATTACK_ARCHERY,	OnAttackArchery);
	SetCmdHandler(MC_ACTION_ATTACK_ARCHERY_DETAIL,	OnAttackArcheryDetail);
	SetCmdHandler(MC_ACTION_GUARD,			OnGuard);
	SetCmdHandler(MC_ACTION_GUARD_FAILED,	OnGuardFailed);
	SetCmdHandler(MC_ACTION_GUARD_RELEASED,	OnGuardReleased);
	SetCmdHandler(MC_ACTION_JUMP,			OnJump);
	SetCmdHandler(MC_ACTION_FALLING,		OnFalling);
	SetCmdHandler(MC_ACTION_FALLING_DAMAGE,	OnFallingDamage);
	SetCmdHandler(MC_ACTION_DIE,			OnDie);
	SetCmdHandler(MC_ACTION_DESPAWN,		OnDespawn);
	SetCmdHandler(MC_ACTION_GUARD_DEFENSE,	OnGuardDefense);
	SetCmdHandler(MC_ACTION_GUARD_DEFENSE_PARTIAL, OnGuardDefensePartial);

	SetCmdHandler(MC_ACTION_CANCEL_NEXT_ATTACK,			OnCancelNextAttack);

	SetCmdHandler(MC_ACTION_STANDUP,			OnStandUp);

	SetCmdHandler(MC_ACTION_TALENT_DRAIN,		OnTalentDrain);	

	SetCmdHandler(MC_ACTION_SIT,			OnSit);
	SetCmdHandler(MC_ACTION_SIT_RISE,		OnSitRise);	

	SetCmdHandler(MC_ACTION_HIDE,			OnHide);	

	SetCmdHandler(MC_ACTION_AFK_BEGIN,			OnAFKBegin);	
	SetCmdHandler(MC_ACTION_AFK_END,			OnAFKEnd);	

	SetCmdHandler(MC_ACTION_END_COMBAT,			OnMyCombatEnd);	
	

	// test
	SetCmdHandler(MC_LOCAL_TEST_SELF_MOTION_FACTOR,		OnHitTalentKnockbackMotionFactor);
}

MCommandResult XCmdHandler_Action::OnMove(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidPlayer;
	UIID nPlayerUIID;
	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(1);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_PC_MOVE* pMI = (TD_PC_MOVE*)pParam->GetPointer();

	XActor* pPlayer = gg.omgr->FindActor_UIID(nPlayerUIID);

	if (pPlayer == NULL) 
	{
		// 모르는 Player면 요청
		if (global.net && global.net->GetNetClient())
		{
			XWhoIsRequester* pWhoIsRequester = global.net->GetNetClient()->GetWhoIsRequester();
			pWhoIsRequester->Request(nPlayerUIID);
		}

		return CR_TRUE;
	}

	XModuleActorControl* pModuleActorControl = pPlayer->GetModuleActorControl();
	if (pModuleActorControl)
	{
		pModuleActorControl->OnMove(pMI);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnStop(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nPlayerUIID;
	vec3 vPos;
	svec2 svDir;

	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,			1, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			2, MPT_SVEC2)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (pObject == NULL)
	{
		_ASSERT(0);
		return CR_TRUE;
	}

	XModuleNetControl* pModuleNetControl = pObject->GetModuleNetControl();
	if (pModuleNetControl)
	{
		vec3 vDir;
		vDir = svDir;
		pModuleNetControl->OnStop(&vPos, &vDir);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnRotate(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nPlayerUIID;
	svec2 svDir;

	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			1, MPT_SVEC2)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (pObject == NULL) 
	{
		_ASSERT(0);
		return CR_TRUE;
	}

	XModuleNetControl* pModuleNetControl = pObject->GetModuleNetControl();
	if (pModuleNetControl)
	{
		vec3 vDir;
		vDir = svDir;
		pModuleNetControl->OnRotate(vDir);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnChangeStance(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nPlayerUIID;
	int8 nValue=0;

	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nValue,			1, MPT_CHAR)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer_UIID(nPlayerUIID);
	if (pPlayer)
	{
		CHAR_STANCE nNextStance = CS_NORMAL;
		if (nValue == 0 || nValue == 2) nNextStance = CS_NORMAL;
		else if (nValue == 1 || nValue == 3) nNextStance = CS_BATTLE;
		pPlayer->ChangeStance(nNextStance, (nValue>=2) ? true : false);
	}
	
	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnPrepareAttack(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nPlayerUIID;
	int nTalentID;
	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;


	XActor* pActor = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (!pActor) return CR_TRUE;

	if (nPlayerUIID != XGetMyUIID())
	{
		XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
		if( pModuleActorControl)
		{
			vec3 vPos = pActor->GetPosition();
			vec3 vDir = pActor->GetDirection();
			pModuleActorControl->OnUseTalent(nTalentID, vPos, vDir);
		}
	}


	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnAttackCancel(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nPlayerUIID;
	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;

	if(XGetMyUIID() != nPlayerUIID)
	{
		XObject* pOtherObject = gg.omgr->FindActor_UIID(nPlayerUIID);
		if (pOtherObject)
		{
			// TODO : 여기서는 플레이어 콘트롤을 부르는게 맞나요?
			XModuleActorControl* pModuleActorControl = pOtherObject->GetModuleActorControl();
			if(pModuleActorControl)
				pModuleActorControl->Stop();
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnBreakPart(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidActor;
	MUID uidAttacker;
	int8 nPartID=0;

	if (pCommand->GetParameter(&uidActor,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidAttacker,	1, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nPartID,	2, MPT_CHAR)==false) return CR_ERROR;

	XObject* pObj = gg.omgr->Find(uidActor);
	if (pObj == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObj->GetModuleActorControl();
	if (pModuleActorControl)
	{
		pModuleActorControl->OnBreakParts(uidAttacker, nPartID);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnBreakPartRecovery(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidActor;

	if (pCommand->GetParameter(&uidActor,	0, MPT_UID)==false) return CR_ERROR;

	XObject* pObj = gg.omgr->Find(uidActor);
	if (pObj == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObj->GetModuleActorControl();
	if (pModuleActorControl)
	{
		pModuleActorControl->OnBreakPartsRecovery();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnBreakPartEx(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidActor;
	MUID uidAttacker;
	int8 nPartID=0;

	if (pCommand->GetParameter(&uidActor,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidAttacker,	1, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nPartID,	2, MPT_CHAR)==false) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(3);
	if(pParam->GetType() != MPT_BLOB) return CR_ERROR;

	TD_USABLE_BPART* pUsableBpart = (TD_USABLE_BPART*)pParam->GetPointer();
	int nUsableBpartCount = pParam->GetBlobCount();


	XObject* pObj = gg.omgr->Find(uidActor);
	if (pObj == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObj->GetModuleActorControl();
	if (pModuleActorControl)
	{
		pModuleActorControl->OnBreakPartEx(uidAttacker, nPartID, pUsableBpart, nUsableBpartCount);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnActTalent(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nTalentID;
	svec3 svDir;
	vec3 vDir;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			2, MPT_SVEC)==false) return CR_ERROR;
	vDir = svDir;

	XActor* pActor = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnActTalent(nTalentID, vDir);
	}

	if (nEntityUIID == XGetMyUIID())
	{
		// 소비
		XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
		gvar.MyInfo.SubChrStatus(pTalentInfo);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnUseTalent(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nTalentID=0;
	vec3 vDir = vec3::ZERO;
	vec3 vPos;
	svec3 svDir;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			2, MPT_SVEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,			3, MPT_VEC)==false) return CR_ERROR;

	//mlog("XCmdHandler_Action::OnUseTalent - %d\n", nTalentID);

	vDir = svDir;

	XActor* pActor = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if( pModuleActorControl)
	{
		pModuleActorControl->OnUseTalent(nTalentID, vPos, vDir);
	}

	if (nEntityUIID == XGetMyUIID())
	{
		// 소비
		XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);

		// 즉시형만
		if(pTalentInfo && pTalentInfo->m_fCastingTime == 0.0f)
			gvar.MyInfo.SubChrStatus(pTalentInfo);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnActSpellProjectile(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidEntity;
	MUID uidTarget	= MUID::ZERO;
	vec3 vDir		= vec3::ZERO;
	vec3 vTargetPos = vec3::ZERO;
	int nTalentID	= 0;
	int8 nGroupID	= -1;
	int8 nCapsuleID	= -1;

	if (pCommand->GetParameter(&uidEntity,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vDir,		2, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vTargetPos,	3, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidTarget,	4, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nGroupID,	5, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nCapsuleID,	6, MPT_CHAR)==false) return CR_ERROR;


	XActor* pActor = gg.omgr->FindActor(uidEntity);
	if (!pActor) return CR_TRUE;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
	if (pTalentInfo == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnActSpellProjectile(nTalentID, vDir, vTargetPos, uidTarget, nGroupID, nCapsuleID);
	}

	if (uidEntity == XGetMyUID())
	{
		// 소비
		XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
		gvar.MyInfo.SubChrStatus(pTalentInfo);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnActSpellMagicArea(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nTalentID=0;
	vec3 vTargetPos = vec3::ZERO;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vTargetPos,		2, MPT_VEC)==false) return CR_ERROR;


	XActor* pActor = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pActor) return CR_TRUE;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
	if (pTalentInfo == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnActSpellMagicArea(nTalentID, vTargetPos);
	}

	if (nEntityUIID == XGetMyUIID())
	{
		// 소비
		gvar.MyInfo.SubChrStatus(pTalentInfo);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnCancelTalent(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nTalentID=0;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pActor) return CR_TRUE;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
	if (pTalentInfo == NULL) return CR_TRUE;

	XModuleActorControl* pModulePlayerContorl = pActor->GetModuleActorControl();
	if(pModulePlayerContorl)
	{
		pModulePlayerContorl->OnCancelTalent();
	}

	if (XGetMyUIID() == nEntityUIID)
	{
		global.ui->OnSystemMsg( XGetStr( L"TALENT_CANCELLED"), FSParam(pTalentInfo->GetName()));

		// "즉시 시전 탈랜트"는 사용한 그 순간 이미 탈랜트가 발동된 것으로 보기 때문에 쿨타임을 초기화하지 않습니다.
		if ( CSTalentInfoHelper::IsImmediateTalent( pTalentInfo) == false)
			gvar.MyInfo.Talent.CoolTime.Canceled( nTalentID);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnRequestFailTalent(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nTalentID = 0;

	if (pCommand->GetParameter(&nTalentID,	0, MPT_INT)==false) return CR_ERROR;

	XActor* pActor = gvar.Game.pMyPlayer;
	if (!pActor) return CR_TRUE;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
	if (pTalentInfo == NULL) return CR_TRUE;

	XModuleActorControl* pModulePlayerContorl = pActor->GetModuleActorControl();
	if(pModulePlayerContorl)
	{
		pModulePlayerContorl->OnCancelTalent();
	}

	global.ui->OnSystemMsg( XGetStr( L"TALENT_FAILED"), FSParam(pTalentInfo->GetName()));

	gvar.MyInfo.Talent.CoolTime.Canceled( nTalentID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnTalentHit(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_TALENT_HIT* td_talent_hit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

	XObject* pObject = gg.omgr->FindActor_UIID(td_talent_hit->nVictimUIID);
	if (pObject == NULL) 
	{
		return CR_TRUE;
	}

	PFI_BLOCK_THISFUNC( 501 );

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		XTalentHitParam talentHitParam;
		talentHitParam.Build(td_talent_hit);

		pModuleActorControl->OnHit(&talentHitParam);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnHitTalentKnockbackMotionFactor(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_TALENT_HIT* td_talent_hit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

	MCommandParameter* pParam1 = pCommand->GetParameter(1);
	if(pParam1->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_TALENT_HIT_EX_KNOCKBACK* td_talent_hit_ex_knockback = static_cast<TD_TALENT_HIT_EX_KNOCKBACK*>(pParam1->GetPointer());


	XObject* pObject = gg.omgr->FindActor_UIID(td_talent_hit->nVictimUIID);
	if (pObject == NULL) 
	{
		return CR_TRUE;
	}

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		XTalentHitParam talentHitParam;
		talentHitParam.Build_KnockBack(td_talent_hit, td_talent_hit_ex_knockback);

		pModuleActorControl->OnHit(&talentHitParam);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnHitTalentNoMotionFactor(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nVictimUIID, nAttackerUIID;
	int nTalentID;

	if (pCommand->GetParameter(&nVictimUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nAttackerUIID,	1, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		2, MPT_INT)==false) return CR_ERROR;


	XObject* pObject = gg.omgr->FindActor_UIID(nVictimUIID);
	if (!pObject) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		XTalentHitParam talentHitParam;
		talentHitParam.Build_NoMotionFactor(nAttackerUIID, nVictimUIID, nTalentID);
		
		pModuleActorControl->OnHit(&talentHitParam);
	}


	return CR_TRUE;
}


MCommandResult XCmdHandler_Action::OnBuffHit(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_BUFF_HIT* td_buff_hit = (TD_BUFF_HIT*)pParam->GetPointer();

	XObject* pObject = gg.omgr->FindActor_UIID(td_buff_hit->nVictimUIID);
	if (!pObject) 
	{
		return CR_TRUE;
	}

	PFI_BLOCK_THISFUNC( 502 );

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		//_ASSERT((nTalentID != 0) && "XCmdHandler_Action::OnTalentHitted -> nTalentID == 0");

		pModuleActorControl->OnBuffHit(td_buff_hit, true);
	}

	XBuffInfo* pInfo = info.buff->Get(td_buff_hit->nBuffID);
	if (!pInfo)		return CR_ERROR;

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnHitBuffNoMotionFactor(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nTalentID;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;


	XObject* pObject = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pObject) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		XTalentHitParam talentHitParam;
		talentHitParam.Build_BuffNoMotionFactor(nEntityUIID, nTalentID);

		pModuleActorControl->OnHit(&talentHitParam);
		pModuleActorControl->OnBuffHit(nTalentID);
	}


	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnAttackArchery(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidEntity, uidTarget;
	vec3 vPos, vDir;
	int nTalentID=0;

	if (pCommand->GetParameter(&uidEntity,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,		2, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vDir,		3, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidTarget,	4, MPT_UID)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor(uidEntity);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnActArchery(nTalentID, vDir, uidTarget);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnAttackArcheryDetail(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidEntity, uidTarget;
	vec3 vPos, vDir;
	int nTalentID=0;
	int8 nCapsuleGroupIndex;
	int8 nCapsuleIndex;

	if (pCommand->GetParameter(&uidEntity,			0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,			1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,				2, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vDir,				3, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidTarget,			4, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nCapsuleGroupIndex,	5, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nCapsuleIndex,		6, MPT_CHAR)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor(uidEntity);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnActArcheryDetail(nTalentID, vDir, uidTarget, nCapsuleGroupIndex, nCapsuleIndex);
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_Action::OnMoveStop(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nPlayerUIID;
	vec3 pos;

	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&pos,			1, MPT_VECTOR)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleNetControl* pModuleNetControl = pObject->GetModuleNetControl();
	if (pModuleNetControl)
	{
		pModuleNetControl->OnMoveStop(&pos);
	}


	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnCancelNextAttack(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nParam;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nParam,			1, MPT_INT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nEntityUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleMyControl* pModuleMyControl = pObject->GetModuleMyControl();
	if (pModuleMyControl)
	{
		pModuleMyControl->SetCancelAction(nParam);
	}


	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnStandUp(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nEntityUIID);
	if (pObject == NULL) return CR_TRUE;

	// 죽었다면...
	XActor * pOwnerActor = static_cast<XActor*>(pObject);
	if(pOwnerActor->IsDead())
	{
		pOwnerActor->DeadProcEnterField();
		return CR_TRUE;
	}

	XEvent msg;
	msg.m_nID = XEVTL_ON_MOTION_EVENT; 
	msg.m_pData = MOTION_EVENT_MF_KNOCKDOWN_EVENT;
	pObject->Event(msg);

	return CR_TRUE;
}


MCommandResult XCmdHandler_Action::OnGuard(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	svec2 svDir;
	vec3 vPos, vDir;
	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			1, MPT_SVEC2)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,			2, MPT_VEC)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nEntityUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if (pModuleActorControl)
	{
		vDir = svDir;
		pModuleActorControl->OnGuard(vPos, vDir);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnGuardFailed(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nEntityUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if (pModuleActorControl)
	{
		pModuleActorControl->OnGuardFailed();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnGuardReleased(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nEntityUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if (pModuleActorControl)
	{
		pModuleActorControl->OnGuardReleased();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnJump(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nPlayerUIID;
	vec3 pos, dir;
	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&pos,			1, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&dir,			2, MPT_VEC)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleNetControl* pModuleNetControl = pObject->GetModuleNetControl();
	if (pModuleNetControl)
	{
		pModuleNetControl->OnJump(pos, dir);
	}
	
	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnFalling(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nPlayerUIID;
	vec3 pos, dir;
	svec3 svDir;

	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&pos,			1, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			2, MPT_SVEC)==false) return CR_ERROR;
	dir = svDir;

	XObject* pObject = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleNetControl* pModuleNetControl = pObject->GetModuleNetControl();
	if (pModuleNetControl)
	{
		pModuleNetControl->OnFalling(pos, dir);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnFallingDamage(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nPlayerUIID;
	int nDamage = 0;
	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nDamage,		1, MPT_INT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnFallingDamage(nDamage);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnDie(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID uidVictim;
	MUID uidAttacker;
	if ( pCommand->GetParameter( &uidVictim, 0, MPT_USHORT) == false)		return CR_ERROR;
	if ( pCommand->GetParameter( &uidAttacker, 1, MPT_UID) == false)		return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID( uidVictim);
	if ( pObject == NULL)					return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if ( pModuleActorControl != NULL)		pModuleActorControl->OnDie();


	XNonPlayer* pVictim = NULL;
	if ( gg.controller)
	{
		// 전투메세지 분리 _by tripleJ 110504
		if ( uidVictim == XGetMyUIID())
		{
			gvar.MyInfo.nDieTime = XGetNowTime();
			global.ui->OnBattleMsg( XGetStr( L"MSG_DIE"));
		}

		XObject* pObj = gg.omgr->FindActor_UIID( uidVictim);
		if ( pObj  &&  pObj->GetEntityType() == ETID_NPC)
		{
			pVictim = AsNPC( pObj);

			if (pVictim->GetInfo()
				&& (pVictim->GetInfo()->nNpcType != NPC_TYPE_OBJECT) 
				&& (pVictim->GetInfo()->nGatherType == GATHER_NONE))
			{
				global.ui->OnBattleMsg( XGetStr( L"MSG_DIEOTHER"), FSParam(pVictim->GetInfo()->GetName()));
			}
		}
	}


	// 내가 죽였으면 카운트 증가		_by tripleJ 110516 Die로 죽었을 경우 카운팅 금지...
	if ( uidAttacker.IsValid() == true  &&  uidAttacker == XGetMyUID() && ( uidVictim != XGetMyUIID()) )
	{
		gvar.MyInfo.nKilledEnemy++;

		if ( pVictim != NULL  && pVictim->GetInfo() != NULL && pVictim->GetInfo()->nRarity == NPC_RARITY_EPIC)
		{
			gvar.MyInfo.nKilledEpicEnemy++;
		}
	}


	if(XGetInteractionInfo().GetTargetUIID() == uidVictim)
	{
		if(XGetInteractionInfo().stTargetActorInfo.bLockOnTarget == true)
		{
			XGetInteractionInfo().stTargetActorInfo.bLockOnTarget = false;

			if(global.script)	global.script->GetGlueGameEvent().OnEnemyInfoRefresh();
		}

		if(global.script)	global.script->GetGlueGameEvent().OnTargetInfoRefresh();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnDespawn(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nEntityUIID;
	if (pCommand->GetParameter(&nEntityUIID,		0, MPT_USHORT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->FindActor_UIID(nEntityUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnSetLootable(false);
		pModuleActorControl->OnSetDeSpawn();
		pModuleActorControl->OnDespawn();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnReCastingSpell(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nTalentID	= 0;
	int nStepID		= 0;
	UIID nEntityUIID = UIID_INVALID;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nStepID,		2, MPT_INT)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnReCastingTalent(nTalentID, nStepID);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnGuardDefense(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_TALENT_GUARD* td_talent_guard = (TD_TALENT_GUARD*)pParam->GetPointer();

	XActor* pActor = gg.omgr->FindActor_UIID(td_talent_guard->nGuardUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->GuardDefense(td_talent_guard);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnGuardDefensePartial(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_TALENT_PARTIAL_GUARD* td_talent_partial_guard = (TD_TALENT_PARTIAL_GUARD*)pParam->GetPointer();

	XActor* pActor = gg.omgr->FindActor_UIID(td_talent_partial_guard->nGuardUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->GuardDefense(td_talent_partial_guard);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnEndTalentCooldown(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nDelegateTalentID	= 0;

	if (pCommand->GetParameter(&nDelegateTalentID,	0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.Talent.CoolTime.Canceled( nDelegateTalentID);
	return CR_TRUE;
}



MCommandResult XCmdHandler_Action::OnExtraActTalent(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nTalentID=0;
	vec3 vPos = vec3::ZERO;
	vec3 vDir = -vec3::AXISY;
	svec3 svDir;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,			2, MPT_VECTOR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			3, MPT_SVEC)==false) return CR_ERROR;
	vDir = svDir;

	XActor* pActor = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pActor) return CR_TRUE;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
	if (pTalentInfo == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnExtraActTalent(pTalentInfo, vPos, vDir);
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_Action::OnTalentHitDodge(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nVictimUIID, nAttackerUIID;
	int nAttackTalentID;

	if (pCommand->GetParameter(&nVictimUIID,		0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nAttackerUIID,		1, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nAttackTalentID,	2, MPT_INT)==false) return CR_ERROR;

	XActor* pVictim = gg.omgr->FindActor_UIID(nVictimUIID);
	if (!pVictim) return CR_TRUE;

	XModuleEffect* pModuleEffect = pVictim->GetModuleEffect();
	XModuleCollision* pModuleCollision = pVictim->GetModuleCollision();

	if(pModuleEffect && pModuleCollision)
	{
		_DamageInfo info;
		info.nAttackerUIID = nAttackerUIID;
		info.nFlags = 0;
		info.bHitDodge = true;

		XObject* pAttacker = gg.omgr->FindActor_UIID(nAttackerUIID);
		if ( pAttacker != NULL)
		{
			vec3 vHitPos = pModuleCollision->GetMeleeHitEffectPosition(pAttacker, nAttackTalentID);
			vec3 vDamagePos = pModuleEffect->GetDamageCaptionEffectPos(vHitPos);
			XHitDamageEffectEventData * pEffectEvent = new XHitDamageEffectEventData;
			pEffectEvent->m_Info = info;
			pEffectEvent->m_vOffset = vDamagePos;
			pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_SHOW_DAMAGE, pEffectEvent);
		}
	}

	return CR_TRUE;

}

MCommandResult XCmdHandler_Action::OnTalentDrain(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nTalentID;
	TALENT_DRAIN_TYPE nDrainType;
	int nDrainAmount;

	if (pCommand->GetParameter(&nTalentID,		0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nDrainType,		1, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nDrainAmount,	2, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.SetHP(gvar.MyInfo.ChrStatus.nHP + nDrainAmount);

	if (nDrainAmount > 0)
	{
		global.ui->OnSystemMsg(XGetStr(L"SMSG_TALENT_DRAIN"), FSParam(nDrainAmount, XStrings::DrainTypeToChatString(nDrainType)));
	}	

	return CR_TRUE;
}

//넷플레이어 상태
MCommandResult XCmdHandler_Action::OnSit(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nPlayerUIID;
	if (pCommand->GetParameter(&nPlayerUIID,		0, MPT_USHORT)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->DoActionSitDown();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnSitRise(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nPlayerUIID;
	if (pCommand->GetParameter(&nPlayerUIID,		0, MPT_USHORT)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor_UIID(nPlayerUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->DoActionSitRise();
	}
	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnHide(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;
	if (gg.currentgamestate == NULL) return CR_TRUE;

	MUID uidActor;
	if (pCommand->GetParameter(&uidActor,		0, MPT_UID)==false) return CR_ERROR;

	if (uidActor == XGetMyUID()) return CR_TRUE;	

	XActor* pActor = gg.omgr->FindActor(uidActor);
	if (pActor)
	{
		if (pActor->IsPlayer())
		{
			gg.currentgamestate->OutPlayer(uidActor);
		}
		else if (pActor->IsNPC())
		{
			gg.currentgamestate->OutNPC(uidActor);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnActTalentWithGround(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nEntityUIID;
	int nTalentID;
	svec3 svDir;
	vec3 vPos;

	if (pCommand->GetParameter(&nEntityUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			2, MPT_SVEC)==false) return CR_ERROR;	
	if (pCommand->GetParameter(&vPos,			3, MPT_VECTOR)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor_UIID(nEntityUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		vec3 vDir;
		vDir = svDir;
		pModuleActorControl->OnActGoundTarget(nTalentID, vDir, vPos);
	}

	if (nEntityUIID == XGetMyUIID())
	{
		// 소비
		XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(nTalentID, pActor);
		gvar.MyInfo.SubChrStatus(pTalentInfo);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnTalentHeal(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nUserUIID;
	UIID nTargetUIID;
	int nTalentID;
	int nAmount;
	if (pCommand->GetParameter(&nUserUIID,		0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTargetUIID,	1, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTalentID,		2, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nAmount,		3, MPT_INT)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor_UIID(nTargetUIID);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnTalentHeal(nUserUIID, nTalentID, nAmount);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnAFKBegin(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer(uidPlayer);
	if (!pPlayer)	return CR_ERROR;

	XModuleActorControl* pModuleActorControl = pPlayer->GetModuleActorControl();
	if (!pModuleActorControl)	return CR_ERROR;
	pModuleActorControl->SetAFK(true);	

	return CR_TRUE;
}

MCommandResult XCmdHandler_Action::OnAFKEnd(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer(uidPlayer);
	if (!pPlayer)	return CR_ERROR;

	XModuleActorControl* pModuleActorControl = pPlayer->GetModuleActorControl();
	if (!pModuleActorControl)	return CR_ERROR;
	pModuleActorControl->SetAFK(false);	

	return CR_TRUE;
}


MCommandResult XCmdHandler_Action::OnMyCombatEnd(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	gvar.Game.MyCombatStanceAutoReleaserData.bReserved = true;
	gvar.Game.MyCombatStanceAutoReleaserData.nReserveTime = XGetNowTime();

	return CR_TRUE;
}
