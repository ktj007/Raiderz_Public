#include "stdafx.h"
#include "GEntityActor.h"
#include "GItemManager.h"
#include "GModuleCombat.h"
#include "GModuleBuff.h"
#include "GCommandTable.h"
#include "GField.h"
#include "CTransData.h"
#include "GItemSystem.h"
#include "GTalent.h"
#include "GMessageID.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GCalculator.h"
#include "GConst.h"
#include "GActorMeshInfo.h"
#include "GBuffInfo.h"
#include "GTalentEffector.h"
#include "GCombatCalculator.h"
#include "GConditionsSystem.h"
#include "GCommandCenter.h"
#include "GActorObserver.h"
#include "GUseCostMgr.h"
#include "GTalentInfoMgr.h"
#include "GCommand.h"
#include "GSystem.h"
#include "GConfig.h"
#include "GCollision.h"
#include "GTalentHelper.h"
#include "GTalentInfoDef.h"
#include "GScriptGroup_Field.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GMotionFactorHandler.h"
#include "GFieldMgr.h"
#include "GGlueActor.h"
#include "GScriptGroup_NPC.h"
#include "GGlueNPC.h"
#include "GPlayerTalent.h"
#include "GNPCInfoMgr.h"
#include "GHateTable.h"
#include "GNPCMgr.h"
#include "GFactionSystem.h"
#include "GuardEffector.h"
#include "GActorDamageCalculator.h"
#include "GActorInvisibility.h"
#include "GTalentHitCommandFactory.h"
#include "GTalentHitRouter.h"
#include "GItemUser.h"
#include "GActorCounter.h"
#include "CSCharHelper.h"
#include "GActorCooltimeChecker.h"
#include "GActorHateTracker.h"
#include "GMsgCommandFacade.h"
#include "CCommandResultTable.h"
#include "GActorBuffEntity.h"
#include "GActorTalent.h"
#include "CSMotionFactorHelper.h"
#include "GTestSystem.h"
#include "GMotionFactorHelper.h"
#include "GRelationChecker.h"
#include "GAIMonitorPort.h"

#define SENSOR_VALID_RADIUS				(1000.0f)
#define SENSOR_ACTIVE_RADIUS_GAP		(500.0f)


GEntityActor* ToEntityActor( GEntity* pEntity )
{
	VALID_RET(pEntity, NULL);
	return static_cast<GEntityActor*>(pEntity);
}

GEntityActor::GEntityActor() 
: GEntitySync()
, m_nUIID(0)
, m_bScheduleMove(false)
, m_nActionState(AS_NORMAL)
, m_pChrInfo(NULL)
, m_pModuleCombat(NULL)
, m_pModuleBuff(NULL)
, m_pDelegator(NULL)
, m_bNowUnableAction(false)
, m_isMoving(false)
, m_isJumping(false)
, m_bRegen(true)
{
	m_nType |= ET_ACTOR;
	m_nTypeID = ETID_ACTOR;

	m_bCancelAction = false;

	m_MFReleaser.Stop();

	m_nTalentStartTime = 0;

	m_pInvisibility = new GActorInvisibility(this);
	m_pCounter = new GActorCounter(this);
	m_pCoolTimeChecker = new GActorCooltimeChecker(this);
	m_pHateTracker = new GActorHateTracker(this);
	m_pBuffEntity = new GActorBuffEntity(this);
	m_pActorTalent = new GActorTalent(this);
}

GEntityActor::~GEntityActor()
{
	SAFE_DELETE(m_pDelegator);
	SAFE_DELETE(m_pModuleBuff);
	SAFE_DELETE(m_pInvisibility);
	SAFE_DELETE(m_pCounter);
	SAFE_DELETE(m_pCoolTimeChecker);
	SAFE_DELETE(m_pModuleCombat);
	SAFE_DELETE(m_pHateTracker);
	SAFE_DELETE(m_pBuffEntity);
	SAFE_DELETE(m_pActorTalent);
}

bool GEntityActor::Create(MUID& uid)
{
	CreateDelegator();

	m_pModuleCombat = new GModuleCombat(this);
	m_pModuleBuff = new GModuleBuff(this);	
	
	return GEntitySync::Create(uid);
}

void GEntityActor::Destroy()
{
	if (!m_pModuleBuff)
		return;

	m_pModuleBuff->LostAll(false);

	__super::Destroy();
}

void GEntityActor::RegisterModules()
{
	GEntitySync::RegisterModules();

	m_pModuleCombat->Initialize();
	m_Modules.AddModule(m_pModuleCombat, true);

	m_pModuleBuff->Initialize();
	m_Modules.AddModule(m_pModuleBuff, true);
}

void GEntityActor::UnregisterModlues()
{
	m_Modules.RemoveModule(m_pModuleCombat);
	m_pModuleCombat->Finalize();

	m_Modules.RemoveModule(m_pModuleBuff);
	m_pModuleBuff->Finalize();

	GEntitySync::UnregisterModlues();
}

bool GEntityActor::SetPos(const vec3& pos)
{
	// GEntity::SetPos에서 옮겨왔다.
	if (m_pField && m_pField->GetInfo())
	{
		const GFieldInfo* pFieldInfo = m_pField->GetInfo();
		if ( (pos.x < pFieldInfo->MinX()) ||
			(pos.x > pFieldInfo->MaxX()) ||
			(pos.y < pFieldInfo->MinY()) ||
			(pos.y > pFieldInfo->MaxY()) ) 
			return false;
	}

	if (GEntitySync::SetPos(pos) == false)
		return false;

	return true;
}

float GEntityActor::DistanceTo(GEntityActor* pTarget)
{	
	if (NULL == pTarget) return 0.0f;

	return GetPos().DistanceTo(pTarget->GetPos());
}

void GEntityActor::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(102);

	GEntitySync::OnUpdate(fDelta);

	_UpdateMotionFactor(fDelta);
	m_RegenUpdater.Update(fDelta, this);
	DoScheduleMove();
}

void GEntityActor::_UpdateMotionFactor(float fDelta)
{
	m_MotionFactor.Update(fDelta);

	if (m_MFReleaser.IsReady(fDelta))
	{
		OnEndMotionFactor();
		ReleaseMotionFactor();
	}
}

void GEntityActor::ReleaseMotionFactor(bool bStateToNormal)
{
	if (bStateToNormal)
	{
		ChangeActionState(AS_NORMAL);
	}

	m_MFReleaser.Stop();
	m_MotionFactor.OnReleased();
}

inline
bool GEntityActor::IsBeginUnableAction()
{
	if (m_pModuleBuff->IsMesmerize())	return true;
	return false;
}

bool GEntityActor::IsEndUnableAction()
{
	if (!m_bNowUnableAction)
		return false;

	return !IsBeginUnableAction();
}

void GEntityActor::OnBeginUnableAction()
{
	doGuardReleased();
	m_pModuleCombat->CancelTalentForce(true);

	m_bNowUnableAction = true;
}

void GEntityActor::OnEndUnableAction()
{
	m_bNowUnableAction = false;
}

bool GEntityActor::IsNowUnableAction()
{
	return m_bNowUnableAction;
}

bool GEntityActor::IsDie()
{
	return (m_ChrStatus.nHP <= 0)?true:false;
}

bool GEntityActor::doDamage(MUID uidAttacker, DAMAGE_ATTRIB nDamageType, uint32 nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID)
{
	m_ChrStatus.nHP -= nDamage;

	OnDamage(this, uidAttacker, nDamage, nEffectSourceType, nEffectSourceID);
		
	return IsDie();
}

void GEntityActor::doHeal(MUID uidAttacker, uint32 nHealAmount)
{
	SetHP(m_ChrStatus.nHP + nHealAmount);
}

void GEntityActor::doAttack(MUID uidTarget, int nDamage)
{
	GEntitySync::doAttack(uidTarget, nDamage);

	EVENT_ATTACK_INFO info;
	info.uidTarget	=	uidTarget;
	info.nDamage	=	nDamage;

	GMessage msg(GMSG_AI_EVT_ATTACK, &info);
	Message(msg);
}

