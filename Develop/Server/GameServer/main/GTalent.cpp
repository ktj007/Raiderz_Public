#include "stdafx.h"
#include "GTalent.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GTalentInfo.h"
#include "GModuleAI.h"
#include "GModuleCombat.h"
#include "GTalentRouter.h"
#include "GConst.h"
#include "GAIProfiler.h"
#include "GTalentEffector.h"
#include "GItemHolder.h"
#include "GNPCMode.h"
#include "GBuffRequirement.h"
#include "GEquipmentSlot.h"


GTalent::GTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress) 
: m_pOwner(pOwner)
, m_pTalentInfo(pTalentInfo)
, m_fElapsedTime(0.0f)
, m_fActTime(-1.0f)
, m_nPhase(TALENT_PHASE_NONE)
, m_Target(Target)
, m_TalentStress(bGainStress)
, m_bAutoCastingComplete(false)
, m_bExpired(false)
, m_pRouter(NULL)
, m_TalentEventChecker()
{
	m_pRouter = new GTalentRouter(this);
}

GTalent::~GTalent() 
{
	SAFE_DELETE(m_pRouter);

	// NOTIFY_OBSERVERS 순환중에 GTalentStress 에서 콜트가 실행되어,
	// 현재의 탤런트가 취소될 수 있음 
	// 순환 도중에 소멸되어 옵저버를 컨테이너에서 지우면 iterator++에서 오류
	 DetachObservers();
}

void GTalent::Start()
{
	//PFI_BLOCK_THISFUNC(706);
	//AI_PFI_TALENT_BLOCK(GetID() * 10, m_pTalentInfo->m_szName);

	m_vInitialDir = m_pOwner->GetDir();
	m_vInitialPos = m_pOwner->GetPos();
	GetTargetPos(m_vInitialTargetPos);
	m_TalentExpirer.SetExpiredTime(NO_EXPIRE_TALENT);	// 다음 페이즈로 안내되기전에는 만료시키지 않음

	OnStart();

	if (m_bExpired)
		return;

	if (m_pTalentInfo->IsGuard())
	{
		ChangePhase(TALENT_PHASE_FINISH);
		return;
	}

	GBuffRequirement buff_requirement;
	if (!buff_requirement.ProcessRequirement(m_pOwner, m_pTalentInfo->m_RequireBuff))
	{
		ChangePhase(TALENT_PHASE_CANCELED);
		return; // 버프 필요조건 불충족
	}

	if (m_pTalentInfo->HasCastingTime())
	{
		if (m_pOwner->IsNPC())
		{
			// NPC는 act 명령 대기 없이 바로 캐스팅 끝냄
			m_bAutoCastingComplete = true;
		}

		ChangePhase(TALENT_PHASE_PREPARE);
		return;
	}

	ChangePhase(TALENT_PHASE_ACT);
}


void GTalent::Cancel(bool bPostCommand)
{
	VALID(m_pTalentInfo);
	PFI_BLOCK_THISFUNC(709);

	if (m_nPhase != TALENT_PHASE_PREPARE)
		return;		// 준비 단계에서만 취소 가능

	CancelForce(bPostCommand);
}

void GTalent::CancelForce(bool bPostCommand)
{
	ChangePhase(TALENT_PHASE_CANCELED);

	if (bPostCommand)
	{
		m_pRouter->RouteCancelTalent(m_pTalentInfo);
	}
}


void GTalent::ChangePhase( TALENT_PHASE nNextPhase ) 
{
	PFI_BLOCK_THISFUNC(705);
	
	if (m_nPhase == nNextPhase) 
		return;

	if (GetPhase() == TALENT_PHASE_CANCELED ||
		GetPhase() == TALENT_PHASE_FINISH)
		return;	// 이미 만료된 상태

	OnLeavePhase(m_nPhase);
	m_nPhase = nNextPhase;
	OnEnterPhase(nNextPhase);

	// 페이즈 만료 처리
	OnPhaseEnd();
}


