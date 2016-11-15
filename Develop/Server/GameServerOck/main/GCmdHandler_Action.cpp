#include "StdAfx.h"
#include "GCmdHandler_Action.h"
#include "CCommandTable.h"
#include "MiNetCommon.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GEntity.h"
#include "GModuleCombat.h"
#include "GPaletteSystem.h"
#include "GPalette.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GSystem.h"
#include "GMath.h"
#include "GDBManager.h"
#include "GPlayerFalling.h"
#include "GPlayerDoing.h"
#include "GTalent.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "GMovableComparer.h"
#include "GPlayerSystem.h"
#include "GConst.h"
#include "GPlayerAFK.h"
#include "GEquipmentSlot.h"
#include "GPlayerStance.h"

#define REQUIRED_RANGE_ATTACK_PREPARE_TIME	500 //ms

GCmdHandler_Action::GCmdHandler_Action(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_ACTION_MOVE_REQ,		OnRequestMove);
	SetCmdHandler(MC_ACTION_STOP_REQ,		OnRequestStop);
	SetCmdHandler(MC_ACTION_STOP_MOVE_REQ,	OnRequestMoveStop);
	SetCmdHandler(MC_ACTION_ROTATE_REQ,		OnRequestRotate);
	SetCmdHandler(MC_ACTION_POS_SYNC_REQ,	OnRequestPositionSync);
	SetCmdHandler(MC_ACTION_MOVING_MF_POS_SYNC_REQ,	OnRequestPositionSyncWhenFinishMovingMotionFactor);

	SetCmdHandler(MC_ACTION_CHANGE_STANCE_REQ,			OnRequestChangeStance);
	SetCmdHandler(MC_ACTION_ATTACK_MELEE_REQ,			OnRequestAttackMelee);
	SetCmdHandler(MC_ACTION_ATTACK_ARCHERY_REQ,			OnRequestAttackArchery);
	SetCmdHandler(MC_ACTION_ATTACK_ARCHERY_DETAIL_REQ,	OnRequestAttackArcheryDetail);
//	SetCmdHandler(MC_ACTION_ATTACK_GUN_REQ,				OnRequestAttackGun);
	SetCmdHandler(MC_ACTION_ATTACK_CANCEL_REQ,			OnRequestAttackCancel);
	SetCmdHandler(MC_ACTION_PREPARE_ATTACK_REQ,			OnRequestAttackPrepare);
	SetCmdHandler(MC_ACTION_JUMP_REQ,					OnRequestJump);
	SetCmdHandler(MC_ACTION_FALLING_REQ,				OnRequestFalling);
	SetCmdHandler(MC_ACTION_END_FALLING,				OnEndFalling);
	SetCmdHandler(MC_ACTION_GUARD_REQ,					OnRequestGuard);
	SetCmdHandler(MC_ACTION_GUARD_RELEASED_REQ,			OnRequestGuardReleased);

	SetCmdHandler(MC_ACTION_USE_TALENT_REQ,				OnRequestUseTalent);
	SetCmdHandler(MC_ACTION_ACT_TALENT_REQ,				OnRequestActTalent);
	SetCmdHandler(MC_ACTION_ACT_TALENT_WITH_HITCAPSULE_REQ,	OnRequestActTalentWithHitcapsule);
	SetCmdHandler(MC_ACTION_ACT_TALENT_WITH_GROUND_REQ,	OnRequestActTalentWithGround);
	SetCmdHandler(MC_ACTION_CANCEL_TALENT_REQ,			OnRequestCancelTalent);
	SetCmdHandler(MC_ACTION_STANDUP_REQ,				OnRequestStandUp);
	SetCmdHandler(MC_ACTION_SWITCHING_WEAPON_BEGIN_REQ,	OnRequestSwitchingWeaponBegin);
	SetCmdHandler(MC_ACTION_SWITCHING_WEAPON_REQ,		OnRequestSwitchingWeapon);
	SetCmdHandler(MC_ACTION_SWIM_REQ,					OnRequestSwim);

	SetCmdHandler(MC_ACTION_SIT_REQ,					OnRequestSit);
	SetCmdHandler(MC_ACTION_SIT_RISE_REQ,				OnRequestSitRise);
	SetCmdHandler(MC_ACTION_AFK_REQ,					OnRequestAFK);
}