void GEntityActor::doDie()
{
	// 이동예약 처리수행
	DoScheduleMove();

	SetHP(0);
	GEntitySync::doDie();

	doGuardReleased();
	m_bDead = true;

	GEntityActor* pKiller = FindActor(GetKiller());
	if (pKiller)
	{
		pKiller->OnKilled(this);
	}

	// 이벤트 함수
	OnDie();
}

GMFApplyReturnValue GEntityActor::ApplyMotionFactor(const GMFApplyArgs args)
{
	PFI_BLOCK_THISFUNC(9034);

	GMFApplyReturnValue retValue(MF_NONE, 0);

	GModuleCombat* pmCombat = GetModuleCombat();
	if (pmCombat == NULL) return retValue;

	if (this->IsNPC())
	{
		if (GetChrInfo())
		{
			if (CSAltitudeHelper::IsApplicableToMotionFactor(GetChrInfo()->nAltitude) == false)
			{
				return retValue;
			}
		}	

		// NPC가 움직이는 탤런트를 사용중이면 모션팩터는 무시한다.
		// SEAL(pyo): NPC가 움직이는 탤런트 중에도 모션팩터 적용하게 방침이 바뀜
//		if (m_pModuleCombat->IsUsingMovingTalent()) 
//			return retValue;		
	}

	if (this->IsPlayer())
	{
		// 수영중에는 모션 팩터 무시
		if (GetActionState() == AS_SWIMMING) return retValue;
	}
	
	// 탤런트가 취소불가능이면 모션 팩터는 무시한다. (잡기는 예외)
	if (args.nForceMF != MF_GRAPPLED &&
		args.nForceMF != MF_SWALLOWED && 
		m_pModuleCombat->IsUsingNotCancelableTalent()) return retValue;

	bool bApplyMF = true;
	if (args.nForceMF == MF_NONE)
	{
		PerMulVar<short>	mf_modifier_value[MF_PRIORITY_SIZE];
		PerMulVar<short>	mf_modifier_weight[MF_PRIORITY_SIZE];

		vector<int> vecAdditionIgnoreMF;
		for (int i=0; i<MF_PRIORITY_SIZE; i++)
		{
			// 슈퍼 아머중이면 비튼 모션팩터 무시
			if (pmCombat->IsNowSuperarmorTime() && 
				MF_STATE(i) == MF_BEATEN)
			{
				vecAdditionIgnoreMF.push_back(i);
			}

			mf_modifier_value[i].fPer = 1.0f + GetChrStatus()->ActorModifier.MotionFactorValue[i].fPer;
			mf_modifier_weight[i].fPer = 1.0f + GetChrStatus()->ActorModifier.MotionFactorWeight[i].fPer;

			
			// 상태보정에 따른 모션팩터 적용
			mf_modifier_value[i].fPer *= args.fStateMotionFactor;


			if (IsNPC())
			{
				GEntityNPC* pThisNPC = static_cast<GEntityNPC*>(this);
				mf_modifier_value[i].fPer *= pThisNPC->GetNPCInfo()->MFModifier.Value[i].fPer;
				mf_modifier_weight[i].fPer *= pThisNPC->GetNPCInfo()->MFModifier.Weight[i].fPer;
			}
		}

		bApplyMF = m_MotionFactor.Apply(*args.pMotionFactorGroup, 
										mf_modifier_value, 
										mf_modifier_weight, 
										args.nNotApplyMF, 
										&vecAdditionIgnoreMF,
										args.pOpponent==NULL?false:args.pOpponent->IsSticky());
	}
	else
	{
		bApplyMF = true;
		m_MotionFactor.ClearVars();
		m_MotionFactor.Force(args.nForceMF, args.nForceMFWeight);
	}

	if (bApplyMF)
	{
		retValue.nMotionFactor = m_MotionFactor.GetCurrMF();
		retValue.nWeight = m_MotionFactor.GetCurrWeight();
		
		DoMotionFactor(retValue.nMotionFactor, retValue.nWeight, args.pOpponent, args.bKnockbackDirSyncToOpponentDir);
	}

	return retValue;
}

void GEntityActor::DoMotionFactor(MF_STATE nMF, int nWeight, GEntityActor* pOpponent, bool bKnockbackDirSyncToOpponentDir)
{
	if (nMF == MF_NONE) return;

	if (CSMotionFactorHelper::IsFakeMotionFactor(nMF))
	{
		m_MotionFactor.ReleaseFakes();

		GMotionFactorHandler motionFactorHandler;
		motionFactorHandler.Handle(this, pOpponent, nMF, nWeight, bKnockbackDirSyncToOpponentDir);

		return;
	}

	float fMFReleaseTime = GMotionFactorHelper::CalcReleaseTime(nMF, nWeight, GetStandupTime(), GetTypeID());


	m_MFReleaser.SetTickTime(fMFReleaseTime);
	m_MFReleaser.Start();

	ChangeActionState(AS_MF);

	OnBeginMotionFactor();

	GMotionFactorHandler motionFactorHandler;
	motionFactorHandler.Handle(this, pOpponent, nMF, nWeight, bKnockbackDirSyncToOpponentDir);

	// 탤런트 취소
	//	: 모션팩터로 취소될때는 발동 비용을 지불해야함
	doCancelTalent(false);	
	GTalent* pTalent = GetModuleCombat()->GetTalent();
	if (pTalent &&
		pTalent->IsCancelable())
	{
		// 강제로 탤런트 취소
		//	: 모션팩터로 취소될때는 탤런트 페이즈가 Prepare가 아닐때도 취소해야함
		doCancelTalentForce(false);		
	}
	
	doGuardReleased();
}

bool GEntityActor::doUseTalent(int nTalentID, TALENT_TARGET_INFO Target, bool bCheckEnabled, bool bGainStress)
{
	doGuardReleased();
	GTalentInfo* pTalentInfo = GTalentHelper::FindTalentInfo(nTalentID, this);
	if (!pTalentInfo)
	{
		OnUseTalentFailed(nTalentID, CR_FAIL_SYSTEM_INVALID_TALENT_ID);
		return false;
	}

	return doUseTalent(pTalentInfo, Target, bCheckEnabled, bGainStress);
}

bool GEntityActor::doUseTalent(GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bCheckEnabled, bool bGainStress)
{
	VALID_RET(pTalentInfo, false);

	m_nTalentStartTime = gsys.pSystem->GetNowTime();

	doGuardReleased();

	// 모션 팩터 상태이면 해제함
	if (GetActionState() == AS_MF)
	{
		ReleaseMotionFactor();
	}

	if (IsBeginUnableAction() &&
		!pTalentInfo->m_bIgnoreMesmerize) // 무시 탤런트면 예외
	{
		return false; // 행동 불가 상태
	}

	CCommandResultTable ret =
		m_pModuleCombat->UseTalent(pTalentInfo, Target, bCheckEnabled, bGainStress);

	bool bSuccess = ret == CR_SUCCESS;
	if (!bSuccess)
	{
		OnUseTalentFailed(pTalentInfo->m_nID, ret);
		return false;
	}
	
	return true;
}

void GEntityActor::doCancelTalent(bool bPostCommand)
{
	bool bCancelable=IsNowRunningCancelableTalent();
	
	if (bCancelable)
		m_pModuleCombat->CancelTalent(bPostCommand);
}

void GEntityActor::doCancelTalentForce(bool bPostCommand)
{
	m_pModuleCombat->CancelTalentForce(bPostCommand);
}

void GEntityActor::SetHP(int nHP)
{
	m_ChrStatus.nHP = nHP;
	if (m_ChrStatus.nHP < 0) m_ChrStatus.nHP = 0;
	if (m_ChrStatus.nHP > GetMaxHP()) m_ChrStatus.nHP = GetMaxHP();
}

void GEntityActor::SetEN(int nEN)
{
	m_ChrStatus.nEN = nEN;
	if (m_ChrStatus.nEN < 0) m_ChrStatus.nEN = 0;
	if (m_ChrStatus.nEN > GetMaxEN()) m_ChrStatus.nEN = GetMaxEN();
}

