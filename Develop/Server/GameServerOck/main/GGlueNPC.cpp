#include "stdafx.h"
#include "GGlueNPC.h"
#include "GEntityNPC.h"
#include "GNullGlueActor.h"
#include "GNullGluePlayer.h"
#include "GNullGlueNPC.h"
#include "GScriptUtil.h"
#include "GScriptManager.h"
#include "GScriptGroup_Field.h"
#include "GScriptGroup_NPC.h"
#include "GGlueField.h"
#include "GNPCDecay.h"
#include "GNPCInteraction.h"
#include "GEntityPlayer.h"
#include "GNPCSwallow.h"
#include "GNPCSensorChecker.h"
#include "GJobRunner.h"
#include "GNPCTarget.h"
#include "GNPCMinion.h"
#include "GNPCBParts.h"
#include "GNPCMove.h"
#include "GGlobal.h"
#include "GTestSystem.h"
#include "GActorCooltimeChecker.h"
#include "GJobMgr.h"
#include "MLocale.h"
#include "GPlayerEnemyInfoSender.h"
#include "GHateTable.h"
#include "GConst.h"
#include "GAIMonitorPort.h"

GGlueNPC::GGlueNPC(GEntityNPC* pOwner)
: GGlueActor(pOwner)
{
	// do nothing
}

GGlueNPC::~GGlueNPC()
{
	// do nothing
}


GEntityNPC* GGlueNPC::GetOwnerNPC()
{
	return static_cast<GEntityNPC*>(m_pOwner);
}

int GGlueNPC::GetUserData(int index)
{
	if (IsInvalidOwner())	return -1;
	return GetOwnerNPC()->GetUserdata(index);
}

void GGlueNPC::SetUserData(int index, int value)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->SetUserdata(index, value);
}

void GGlueNPC::ReserveUserData(int nNPCID, int nSize)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->ReserveUserdata(nNPCID, nSize);
}

void GGlueNPC::Move(vec3 vTarPos)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vTarPos);

	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Move(vTarPos);
}

void GGlueNPC::MoveToMarker(int nMarkerID)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->MoveToMarker(nMarkerID);
}

void GGlueNPC::MoveToSensor(unsigned long nMarkerID )
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->MoveToSensor(nMarkerID);
}

void GGlueNPC::Attack(GGlueActor* pTargetActorDelegator)
{
	if (IsInvalidOwner())	return;

	if (pTargetActorDelegator == NULL || pTargetActorDelegator->IsNull())
	{
		return;
	}

	GEntityActor* pActor = pTargetActorDelegator->GetOwnerActor();
	return GetOwnerNPC()->Attack(pActor);
}

void GGlueNPC::Follow(GGlueActor* pActorDelegator, float fLeastDistance)
{
	if (IsInvalidOwner())	return;

	if (pActorDelegator == NULL || pActorDelegator->IsNull())		
	{
		return;
	}

	GEntityActor* pActor = pActorDelegator->GetOwnerActor();
	return GetOwnerNPC()->Follow(pActor, fLeastDistance);
}

void GGlueNPC::EnableCombat(void)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->ChangeAttackable(NAT_ALWAYS);
}

void GGlueNPC::EnableCombatWithFaction(void)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->ChangeAttackable(NAT_FACTION);
}

void GGlueNPC::DisableCombat(void)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->ChangeAttackable(NAT_NONE);
}

void GGlueNPC::EnableInteract( bool bEnable )
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->EnableInteract(bEnable);
}

bool GGlueNPC::UseTalent(int nTalentID, GGlueActor* pActorDelegator)
{
	if (IsInvalidOwner())	return false;

	if (pActorDelegator == NULL || pActorDelegator->IsNull())
	{
		return false;
	}

	GEntityActor* pActor = pActorDelegator->GetOwnerActor();
	MUID uidOpponent = pActor->GetUID();
	return GetOwnerNPC()->UseTalent(nTalentID, uidOpponent, true);
}

bool GGlueNPC::UseTalentSelf(int nTalentID)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerNPC()->UseTalentSelf(nTalentID);
}

bool GGlueNPC::MoveToActor(GGlueActor* pTargetActorDelegator, int nTalentID)
{
	if (IsInvalidOwner())	return false;

	if (pTargetActorDelegator == NULL || pTargetActorDelegator->IsNull())
	{
		return false;
	}

	GEntityActor* pActor = pTargetActorDelegator->GetOwnerActor();
	return GetOwnerNPC()->MoveToUseTalent(pActor, nTalentID);
}