MCommandResult GCmdHandler_Action::OnRequestMove(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = NULL;
	{
		pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
		if (NULL == pPlayer) return CR_FALSE;
		if (!pPlayer->GetField()) return CR_FALSE;
	}
	
//#ifndef _PUBLISH
//	GMovableComparer comparer(pPlayer);
//#endif

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"HasMotionfactor");
//#endif

	if (pPlayer->IsBeginUnableAction()) 
		return CR_TRUE;

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"IsBeginUnableAction");
//#endif

	MUID uidPlayer = pCmd->GetSenderUID();

	MCommandParameter* pParam = pCmd->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;

	TD_PC_MOVE* pMI = (TD_PC_MOVE*)pParam->GetPointer();

	vec3& vTarPos = pMI->vTarPos;
	vec3 vDir(pMI->svDir.X(), pMI->svDir.Y(), 0.0f);

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"Copy Parameter");
//#endif

	if (pPlayer->IsGuarding())
	{
		// 너무 조금 움직인 것은 그냥 가드상태일 수도 있다.
		float dist = vTarPos.DistanceToSq(pPlayer->GetPos());
		if (vTarPos.DistanceToSq(pPlayer->GetPos()) > 2500.0f) 
		{
			pPlayer->doGuardReleased();
		}
	}

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"Cancel Guard");
//#endif

	// 이동하면 탤런트 취소
	GTalent* pTalent = pPlayer->GetCurrentTalent();
	if (pTalent)
	{
		VALID_RET(pTalent->GetInfo(), CR_FALSE);

		if (pTalent->IsNowCasting() &&
			!pTalent->GetInfo()->IsMovingWithCasting())
		{
			if (IsRunForTest())
			{
				pPlayer->FailAndRouteSystemMsg(CR_FAIL_TALENTCANCEL_MOVE);
			}

			pPlayer->doCancelTalentForce(true);
		}
	}

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"Cancel UseTalent");
//#endif

	// Movement 모듈에서 처리하도록 하자.
//	pPlayer->GetField()->PickingZ(vTarPos, vTarPos);
	pPlayer->SetPos(vTarPos);
	pPlayer->SetDir(vDir);
	pPlayer->SetFacingDir(vDir);
	pPlayer->OnDoSomething();

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"Set Pos/Dir");
//#endif

	pPlayer->CheatCheck_Walk();

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"CheatCheck_Walk");
//#endif

	// 점프, Falling, 탤런트 사용, 피격시에는 이동 정보를 다른 플레이어에게 보내지 않는다.
	if ((pMI->nFlags & MOVEMENTFLAG_JUMPING) || 
		(pMI->nFlags & MOVEMENTFLAG_FALLING) ||
		(pMI->nFlags & MOVEMENTFLAG_USING_TALENT) || 
		(pMI->nFlags & MOVEMENTFLAG_MOTION_FACTOR))
	{
		return CR_FALSE;
	}


	// 이동 시작
	pPlayer->doMove();


	const UIID nPlayerUIID = pPlayer->GetUIID();

	// 플레이어에게 이동 정보를 보내준다.
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_MOVE, 2, NEW_USHORT(nPlayerUIID), NEW_SINGLE_BLOB(pMI, sizeof(TD_PC_MOVE)));

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"MakeNewCommand");
//#endif

	pPlayer->RouteToThisCellExceptMe(pNewCommand);