void GEntityActor::SetSTA(int nSTA)
{
	m_ChrStatus.nSTA = nSTA;
	if (m_ChrStatus.nSTA < 0) m_ChrStatus.nSTA = 0;
	if (m_ChrStatus.nSTA > GetMaxSTA()) m_ChrStatus.nSTA = GetMaxSTA();
}

bool GEntityActor::IsValidSensor(const unsigned long nSensorID, vec3 vEventPos)
{
	/*assert(m_pField);*/
	if (!m_pField)														return false;
	assert(m_pField->GetInfo());
	if (!m_pField->GetInfo())											return false;
	
	const SENSOR_INFO* pSensor = m_pField->GetInfo()->FindSensor(nSensorID);
	if (!pSensor)														return false;
//	if (!m_pField->IsEnableSensor(nSensorID))							return false;	// Set/Clear시 직접 처리
	if (GetPos().DistanceTo(vEventPos) > SENSOR_VALID_RADIUS)			return false;	// 센서를 밟은 위치와 서버의 위치가 너무 차이난다.
		
	return true;
}

void GEntityActor::EnterSensor(uint32 nSensorID)
{
	assert(m_pField);
	if (!m_pField)														return;
	assert(m_pField->GetInfo());
	if (!m_pField->GetInfo())											return;

	const SENSOR_INFO* pSensor = m_pField->GetInfo()->FindSensor(nSensorID);
	if (!pSensor)														return;

	float fDistance = pSensor->vPoint.DistanceTo(GetPos());
	if (fDistance >= pSensor->fRadius+SENSOR_ACTIVE_RADIUS_GAP)			return;
	if (IsEnteredSensorID(nSensorID))									return;	// 이미 입장한 센서의 경우

	if (!m_pField->IsEnableSensor(nSensorID))							return;
	AddEnteredSensorID(nSensorID);

	if (_HandleCheckpointTrigger(*pSensor))								return;

	// 워프 - 워프가 일어나면, 이하 센서는 무시한다.
	if (pSensor->HasInnroomWarp())
	{
		if(_HandleInnroomWarpTrigger(*pSensor, TGT_AUTO))				return;
	}
	else
	{
		if (_HandleAutoWarpTrigger(*pSensor))							return;		
	}

	_HandleQuestTrigger(*pSensor);

	// 워프 트리거가 없을 경우, 스크립트로 제어권 이양

	GetScriptField()->OnSensorEnter(m_pField->GetDelegator(), pSensor->nID, GetDelegator());

	if (GConst::LOG_SENSOR)
	{
		mlog("[SensorLog] EnterSensor(id:%d) - owner:%s(%u)\n", nSensorID, MLocale::ConvUTF16ToAnsi(GetName()).c_str(), GetUID());
	}
}

void GEntityActor::LeaveSensor(uint32 nSensorID)
{
	if (m_pField == NULL)	return;		// 최초입장 시, 필드 없음
	VALID(m_pField->GetInfo());

	if (!IsEnteredSensorID(nSensorID))				return;

	RemoveEnteredSensorID(nSensorID);
	if (!m_pField->IsEnableSensor(nSensorID))	return;

	const SENSOR_INFO* pSensor = m_pField->GetInfo()->FindSensor(nSensorID);
	if (!pSensor)														return;

	float fDistance = pSensor->vPoint.DistanceTo(GetPos());
	if (fDistance < pSensor->fRadius-SENSOR_ACTIVE_RADIUS_GAP)			return;

	GetScriptField()->OnSensorLeave(m_pField->GetDelegator(), pSensor->nID, GetDelegator());

	if (GConst::LOG_SENSOR)
	{
		mlog("[SensorLog] LeaveSensor(id:%d) - owner:%s(%u)\n", nSensorID, MLocale::ConvUTF16ToAnsi(GetName()).c_str(), GetUID());
	}
}

void GEntityActor::ClearEnteredSensorID(void)
{
	m_listEnteredSensorID.clear();
}

void GEntityActor::SensorUseTalent(const int nTalentID)
{
	uint32 nForcusedSensorID = GetForcusedSensorID();
	const SENSOR_INFO* pSensor = m_pField->GetInfo()->FindSensor(nForcusedSensorID);
	if (!pSensor)																return;

	// 센서와의 거리 판정
	float fDistance = pSensor->vPoint.DistanceTo(GetPos());
	if (fDistance >= pSensor->fRadius)											return;

	if (!m_pField->IsEnableSensor(nForcusedSensorID))							return;

	GetScriptField()->OnSensorTalent(m_pField->GetDelegator(), pSensor->nID, GetDelegator(), nTalentID);
}

bool GEntityActor::IsEnteredSensorID(int nSensorID)
{
	list<uint32>::iterator it = find(m_listEnteredSensorID.begin(), m_listEnteredSensorID.end(), nSensorID);
	return it != m_listEnteredSensorID.end();
}

void GEntityActor::AddEnteredSensorID(int nSensorID)
{
	if (nSensorID == TRIGGER_INVALID_SENSOR_ID)	return;
	if (IsEnteredSensorID(nSensorID))			return;

	m_listEnteredSensorID.push_back(nSensorID);
}

void GEntityActor::RemoveEnteredSensorID(int nSensorID)
{
	if (!IsEnteredSensorID(nSensorID)) return;

	list<uint32>::iterator it = find(m_listEnteredSensorID.begin(), m_listEnteredSensorID.end(), nSensorID);
	m_listEnteredSensorID.erase(it);
}

uint32 GEntityActor::GetForcusedSensorID()
{
	if (m_listEnteredSensorID.empty())	return TRIGGER_INVALID_SENSOR_ID;
	return m_listEnteredSensorID.back();
}

void GEntityActor::RouteSimpleStatus(MUID uidListener)
{
	TD_UPDATE_SIMPLE_STATUS td_simple_status;
	td_simple_status.nHP = (uint16)m_ChrStatus.nHP;
	td_simple_status.nEN = (uint16)m_ChrStatus.nEN;
	td_simple_status.nSTA = (uint16)m_ChrStatus.nSTA;

	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_UPDATE_SIMPLE_STATUS, uidListener, 1, NEW_SINGLE_BLOB(&td_simple_status, sizeof(TD_UPDATE_SIMPLE_STATUS)));
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GEntityActor::SetScheduleMove(vec3 pos)
{
	m_bScheduleMove = true;
	m_vScheduleMovePos = pos;
}

vec3 GEntityActor::GetScheduleMovePos() const
{
	if (!m_bScheduleMove)
		return vec3::ZERO;

	return m_vScheduleMovePos;
}

bool GEntityActor::DoScheduleMove()
{
	if (!m_bScheduleMove) return false;

	m_bScheduleMove = false;
	SetPos(m_vScheduleMovePos);

	return true;
}

void GEntityActor::ChangeActionState(ACTION_STATE nState)
{
	if (m_nActionState == nState) return;

	OnChangeActionState(nState);

	m_nActionState = nState;
}

void GEntityActor::RouteToMe(MCommand* pCommand) const
{
	SAFE_DELETE(pCommand);
}

bool GEntityActor::IsGuardableDirection(GEntityActor* pAttacker)
{
	if (m_GuardInfo.bGuarding == false) return false;
	TALENT_GUARD_DIRECTION nDirection = m_GuardInfo.nDirection;

	if (nDirection == GUARD_DIRECTION_ALL) return true;

	vec3 attack_dir = pAttacker->GetPos() - this->GetPos();
	attack_dir.Normalize();
	float dot = this->GetDir().DotProduct(attack_dir);

	// 방어 가능한 방향인지 체크
	if (nDirection == GUARD_DIRECTION_FRONT)
	{
		if (dot >= 0.5f)	// 60도
		{
			return true;
		}
	}
	else if (nDirection == GUARD_DIRECTION_FRONT)
	{
		if (dot <= 0) return true;
	}

	return false;
}