bool GGlueNPC::MoveToActorWithDuration(GGlueActor* pTargetActorDelegator, int nTalentID, float fDuration)
{
	if (IsInvalidOwner())	return false;

	if (pTargetActorDelegator == NULL || pTargetActorDelegator->IsNull())
	{
		return false;
	}

	GEntityActor* pActor = pTargetActorDelegator->GetOwnerActor();
	return GetOwnerNPC()->MoveToUseTalentCloserWithDuration(pActor, nTalentID, fDuration);
}

void GGlueNPC::Roam(float fRadius, float fDurationTime)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Roam(fRadius, fDurationTime);
}

void GGlueNPC::Patrol(MWLua::table tableMarkers, int nPatrolType)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Patrol(tableMarkers, nPatrolType);
}

void GGlueNPC::PatrolInstant(MWLua::table tableMarkers, int nPatrolType)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->PatrolInstant(tableMarkers, nPatrolType);
}

void GGlueNPC::ClearJob(void)
{
	if (IsInvalidOwner())	return;
	
	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(GetOwnerNPC()->GetUID()))
	{
		GAIMonitorPort::Inst().OnJobEvent(false, "ClearJob - cause by 'Glue Function'");
	}

	return GetOwnerNPC()->ClearJob();
}

void GGlueNPC::Say(const char* szText, float fMaintainTime /*= 0.0f*/)
{
	if (IsInvalidOwner())	return;	
	
	// 디폴트 파라메터의 경우, Invalid Value인 -1.0f로 변경
	if (fMaintainTime == 0.0f)
	{
		fMaintainTime = -1.0f;
	}

	return GetOwnerNPC()->Say(MLocale::ConvAnsiToUCS2(szText).c_str(), fMaintainTime);
}

void GGlueNPC::Ballon(const char* szText, float fMaintainTime /*= 0.0f*/)
{
	if (IsInvalidOwner())	return;	

	// 디폴트 파라메터의 경우, Invalid Value인 -1.0f로 변경
	if (fMaintainTime == 0.0f)
	{
		fMaintainTime = -1.0f;
	}

	return GetOwnerNPC()->Ballon(MLocale::ConvAnsiToUCS2(szText).c_str(), fMaintainTime);
}

void GGlueNPC::Shout(const char* szText, float fMaintainTime /*= 0.0f*/)
{
	if (IsInvalidOwner())	return;

	// 디폴트 파라메터의 경우, Invalid Value인 -1.0f로 변경
	if (fMaintainTime == 0.0f)
	{
		fMaintainTime = -1.0f;
	}

	return GetOwnerNPC()->Shout(MLocale::ConvAnsiToUCS2(szText).c_str(), fMaintainTime);
}

void GGlueNPC::SayAndNarration(const char* szText, float fMaintainTime /*= 0.0f*/)
{
	if (IsInvalidOwner())	return;	

	// 디폴트 파라메터의 경우, Invalid Value인 -1.0f로 변경
	if (fMaintainTime == 0.0f)
	{
		fMaintainTime = -1.0f;
	}

	return GetOwnerNPC()->SayAndNarration(MLocale::ConvAnsiToUCS2(szText).c_str(), fMaintainTime);
}

void GGlueNPC::SayAndNarrationNow(const char* szText)
{
	if (IsInvalidOwner())	return;	
	return GetOwnerNPC()->SayAndNarrationNow(MLocale::ConvAnsiToUCS2(szText).c_str());
}

void GGlueNPC::SayNow(const char* szText, float fMaintainTime /*= 0.0f*/)
{
	if (IsInvalidOwner())	return;	

	// 디폴트 파라메터의 경우, Invalid Value인 -1.0f로 변경
	if (fMaintainTime == 0.0f)
	{
		fMaintainTime = -1.0f;
	}

	return GetOwnerNPC()->doSay(MLocale::ConvAnsiToUCS2(szText).c_str(), fMaintainTime);
}

void GGlueNPC::BallonNow(const char* szText, float fMaintainTime /*= 0.0f*/)
{
	if (IsInvalidOwner())	return;	

	// 디폴트 파라메터의 경우, Invalid Value인 -1.0f로 변경
	if (fMaintainTime == 0.0f)
	{
		fMaintainTime = -1.0f;
	}

	return GetOwnerNPC()->doBalloon(MLocale::ConvAnsiToUCS2(szText).c_str(), fMaintainTime);
}

void GGlueNPC::NonDelaySay(const char* szText)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Say(MLocale::ConvAnsiToUCS2(szText).c_str(), 0.0f);
}

void GGlueNPC::NonDelayBallon(const char* szText)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Ballon(MLocale::ConvAnsiToUCS2(szText).c_str(), 0.0f);
}

