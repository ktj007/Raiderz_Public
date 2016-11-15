#include "StdAfx.h"

#include "GScriptGroup_NPC.h"
#include "GDef.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GGlueNPC.h"
#include "GGluePlayer.h"
#include "GLuaThisEnchanter.h"
#include "GConst.h"
#include "GNullGlueNPC.h"
#include "GNullGlueActor.h"
#include "GNPCScriptBuilder.h"
#include "GNullGluePlayer.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GAIMonitorPort.h"

#define SEND_CALLBACK_MESSAGE_TO_VIEWER(...) \
	if (GConst::AIMON_ENABLE && GAIMonitorPort::Inst().IsSetedNPC(pNPC->GetUID())) \
	GAIMonitorPort::Inst().SendCallbackMessageToViewer(__VA_ARGS__)

GScriptGroup_NPC::GScriptGroup_NPC(void)
{
}

GScriptGroup_NPC::~GScriptGroup_NPC(void)
{
}

void GScriptGroup_NPC::RegisterGlues()
{
	MWLua::class_<GGlueNPC>(WLUA->GetState(), "EntityNPC")
		.inh<GGlueActor>()
		.def(MWLua::constructor<GEntityNPC*>())	
		.def("GetID",					&GGlueNPC::GetID)
		.def("GetSpawnPos",				&GGlueNPC::GetSpawnPos)
		.def("Despawn",					&GGlueNPC::Despawn)
		.def("DespawnNow",				&GGlueNPC::DespawnNow)
		.def("Die",						&GGlueNPC::Die)
		.def("Patrol",					&GGlueNPC::Patrol)
		.def("PatrolInstant",			&GGlueNPC::PatrolInstant)
		.def("Roam",					&GGlueNPC::Roam)
		.def("Move",					&GGlueNPC::Move)
		.def("MoveToMarker",			&GGlueNPC::MoveToMarker)
		.def("MoveToSensor",			&GGlueNPC::MoveToSensor)
		.def("Follow",					&GGlueNPC::Follow)
		.def("ClearJob",				&GGlueNPC::ClearJob)
		.def("ReturnToHome",			&GGlueNPC::ReturnToHome)
		.def("MoveToActor",				&GGlueNPC::MoveToActor)
		.def("MoveToActorWithDuration",	&GGlueNPC::MoveToActorWithDuration)		
		.def("UseTalent",				&GGlueNPC::UseTalent)
		.def("UseTalentSelf",			&GGlueNPC::UseTalentSelf)
		.def("CheckBPart",				&GGlueNPC::IsCheckBPart)
		.def("GetDamagedInfo",			&GGlueNPC::GetDamagedInfo)
		.def("Say",						&GGlueNPC::Say)
		.def("Balloon",					&GGlueNPC::Ballon)
		.def("Shout",					&GGlueNPC::Shout)		
		.def("SayAndNarration",			&GGlueNPC::SayAndNarration)
		.def("SayAndNarrationNow",		&GGlueNPC::SayAndNarrationNow)
		.def("NonDelayShout",			&GGlueNPC::NonDelayShout)		
		.def("NonDelaySay",				&GGlueNPC::NonDelaySay)
		.def("NonDelayBalloon",			&GGlueNPC::NonDelayBallon)
		.def("NonDelaySayAndNarration",	&GGlueNPC::NonDelaySayAndNarration)
		.def("Assist",					&GGlueNPC::Assist)
		.def("Attack",					&GGlueNPC::Attack)
		.def("Yell",					&GGlueNPC::Yell)
		.def("ReserveUserData",			&GGlueNPC::ReserveUserData)
		.def("SetUserData",				&GGlueNPC::SetUserData)
		.def("GetUserData",				&GGlueNPC::GetUserData)
		.def("IsNowCombat",				&GGlueNPC::IsNowCombat)
		.def("IsEnableCombat",			&GGlueNPC::IsAlwaysAttackable)
		.def("EnableCombat",			&GGlueNPC::EnableCombat)
		.def("EnableCombatWithFaction",	&GGlueNPC::EnableCombatWithFaction)
		.def("DisableCombat",			&GGlueNPC::DisableCombat)
		.def("ScriptSelf",				&GGlueNPC::ScriptSelf)
		.def("Wait",					&GGlueNPC::Wait)
		.def("Aggro",					&GGlueNPC::Aggro)
		.def("GetCurrentTalent",		&GGlueNPC::GetCurrentTalent)
		.def("BreakPart",				&GGlueNPC::BreakPart)
		.def("SetTimer",				&GGlueNPC::SetTimer)
		.def("KillTimer",				&GGlueNPC::KillTimer)
		.def("GetMode",					&GGlueNPC::GetMode)
		.def("GetSightRange",			&GGlueNPC::GetSightRange)
		.def("EnableInteract",			&GGlueNPC::EnableInteract)		
		.def("FleeFromEnemy",			&GGlueNPC::FleeFromEnemy)
		.def("FleeToHome",				&GGlueNPC::FleeToHome)
		.def("SetDecayTime",			&GGlueNPC::SetDecayTimeSec)
		.def("FaceTo",					&GGlueNPC::FaceTo)
		.def("SetAlwaysRun",			&GGlueNPC::SetAlwaysRun)
		.def("EnableInteraction",		&GGlueNPC::EnableInteraction)
		.def("DisableInteraction",		&GGlueNPC::DisableInteraction)
		.def("GetLastInteractPlayer",	&GGlueNPC::GetLastInteractPlayer)
		.def("IsSwallowable",			&GGlueNPC::IsSwallowable)
		.def("Spewup",					&GGlueNPC::Spewup)
		.def("SetTauntable",			&GGlueNPC::SetTauntable)
		.def("EnableSensorCheck",		&GGlueNPC::EnableSensorCheck)
		.def("WaitFor",					&GGlueNPC::WaitFor)
		.def("NextAction",				&GGlueNPC::NextAction)
		.def("GetTarget",				&GGlueNPC::GetTarget)
		.def("SetTarget",				&GGlueNPC::SetTarget)
		.def("Summon",					&GGlueNPC::Summon)
		.def("SummonNow",				&GGlueNPC::SummonNow)
		.def("GetSummoner",				&GGlueNPC::GetSummoner)
		.def("GetSummonCount",			&GGlueNPC::GetSummonCount)
		.def("StayawayFrom",			&GGlueNPC::StayawayFrom)
		.def("IsCooldown",				&GGlueNPC::IsCooldown)
		.def("ResetNPC",				&GGlueNPC::ResetNPC)
		.def("ChangeAA",				&GGlueNPC::ChangeAA)
		.def("SaveHomePoint",			&GGlueNPC::SaveHomePoint)
		.def("AttachObserveDuel",		&GGlueNPC::AttachObserveDuel)
		.def("DetachObserveDuel",		&GGlueNPC::DetachObserveDuel)
		.def("SayNow",					&GGlueNPC::SayNow)
		.def("BalloonNow",				&GGlueNPC::BallonNow)
		
		
		;
}