void GEntityActor::RouteImmune( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	GTalentHitRouter talentHitRouter;
	talentHitRouter.Route_Immune(pAttacker, this, pTalentInfo);
}

void GEntityActor::RouteAvoid( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	GTalentHitRouter talentHitRouter;
	talentHitRouter.Route_Avoid(pAttacker, this, pTalentInfo);
}

void GEntityActor::RouteMiss( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	GTalentHitRouter talentHitRouter;
	talentHitRouter.Route_Miss(pAttacker, this, pTalentInfo);
}


void GEntityActor::ForceStandUp()
{
	if (GetCurrentMotionFactor() == MF_KNOCKDOWN)
	{
		m_MFReleaser.SetTickTime(0.5);
		m_MFReleaser.Start();
		MCommand* pNewCommand = MakeNewCommand(MC_ACTION_STANDUP, 1, NEW_USHORT(GetUIID()));
		RouteToThisCell(pNewCommand);
	}
}

bool GEntityActor::SetPosToGroundVertical(vec3* pvTarPos)
{
	PFI_BLOCK_THISFUNC(9092);

	if (m_pField)
	{
		vec3 tar_pos;
		tar_pos = (pvTarPos!=NULL) ? *pvTarPos : GetPos();

		PFI_B(9090, "GEntityActor::SetPosToGroundVertical-GField::PickingZ");
		if (m_pField->PickingZ(tar_pos, tar_pos) == false)
		{
#ifdef _DEBUG
			if (GetTypeID() == ETID_NPC)
			{
				GEntityNPC* pThisNPC = static_cast<GEntityNPC*>(this);

				vec3 a, b;
				a = tar_pos; b = pThisNPC->GetSpawnPos();
				b.z = a.z;
				float fDist = a.DistanceTo(b);

				blog("Pick 실패 : Field = %d, SpawnID = %d, pos = (%.1f %.1f %.1f) , dist = %.1f\n",
					pThisNPC->GetFieldID(),
					pThisNPC->GetSpawnInfo()?pThisNPC->GetSpawnInfo()->nID:0,
					tar_pos.x, tar_pos.y, tar_pos.z,
					fDist);
			}

			gsys.pTestSystem->PickingLog(GetUID(), L"pick_fail: GEntityActor::SetPosToGroundVertical");
#endif
		}
		PFI_E(9090);


		//{
		//	vec3 tar_pos = vec3(10678.553f, 8236.6719f, 2800.0f);
		//	if (m_pField->GetGroundVerticalPos(tar_pos, tar_pos) == false)
		//	{

		//	}

		//	mlog("%.4f\n", tar_pos.z);

		//}

		PFI_B(9091, "GEntityActor::SetPosToGroundVertical-GEntityActor::SetPos");
		if (SetPos(tar_pos) == false) return false;
		PFI_E(9091);


		return true;
	}

	// Pick 버그 때문에 주석처리
	//_ASSERT(0);
	return false;
}

bool GEntityActor::IsHittable( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	if (!IsActivateUpdate())
		return false;

	if (IsDead()) 
		return false;
	
	if (false == pAttackTalentInfo->IsEffective(pAttacker, this)) 
		return false;

	return true;
}

bool GEntityActor::IsHittable(GBuffInfo* pBuffInfo )
{
	if (!IsActivateUpdate())
		return false;

	if (IsDead()) 
		return false;

	return true;
}

bool GEntityActor::doGuard( int nGuardTalentID )
{
	if (IsGuarding()) 
		return false;

	GTalentInfo* pGuardTalentInfo = GTalentHelper::FindTalentInfo(nGuardTalentID, this);
	if (pGuardTalentInfo == NULL || pGuardTalentInfo->m_nSkillType != ST_GUARD) 
		return false;

	// 가드할 때 스테미너 소모 안하도록 수정 (사장님 요청사항)
	//GUseCostMgr costMgr;
	//if (!costMgr.Pay_TalentCost(this, pGuardTalentInfo))
	//{
	//	return false;	// 비용 지불 실패
	//}

	m_GuardInfo.bGuarding = true;
	m_GuardInfo.nGuardingTalentID = nGuardTalentID;
	m_GuardInfo.nGuardStartTime = gsys.pSystem->GetNowTime();
	m_GuardInfo.nDirection = pGuardTalentInfo->m_nGuardDirection;

	return true;
}

bool GEntityActor::doGuardReleased()
{
	if (!IsGuarding())	return false;				// 가드중일때만 가드 푼다.

	m_GuardInfo.bGuarding = false;
	m_GuardInfo.nGuardingTalentID = 0;
	m_GuardInfo.nGuardStartTime = 0;
	

	return true;
}

void GEntityActor::Warp(vec3 vPos)
{
	Warp(vPos, GetDir(), true);
}

bool GEntityActor::RemoveBuff(int nBuffID)
{
	if (!GetModuleBuff()->IsGained(nBuffID))
		return false;

	GetModuleBuff()->CancelBuff(nBuffID);

	return true;
}

bool GEntityActor::GainBuff( int nBuffID, GTalentInfo* pTalentInfo/*=NULL*/, GEntityActor* pUser )
{
	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (!pBuffInfo)		
		return false; // 유효하지 않은 버프

	return GainBuffDetail(nBuffID, pBuffInfo->m_fDuration, pBuffInfo->m_fPeriod, pTalentInfo, pUser);
}

bool GEntityActor::GainBuffDetail( int nBuffID, float fDuration, float fPeriod, GTalentInfo* pTalentInfo/*=NULL*/, GEntityActor* pUser )
{
	//	VALID_RET(pTalentInfo, false);	// 탤런트가 NULL 인 것은 허용
	//	VALID_RET(pUser, false);

	if (nBuffID == INVALID_BUFF_ID)
		return false; // 유효하지 않은 버프

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (!pBuffInfo)		
		return false; // 유효하지 않은 버프

	CSBuffEnchantInfo BuffInvokeInfo;
	BuffInvokeInfo.nEnchantEvent = TC_BUFF_GAIN_N_PERIOD;
	BuffInvokeInfo.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;

	return GetModuleBuff()->GainBuffDetail(pBuffInfo, BuffInvokeInfo, pTalentInfo, pUser, fDuration, fPeriod);
}

void GEntityActor::GetBuffList( vector<int>& vBuffList ) const
{
	if (m_pModuleBuff == NULL)		return;

	vBuffList = m_pModuleBuff->GetAllBuffs();
	if (vBuffList.size() >= MAX_OWN_BUFF_NUMBER)
	{
		// 버프 허용 갯수 초과시 최대 갯수로 고정
		vBuffList.resize(MAX_OWN_BUFF_NUMBER);
	}
}

void GEntityActor::GetBuffList(int* outBuffs)
{
	vector<int> vBuffList;
	GetBuffList(vBuffList);

	for (size_t i = 0; i < vBuffList.size(); ++i)
	{
		outBuffs[i] = vBuffList[i];
	}
}

void GEntityActor::RouteActionCancelNextAttack( UIID nEntityUIID, CANCEL_NEXT_ATTACK_PARAM nParam )
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_CANCEL_NEXT_ATTACK, 2, NEW_USHORT(nEntityUIID), NEW_INT(nParam));
	RouteToThisCell(pNewCommand);
}

void GEntityActor::RouteCancelTalent( int nTalentID )
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_CANCEL_TALENT, 2, NEW_USHORT(GetUIID()), NEW_INT(nTalentID));
	RouteToMe(pNewCommand);
}

void GEntityActor::RouteUseTalentRequestFailed( int nTalentID )
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_REQUEST_FAIL_TALENT, 1, NEW_INT(nTalentID));
	RouteToMe(pNewCommand);
}