void GGlueNPC::NonDelayShout(const char* szText)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Shout(MLocale::ConvAnsiToUCS2(szText).c_str(), 0.0f);
}

void GGlueNPC::NonDelaySayAndNarration(const char* szText)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->SayAndNarration(MLocale::ConvAnsiToUCS2(szText).c_str(), 0.0f);
}

void GGlueNPC::ScriptSelf(const char* szText)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->ScriptSelf(MLocale::ConvAnsiToUCS2(szText).c_str());
}

void GGlueNPC::Wait(float fSecond)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Wait(fSecond);
}

GGlueActor* GGlueNPC::Aggro(const char* pszType, float fDuration)
{
	if (IsInvalidOwner())	return GNullGlueActor::GetNull();
	
	GEntityActor* pActor = GetOwnerNPC()->Aggro(MLocale::ConvAnsiToUCS2(pszType).c_str(), fDuration);	
	return TO_SAFE_LUAPTR(pActor);
}

int GGlueNPC::GetCurrentTalent(void)
{
	if (IsInvalidOwner())	return -1;
	return GetOwnerNPC()->GetCurrentTalent();
}

void GGlueNPC::BreakPart(uint8 nPartID, GGlueActor* pAttackerDelegator)
{
	if (IsInvalidOwner())	return;
	
	if (pAttackerDelegator == NULL || pAttackerDelegator->IsNull())
	{
		return;
	}

	GEntityActor* pActor = pAttackerDelegator->GetOwnerActor();
	MUID uidOpponent = pActor->GetUID();
	
	if (GetOwnerNPC()->GetNPCBParts())
		GetOwnerNPC()->GetNPCBParts()->Break(nPartID, uidOpponent);
}

bool GGlueNPC::SetTimer( int nTimerID, float fIntervalTime, bool bRepeat)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerNPC()->SetTimer(nTimerID, fIntervalTime, bRepeat);
}

bool GGlueNPC::KillTimer( int nTimerID )
{
	if (IsInvalidOwner())	return false;
	return GetOwnerNPC()->KillTimer(nTimerID);
}

int GGlueNPC::GetMode(void)
{
	if (IsInvalidOwner())	return -1;
	return GetOwnerNPC()->GetMode();
}

void GGlueNPC::Despawn(bool bSpawnEnable)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Despawn(bSpawnEnable);
}

void GGlueNPC::DespawnNow(bool bSpawnEnable)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->DespawnNow(bSpawnEnable);
}

void GGlueNPC::Die(GGlueActor* pKiller)
{
	if (IsInvalidOwner())	return;
	if (!pKiller) return;
	if (!pKiller->GetOwnerActor()) return;

	// GJobT_Die::OnStart() 코드 옮겨옴
	MUID uidKiller = pKiller->GetOwnerActor()->GetUID();
	if (uidKiller.IsValid())
	{
		GEntityPlayer* pRewarder = GetOwnerNPC()->FindPlayer(uidKiller);
		if (pRewarder)
		{
			GetOwnerNPC()->GetHateTable().AddPoint_FoundEnemy(pRewarder);
		}
	}

	return GetOwnerNPC()->doDie();
}

void GGlueNPC::Assist(GGlueNPC* pAlliedNPCDelegator)
{
	if (IsInvalidOwner())	return;

	if (pAlliedNPCDelegator == NULL || pAlliedNPCDelegator->IsNull())
	{
		return;
	}

	GEntityNPC* pNpc = pAlliedNPCDelegator->GetOwnerNPC();
	return GetOwnerNPC()->Assist(pNpc);
}

void GGlueNPC::Yell(int nRange)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Yell(nRange);
}

int GGlueNPC::GetSightRange(GGlueActor* pEnemy)
{
	if (IsInvalidOwner())	return -1;

	if (pEnemy == NULL || pEnemy->IsNull())
	{
		return -1;
	}

	return GetOwnerNPC()->GetSightRange(pEnemy->GetOwnerActor()->GetLevel());
}

void GGlueNPC::ReturnToHome(void)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->ReturnToHomePoint();
}

bool GGlueNPC::IsCheckBPart(int nPartID)
{
	if (IsInvalidOwner())	return false;
	bool ret = false;
	
	if (GetOwnerNPC()->GetNPCBParts())
		ret = GetOwnerNPC()->GetNPCBParts()->IsBroken(nPartID);

	return ret;
}

bool GGlueNPC::IsAlwaysAttackable(void)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerNPC()->IsAlwaysAttackable();
}