bool GScriptGroup_NPC::PreCheck()
{
	return GetScriptManager()->IsRunning();
}

bool GScriptGroup_NPC::Reload( GNPCInfo* pInfo )
{
	return RUN_LUA(MLocale::ConvUTF16ToAnsi(PATH_SCRIPT_NPC_LUA).c_str(), MLocale::ConvUTF16ToAnsi(pInfo->strNameInLua.c_str()).c_str());
}

bool GScriptGroup_NPC::OnInit(GNPCInfo* pNewInfo)
{
	if (!PreCheck())	return false;
	VALID_RET(pNewInfo, false);
	VALID_RET(pNewInfo->nID != INVALID_ID, false);

	string strTableName = MLocale::ConvUTF16ToAnsi(pNewInfo->strNameInLua.c_str());
	string strFuncName = LUA_CALLBACK_FUNCNAME_NPC_INIT;

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	MLog1(MLOG_FILE, "NPCScriptLoad (id: %d)\n", pNewInfo->nID);

	if (!WMEMCALL1(WLUA, strTableName.c_str(), strFuncName.c_str(), 
			pNewInfo->nID, WNULL))
	{
		mlog3("%s:%s() Failed. ID:%d(%s)\n", 
			strTableName.c_str(), strFuncName.c_str(), pNewInfo->nID, pNewInfo->strName.c_str());
		return false;
	}

	return true;
}

bool GScriptGroup_NPC::OnSpawn( GGlueNPC* pNPC, GGlueField* pField, int nSpawnID )
{
	
	if (!PreCheck())	return false;

	MWLua::table t(WLUA->GetState());
	m_ScriptUtil.MakeSpawnTable(&t, nSpawnID, pNPC, pField);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnSpawn_%d", nSpawnID);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_SPAWN, t); 
}

bool GScriptGroup_NPC::OnDie( GGlueNPC* pNPC, GGlueField* pField, int nSpawnID )
{
	
	if (!PreCheck())	return false;

	MWLua::table t(WLUA->GetState());
	m_ScriptUtil.MakeSpawnTable(&t, nSpawnID, pNPC, pField);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnDie (SpawnID: %d)", nSpawnID);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_DESPAWN, t); 
}

bool GScriptGroup_NPC::OnYell( GGlueNPC* pNPC, GGlueNPC* pRequester, GGlueActor* pTarget)
{
	
	if (!PreCheck())	return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnYell");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_YELL, pRequester, pTarget); 
}


