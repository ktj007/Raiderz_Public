#include "StdAfx.h"
#include "GScriptManager.h"
#include "GScriptGroup_Field.h"
#include "GScriptGroup_Quest.h"
#include "GScriptGroup_NPC.h"
#include "GScriptGroup_QuestPVP.h"
#include "GField.h"
#include "GEntity.h"
#include "GEntitySync.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GLuaGlues.h"
#include "GLuaResult.h"
#include "GConst.h"
#include "GMath.h"
#include "GGlobal.h"
#include "GGlueActor.h"
#include "GGluePlayer.h"
#include "GNullGlueActor.h"
#include "GNullGlueNPC.h"
#include "GNullGluePlayer.h"
#include "GNullGlueField.h"
#include "GNullGlueQuest.h"
#include "GNullGlueQuestPVP.h"
#include "GSystem.h"
#include "CSLuaCallbackLogger.h"
#include "GFieldNPCSession.h"


static void MaietWLuaLog(const char* text)
{
	if (!GConst::RUN_FOR_TEST)
		return;

	if (GConst::SHOW_LUA_ERROR_LOG)
	{
		mlog3(text);
	}
	else
	{
		MGetDefaultLogger()->Output(MLOG_FILE, text, MLOG_LEVEL_DEBUG);
	}
}

static void WLuaPreCallback()
{
	PFI_B(7800, "WLua");
}

static void WLuaPostCallback()
{
	PFI_E(7800);
}

GScriptGroup_Field* GetScriptField()		
{ 
	return gsys.pScriptManager->GetScriptField(); 
}

GScriptGroup_Quest* GetScriptQuest()
{
	return gsys.pScriptManager->GetScriptQuest(); 
}

GScriptGroup_NPC* GetScriptNPC()
{
	return gsys.pScriptManager->GetScriptNPC(); 
}

GScriptGroup_QuestPVP* GetScriptQuestPVP()
{
	return gsys.pScriptManager->GetScriptQuestPVP(); 
}





GScriptManager::GScriptManager() 
: m_bRunning(false)
, m_pHandlerField(NULL)
, m_pHandlerQuest(NULL)
, m_pHandlerNPC(NULL)
, m_pHandlerQuestPVP(NULL)
{

}

GScriptManager::~GScriptManager()
{
	_ASSERT(!IsRunning() && L"GScriptManager::Fini()을 명시적으로 호출해야합니다.");
}

bool GScriptManager::Init(bool bUseCallbackLogger)
{
	if (m_bRunning)
	{
		Fini();
	}

	m_Lua.Open();
	m_Lua.SetLogFunc(MaietWLuaLog);
	m_Lua.SetCallbackFunc(WLuaPreCallback, WLuaPostCallback);
	InitHandler();

	if (true == bUseCallbackLogger)
	{
		m_Lua.SetExceptionFunc(CSLuaCallbackLogger::Exception);
	}

	if (!RegisterGlues())
		return false;

	WRUNFILE(&m_Lua, MLocale::ConvUTF16ToAnsi(FILENAME_SCRIPT_ENTRY).c_str());

	m_bRunning = true;

	InitNullObject();


	return true;
}

void GScriptManager::Fini()
{
	if (!m_bRunning)
		return;

	m_Lua.Close();
	FinitHandler();

	FiniNullObject();


	m_bRunning = false;
}

bool GScriptManager::RegisterGlues()
{
	RegisterGlue_Const();
	RegisterGlue_Foundation();
	RegisterGlue_Customdata();
	RegisterGlue_Debug();
	RegisterGlue_EntityActor();
	RegisterGlue_EntityPlayer();
	RegisterGlue_NullObject();
	RegisterGlue_Util();	
	RegisterGlue_NPCSession();
	
	// 테스트 코드로 보호된 함수
	//{{
	

	m_Lua.Def("GetMarker", &GLuaGlues::GetMarker);	
	
	
	//}}
		
	m_Lua.Def("GetHP", &GLuaGlues::GetHP);
	m_Lua.Def("GetEN", &GLuaGlues::GetEN);
	m_Lua.Def("GetSTA", &GLuaGlues::GetSTA);

	m_Lua.Def("IsIdle", &GLuaGlues::IsIdle);
	m_Lua.Def("GetPos", &GLuaGlues::GetPos);
	m_Lua.Def("DoMove", &GLuaGlues::DoMove);
		
	m_Lua.Def("GetEnemyHP", &GLuaGlues::GetEnemyHP);
	

	m_Lua.Def("OutDebugUID", &GLuaGlues::OutDebugUID);
	m_Lua.Def("OutDebugStr", &GLuaGlues::OutDebugStr);
	m_Lua.Def("OutDebugInt", &GLuaGlues::OutDebugInt);
	
	m_Lua.Def("Combat_DoAttack", &GLuaGlues::DoAttack);
	m_Lua.Def("SetPosition", &GLuaGlues::SetPosition);


	// 핸들러 글루함수 등록 --------------------------------
	if (!RegisterGlue_Handlers())
		return false;

	return true;
}