bool GTalent::Update( float fDelta ) 
{
	PFI_BLOCK_THISFUNC(701);
	AI_PFI_TALENT_BLOCK(GetID() * 10, m_pTalentInfo->m_szName);	
	
	if (m_bExpired)
		return false;

	m_fElapsedTime += fDelta;
	
	switch(m_nPhase)
	{
	case TALENT_PHASE_PREPARE:		OnUpdatePhasePrepare(fDelta);	break;
	case TALENT_PHASE_ACT:			OnUpdatePhaseAct(fDelta);		break;
	case TALENT_PHASE_EXTRA:		OnUpdatePhaseExtra(fDelta);		break;
	case TALENT_PHASE_EXTRA2:		OnUpdatePhaseExtra2(fDelta);	break;
	case TALENT_PHASE_EXTRA3:		OnUpdatePhaseExtra3(fDelta);	break;
	}

	return true;
}

void GTalent::UpdateExpired( float fDelta )
{
	PFI_BLOCK_THISFUNC(9053);

	if (m_TalentExpirer.IsExpired(m_fElapsedTime))
	{
		ChangePhase(TALENT_PHASE_FINISH);
	}
}

void GTalent::OnEnterPhase(TALENT_PHASE nPhase)
{
	PFI_BLOCK_THISFUNC(707);

	switch (nPhase)
	{
	case TALENT_PHASE_ACT:		{ OnEnterPhaseAct();		}break;	
	case TALENT_PHASE_CANCELED:	{ OnEnterPhaseCanceled();	}break;
	case TALENT_PHASE_PREPARE:	{ OnEnterPhasePrepare();	}break;
	case TALENT_PHASE_FINISH:	{ OnEnterPhaseFinish();		}break;
	case TALENT_PHASE_EXTRA:	{ OnEnterPhaseExtra();		}break;
	case TALENT_PHASE_EXTRA2:	{ OnEnterPhaseExtra2();		}break;
	case TALENT_PHASE_EXTRA3:	{ OnEnterPhaseExtra3();		}break;
	}	// switch
}

void GTalent::OnLeavePhase(TALENT_PHASE nPhase)
{
	PFI_BLOCK_THISFUNC(708);

	switch (nPhase)
	{
	case TALENT_PHASE_ACT:		{ OnLeavePhaseAct();		}break;
	case TALENT_PHASE_CANCELED:	{ OnLeavePhaseCanceled();	}break;
	case TALENT_PHASE_PREPARE:	{ OnLeavePhasePrepare();	}break;
	case TALENT_PHASE_FINISH:	{ OnLeavePhaseFinish();		}break;
	case TALENT_PHASE_EXTRA:	{ OnLeavePhaseExtra();		}break;
	case TALENT_PHASE_EXTRA2:	{ OnLeavePhaseExtra2();		}break;
	case TALENT_PHASE_EXTRA3:	{ OnLeavePhaseExtra3();		}break;
	}	// switch
}

void GTalent::OnStart()
{
	AttachObservers();
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnStart(this));

	if (GetPhase() == TALENT_PHASE_CANCELED)
		return;	// 초기화 실패로 취소됨

	// OnUseTalent 이벤트
	PFI_B(3050, "GEntityActor::OnUseTalent");
	m_pOwner->OnUseTalent(m_pOwner, m_pTalentInfo);
	PFI_E(3050);

	m_pRouter->RouteUseTalent(m_pTalentInfo, GetInitPos(), GetInitDir());
}

void GTalent::OnExit()
{
	m_TalentHit.OnExit(this);
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnExit(this));
}

void GTalent::OnEnterPhasePrepare()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnEnterPhasePrepare(this));
}