bool GScriptGroup_NPC::OnInteract( GGlueNPC* pNPC, GGluePlayer* pInteractor)
{
	
	if (!PreCheck())	return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnInteract");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_INTERACT, pInteractor); 
}

bool GScriptGroup_NPC::OnDialogExit( GGlueNPC* pNPC, GGluePlayer* pPlayer, int nDialogID, int nExit)
{
	
	if (!PreCheck())	return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnDialogExit (DialogID = %d, Exit = %d)", nDialogID, nExit);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_DIALOGEXIT, pPlayer, nDialogID, nExit); 
}

bool GScriptGroup_NPC::OnAggro( GGlueNPC* pNPC, GGlueActor* pTarget)
{
	
	if (!PreCheck())	return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnAggro");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_AGGRO, pTarget); 
}

bool GScriptGroup_NPC::OnTimer( GGlueNPC* pNPC, int nTimerID)
{
	
	if (!PreCheck())	return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnTimer_%d", nTimerID);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_TIMER, nTimerID); 
}

bool GScriptGroup_NPC::OnUserdataChanged( GGlueNPC* pNPC, int nIndex, int nValue)
{
	
	if (!PreCheck())	return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnUserdataChanged_%d (value=%d)", nIndex, nValue);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_USERDATACHANGED, nIndex, nValue); 
}

bool GScriptGroup_NPC::OnHitCapsule( GGlueNPC* pNPC, GHitInfo& infoHit)
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	// 널포인트 점검
	infoHit.pAttackerForLua = TO_SAFE_LUAPTR(infoHit.pAttacker);
	infoHit.pVictimForLua = TO_SAFE_LUAPTR(dynamic_cast<GEntityNPC*>(infoHit.pVictim));

	string strTableName = pNPC->GetLuaTableName();
	string strFuncName = 
		MAKE_LUAFUNC2(LUA_CALLBACK_FUNCNAME_NPC_HITCAPSULE, infoHit.nCapsuleGroup, infoHit.nCapsuleIndex);

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnHitCapsule_%d_%d", infoHit.nCapsuleGroup, infoHit.nCapsuleIndex);

	return WMEMCALL2(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pNPC, infoHit, &infoHit);
}

bool GScriptGroup_NPC::OnSwalloweBegin( GGlueNPC* pNPC, GGlueActor* pTarget)
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);
	VALID_RET(pTarget, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnSwalloweBegin");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_SWALLOW_BEGIN, pTarget); 
}

bool GScriptGroup_NPC::OnHitSwallowed( GGlueNPC* pNPC, GHitInfo& infoHit)
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	// 널포인트 점검
	infoHit.pAttackerForLua = TO_SAFE_LUAPTR(infoHit.pAttacker);
	infoHit.pVictimForLua = TO_SAFE_LUAPTR(dynamic_cast<GEntityNPC*>(infoHit.pVictim));

	string strTableName = pNPC->GetLuaTableName();
	string strFuncName = 
		MAKE_LUAFUNC0(LUA_CALLBACK_FUNCNAME_NPC_HITSWALLOWED);

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnHitSwallowed");
	return WMEMCALL2(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pNPC, infoHit, &infoHit);
}

bool GScriptGroup_NPC::OnTryHit( GGlueNPC* pNPC, GGlueActor* pActor, const int nTalentID )
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);
	VALID_RET(pActor, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnTryHit(TalentID = %d)", nTalentID);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_TRYHIT, pActor, nTalentID); 
}

bool GScriptGroup_NPC::OnChangeMode(GGlueNPC* pNPC, NPC_MODE nBeforeMode, NPC_MODE nMode)
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnChangeMode_%d", nMode);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_CHANGEMODE, nBeforeMode, nMode); 
}

bool GScriptGroup_NPC::OnTalentEnd(GGlueNPC* pNPC, int nTalentID)
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnTalentEnd(TalentID = %d)", nTalentID);
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_TALENTEND, nTalentID);
}

bool GScriptGroup_NPC::OnVictory(GGlueNPC* pNPC)
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnVictory");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_VICTORY);
}

bool GScriptGroup_NPC::OnCombatBegin( GGlueNPC* pNPC )
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnCombatBegin");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_COMBAT_BEGIN);
}

bool GScriptGroup_NPC::OnCombatEnd( GGlueNPC* pNPC )
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnCombatEnd");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_COMBAT_END);
}

bool GScriptGroup_NPC::OnBPartRecver( GGlueNPC* pNPC )
{
	
	if (!PreCheck())	return false;
	VALID_RET(pNPC, false);

	SEND_CALLBACK_MESSAGE_TO_VIEWER("OnBPartRecover");
	return LuaCallback(pNPC, LUA_CALLBACK_FUNCNAME_NPC_BPART_RECOVER);
}