bool GScriptManager::RegisterGlue_Handlers()
{
	VALID_RET(m_pHandlerField, false);
	VALID_RET(m_pHandlerQuest, false);
	VALID_RET(m_pHandlerNPC, false);
	VALID_RET(m_pHandlerQuestPVP, false);
	m_pHandlerField->RegisterGlues();
	m_pHandlerQuest->RegisterGlues();
	m_pHandlerNPC->RegisterGlues();
	m_pHandlerQuestPVP->RegisterGlues();

	return true;
}


void GScriptManager::RegisterGlue_Const()
{
	// MF_STATE
	for (int i=0; i<MF_SIZE; i++)
		m_Lua.SetVar(MLocale::ConvUTF16ToAnsi(MF_STATE_NAME[i]).c_str(), i);

	// GAME_TIME_TYPE
	for (int i=0; i<MAX_TIME_TYPE; i++)
		m_Lua.SetVar(MLocale::ConvUTF16ToAnsi(GAME_TIME_TYPE_NAME[i]).c_str(), i);

	// GAME_WEATHER_TYPE
	for (int i=0; i<MAX_WEATHER_TYPE; i++)
		m_Lua.SetVar(MLocale::ConvUTF16ToAnsi(GAME_WEATHER_TYPE_NAME[i]).c_str(), i);

	// DAMAGE_ATTRIB
	for (int i=0; i<DA_MAX; i++)
		m_Lua.SetVar(MLocale::ConvUTF16ToAnsi(DAMAGE_ATTRIB_NAME[i]).c_str(), i);

	// AA 타입
	for (int i=0; i<NAAT_MAX; i++)
		m_Lua.SetVar(MLocale::ConvUTF16ToAnsi(NPC_AA_TYPE_STRING[i]).c_str(), i);
}

void GScriptManager::RegisterGlue_Foundation()
{
	MWLua::class_<MUID>(m_Lua.GetState(), "UID")
		.def(MWLua::constructor<uint64>())
		.def(MWLua::constructor<unsigned long int, unsigned long int>())
		.def_readwrite("Value", &MUID::Value) 
		.def_readwrite("High", &MUID::High) 
		.def_readwrite("Low", &MUID::Low)
		;

	MWLua::class_<string>(m_Lua.GetState(), "string")
		.def(MWLua::constructor<>())
		;

	MWLua::class_<vec3>(m_Lua.GetState(), "vec3")
		.def(MWLua::constructor<>())
		.def(MWLua::constructor<float, float, float>())
		.def_readwrite("x", &vec3::x)
		.def_readwrite("y", &vec3::y)
		.def_readwrite("z", &vec3::z)
		;
}