void GTalent::OnEnterPhaseAct()
{
	// OnActTalent 이벤트
	m_pOwner->OnActTalent(m_pOwner, m_pTalentInfo);

	SetExpierdTime();

	m_fActTime = m_fElapsedTime;
	if (!m_pTalentInfo->IsDelayedAct())
		ApplyTalentEffect(TC_USE_TALENT);

	// npc일 경우 모드 변환
	ChangeNPCMode();

	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnEnterPhaseAct(this));

	RouteActTalent();
}

void GTalent::OnEnterPhaseCanceled()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnEnterPhaseCanceled(this));

	OnExit();
}

void GTalent::OnEnterPhaseFinish()
{
	// OnFinishTalent 이벤트
	m_pOwner->OnFinishTalent(m_pTalentInfo);

	ApplyTalentEffect(TC_FINISH_TALENT);

	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnEnterPhaseFinish(this));

	OnExit();
}

void GTalent::OnEnterPhaseExtra()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnEnterPhaseExtra(this));
}

void GTalent::OnEnterPhaseExtra2()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnEnterPhaseExtra2(this));
}

void GTalent::OnEnterPhaseExtra3()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnEnterPhaseExtra3(this));
}

void GTalent::OnLeavePhasePrepare()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnLeavePhasePrepare(this));
}

void GTalent::OnLeavePhaseAct()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnLeavePhaseAct(this));
}

void GTalent::OnLeavePhaseFinish()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnLeavePhaseFinish(this));
}

void GTalent::OnLeavePhaseCanceled()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnLeavePhaseCanceled(this));
}

void GTalent::OnLeavePhaseExtra()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnLeavePhaseExtra(this));
}

void GTalent::OnLeavePhaseExtra2()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnLeavePhaseExtra2(this));
}

void GTalent::OnLeavePhaseExtra3()
{
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnLeavePhaseExtra3(this));
}

void GTalent::OnUpdatePhasePrepare( float fDelta )
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(9040);

	AI_PFI_TALENT_BLOCK(GetID() * 10 + 1, m_pTalentInfo->m_szName + L"-PREPARE");


	// NPC일경우에는 ACT 명령을 기다리지 않고 바로 넘어감
	if (m_bAutoCastingComplete &&
		IsCastingComplete())
	{
		ChangePhase(TALENT_PHASE_ACT);
		return;
	}

	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnUpdatePhasePrepare(this, fDelta));
}

void GTalent::OnUpdatePhaseAct( float fDelta )
{
	PFI_BLOCK_THISFUNC(9041);

	PFI_B(9058, "GTalent::OnUpdatePhaseAct-AI_PFI_TALENT_BLOCK");
	AI_PFI_TALENT_BLOCK(GetID() * 10 + 2, m_pTalentInfo->m_szName + L"-ACT");
	PFI_E(9058);	

	PFI_B(9047, "GTalentObserver::OnUpdatePhasePreAct");
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnUpdatePhasePreAct(this, fDelta));
	PFI_E(9047);

	UpdateHit();

	PFI_B(9048, "GTalentObserver::OnUpdatePhaseAct");
	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnUpdatePhaseAct(this, fDelta));
	PFI_E(9048);
}

void GTalent::OnUpdatePhaseExtra( float fDelta )
{
	PFI_BLOCK_THISFUNC(9042);

	AI_PFI_TALENT_BLOCK(GetID() * 10 + 3, m_pTalentInfo->m_szName + L"-EXTRA");

	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnUpdatePhaseExtra(this, fDelta));
}

void GTalent::OnUpdatePhaseExtra2( float fDelta )
{
	PFI_BLOCK_THISFUNC(9043);

	AI_PFI_TALENT_BLOCK(GetID() * 10 + 4, m_pTalentInfo->m_szName + L"-EXTRA2");

	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnUpdatePhaseExtra2(this, fDelta));
}

void GTalent::OnUpdatePhaseExtra3( float fDelta )
{
	PFI_BLOCK_THISFUNC(9045);

	AI_PFI_TALENT_BLOCK(GetID() * 10 + 5, m_pTalentInfo->m_szName + L"-EXTRA3");

	NOTIFY_OBSERVERS(GTalentObserver, m_vecObservers, OnUpdatePhaseExtra3(this, fDelta));
}