void GEntityActor::RouteRebirth()
{
	TD_UPDATE_SIMPLE_STATUS td_simple_status;
	td_simple_status.nHP = (uint16)GetHP();
	td_simple_status.nEN = (uint16)GetEN();
	td_simple_status.nSTA = (uint16)GetSTA();

	MCommand* pNewMyCmd = MakeNewCommand(MC_CHAR_REBIRTH_MYPLAYER, 
		4,
		NEW_INT(GetField()->GetInfo()->m_nFieldID),
		NEW_VEC(GetPos()),
		NEW_SVEC2(GetDir()),
		NEW_SINGLE_BLOB(&td_simple_status, sizeof(TD_UPDATE_SIMPLE_STATUS)));
	RouteToMe(pNewMyCmd);	

	MCommand* pNewCommand = MakeNewCommand(MC_CHAR_REBIRTH_NETPLAYER, 1, NEW_UID(GetUID()));
	RouteToThisCellExceptMe(pNewCommand);
}

void GEntityActor::RouteTalentDrain(const GEntityActor* pAttacker, int nTalentID, TALENT_DRAIN_TYPE nDrainType, int nDrainAmount)
{
	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_TALENT_DRAIN,
		3,
		NEW_INT(nTalentID),
		NEW_UCHAR(nDrainType),
		NEW_INT(nDrainAmount));

	pAttacker->RouteToMe(pNewCmd);
}

float GEntityActor::GetColRadius()
{
	if (GetMeshInfo())
	{
		return GetMeshInfo()->GetColRadius();
	}
	return DEFAULT_ENTITY_RADIUS;
}

float GEntityActor::GetHitRadius()
{
	if (GetMeshInfo())
	{
		return GetMeshInfo()->GetHitRadius();
	}
	return DEFAULT_ENTITY_RADIUS;
}

void GEntityActor::doTryHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo, int nAddtiveDamage, uint16 nCheckTime, int nCapsuleGroup, int nCapsuleIndex)
{
	if (!IsHittable(pAttacker, pTalentInfo)) return;

	PFI_BLOCK_THISFUNC(2002);

	MUID uidThis = GetUID();
	MUID uidAttacker = pAttacker->GetUID();

	if (GConst::AIMON_ENABLE &&
		GConst::AIMON_AUTO_TARGET &&
		IsNPC())
	{
		GAIMonitorPort::Inst().SetNPC(GetUID());
		gsys.pTestSystem->SetNPCMonitor(pAttacker->GetUIID(), GetUIID());
	}
	
	// 면역 처리
	if (HandleImmune(pAttacker, pTalentInfo))
		return;

	GActorDamageCalculator	DamageCalculator;
	GActorDamageCalculator::Desc desc;
	desc.pAttacker = pAttacker;
	desc.pVictim = this;
	desc.pTalentInfo = pTalentInfo;
	desc.DamageInfo = GDamageRangedInfo(pTalentInfo->m_nMaxDamage, pTalentInfo->m_nMinDamage);
	desc.HealInfo = GHealRangedInfo(pTalentInfo->m_nMaxHeal, pTalentInfo->m_nMinHeal);
	desc.nHitCapGroup = nCapsuleGroup;
	desc.nHitCapIndex = nCapsuleIndex;
	GCombatTurnResult combat_result = 
		DamageCalculator.CalcDamage(desc);
	int nDamage = combat_result.nDamage;
	int nHealAmount = combat_result.nHealAmount;

	GuardEffector guard_effector;
	GuardEffector::Result nGuardResult = 
		guard_effector.CheckGuard(this, pAttacker, pTalentInfo);

	switch (nGuardResult)
	{
	case GuardEffector::GUARD_SUCCESS:
		{
			// 일반 방어처리
			GUARD_TYPE nGuardType = guard_effector.GetGuardLevel(this, pAttacker, pTalentInfo);
			if (nGuardType == GT_INVALID)
				break;	// 2차 방어검사에서 방어 실패로 처리됨

			guard_effector.ApplyEffect_Guard(this, pAttacker, pTalentInfo, nGuardType, nCheckTime, nDamage);
			OnGuard(pAttacker, pTalentInfo);

			// 세부 방어처리
			switch (nGuardType)
			{
			case GT_PARTIAL:
				guard_effector.ApplyEffect_PartialGuard(this, pAttacker, pTalentInfo, nDamage);
				OnPartialGuard(this, pAttacker, pTalentInfo);
				break;
			case GT_PERFECT:
				guard_effector.ApplyEffect_PerfectGuard(this, pAttacker, pTalentInfo, nDamage);
				OnPerfectGuard(pAttacker, pTalentInfo);
				break;
			case GT_ABSOLUTE:
				OnAbsoluteGuard(pAttacker, pTalentInfo);
				break;
			}

			// 방어 성공, 종료됨
			return;	// 방어 성공시 여기서 끝!!
		}break;
	case GuardEffector::GUARD_FAILED:
		{

		}break;
	}

	GHitInfo infoHit;
	infoHit.bHitProcessed		= false;
	infoHit.nDamageType			= GetDamageType(pTalentInfo);
	infoHit.nDamage				= nDamage;
	infoHit.nHealAmount			= nHealAmount;
	infoHit.nCheckTime			= nCheckTime;
	infoHit.nCapsuleGroup		= nCapsuleGroup;
	infoHit.nCapsuleIndex		= nCapsuleIndex;
	infoHit.bTryGuard			= guard_effector.IsTryGuard(this);
	infoHit.bCriticalHit		= CheckBitSet(combat_result.nResultFlags,CTR_CRITICAL) != 0;
	infoHit.bBackHit			= IsBack(pAttacker->GetPos());
	infoHit.nTalentID			= pTalentInfo->m_nID;

	infoHit.pTalentInfo			= pTalentInfo;

	
	if (CheckBitSet(combat_result.nResultFlags,CTR_RESIST_PERFECT))
	{
		// 완전 저항 성공
		infoHit.bHitProcessed			= true;

		// 완전 저항도 패킷 날림
		GTalentHitRouter talentHitRouter;
		talentHitRouter.Route_Resist(pAttacker, this, pTalentInfo, infoHit.nCheckTime, combat_result.nResultFlags);

		return;
	}
	
	if (CheckBitSet(combat_result.nResultFlags,CTR_MISS))
	{
		// 회피 성공
		OnMiss(combat_result.nResultFlags, pAttacker, pTalentInfo);

		return;
	}
	
	// 기획변경(pyo): RIPOSTE는 상대방과 함께 자기자신도 맞도록 수정됨
	if (CheckBitSet(combat_result.nResultFlags,CTR_RIPOSTE))
	{
		// Riposte는 대미지만 적용시킨다.
		float fRiposteRate = m_Riposte.GetRiposteRate(pTalentInfo->m_nCategory);
		int nDamage = (int)(gsys.pCombatCalculator->CalcDamage(	pAttacker, 
																this, 
																pTalentInfo, 
																false, 
																1.0f, 
																GDamageRangedInfo(pTalentInfo->m_nMaxDamage, pTalentInfo->m_nMinDamage)) * fRiposteRate);

		nDamage = max(nDamage, 1); // 최소 1의 피해는 줌

		pAttacker->doDamage(GetUID(), pTalentInfo->m_nDamageAttrib, nDamage);
		GTalentHitRouter talentHitRouter;
		talentHitRouter.RouteTalentHit(this, pAttacker, pTalentInfo, nDamage, MF_NONE, 0, 0, combat_result.nResultFlags);
		ClearBitSet(combat_result.nResultFlags,CTR_RIPOSTE);
	}


	infoHit.nCombatResultFlags	= combat_result.nResultFlags;
	infoHit.pAttacker			= pAttacker;
	infoHit.pVictim				= this;
	bool bRouteHit = pTalentInfo->m_nSkillType != ST_ITEM;
	PFI_B(9001, "GEntityActor::OnHit");
	OnHit(infoHit, bRouteHit);
	PFI_E(9001);

	// 흡수 데미지 처리
	if (CheckBitSet(combat_result.nResultFlags,CTR_DRAIN))
	{
		int nDrain = (int)(nDamage * pTalentInfo->m_Drain.m_fRate);
		switch (pTalentInfo->m_Drain.m_nType)
		{
		case TALENT_DRAIN_HP: pAttacker->SetHP(pAttacker->GetHP()+nDrain);	break;
		case TALENT_DRAIN_EN: pAttacker->SetEN(pAttacker->GetEN()+nDrain);	break;
		case TALENT_DRAIN_SP: pAttacker->SetSTA(pAttacker->GetSTA()+nDrain);	break;
		};

		RouteTalentDrain(pAttacker, pTalentInfo->m_nID, pTalentInfo->m_Drain.m_nType, nDrain);
	}
}