void GScriptManager::RegisterGlue_Customdata()
{
	MWLua::class_<MARKER_INFO>(m_Lua.GetState(), "Marker")
		.def(MWLua::constructor<>())
		.def(MWLua::constructor<vec3>())
		.def_readwrite("ID",			&MARKER_INFO::nID)
		.def_readwrite("Name",		&MARKER_INFO::strName)
		.def_readwrite("Dir",			&MARKER_INFO::vDir)
		.def_readwrite("Point",		&MARKER_INFO::vPoint)
		.def_readwrite("RandomDir",	&MARKER_INFO::bRandomDir)
		.def("IsValid", &MARKER_INFO::IsValid)
		;

	MWLua::class_<GDamageInfo>(m_Lua.GetState(), "GDamageInfo")
		.def(MWLua::constructor<>())
		.def_readwrite("Type",		&GDamageInfo::nType)
		.def_readwrite("Amount",	&GDamageInfo::nAmount)
		;

	MWLua::class_<MOTION_FACTOR_VAR>(m_Lua.GetState(), "MOTION_FACTOR_VAR")
		.def(MWLua::constructor<>())
		.def_readwrite("Value",		&MOTION_FACTOR_VAR::nValue)
		.def_readwrite("Weight",		&MOTION_FACTOR_VAR::nWeight)
		;

	MWLua::class_<MOTION_FACTOR_GROUP>(m_Lua.GetState(), "MOTION_FACTOR_GROUP")
		.def(MWLua::constructor<>())
		.def("Adjust",		&MOTION_FACTOR_GROUP::Adjust)
		.def("Get",			&MOTION_FACTOR_GROUP::Get)
		;

	MWLua::class_<GHitInfo>(m_Lua.GetState(), "GHitInfo")
		.def(MWLua::constructor<>())
		.def_readwrite("Type",		&GHitInfo::nDamageType)
		.def_readwrite("Damage",	&GHitInfo::nDamage)
		.def_readwrite("MF",		&GHitInfo::pMotionFactorGroup)
		.def_readwrite("Attacker",	&GHitInfo::pAttackerForLua)
		.def_readwrite("Victim",	&GHitInfo::pVictimForLua)		
		.def_readwrite("TalentID",	&GHitInfo::nTalentID)		
		;
}

void GScriptManager::RegisterGlue_Debug()
{
	m_Lua.Def("GLog", &GLuaGlues::GLog);
	m_Lua.Def("GLog0", &GLuaGlues::GLog0);
	m_Lua.Def("GLog1", &GLuaGlues::GLog1);
	m_Lua.Def("GLog2", &GLuaGlues::GLog2);
	m_Lua.Def("GLog3", &GLuaGlues::GLog3);
	m_Lua.Def("GLogFilter", &GLuaGlues::GLogFilter);
	m_Lua.Def("Debug_AddLogFilter", &MAddLogFilter);
}

void GScriptManager::RegisterGlue_Util()
{
	m_Lua.Def("CastString",				&GLuaGlues::CastString);

	m_Lua.Def("Math_LocalToWorld",		&GLuaGlues::LocalPosToWorldPos);
	m_Lua.Def("Math_GetDistance",		&GLuaGlues::GetDistance);
	m_Lua.Def("Math_GetPolygonPoints",	&GLuaGlues::GetPolygonPoints);
	m_Lua.Def("Math_GetDistancePoint",	&GLuaGlues::GetDistancePoint);
	m_Lua.Def("NPC_ReserveUserData",	&GLuaGlues::ReserveUserData);
	m_Lua.Def("AsNPC",					&GLuaGlues::AsNPC);
	m_Lua.Def("AsPlayer",				&GLuaGlues::AsPlayer);	

	m_Lua.Def("Format",					&GLuaGlues::Format);
}

void GScriptManager::RegisterGlue_EntityActor()
{
	MWLua::class_<GGlueActor>(m_Lua.GetState(), "EntityActor")
		.def(MWLua::constructor<GEntityActor*>())
		// 테스트 코드로 보호된 함수
		//{{		
		.def("GetDir",					&GGlueActor::GetDir)
		.def("GetFowardDir",			&GGlueActor::GetFowardDir)
		.def("GetPos",					&GGlueActor::GetPos)
		.def("DistanceTo",				&GGlueActor::DistanceTo)
		.def("Warp",					&GGlueActor::Warp)
		.def("GetMotionFactor",			&GGlueActor::GetCurrentMotionFactor)
		.def("GetFieldID",				&GGlueActor::GetFieldID)
		.def("IsPlayer",				&GGlueActor::IsPlayer)
		.def("IsNPC",					&GGlueActor::IsNPC)
		.def("SetMFModValue",			&GGlueActor::SetMFModValue)
		.def("SetMFModWeight",			&GGlueActor::SetMFModWeight)
		.def("CheckBuff",				&GGlueActor::CheckBuff)
		.def("GetDebuffCount",			&GGlueActor::GetDebuffCount)
		.def("GainBuff",				&GGlueActor::GainBuff)
		.def("GainBuffDetail",			&GGlueActor::GainBuffDetail)
		.def("RemoveBuff",				&GGlueActor::RemoveBuff)
		.def("RemoveAllBuff",			&GGlueActor::RemoveAllBuff)
		.def("GetField",				&GGlueActor::GetField)		
		.def("GetHP",					&GGlueActor::GetHP)
		.def("GetEN",					&GGlueActor::GetEN)
		.def("GetSTA",					&GGlueActor::GetSTA)
		.def("GetMaxHP",				&GGlueActor::GetMaxHP)
		.def("GetMaxEN",				&GGlueActor::GetMaxEN)
		.def("GetMaxSTA",				&GGlueActor::GetMaxSTA)
		.def("Narration",				&GGlueActor::Narration)
		.def("NarrationNow",			&GGlueActor::NarrationNow)
		.def("GetName",					&GGlueActor::GetName)
		.def("IsRear",					&GGlueActor::IsRear)
		.def("IsFront",					&GGlueActor::IsFront)
		.def("IsDead",					&GGlueActor::IsDead)
		.def("MakePeace",				&GGlueActor::MakePeace)
		.def("GetLevel",				&GGlueActor::GetLevel)
		.def("RunFieldEvent",			&GGlueActor::RunFieldEvent)
		
		//}}

		.def("IsNull",					&GGlueActor::IsNull)
		;
}