void GTalent::OnHitArea( GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex )
{
	// TODO(pyo): 각 탤런트 특성에 맞게 바꿀 수 있도록 해야함
	GTalentHitter_Instant* pNewHitter = GTalentHitter::NewTalentHitter< GTalentHitter_Instant >();
	pNewHitter->Setup(m_pOwner->GetUID(), m_pTalentInfo, pVictim, nCheckTime, nCapsuleGroupIndex, nCapsuleIndex);

	if (m_pOwner->GetModuleCombat()) 
		m_pOwner->GetModuleCombat()->AddTargetHitter(pNewHitter);
}

bool GTalent::IsNowInvincibleTime()
{
	if (!m_pTalentInfo->IsUseInvincibleTime())	
		return false;

	return (m_fElapsedTime >= m_pTalentInfo->m_fInvincibleTimeStart && 
		m_fElapsedTime <= m_pTalentInfo->m_fInvincibleTimeEnd);
}

bool GTalent::IsNowSuperarmorTime()
{
	if (!m_pTalentInfo->IsUseSuperarmorTime())	
		return false;

	return (m_fElapsedTime >= m_pTalentInfo->m_fSuperarmorTimeStart && 
		m_fElapsedTime <= m_pTalentInfo->m_fSuperarmorTimeEnd);
}

bool GTalent::IsNowAvoidTime()
{
	if (!m_pTalentInfo->IsUseAvoidTime())	
		return false;

	return (m_fElapsedTime >= m_pTalentInfo->m_fAvoidTimeStart && 
		m_fElapsedTime <= m_pTalentInfo->m_fAvoidTimeEnd);
}

bool GTalent::IsExpired()
{
	if (m_nPhase == TALENT_PHASE_PREPARE)
		return false; // 탤런트 사용 준비중

	// 탤런트 지속시간이 오버한 경우
#define TALENT_EXPIRETIME_TOLERANCE 0.2f
	if (GetElapsedTime() + TALENT_EXPIRETIME_TOLERANCE >= GetExpiredTime())
		return true; // 탤런트 사용시간 지남

	return m_bExpired;
}

bool GTalent::IsCancelable()
{
	if (!m_pTalentInfo)	return false;
	return m_pTalentInfo->m_bCancelable;
}

bool GTalent::IsMovable()
{
	if (!m_pTalentInfo)	return false;
	return m_pTalentInfo->IsMoving();
}

bool GTalent::IsCastingComplete()
{
	return m_fElapsedTime >= m_pTalentInfo->m_fCastingTime;
}

void GTalent::AttachObserver( GTalentObserver* pOvr )
{
	VALID(pOvr);

	for each (GTalentObserver* pObserver in m_vecObservers)
	{
		VALID(pObserver != pOvr);
	}	

	m_vecObservers.push_back(pOvr);
}

void GTalent::AttachObservers()
{
	AttachObserver(&m_TalentHitcapsule);
	AttachObserver(&m_TalentScript);
	AttachObserver(&m_TalentStress);
	AttachObserver(&m_TalentMove);
	AttachObserver(&m_TalentEventChecker);
	if (GConst::LOG_TALENT)
		AttachObserver(&m_TalentLogger);
}

void GTalent::DetachObservers()
{
	m_vecObservers.clear();
}

void GTalent::HitArea( GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex )
{
	OnHitArea(pVictim, nCheckTime, nCapsuleGroupIndex, nCapsuleIndex);
}

float GTalent::GetActElapsedTime() /*/< Act Phase 후 경과 시간 */
{
	if (m_fActTime == -1.0f)
		return 0.0f;	// 액트 타임이 설정되지 않으면 의미가 없음

	return (m_fElapsedTime - m_fActTime);
}