void GEntityActor::OnHit(GHitInfo infoHit, bool bRouteHit)
{
	GTalentInfo* pTalentInfo = infoHit.pTalentInfo;
	GEntityActor* pAttacker = infoHit.pAttacker;
	VALID(pTalentInfo);
	VALID(pAttacker);

	m_uidLastHitOrHittedEnemy = pAttacker->GetUID();

	// 히트정보 저장
	m_LastHitinfo = infoHit;
	bool bCriticalHit = infoHit.bCriticalHit;	
	
	// 이벤트 호출
	__super::OnHit(this, pAttacker, pTalentInfo);	

	switch(pTalentInfo->m_nCategory)
	{
	case TC_MELEE:
		OnMeleeHit(pAttacker, pTalentInfo);	
		break;
	case TC_MAGIC:
		OnMagicHit(pAttacker, pTalentInfo);	
		break;
	}

 	pAttacker->OnHitEnemy(infoHit.nCombatResultFlags, this, pTalentInfo);

	// 공격자가 피격자에게 효과를 줌
	GTalentEffector::Info info;
	info.pTalentInfo = pTalentInfo;
	info.pUser = pAttacker;
	info.pTarget = this;
	info.nTalentCondition = TC_HIT_TALENT;

	PFI_B(9002, "GEntityActor::OnHit-GTalentEffector::ApplyTalent");
	m_TalentEffector.ApplyTalent(info);
	PFI_E(9002);

	// 크리티컬 이벤트
	if (bCriticalHit)
	{
		OnCriHit(pAttacker, pTalentInfo);
	}
}

void GEntityActor::OnHitSuccess( GTalentInfo* pTalentInfo )
{
	VALID(pTalentInfo);

	__super::OnHitSuccess(this, pTalentInfo);

	// 공격자가 피격자에게 효과를 줌
	GTalentEffector::Info info;
	info.pTalentInfo = pTalentInfo;
	info.pUser = this;
	info.nTalentCondition = TC_HIT_SUCCESS;
	m_TalentEffector.ApplyTalent(info);
}

void GEntityActor::OnHitAll(GTalentInfo* pTalentInfo)
{
	VALID(pTalentInfo);

	__super::OnHitAll(this, pTalentInfo);

	// 공격자가 피격자에게 효과를 줌
	GTalentEffector::Info info;
	info.pTalentInfo = pTalentInfo;
	info.pUser = this;
	info.nTalentCondition = TC_HIT_ALL_TALENT;
	m_TalentEffector.ApplyTalent(info);
}

void GEntityActor::OnMiss( uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttacker);

	GTalentEffector::Info info;
	info.pTalentInfo = pAttackTalentInfo;
	info.pUser = pAttacker;
	info.pTarget = this;
	info.nTalentCondition = TC_MISS_ME;
	m_TalentEffector.ApplyTalent(info);

	__super::OnMiss(this, nCombatResultFalg, pAttacker, pAttackTalentInfo);
	RouteMiss(pAttacker, pAttackTalentInfo);

	pAttacker->OnMissEnemy(nCombatResultFalg, this, pAttackTalentInfo);
}

void GEntityActor::OnGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttacker);

	GTalentInfo* pGuardTalentInfo = GTalentHelper::FindTalentInfo(m_GuardInfo.nGuardingTalentID, this);
	VALID(pGuardTalentInfo);

	GTalentEffector::Info info;
	info.pTalentInfo = pGuardTalentInfo;
	info.pUser = this;
	info.pTarget = pAttacker;
	info.nTalentCondition = TC_GUARD_ME;
	info.bIgnoreMF = pAttackTalentInfo->m_nSkillType != ST_MELEE; 	// 밀리 공격이 아니면 방어에 의한 모션팩터 무시
	m_TalentEffector.ApplyTalent(info);

	// 가드 정보 저장
	m_LastHitinfo.bHitProcessed = false;
	m_LastHitinfo.bGuarded = true;

	__super::OnGuard(this, pAttacker, pAttackTalentInfo);

	pAttacker->OnGuardEnemy(this, pAttackTalentInfo);
}

void GEntityActor::OnAbsoluteGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	__super::OnAbsoluteGuard(this, pAttacker, pAttackTalentInfo);

	GTalentInfo* pGuardTalentInfo = GTalentHelper::FindTalentInfo(m_GuardInfo.nGuardingTalentID, this);
	VALID(pGuardTalentInfo);

	GTalentEffector::Info info;
	info.pTalentInfo = pGuardTalentInfo;
	info.pUser = this;
	info.pTarget = pAttacker;
	info.nTalentCondition = TC_PERFECT_GUARD_ME;
	info.bIgnoreMF = pAttackTalentInfo->m_nSkillType != ST_MELEE; 	// 밀리 공격이 아니면 방어에 의한 모션팩터 무시
	m_TalentEffector.ApplyTalent(info);
}

void GEntityActor::OnPerfectGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	__super::OnPerfectGuard(this, pAttacker, pAttackTalentInfo);

	GTalentInfo* pGuardTalentInfo = GTalentHelper::FindTalentInfo(m_GuardInfo.nGuardingTalentID, this);
	VALID(pGuardTalentInfo);

	GTalentEffector::Info info;
	info.pTalentInfo = pGuardTalentInfo;
	info.pUser = this;
	info.pTarget = pAttacker;
	info.nTalentCondition = TC_PERFECT_GUARD_ME;
	info.bIgnoreMF = pAttackTalentInfo->m_nSkillType != ST_MELEE; 	// 밀리 공격이 아니면 방어에 의한 모션팩터 무시
	m_TalentEffector.ApplyTalent(info);
}

void GEntityActor::OnHitEnemy( uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	m_uidLastHitOrHittedEnemy = pTarget->GetUID();

	GTalent* pTalent = GetModuleCombat()->GetTalent();
	if (pTalent)
	{
		pTalent->OnGainDamage(pTarget);
	}

	GTalentEffector::Info info;
	info.pTalentInfo = pTalentInfo;
	info.pUser = this;
	info.pTarget = pTarget;
	info.nTalentCondition = TC_HIT_ENEMY;
	m_TalentEffector.ApplyTalent(info);	
		
	__super::OnHitEnemy(this, nCombatResultFalg, pTarget, pTalentInfo);

	switch(pTalentInfo->m_nCategory)
	{
	case TC_MELEE:
		OnMeleeHitEnemy(pTarget, pTalentInfo);	
		break;
	case TC_MAGIC:
		OnMagicHitEnemy(pTarget, pTalentInfo);	
		break;
	}

}

void GEntityActor::OnCriHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	GTalentEffector::Info info;
	info.pTalentInfo = pTalentInfo;
	info.pUser = pAttacker;
	info.pTarget = this;
	info.nTalentCondition = TC_CRITICAL_HIT_ME;
	m_TalentEffector.ApplyTalent(info);

	__super::OnCriHit(this, pAttacker, pTalentInfo);

	pAttacker->OnCriHitEnemy(this, pTalentInfo);
}

void GEntityActor::OnCriHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	GTalentEffector::Info info;
	info.pTalentInfo = pTalentInfo;
	info.pUser = this;
	info.pTarget = pTarget;
	info.nTalentCondition = TC_CRITICAL_HIT_ENEMY;
	m_TalentEffector.ApplyTalent(info);

	__super::OnCriHitEnemy(this, pTarget, pTalentInfo);
}

void GEntityActor::OnMissEnemy( uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	GTalentEffector::Info info;
	info.pTalentInfo = pAttackTalentInfo;
	info.pUser = this;
	info.pTarget = pTarget;
	info.nTalentCondition = TC_MISS_ENEMY;
	m_TalentEffector.ApplyTalent(info);

	__super::OnMissEnemy(this, nCombatResultFalg, pTarget, pAttackTalentInfo);
}