bool GGlueNPC::IsNowCombat(void)
{
	if (IsInvalidOwner())	return false;
	return GetOwnerNPC()->IsNowCombat();
}

vec3 GGlueNPC::GetSpawnPos(void)
{
	if (IsInvalidOwner())	return vec3::ZERO;
	return GetOwnerNPC()->GetSpawnPos();
}

void GGlueNPC::FleeFromEnemy(float fDurationTime)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Flee(Flee_FromEnemy, fDurationTime);
}

void GGlueNPC::FleeToHome(float fDurationTime)
{
	if (IsInvalidOwner())	return;
	return GetOwnerNPC()->Flee(Flee_ToHome, fDurationTime);
}

GDamageInfo& GGlueNPC::GetDamagedInfo(void)
{
	if (IsInvalidOwner())	
	{
		static GDamageInfo null_info;
		return null_info;
	}
	return GetOwnerNPC()->GetLastDamagedInfo();
}

int GGlueNPC::GetID()
{
	if (IsInvalidOwner())	return -1;
	return GetOwnerNPC()->GetID();
}

const string GGlueNPC::GetLuaTableName()
{
	if (IsInvalidOwner())	return "NullLuaTable";
	if (!GetOwnerNPC()->GetNPCInfo())	return "NullLuaTable";

	return MLocale::ConvUTF16ToAnsi(GetOwnerNPC()->GetNPCInfo()->strNameInLua.c_str());	
}

void GGlueNPC::RunSpawnScript(const SPAWN_INFO* pSpawnInfo)
{
	if (IsInvalidOwner())	return;
	int nSpawnID = (NULL == pSpawnInfo) ? 0 : pSpawnInfo->nID;
	GetScriptNPC()->OnSpawn(this, GetField(), nSpawnID);
	GetScriptField()->OnSpawn(GetField(), this, nSpawnID);
}

void GGlueNPC::RunDieScript(const SPAWN_INFO* pSpawnInfo)
{
	if (IsInvalidOwner())	return;
	int nSpawnID = (NULL == pSpawnInfo) ? 0 : pSpawnInfo->nID;
	
	GetScriptNPC()->OnDie(this, GetField(), nSpawnID);
	GetScriptField()->OnDie(GetField(), this, nSpawnID);
}


void GGlueNPC::SetDecayTimeSec( float fTime )
{
	if (IsInvalidOwner())	return;
	GEntityNPC* pNPC = reinterpret_cast<GEntityNPC*>(m_pOwner);
	pNPC->GetNPCDecay().SetCustomDecayTick(fTime);
}

void GGlueNPC::FaceTo(GGlueActor* pTargetActorDelegator)
{
	if (IsInvalidOwner())	return;
	if (pTargetActorDelegator == NULL || pTargetActorDelegator->IsNull())	return;
	GetOwnerNPC()->FaceTo(pTargetActorDelegator->GetOwnerActor());
}

void GGlueNPC::SetAlwaysRun(bool bRun)
{
	if (IsInvalidOwner())	return;
	GetOwnerNPC()->GetNPCMove().SetAlwaysRun(bRun);
}

void GGlueNPC::EnableInteraction()
{
	if (IsInvalidOwner())	return;
	GetOwnerNPC()->GetNPCInteraction().Enable();
}

void GGlueNPC::DisableInteraction()
{
	if (IsInvalidOwner())	return;
	GetOwnerNPC()->GetNPCInteraction().Disable();
}

GGluePlayer* GGlueNPC::GetLastInteractPlayer()
{
	if (IsInvalidOwner())	return GNullGluePlayer::GetNull();

	GEntityPlayer* pLastInteractPlayer = GetOwnerNPC()->GetNPCInteraction().GetLastInteractPlayer();

	return TO_SAFE_LUAPTR(pLastInteractPlayer);
}

bool GGlueNPC::IsCooldown(int nTalentID)
{
	if (IsInvalidOwner())	return false;

	GActorCooltimeChecker& checker = GetOwnerNPC()->GetActorCooltimeChecker();
	return checker.IsCooldown(nTalentID);
}
bool GGlueNPC::IsSwallowable()
{
	if (IsInvalidOwner())	return false;

	return GetOwnerNPC()->GetNPCSwallow().IsSwallowable();
}

bool GGlueNPC::Spewup()
{
	if (IsInvalidOwner())	return false;

	return GetOwnerNPC()->GetNPCSwallow().Spewup(true);
}

void GGlueNPC::SetTauntable(bool bEnable)
{
	if (IsInvalidOwner())	return;

	GetOwnerNPC()->SetTauntable(bEnable);
}