void GTalent::TalentFinish()
{
	ChangePhase(TALENT_PHASE_FINISH);
}

void GTalent::ApplyTalentEffect(TALENT_CONDITION nTalentCondition)
{
	GEntityActor* pTarget = NULL;
	if (m_Target.IsValid() && m_pOwner->GetField())
		pTarget = m_pOwner->GetField()->FindActor(m_Target());

	GTalentEffector::Info info;
	info.pTalentInfo = m_pTalentInfo;
	info.pUser = m_pOwner;
	info.pTarget = pTarget;
	info.nTalentCondition = nTalentCondition;
	info.vGroundPoint = m_Target.vGroundPoint;
	m_TalentEffector.ApplyTalent(info);
}

void GTalent::ApplyDelayedAct()
{
	ApplyTalentEffect(TC_USE_TALENT);

	m_TalentScript.OnDelayedAct(this);
}

void GTalent::ChangeNPCMode()
{
	if (!m_pTalentInfo->m_ChangeModeInfo.m_bChangeMode)	return;
	if (!m_pOwner->IsNPC())								return;

	GEntityNPC* pOwnerNPC = ToEntityNPC(m_pOwner);
	pOwnerNPC->GetNPCMode().ChangeMode(m_pTalentInfo->m_ChangeModeInfo.m_nModeTo);
}

bool GTalent::GetTargetPos( vec3& vTargetPos )
{
	vTargetPos = vec3::ZERO;

	GEntityActor* pTarget = GetTargetActor();
	if (pTarget == NULL) return false;

	float fDistance = m_pOwner->GetPos().DistanceToSq(pTarget->GetPos());
	if (fDistance <= m_pTalentInfo->GetLeastProjectileDistance())
	{
		// 최소거리보다 적은 경우에는 임의의 먼 거리를 반환
#define LEAST_PROJECT_RANGE		(1000.0f) // 10m
		vTargetPos = m_pOwner->GetPos() + (m_pOwner->GetFacingDir() * LEAST_PROJECT_RANGE);
		return true;
	}

	vTargetPos = pTarget->GetPos();
	
	float fHeight = 100.0f;
	if(pTarget->GetMeshInfo())
	{
		fHeight = (pTarget->GetMeshInfo()->GetSimpleCapsule().height * 0.7f);
	}
	vTargetPos.z += min(m_pTalentInfo->GetLowestProjectileStartPos().z, fHeight);
	//vTargetPos.z += m_pTalentInfo->GetLowestProjectileStartPos().z;
	
	return true;
}

void GTalent::ActTalent( TALENT_TARGET_INFO Target )
{
	if (m_nPhase != TALENT_PHASE_PREPARE)
		return;	// prepare페이즈에서만 쓸 수 있음

	m_Target = Target;

	if (!IsCastingComplete())
	{
		// 캐스팅 준비시간이 충분히 진행되지 않았다면 될때까지 대기
		m_bAutoCastingComplete = true;
		return;	
	}

#define ACT_POS_GAP	(160.0f) //cm
	if (m_vInitialPos.DistanceTo(m_pOwner->GetPos()) > ACT_POS_GAP)
	{
		// 캐스팅후에 이동하면 안됨 (취소시키기)
		Cancel(true);
		return;
	}

	ChangePhase(TALENT_PHASE_ACT);
}

void GTalent::RouteActTalent()
{
	if (m_pTalentInfo->HasProjectileEvent())
	{
		vec3 vTargetPos;
		if (IsAimingDirectionTalent())
		{
			GetTargetPos(vTargetPos);
			m_vInitialTargetPos = vTargetPos;
		}
		else
		{
			vTargetPos = GetInitTargetPos();
		}
		
		// 발사체용 액트패킷 날리기
		m_pRouter->RouteActTalentProjectileMissile(	m_pTalentInfo->m_nID, 
													vTargetPos, 
													m_Target.uidTarget,
													m_Target.nCapsuleGroupIndex,
													m_Target.nCapsuleIndex);
	}
	else
	{
		// 일반 액트패킷 날리기
		if (!CSTalentInfoHelper::IsImmediateTalent(m_pTalentInfo))
		{
			if (m_Target.HasGroundPoint())
			{
				m_pRouter->RouteActTalentWithGround(m_Target.vGroundPoint, m_pTalentInfo, GetInitDir());
			}
			else
			{
				m_pRouter->RouteActTalent(m_pTalentInfo, GetInitDir());
			}
		}
	}
}