void GEntityActor::OnGuardEnemy( GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	GTalentEffector::Info info;
	info.pTalentInfo = pAttackTalentInfo;
	info.pUser = this;
	info.pTarget = pTarget;
	info.nTalentCondition = TC_GUARD_ENEMY;
	m_TalentEffector.ApplyTalent(info);

	__super::OnGuardEnemy(this, pTarget, pAttackTalentInfo);
}

void GEntityActor::OnFinishTalent( GTalentInfo* pTalentInfo )
{
	__super::OnFinishTalent(pTalentInfo);

	SensorUseTalent(pTalentInfo->m_nID);
	GetActorHateTracker().AddHate_UseTalent(pTalentInfo->m_nHateAdj);
}

bool GEntityActor::SetMFModValue( int nType, float fValue )
{
	if (nType < 0 || nType >= MF_PRIORITY_SIZE)
	{
		dlog("GEntityActor::SetMFModValue(): type(%d)의 범위가 유효하지 않습니다.\n", nType);
		return false;
	}
	
	m_ChrStatus.ActorModifier.MotionFactorValue[nType].fPer += fValue;

	return true;
}

bool GEntityActor::SetMFModWeight( int nType, float fWeight )
{
	if (nType < 0 || nType >= MF_PRIORITY_SIZE)
	{
		dlog("GEntityActor::SetMFModWeight(): type(%d)의 범위가 유효하지 않습니다.\n", nType);
		return false;
	}

	m_ChrStatus.ActorModifier.MotionFactorWeight[nType].fPer += fWeight;

	return true;
}

void GEntityActor::UpdateCombat( float fDelta )
{
	m_pModuleCombat->Update(fDelta);
}

void GEntityActor::UpdateBuff( float fDelta )
{
	m_pModuleBuff->Update(fDelta);
}


bool GEntityActor::HasBuff( int nBuffID )
{
	GModuleBuff* pmBuff = GetModuleBuff();
	VALID_RET(pmBuff, false);

	return pmBuff->IsGained(nBuffID);
}

bool GEntityActor::HasBuff_ForTest( int nBuffID )
{
	GModuleBuff* pmBuff = GetModuleBuff();
	VALID_RET(pmBuff, false);

	return pmBuff->IsGained_ForTest(nBuffID);
}

int GEntityActor::GetBuffQty()
{
	GModuleBuff* pmBuff = GetModuleBuff();
	VALID_RET(pmBuff, false);

	return pmBuff->GetBuffQty();
}

int GEntityActor::GetDebuffQty()
{
	GModuleBuff* pmBuff = GetModuleBuff();
	VALID_RET(pmBuff, false);

	return pmBuff->GetDebuffQty();
}

void GEntityActor::LostBuff( int nBuffID )
{
	GModuleBuff* pmBuff = GetModuleBuff();
	VALID(pmBuff);

	pmBuff->CancelBuff(nBuffID);
}

void GEntityActor::LostBuffStack( int nBuffIDSlot )
{
	GModuleBuff* pmBuff = GetModuleBuff();
	VALID(pmBuff);

	pmBuff->LostBuffStack(nBuffIDSlot);
}

void GEntityActor::AddRiposte( CSRiposte infoRiposte )
{
	m_Riposte.AddRiposte(infoRiposte.nType, infoRiposte.fRate);
}

void GEntityActor::EraseRiposte( CSRiposte infoRiposte )
{
	m_Riposte.EraseRiposte(infoRiposte.nType, infoRiposte.fRate);
}

void GEntityActor::SetDirForwardToActor(GEntityActor* pTarget, bool bRoute)
{
	if (!pTarget)		return;
	if (pTarget == this) return;

	vec3 vDir = pTarget->GetPos() - GetPos();
	GMath::NormalizeZ0(vDir);

	if (vDir.LengthSq() <= 0.001f) return;

	SetDir(vDir);
	SetFacingDir(vDir);

	if (bRoute)
	{
		RouteFaceTo(vDir);
		return;

	}
}

void GEntityActor::SetDirForwardToActor(MUID uidTarget, bool bRoute)
{
	if (uidTarget == GetUID()) return;
	GEntityActor* pTarget = FindActor(uidTarget);
	if (!pTarget)		return;

	SetDirForwardToActor(pTarget, bRoute);
}

GEntityActor* GEntityActor::FindActor(const MUID& uidActor) const
{
	if (!m_pField)
		return NULL;

	return m_pField->FindActor(uidActor);
}

void GEntityActor::_FillRegenVar()
{
	VALID(m_pChrInfo);
	m_pChrInfo->nHPRegen = GConst::HP_REGEN_AMOUNT;
	m_pChrInfo->nENRegen = GConst::EN_REGEN_AMOUNT;
	m_pChrInfo->nSTARegen = GConst::STA_REGEN_AMOUNT;
}

void GEntityActor::ClearLastHitInfo()
{
	m_LastHitinfo.bHitProcessed		= true;

}

DAMAGE_ATTRIB GEntityActor::GetEnchantAttrib() const
{
	GTalent* pTalent = m_pModuleCombat->GetTalent();
	if (NULL == pTalent) return DA_NONE;	

	return pTalent->GetInfo()->m_nDamageAttrib;
}

bool GEntityActor::CheckTalentRange( GEntityActor* pTargetActor, int nTalentID )
{
	return m_pModuleCombat->CheckTalentRange(pTargetActor, nTalentID);
}

bool GEntityActor::IsTalentCasting()
{
	GTalent* pTalent = m_pModuleCombat->GetTalent();
	if (!pTalent)										return false;	// 사용중인 탤런트없음
	if (pTalent->GetPhase() != TALENT_PHASE_PREPARE)	return false;	// 탤런트 준비 단계 아님
	return true;
}

GTalent* GEntityActor::GetCurrentTalent()
{
	return m_pModuleCombat->GetTalent();
}

GGlueActor* GEntityActor::NewDelegator()
{
	return new GGlueActor(this);
}

GGlueActor* GEntityActor::GetDelegator()
{
	VALID_RET(m_pDelegator, NULL);
	return m_pDelegator;
}

void GEntityActor::CreateDelegator()
{
	m_pDelegator = NewDelegator();
}

bool GEntityActor::IsNowInvincibility()
{
	VALID_RET(m_pModuleCombat, false);
	VALID_RET(m_pModuleBuff, false);
	if (m_pModuleCombat->IsNowInvincibleTime())		return true;
	if (m_pModuleBuff->IsNowInvincibility())		return true;
	
	return false;
}

bool GEntityActor::IsNowAvoidTime()
{
	VALID_RET(m_pModuleCombat, false);
	if (m_pModuleCombat->IsNowAvoidTime())		return true;

	return false;
}

bool GEntityActor::IsNowInvisibility() const
{
	if (!m_pModuleBuff)
		return false;

	if (m_pModuleBuff->IsNowInvisibility())		return true;

	return false;
}

bool GEntityActor::IsNowInvisibilityToNPC()
{
	VALID_RET(m_pModuleCombat, false);
	VALID_RET(m_pModuleBuff, false);
	if (m_pModuleBuff->IsNowInvisibilityToNPC())		return true;

	return false;
}

bool GEntityActor::IsDisableTalent( GTalentInfo* pTalentInfo )
{
	VALID_RET(pTalentInfo, false);
	VALID_RET(m_pModuleBuff, false);
	if (pTalentInfo->m_bIgnoreMesmerize)				return false;
	if (m_pModuleBuff->IsDisableAllTalent())		return true;
	if (m_pModuleBuff->IsDisableTalent(pTalentInfo->m_nCategory))	return true;
	if (m_pModuleBuff->IsMesmerize())				return true;

	return false;
}

bool GEntityActor::IsMovable()
{
	VALID_RET(m_pModuleBuff, false);
	if (m_pModuleBuff->IsMesmerize())		return false;
	if (GetWalkSpeed() <= 0.0f)				return false;

	return true;
}