void GScriptManager::RegisterGlue_EntityPlayer()
{
	MWLua::class_<GGluePlayer>(m_Lua.GetState(), "EntityPlayer")
		.inh<GGlueActor>()
		.def(MWLua::constructor<GEntityPlayer*>())
		.def("UpdateQuestVar",		&GGluePlayer::UpdateQuestVar)
		.def("GetQuestVar",			&GGluePlayer::GetQuestVar)
		.def("IsCompleteQuest",		&GGluePlayer::IsCompleteQuest)
		.def("AddQuest",			&GGluePlayer::AddQuest)
		.def("RuinQuest",			&GGluePlayer::RuinQuest)

		.def("BindSoul",			&GGluePlayer::BindSoul)

		.def("GetItem",				&GGluePlayer::GetItemQty)

		.def("CheckCondition",		&GGluePlayer::CheckCondition)

		.def("GateToTrial",			&GGluePlayer::GateToTrial)
		.def("GateToInnRoom",		&GGluePlayer::GateToInnRoom)

		.def("SetFatigue",			&GGluePlayer::SetFatigue)
		.def("Tip",					&GGluePlayer::Tip)

		.def("AddSilver",			&GGluePlayer::AddMoney)
		.def("RemoveSilver",		&GGluePlayer::RemoveMoney)
		.def("GetSilver",			&GGluePlayer::GetMoney)

		.def("GateToMarker",		&GGluePlayer::GateToMarker)
		.def("GateToActor",			&GGluePlayer::GateToActor)		
		.def("Gate",				&GGluePlayer::Gate)
		.def("GetInteractNPC",		&GGluePlayer::GetInteractNPC)

		.def("ArenaRegister",		&GGluePlayer::ArenaRegister)
		.def("Cutscene",			&GGluePlayer::Cutscene)
		.def("PartyCutscene",		&GGluePlayer::PartyCutscene)
		.def("AddAmity",			&GGluePlayer::AddAmity)
		.def("RemoveAmity",			&GGluePlayer::RemoveAmity)
		.def("SetAmity",			&GGluePlayer::SetAmity)
		;
}