GEntityActor* GTalent::GetTargetActor()
{
	GField* pField = m_pOwner->GetField();
	if (!pField) return NULL;

	return pField->FindActor(m_Target.uidTarget);
}

void GTalent::UpdateHit()
{
	PFI_BLOCK_THISFUNC(9046);

	if (m_pTalentInfo->m_EffectInfo.m_nPoint != CSEffectInfo::POINT_HITCAPSULE)
		return;	// 히트캡슐 타겟만 가능함

	m_TalentHit.UpdateHit(this, GetActElapsedTime());
}

bool GTalent::IsAimingDirectionTalent()
{
	VALID_RET(m_pTalentInfo, false);
	if (!m_pOwner->IsNPC())				return false;
	if (!m_pTalentInfo->m_bUseAiming)	return false;
	//if (m_pTalentInfo->IsMoving())		return false;
	if (m_pTalentInfo->m_EffectInfo.m_nRelation == CSEffectInfo::RELATION_ALLIED) return false;

	return true;
}

GTalentMove::MOVEINFO GTalent::GetExpectedMoveInfo( float fActElapsedTime )
{
	if (IsMovable() == false)
	{
		GTalentMove::MOVEINFO info;
		info.vPos = GetInitPos();
		info.vDir = GetInitDir();
		return info;
	}

	return m_TalentMove.GetExpectedMoveInfo(fActElapsedTime);
}

float GTalent::InitExpiredTime()
{
	float fExpiredTime = m_pTalentInfo->GetExpiredTime();
	WEAPON_TYPE nWeaponType = WEAPON_NONE;
	if (m_pOwner->IsPlayer())
	{
		// 플레이어일 경우에 무기에 맞는 애미네이션 시간을 구함
		GItemHolder* pItemHolder = ToEntityPlayer(m_pOwner)->GetItemHolder();
		nWeaponType = pItemHolder->GetEquipment().GetWeaponType();
	}

	float fAnimationTime = m_pTalentInfo->m_ActAnimationTime.GetTime(nWeaponType);
	if (0.0f < fAnimationTime)
	{
		fExpiredTime = max(fExpiredTime, fAnimationTime);
	}

	if (m_pOwner->IsNPC())
	{
		// NPC일 경우에는 후행 딜레이가 적용됨
		fExpiredTime += m_pTalentInfo->m_fPostDelay;
	}

	fExpiredTime += GetElapsedTime();	// 현재 진행 시간 더하기

	return fExpiredTime;
}

void GTalent::OnPhaseEnd()
{
	PFI_BLOCK_THISFUNC(9054);

	switch(m_nPhase)
	{
	case TALENT_PHASE_FINISH:		m_bExpired = true;	break;
	case TALENT_PHASE_CANCELED:		m_bExpired = true;	break;
	}

	if (m_bExpired)
	{
		OnLeavePhase(m_nPhase);
	}
}

float GTalent::GetExpiredTime()
{
	return m_TalentExpirer.GetExpiredTime();
}

void GTalent::SetExpierdTime()
{
	m_TalentExpirer.SetExpiredTime(InitExpiredTime());	// 캐스팅 끝난시점부터 만료시간 시작
}

bool GTalent::IsNowCasting()
{
	return GetPhase() == TALENT_PHASE_PREPARE;
}

void GTalent::OnGainDamage( GEntityActor* pTarget )
{
	m_TalentHit.OnGainDamage(pTarget);
}