bool GEntityActor::IsSticky()
{
	VALID_RET(m_pModuleBuff, false);
	if (m_pModuleBuff->IsMesmerize())		return true;
	
	return false;
}


void GEntityActor::RouteDie()
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_DIE, 2, 
		NEW_USHORT(m_nUIID), NEW_UID(GetKiller()));
	RouteToThisCell(pNewCommand);
}

void GEntityActor::OnRebirth()
{
	GEntity::OnRebirth();

	SetHP(1);
	SetEN(1);
	SetSTA(1);	
}

void GEntityActor::OnRebirthBySelf()
{
	OnRebirth();
}

void GEntityActor::OnRebirthByOther()
{
	OnRebirth();
}

void GEntityActor::doRebirthBySelf()
{
	if (!IsDead()) return;

	OnRebirthBySelf();
}

void GEntityActor::doRebirthByOther()
{
	if (!IsDead()) return;

	OnRebirthByOther();
}

bool GEntityActor::HandleImmune( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	VALID_RET(pAttacker, false);
	VALID_RET(pTalentInfo, false);

	{
		GRelationChecker relationCheker;
		if (relationCheker.IsAlly(pAttacker, this))
			return false; // 아군이 주는 효과면 무적 처리 하지 않음
	}

	// 회피 시간중이면 면역처리
	if (pTalentInfo->m_bAvoidable &&
		IsNowAvoidTime())
	{
		OnAvoid(pTalentInfo, pAttacker);
		return true;	
	}

	// 무적 시간중이면 면역처리
	if (IsNowInvincibility())
	{
		OnImmuned(pTalentInfo, pAttacker);
		return true;	
	}

	// 특정 피해에 대한 면역처리
	if (m_ImmuneHandler.IsDamageTypeImmune(pAttacker->GetDamageType(pTalentInfo)))
	{
		OnImmuned(pTalentInfo, pAttacker);
		return true;
	}

	// 특정 스킬타입에 대한 면역처리
	if (m_ImmuneHandler.IsSkillTypeImmune(pTalentInfo->m_nSkillType))
	{
		OnImmuned(pTalentInfo, pAttacker);
		return true;
	}

	// 드레인에 대한 면역처리
	if (pTalentInfo->m_Drain.IsDrainTalent() &&
		m_ImmuneHandler.IsImmune(immune::IMMUNE_DRAIN))
	{
		OnImmuned(pTalentInfo, pAttacker);
		return true;
	}

	return false;
}

bool GEntityActor::HandleImmune( GEntityActor* pAttacker, GBuffInfo* pBuffInfo )
{
	VALID_RET(pBuffInfo, false);

	// 무적 시간중이면 면역처리
	if (IsNowInvincibility())
	{
		return true;	
	}

	// 특정 피해에 대한 면역처리
	if (m_ImmuneHandler.IsDamageTypeImmune(pBuffInfo->m_nDamageAttrib))
	{
		return true;
	}

	return false;
}

DAMAGE_ATTRIB GEntityActor::GetDamageType( const GTalentInfo* pTalentInfo )
{
	if (pTalentInfo)
	{
		// 탤런트에 피해정보가 있다면 반환
		if (pTalentInfo->m_nDamageAttrib != DA_NONE)
			return pTalentInfo->m_nDamageAttrib;
	}

	return DA_NONE;
}

bool GEntityActor::HasMotionFactor() const
{
	return m_MotionFactor.GetCurrMF() != MF_NONE;
}

int GEntityActor::GetGuardTalentID()
{
	return m_GuardInfo.nGuardingTalentID;
}

void GEntityActor::OnChangeActionState( ACTION_STATE nNextState )
{
	if (nNextState == AS_SWIMMING)
	{
		// 수영 상태가 될때 모션팩터 해제
		ReleaseMotionFactor();
	}
}

int GEntityActor::GetResist( TALENT_RESIST_TYPE nResistType ) const
{
	_ASSERT(nResistType != TRT_CHA);	// 캐릭터 resist값에 cha는 없음
	return m_ChrStatus.ActorModifier.nResistances[nResistType].Calc(m_pChrInfo->Resist.nValue[nResistType]);
}

void GEntityActor::RouteToInvisibleDetectors(MCommand* pCommand, MUID uidExcept) const
{
	VALID(pCommand);

	// 투명화됐을때는 나를 보는 녀석들에게만 알리기
	for each (const MUID& uidSeer in m_pInvisibility->GetDetectors())
	{
		if (uidSeer == uidExcept)
			continue;

		pCommand->AddReceiver(uidSeer);
	}

	if (GetUID() != uidExcept)
	{
		pCommand->AddReceiver(GetUID());
	}

	gsys.pCommandCenter->PostCommand(pCommand);
}

void GEntityActor::RouteToThisCell( MCommand* pCommand, MUID uidExcept/*=0*/) const 
{
	if (IsNowInvisibility())
	{
		// 투명화 됐을때의 섹터 라우팅
		RouteToInvisibleDetectors(pCommand, uidExcept);
	}
	else
	{
		// 일반적인 섹터 라우팅
		__super::RouteToThisCell(pCommand, uidExcept);
	}
}

float GEntityActor::GetValidInteractionDistance()
{
	return CSCharHelper::CalcValidInteractionDistance(GetColRadius());
}

void GEntityActor::doMakePeace()
{
	GetActorHateTracker().Clear();
}

void GEntityActor::OnDestroy()
{
	GActorObserverMgr::OnDestroy();
	GEntitySync::OnDestroy();
}

bool GEntityActor::IsAllowCalcVictimDodge()
{	
	if (GetTypeID() != ETID_PLAYER)		return true;		// 플레이어가 아니면 항상 허용

	if (NULL == GetCurrentTalent())		return true;		// IDLE 상태면 허용
	if (IsMoving())						return true;		// 이동 중이면 허용
	if (IsGuarding())					return true;		// 가드 중이면 허용
	if (IsJumping())					return true;		// 점프 중이여도 허용

	return false;
}

float GEntityActor::DefaultWalkSpeed() const
{
	return m_pChrInfo->fSpeed;
}

void GEntityActor::TrimStats()
{
	if (GetMaxHP() < GetHP())
	{
		SetHP(GetMaxHP());
	}

	if (GetMaxEN() < GetEN())
	{
		SetEN(GetMaxEN());
	}

	if (GetMaxSTA() < GetSTA())
	{
		SetSTA(GetMaxSTA());
	}
}

int GEntityActor::GetMaxHP() const
{
	return max(1, m_ChrStatus.ActorModifier.nHPMax.Calc(GetMaxHPProto()));
}

int GEntityActor::GetMaxEN() const
{
	return max(1, m_ChrStatus.ActorModifier.nENMax.Calc(GetMaxENProto()));
}

int GEntityActor::GetMaxSTA() const
{
	return max(1, m_ChrStatus.ActorModifier.nSTAMax.Calc(GetMaxSTAProto()));
}

void GEntityActor::OnImmuned( GTalentInfo* pTalentInfo, GEntityActor* pAttacker )
{
	VALID(pTalentInfo);
	VALID(pAttacker);
	OnTalentImmuned(this, pAttacker, pTalentInfo);
	RouteImmune(pAttacker, pTalentInfo);

	pAttacker->OnTalentImmunedEnemy(pAttacker, this, pTalentInfo);
}

void GEntityActor::OnAvoid( GTalentInfo* pTalentInfo, GEntityActor* pAttacker )
{
	VALID(pTalentInfo);
	VALID(pAttacker);
	OnTalentAvoid(this, pAttacker, pTalentInfo);
	RouteAvoid(pAttacker, pTalentInfo);

	pAttacker->OnTalentAvoidEnemy(pAttacker, this, pTalentInfo);
}



void GEntityActor::OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	__super::OnActTalent(pUser, pTalentInfo);

	if (pTalentInfo->m_nCategory == TC_MAGIC)
	{
		OnMagicActTalent(pTalentInfo);
	}
}