bool GGlueNPC::EnableSensorCheck(unsigned long nSensorID)
{
	if (IsInvalidOwner())	return false;

	GField* pField = GetOwnerNPC()->GetField();
	if (NULL == pField) return false;

	const SENSOR_INFO* pSensorInfo = pField->GetSensor(nSensorID);
	if (NULL == pSensorInfo) return false;

	if (GetOwnerNPC()->GetNPCSensorChecker())
		GetOwnerNPC()->GetNPCSensorChecker()->Enable(pSensorInfo);

	return true;
}

void GGlueNPC::WaitFor(GGlueNPC* pTarget)
{
	if (IsInvalidOwner())	return;
	if (pTarget == NULL || pTarget->IsNull())	return;

	GetOwnerNPC()->GetJobRunner()->WaitFor(GetOwnerNPC()->GetModuleAI(), pTarget->GetOwnerNPC());
}

void GGlueNPC::NextAction()
{
	if (IsInvalidOwner())	return;

	GetOwnerNPC()->GetJobRunner()->NextAction(GetOwnerNPC()->GetModuleAI());
}

void GGlueNPC::SetTarget(GGlueNPC* pTarget)
{
	if (IsInvalidOwner())	return;
	if (pTarget == NULL || pTarget->IsNull())	return;

	GetOwnerNPC()->GetNPCTarget().SetTargetUID(pTarget->GetUID());
}

GGlueActor* GGlueNPC::GetTarget()
{
	if (IsInvalidOwner())	return GNullGlueActor::GetNull();

	GEntityActor* pTarget = GetOwnerNPC()->GetNPCTarget().GetTarget();
	return TO_SAFE_LUAPTR(pTarget);
}

void GGlueNPC::Summon(int nNPCID, vec3 vStartPos)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vStartPos);

	if (IsInvalidOwner())	return;
	GetOwnerNPC()->Summon(nNPCID, vStartPos);
}

GGlueNPC* GGlueNPC::SummonNow(int nNPCID, vec3 vStartPos)
{
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vStartPos);

	if (IsInvalidOwner())	return GNullGlueNPC::GetNull();

	GEntityNPC* pSpawnNPC = GetOwnerNPC()->GetNPCMinion().Summon(nNPCID, vStartPos);
	return TO_SAFE_LUAPTR(pSpawnNPC);
}

GGlueNPC* GGlueNPC::GetSummoner()
{
	if (IsInvalidOwner())	return GNullGlueNPC::GetNull();

	GEntityNPC* pSpawnNPC = GetOwnerNPC()->GetNPCMinion().GetSummoner();
	return TO_SAFE_LUAPTR(pSpawnNPC);
}

int GGlueNPC::GetSummonCount()
{
	if (IsInvalidOwner())	return 0;
	return GetOwnerNPC()->GetNPCMinion().GetMinionCount();
}

void GGlueNPC::StayawayFrom(GGlueActor* pTargetActor, float fDistance)
{
	if (IsInvalidOwner())	return;
	if (pTargetActor == NULL || pTargetActor->IsNull())	return;
	GetOwnerNPC()->MoveToPos(pTargetActor->GetOwnerActor(), fDistance);
}

void GGlueNPC::ResetNPC()
{
	if (IsInvalidOwner())	return;

	GetOwnerNPC()->Reset();
}

void GGlueNPC::ChangeAA( int nType )
{
	if (IsInvalidOwner())	return;
	VALID (nType >= 0);
	VALID (nType < NAAT_MAX);

	GetOwnerNPC()->GetNPCStatus().nAA = (NPC_AA_TYPE)nType;
}

void GGlueNPC::SaveHomePoint() 
{
	if (IsInvalidOwner())	return;

	GetOwnerNPC()->SetHomePoint(GetOwnerNPC()->GetPos(), GetOwnerNPC()->GetDir());
}

void GGlueNPC::AttachObserveDuel( GGluePlayer* pObserver )
{
	if (IsInvalidOwner())	return;
	if (pObserver == NULL || pObserver->IsNull())	return;

	MUID uidDuel;
	uidDuel.High = GetOwnerNPC()->GetUserdata(1);
	uidDuel.Low = GetOwnerNPC()->GetUserdata(2);

	pObserver->GetOwnerPlayer()->GetEnemyInfoSender().AttachObserveDuel(uidDuel);
}

void GGlueNPC::DetachObserveDuel( GGluePlayer* pObserver )
{
	if (IsInvalidOwner())	return;
	if (pObserver == NULL || pObserver->IsNull())	return;

	pObserver->GetOwnerPlayer()->GetEnemyInfoSender().DetachObserveDuel();
}