void GScriptManager::RegisterGlue_NPCSession()
{
	MWLua::class_<GFieldNPCSession>(m_Lua.GetState(), "NPCSession")
		.def("GetName",				&GFieldNPCSession::GetName)
		.def("AddNPC",				&GFieldNPCSession::AddNPC_ForGlue)
		.def("RemoveNPC",			&GFieldNPCSession::RemoveNPC_ForGlue)
		.def("GetNPCQty",			&GFieldNPCSession::GetNPCQty)
		.def("NonBlocking",			&GFieldNPCSession::NonBlocking)
		.def("Blocking",			&GFieldNPCSession::Blocking)
		.def("EndSession",			&GFieldNPCSession::EndSession)
		.def("Wait",				&GFieldNPCSession::Wait)
		.def("AddChangeSceneRule",	&GFieldNPCSession::AddChangeSceneRule_ForGlue)
		.def("ChangeScene",			&GFieldNPCSession::ChangeState_ForGlue)
		.def("GetSceneName",		&GFieldNPCSession::GetSceneName)
		.def("MakePeace",			&GFieldNPCSession::MakePeace_ForGlue)
		.def("MakeCombat",			&GFieldNPCSession::MakeCombat_ForGlue)
		.def("CombatSession",		&GFieldNPCSession::CombatSession_ForGlue)
		.def("CombatAll",			&GFieldNPCSession::CombatAll_ForGlue)
		.def("CombatNone",			&GFieldNPCSession::CombatNone_ForGlue)
		.def("AddBookmark",			&GFieldNPCSession::AddBookmark)
		.def("FindBookmark",		&GFieldNPCSession::FindBookmark)
		
		//.def("BeginTailCall",		&GFieldNPCSession::BeginTailCall)
		//.def("EndTailCall",			&GFieldNPCSession::EndTailCall)
		//.def("SetTailCallback",		&GFieldNPCSession::SetTailCallback)
		;
}

void GScriptManager::RegisterGlue_NullObject()
{

	MWLua::class_<GNullGlueActor>(WLUA->GetState(), "NullActor")
		.def(MWLua::constructor<>())
		.def("IsNull", &GNullGlueActor::IsNull)
		;

	MWLua::class_<GNullGlueNPC>(WLUA->GetState(), "NullNPC")
		.def(MWLua::constructor<>())
		.def("IsNull", &GNullGlueNPC::IsNull)
		;

	MWLua::class_<GNullGluePlayer>(WLUA->GetState(), "NullPlayer")
		.def(MWLua::constructor<>())
		.def("IsNull", &GNullGluePlayer::IsNull)
		;

	MWLua::class_<GNullGlueField>(WLUA->GetState(), "NullField")
		.def(MWLua::constructor<>())
		.def("IsNull", &GNullGlueField::IsNull)
		;

	MWLua::class_<GNullGlueQuestPVP>(WLUA->GetState(), "NullQuestPVP")
		.def(MWLua::constructor<>())
		.def("IsNull", &GNullGlueQuestPVP::IsNull)
		;
}

//////////////////////////////////////////////////////////////////////////

void GScriptManager::InitHandler()
{
	// 인스턴스 생성
	VALID(!m_pHandlerField);	
	m_pHandlerField = new GScriptGroup_Field();
	VALID(!m_pHandlerQuest);	
	m_pHandlerQuest = new GScriptGroup_Quest();
	VALID(!m_pHandlerNPC);	
	m_pHandlerNPC = new GScriptGroup_NPC();
	VALID(!m_pHandlerQuestPVP);
	m_pHandlerQuestPVP = new GScriptGroup_QuestPVP();	
}

void GScriptManager::FinitHandler()
{
	VALID(m_pHandlerField);
	SAFE_DELETE(m_pHandlerField);
	VALID(m_pHandlerQuest);
	SAFE_DELETE(m_pHandlerQuest);
	VALID(m_pHandlerNPC);
	SAFE_DELETE(m_pHandlerNPC);
	VALID(m_pHandlerQuestPVP);
	SAFE_DELETE(m_pHandlerQuestPVP);
}

GScriptManager* GetScriptManager()
{
	return gsys.pScriptManager;
}

GScriptGroup_Field* GScriptManager::GetScriptField()
{
	return m_pHandlerField;
}

GScriptGroup_Quest* GScriptManager::GetScriptQuest()
{
	return m_pHandlerQuest;
}

GScriptGroup_NPC* GScriptManager::GetScriptNPC()
{
	return m_pHandlerNPC;
}

GScriptGroup_QuestPVP* GScriptManager::GetScriptQuestPVP()
{
	return m_pHandlerQuestPVP;
}

void GScriptManager::InitNullObject()
{
	GNullGlueActor::Init();
	GNullGlueNPC::Init();
	GNullGluePlayer::Init();
	GNullGlueField::Init();
	GNullGlueQuest::Init();
	GNullGlueQuestPVP::Init();
}

void GScriptManager::FiniNullObject()
{
	GNullGlueActor::FIni();
	GNullGlueNPC::FIni();
	GNullGluePlayer::FIni();
	GNullGlueField::FIni();
	GNullGlueQuest::FIni();
	GNullGlueQuestPVP::FIni();
}