//#ifndef _PUBLISH
//	comparer.SaveTimeSlice(L"RouteToThisCellExceptMe");
//#endif

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestStop(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (!pPlayer->GetField()) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();
	vec3 vPos;
	svec2 svDir;

	if (pCmd->GetParameter(&vPos, 0, MPT_VEC)==false) return CR_FALSE;
	if (pCmd->GetParameter(&svDir, 1, MPT_SVEC2)==false) return CR_FALSE;

// 이동 불가상태에서 플레이어가 이동(넉백, NPC 밀기등)되는 문제로 주석처리합니다. (동환씨 요청)
//	if (pPlayer->HasMovingMotionfactor())		
//	{
//		return CR_TRUE;	// 이동관련 모션팩터(넉백,쓰로우업) 적용중이면 무시한다.
//	}
	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	vec3 vDir;
	vDir = svDir;

	// Movement 모듈에서 처리하도록 하자.
	pPlayer->SetPos(vPos);
	pPlayer->SetDir(vDir);
	pPlayer->SetFacingDir(vDir);
	pPlayer->CheatCheck_Walk();
	pPlayer->OnDoSomething();


	// 이동 끝
	pPlayer->doStop();


	const UIID nPlayerUIID = pPlayer->GetUIID();

	// 플레이어에게 이동 정보를 보내준다.
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_STOP, 3, NEW_USHORT(nPlayerUIID), NEW_VEC(vPos), NEW_SVEC2(svDir));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestMoveStop(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (!pPlayer->GetField()) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();
	vec3 vPos;

	if (pCmd->GetParameter(&vPos, 0, MPT_VEC)==false) return CR_FALSE;

	if (pPlayer->HasMovingMotionfactor())		
	{
		return CR_TRUE;	// 이동관련 모션팩터(넉백,쓰로우업) 적용중이면 무시한다.
	}

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	// Movement 모듈에서 처리하도록 하자.
	pPlayer->SetPos(vPos);
	pPlayer->CheatCheck_Walk();

	
	// 이동 끝
	pPlayer->doStop();


	const UIID nPlayerUIID = pPlayer->GetUIID();

	// 플레이어에게 이동 정보를 보내준다.
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_STOP_MOVE, 2, NEW_USHORT(nPlayerUIID), NEW_VEC(vPos));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestRotate(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (!pPlayer->GetField()) return CR_FALSE;

	if (pPlayer->HasMovingMotionfactor())		
		return CR_TRUE;	// 이동관련 모션팩터(넉백,쓰로우업) 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	MUID uidPlayer = pCmd->GetSenderUID();
	svec2 svDir;
	vec3 vDir;

	if (pCmd->GetParameter(&svDir, 0, MPT_SVEC2)==false) return CR_FALSE;

	vDir = svDir;

	// Movement 모듈에서 처리하도록 하자.
	pPlayer->SetDir(vDir);
	pPlayer->SetFacingDir(vDir);
	pPlayer->OnDoSomething();

	const UIID nPlayerUIID = pPlayer->GetUIID();

	// 플레이어에게 정보를 보내준다.
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ROTATE, 2, NEW_USHORT(nPlayerUIID), NEW_SVEC2(vDir));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestPositionSync(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (!pPlayer->GetField()) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();
	vec3 vPos;

	if (pCmd->GetParameter(&vPos, 0, MPT_VEC)==false) return CR_FALSE;

	if (pPlayer->HasMovingMotionfactor())		
	{
		return CR_TRUE;	// 이동관련 모션팩터(넉백,쓰로우업) 적용중이면 무시한다.
	}

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	pPlayer->SetPos(vPos);
	pPlayer->CheatCheck_Walk();
	pPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestPositionSyncWhenFinishMovingMotionFactor(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (!pPlayer->GetField()) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();
	vec3 vPos;

	if (pCmd->GetParameter(&vPos, 0, MPT_VEC)==false) return CR_FALSE;

	if (vPos.DistanceToSq(pPlayer->GetPos()) <= 900.0f)	// 차이가 30 이하이면 무시
	{
		return CR_TRUE;
	}

	if (pPlayer->HasMovingMotionfactor())
	{
		pPlayer->SetReservedPosSync(vPos);
		return CR_TRUE;
	}

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	pPlayer->SyncMovingMotionFactorFinishPos(vPos);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestChangeStance(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int8 nValue = 0;
	if (pCmd->GetParameter(&nValue,0, MPT_CHAR)==false) return CR_FALSE;

	if (!pPlayer->IsStanceChangable())	return CR_FALSE;
	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	CHAR_STANCE nNextStance = CS_NORMAL;
	if (nValue == 0 || nValue == 2) nNextStance = CS_NORMAL;
	else if (nValue == 1 || nValue == 3) nNextStance = CS_BATTLE;

	if (nValue == 2 || nValue == 3)
	{
		if (pPlayer->HasPassiveTalent(TEPT_QUICK_DRAW) == false) return CR_FALSE;
	}

	pPlayer->GetStance().Change(nNextStance);
	pPlayer->OnDoSomething();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_CHANGE_STANCE, 2, NEW_USHORT(pPlayer->GetUIID()), NEW_CHAR(nValue));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestAttackMelee(MCommand* pCmd, MCommandHandler* pHandler)
{
	PFI_B(3000, "GCmdHandler_Action::OnRequestAttackMelee");

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	MUID uidPlayer = pCmd->GetSenderUID();

	MCommandParameter* pParam = pCmd->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_REQUEST_ATTACK* pRA = (TD_REQUEST_ATTACK*)pParam->GetPointer();

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	WEAPON_TYPE nCurrWeaponType = pItemHolder->GetEquipment().GetWeaponType();

	// 발도술 사용가능한지 체크
	if (pRA->nAttackType == NA_SWORD_DRAWING)
	{
		if (pPlayer->GetStance().Get() == CS_BATTLE || (pPlayer->HasPassiveTalent(TEPT_SWORD_DRAWING) == false)) return CR_FALSE;
	}
	else if (pRA->nAttackType == NA_UPPER)
	{
		if (pPlayer->HasPassiveTalent(TEPT_UPPERCUT) == false) return CR_FALSE;
	}
	else if (pRA->nAttackType == NA_RISING)
	{
		if (pPlayer->HasPassiveTalent(TEPT_RISING_ATTACK) == false) return CR_FALSE;
	}
	else if (pRA->nAttackType == NA_COUNTER_ATTACK)
	{
		if (pPlayer->HasPassiveTalent(TEPT_COUNTER) == false) return CR_FALSE;
	}

	int nTalentID = 0;
	bool bEquipShield = false;

	if (pRA->nAttackType == NA_COUNTER_ATTACK)
	{
		bEquipShield = pItemHolder->GetEquipment().IsEquipShield();
	}

	// 기본 공격도 스킬이라 생각함
	nTalentID = CSTalentInfoHelper::NormalAttackTalentID(nCurrWeaponType, pRA->nAttackType, bEquipShield);
	if (nTalentID == 0)
	{
		return CR_FALSE;
	}

	if (pRA->nAttackType == NA_SWORD_DRAWING)
	{
		pPlayer->GetStance().Change(CS_BATTLE);
	}

	pPlayer->SetPos(pRA->vPos);

	vec3 vDir;
	vDir = pRA->svDir;

	pPlayer->SetDir(vDir);
	pPlayer->SetFacingDir(vDir);
	pPlayer->CheatCheck_Walk();
	pPlayer->OnDoSomething();

	if (pPlayer->doUseTalent(nTalentID) == false) return CR_FALSE;

	PFI_E(3000);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestAttackArchery(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	vec3 vPos, vDir;
	MUID uidTarget;
	if (pCmd->GetParameter(&vPos,0, MPT_VECTOR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vDir,1, MPT_VECTOR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&uidTarget,2, MPT_UID)==false) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();

	// 기본 공격도 스킬이라 생각함
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	WEAPON_TYPE nCurrWeaponType = pItemHolder->GetEquipment().GetWeaponType();
	if (nCurrWeaponType != WEAPON_ARCHERY) return CR_FALSE;		// 활 장비중이 아니면 무시
	int nTalentID = CSTalentInfoHelper::NormalAttackTalentID(nCurrWeaponType, NA_NORMAL1);

	bool bAttackPrepareOK = true;

	// 준비시간이 충분하지 못함
	if (pPlayer->m_nLastPrepareAttackTime == 0 ||
		gsys.pSystem->GetNowTime() - pPlayer->m_nLastPrepareAttackTime < REQUIRED_RANGE_ATTACK_PREPARE_TIME)
	{
		bAttackPrepareOK = false;
	}

	if (bAttackPrepareOK == false)
	{
		pPlayer->m_nLastPrepareAttackTime = 0;
		return CR_FALSE;
	}

	pPlayer->SetFacingDir(vDir);
	vec3 vCharDir = vDir;
	GMath::NormalizeZ0(vCharDir);
	pPlayer->SetDir(vCharDir);
	pPlayer->SetPos(vPos);
	pPlayer->CheatCheck_Walk();

	if (pPlayer->doUseTalent(nTalentID, uidTarget) == false) return CR_FALSE;

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestAttackArcheryDetail(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	vec3 vPos, vDir;
	MUID uidTarget;
	int8 nCapsuleGroupIndex, nCapsuleIndex;
	if (pCmd->GetParameter(&vPos,				0, MPT_VECTOR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vDir,				1, MPT_VECTOR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&uidTarget,			2, MPT_UID)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nCapsuleGroupIndex,	3, MPT_CHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nCapsuleIndex,		4, MPT_CHAR)==false) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();

	// 기본 공격도 스킬이라 생각함
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	WEAPON_TYPE nCurrWeaponType = pItemHolder->GetEquipment().GetWeaponType();
	if (nCurrWeaponType != WEAPON_ARCHERY) return CR_FALSE;		// 활 장비중이 아니면 무시
	int nTalentID = CSTalentInfoHelper::NormalAttackTalentID(nCurrWeaponType, NA_NORMAL1);

	bool bAttackPrepareOK = true;

	// 준비시간이 충분하지 못함
	if (pPlayer->m_nLastPrepareAttackTime == 0 ||
		gsys.pSystem->GetNowTime()- pPlayer->m_nLastPrepareAttackTime < REQUIRED_RANGE_ATTACK_PREPARE_TIME)
	{
		bAttackPrepareOK = false;
	}

	if (bAttackPrepareOK == false)
	{
		pPlayer->m_nLastPrepareAttackTime = 0;
		return CR_FALSE;
	}

	pPlayer->SetFacingDir(vDir);
	vec3 vCharDir = vDir;
	GMath::NormalizeZ0(vCharDir);
	pPlayer->SetDir(vCharDir);
	pPlayer->SetPos(vPos);
	pPlayer->CheatCheck_Walk();

	TALENT_TARGET_INFO TalentTargetInfo;
	TalentTargetInfo.uidTarget = uidTarget;
	TalentTargetInfo.nCapsuleGroupIndex = nCapsuleGroupIndex;
	TalentTargetInfo.nCapsuleIndex = nCapsuleIndex;

	if (pPlayer->doUseTalent(nTalentID, TalentTargetInfo) == false)	return CR_FALSE;

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestAttackCancel(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMovingMotionfactor())		
		return CR_TRUE;	// 이동관련 모션팩터(넉백,쓰로우업) 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	UIID nPlayerUIID = pPlayer->GetUIID();

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_ATTACK_CANCEL, 1, NEW_USHORT(nPlayerUIID));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestAttackPrepare(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	pPlayer->m_nLastPrepareAttackTime = gsys.pSystem->GetNowTime();

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	WEAPON_TYPE nCurrWeaponType = pItemHolder->GetEquipment().GetWeaponType();
	int nTalentID = CSTalentInfoHelper::NormalAttackTalentID(nCurrWeaponType, NA_NORMAL1);

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_PREPARE_ATTACK, 2, NEW_USHORT(pPlayer->GetUIID()), NEW_INT(nTalentID));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestJump(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	vec3 pos, dir;
	if (pCmd->GetParameter(&pos, 0, MPT_VEC)==false) return CR_FALSE;
	if (pCmd->GetParameter(&dir, 1, MPT_VEC)==false) return CR_FALSE;


	// 점프 시작
	pPlayer->doJump();
	pPlayer->OnDoSomething();


	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_JUMP,
		3,
		NEW_USHORT(pPlayer->GetUIID()),
		NEW_VEC(pos),
		NEW_VEC(dir));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestFalling(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMovingMotionfactor())		
		return CR_TRUE;	// 이동관련 모션팩터(넉백,쓰로우업) 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	vec3 pos;
	svec3 svDir;
	if (pCmd->GetParameter(&pos,	0, MPT_VEC)==false) return CR_FALSE;
	if (pCmd->GetParameter(&svDir,	1, MPT_SVEC)==false) return CR_FALSE;

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_FALLING,
		3,
		NEW_USHORT(pPlayer->GetUIID()),
		NEW_VEC(pos),
		NEW_SVEC(svDir));
	pPlayer->RouteToThisCellExceptMe(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnEndFalling(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (true == pPlayer->IsNowInvincibility()) return CR_TRUE;

	if (true == pPlayer->IsDead()) return CR_TRUE;

	bool bIsFallenOnWater;
	float fFallingHeight;
	if (pCmd->GetParameter(&bIsFallenOnWater, 0, MPT_BOOL)==false)	return CR_FALSE;
	if (pCmd->GetParameter(&fFallingHeight, 1, MPT_FLOAT)==false)	return CR_FALSE;

	
	// 점프 끝
	pPlayer->doLand();


	pPlayer->GetPlayerFalling()->EndFalling(bIsFallenOnWater, fFallingHeight);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestGuard(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (!pPlayer->IsMovable())			return CR_TRUE;
	if (pPlayer->HasMovingMotionfactor())	return CR_TRUE;
	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	WEAPON_TYPE nCurrWeaponType = pItemHolder->GetEquipment().GetWeaponType();

	int nGuardLevel = pPlayer->GetPassiveValue(TEPT_GUARD);
	int nGuardTalentID = CSTalentInfoHelper::NormalGuardTalentID(nCurrWeaponType, nGuardLevel, pPlayer->IsEquipShield());
	if (nGuardTalentID == 0) return CR_FALSE;

	pPlayer->doGuard(nGuardTalentID);
	pPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestGuardReleased(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	pPlayer->doGuardReleased();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestUseTalent(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nTalentID = 0;
	svec3 svCharDir;
	vec3 vCharDir;
	vec3 vCharPos;
	if (pCmd->GetParameter(&nTalentID, 0, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&svCharDir, 1, MPT_SVEC)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vCharPos, 2, MPT_VECTOR)==false) return CR_FALSE;

	vCharDir = svCharDir;

	if (pPlayer->HasMotionfactor())		
	{
		if (IsRunForTest())
		{
			pPlayer->FailAndRouteSystemMsg(CR_FAIL_TALENTCANCEL_COOLTIMESYNCH);
		}

		// 클라이언트 쿨타임 동기화 용도로 쓰임
		pPlayer->RouteUseTalentRequestFailed(nTalentID);

		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.
	}

	pPlayer->SetFacingDir(vCharDir);
	GMath::NormalizeZ0(vCharDir);
	pPlayer->SetDir(vCharDir);
	pPlayer->SetPos(vCharPos);
	pPlayer->CheatCheck_Walk();
	pPlayer->OnDoSomething();

	// 스킬 사용
	pPlayer->doUseTalent(nTalentID);
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestActTalent(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	svec3 svCharDir;
	vec3 vCharDir;
	MUID uidTarget;

	if (pCmd->GetParameter(&svCharDir, 0, MPT_SVEC)==false) return CR_FALSE;
	if (pCmd->GetParameter(&uidTarget, 1, MPT_UID)==false) return CR_FALSE;
	vCharDir = svCharDir;

	pPlayer->SetFacingDir(vCharDir);
	GMath::NormalizeZ0(vCharDir);
	pPlayer->SetDir(vCharDir);
	pPlayer->CheatCheck_Walk();
	pPlayer->OnDoSomething();

	TALENT_TARGET_INFO Target;
	Target.uidTarget = uidTarget;

	pPlayer->GetModuleCombat()->ActTalent(Target);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestActTalentWithHitcapsule(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	svec3 svCharDir;
	vec3 vCharDir;

	if (pCmd->GetParameter(&svCharDir, 0, MPT_SVEC)==false) return CR_FALSE;
	vCharDir = svCharDir;

	MCommandParameter* pParam = pCmd->GetParameter(1);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;

	TD_TALENT_TARGET_DETAIL* ttl = (TD_TALENT_TARGET_DETAIL*)pParam->GetPointer();

	pPlayer->SetFacingDir(vCharDir);
	GMath::NormalizeZ0(vCharDir);
	pPlayer->SetDir(vCharDir);
	pPlayer->CheatCheck_Walk();
	
	TALENT_TARGET_INFO Target;
	Target.uidTarget = ttl->uidTarget;
	Target.nCapsuleGroupIndex = ttl->nCapsuleGroupIndex;
	Target.nCapsuleIndex = ttl->nCapsuleIndex;

	pPlayer->GetModuleCombat()->ActTalent(Target);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestActTalentWithGround(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->HasMotionfactor())		
		return CR_TRUE;	// 모션팩터가 적용중이면 무시한다.

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	svec3 svCharDir;
	vec3 vCharDir;
	vec3 vGroundPoint;

	if (pCmd->GetParameter(&svCharDir, 0, MPT_SVEC)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vGroundPoint, 1, MPT_VEC)==false) return CR_FALSE;
	vCharDir = svCharDir;

	pPlayer->SetFacingDir(vCharDir);
	GMath::NormalizeZ0(vCharDir);
	pPlayer->SetDir(vCharDir);
	pPlayer->CheatCheck_Walk();

	TALENT_TARGET_INFO Target;
	Target.vGroundPoint = vGroundPoint;
	pPlayer->GetModuleCombat()->ActTalent(Target);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestCancelTalent(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (IsRunForTest())
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_TALENTCANCEL_REQUEST);
	}

	pPlayer->doCancelTalentForce(true);
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestStandUp(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	if (pPlayer->HasPassiveTalent(TEPT_QUICK_RISE))
	{
		pPlayer->ForceStandUp();
	}

	pPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestSwitchingWeaponBegin(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_SWITCHING_WEAPON_BEGIN, 
		1,
		NEW_USHORT(pPlayer->GetUIID()));
	pPlayer->RouteToThisCell(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestSwitchingWeapon(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	int8 nWeaponSet;
	if (false == pCmd->GetParameter(&nWeaponSet,	0, MPT_CHAR)) return CR_FALSE;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (!pItemHolder->GetEquipment().IsWeaponSwitchable(SH_ITEM_SWITCH_WEAPON(nWeaponSet)))		return CR_FALSE;

	GItemHolder* pHolder = pPlayer->GetItemHolder();
	pHolder->GetEquipment().SwitchingWeaponSet(SH_ITEM_SWITCH_WEAPON(nWeaponSet));
	
	PALETTE_NUM nPaletteNum;
	if (ITEMSWITCH_PRIMARY == nWeaponSet)
	{
		nPaletteNum = pPlayer->GetPalette().GetPrimary();
	}
	else
	{
		nPaletteNum = pPlayer->GetPalette().GetSecondary();
	}
	if (true == gsys.pPaletteSystem->IsValidNum(nPaletteNum))
	{
		gsys.pPaletteSystem->Select(pPlayer, nPaletteNum);
	}	

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_SWITCHING_WEAPON, 
		2,
		NEW_USHORT(pPlayer->GetUIID()),
		NEW_CHAR(nWeaponSet));
	pPlayer->RouteToThisCell(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestSwim(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (pPlayer->IsBeginUnableAction()) return CR_TRUE;

	bool bSwimming;
	if (pCmd->GetParameter(&bSwimming,	0, MPT_BOOL)==false) return CR_FALSE;

	if (bSwimming)
	{
		pPlayer->doSwimming();
	}
	else
	{
		pPlayer->doSwimmingRelease();
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestSit(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	gsys.pPlayerSystem->Sit(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestSitRise(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	gsys.pPlayerSystem->SitRise(pPlayer);
	pPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Action::OnRequestAFK(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	pPlayer->GetAFK().Toggle();
	
	return CR_TRUE;
}
