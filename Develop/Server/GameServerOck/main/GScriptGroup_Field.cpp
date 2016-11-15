#include "StdAfx.h"

#include "GScriptGroup_Field.h"
#include "GFieldInfo.h"
#include "GDef.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GGlueActor.h"
#include "GGlueNPC.h"
#include "GLuaThisEnchanter.h"
#include "GConst.h"
#include "GLuaGlues.h"
#include "GNullGlueField.h"
#include "GNullGlueActor.h"
#include "GNullGluePlayer.h"
#include "GNullGlueNPC.h"
#include "GField.h"

const char* pszLuaFieldFuncnames[]={
	"OnCreate",
	"OnDestroy",
	"OnTimeChanged",
	"OnWeatherChanged",
	"OnTimer",
	"OnSensorEnter",
	"OnSensorLeave",
	"OnSensorInteract",
	"OnSensorTalent",
	"OnMarkerArrive",
	"OnSpawn",
	"OnDie",
	"OnEndCutscene",
	"OnSessionScene",
	"OnEvent",
};

#define LUA_CALLBACK_FUNCNAME_FIELD_CREATE				pszLuaFieldFuncnames[0]
#define LUA_CALLBACK_FUNCNAME_FIELD_DESTROY				pszLuaFieldFuncnames[1]
#define LUA_CALLBACK_FUNCNAME_FIELD_TIME_CHANGED		pszLuaFieldFuncnames[2]
#define LUA_CALLBACK_FUNCNAME_FIELD_WEATHER_CHANGED		pszLuaFieldFuncnames[3]
#define LUA_CALLBACK_FUNCNAME_FIELD_TIMER				pszLuaFieldFuncnames[4]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_ENTER		pszLuaFieldFuncnames[5]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_LEAVE		pszLuaFieldFuncnames[6]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_INTERACT		pszLuaFieldFuncnames[7]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_USETALENT	pszLuaFieldFuncnames[8]
#define LUA_CALLBACK_FUNCNAME_FIELD_MARKER_ARRIVE		pszLuaFieldFuncnames[9]
#define LUA_CALLBACK_FUNCNAME_FIELD_NPC_SPAWN			pszLuaFieldFuncnames[10]
#define LUA_CALLBACK_FUNCNAME_FIELD_NPC_DESPAWN			pszLuaFieldFuncnames[11]
#define LUA_CALLBACK_FUNCNAME_FIELD_END_CUTSCENE		pszLuaFieldFuncnames[12]
#define LUA_CALLBACK_FUNCNAME_FIELD_ENTER_SESSION_SCENE	pszLuaFieldFuncnames[13]
#define LUA_CALLBACK_FUNCNAME_FIELD_EVENT				pszLuaFieldFuncnames[14]



GScriptGroup_Field::GScriptGroup_Field(void)
{
}

GScriptGroup_Field::~GScriptGroup_Field(void)
{
}

void GScriptGroup_Field::RegisterGlues()
{
	MWLua::class_<GGlueField>(WLUA->GetState(), "GField")
		.def(MWLua::constructor<GField*>())
		.def("Spawn",				&GGlueField::Spawn)
		.def("Despawn",				&GGlueField::Despawn)
		.def("DespawnNow",			&GGlueField::DespawnNow)
		.def("SpawnLimited",		&GGlueField::SpawnLimited)
		.def("SpawnDelay",			&GGlueField::SpawnDelay)
		.def("SpawnByID",			&GGlueField::SpawnByID)
		.def("SpawnDelayByID",		&GGlueField::SpawnDelayByID)
		.def("DespawnByID",			&GGlueField::DespawnByID)
		.def("GetSpawnNPC",			&GGlueField::GetSpawnNPC)
		.def("EnableSensor",		&GGlueField::EnableSensor)
		.def("DisableSensor",		&GGlueField::DisableSensor)
		.def("CheckSensor",			&GGlueField::CheckSensor)
		.def("GetNPCCount",			&GGlueField::GetNPCCount)
		.def("GetNPC",				&GGlueField::GetNPC)
		.def("SetTimer",			&GGlueField::SetTimer)
		.def("KillTimer",			&GGlueField::KillTimer)
		.def("GetMarkerPos",		&GGlueField::GetMarkerPos)
		.def("GetMarkerDir",		&GGlueField::GetMarkerDir)
		.def("GetID",				&GGlueField::GetID)
		.def("IsNull",				&GGlueField::IsNull)
		.def("EnableInteraction",	&GGlueField::EnableInteraction)
		.def("DisableInteraction",	&GGlueField::DisableInteraction)
		.def("ActivateSpawnGroup",  &GGlueField::ActivateSpawnGroup)
		.def("PlayBGM",				&GGlueField::PlayBGM)
		.def("StopBGM",				&GGlueField::StopBGM)
		.def("Notice",				&GGlueField::Notice)
		.def("NoticeToField",		&GGlueField::NoticeToField)
		.def("ChangeWeather",		&GGlueField::ChangeWeather)
		.def("GetQuestPVP",			&GGlueField::GetQuestPVP)
		.def("MakeSession",			&GGlueField::MakeSession)
		.def("FindSession",			&GGlueField::FindSession)
		;
}

bool GScriptGroup_Field::PreCheck()
{
	return GetScriptManager()->IsRunning();
}

void GScriptGroup_Field::Load(GFieldInfo* pFieldInfo)
{
	string strFileName = MAKE_LUAFILE_FIELD(pFieldInfo->m_nFieldID);
	string strTableName = MAKE_LUATABLE_FIELD(pFieldInfo->m_nFieldID);
	MAKE_TABLE(strTableName.c_str());

	// 스크립트 전처리
	string strScript;
	const string strScriptFilename = MAKE_LUAFILE(MLocale::ConvUTF16ToAnsi(PATH_SCRIPT_FIELD).c_str(), strFileName);
	GLuaThisEnchanter enchanterThis;
	
	for (size_t i=0; i<ArraySize(pszLuaFieldFuncnames); ++i)
		enchanterThis.AddMatchMemFunc(pszLuaFieldFuncnames[i]);

	if (!enchanterThis.ConvertFromFile(strScriptFilename, strScript))
		return;

	// 스크립트 실행
	if (strScript.length() == 0)
		return;

	MLog1(MLOG_FILE, "FieldScriptLoad (id: %d)\n", pFieldInfo->m_nFieldID);

	if (!WRUNSTRING(WLUA, strScript))
	{
		mlog3("필드 스크립트 로딩 실패. Field:%s(%d)\n", MLocale::ConvUTF16ToAnsi(pFieldInfo->m_strFieldName.c_str()).c_str(), pFieldInfo->m_nFieldID);
		return;
	}

	if (IsRunForTest())
	{
		if (GConst::DEBUG_CREATE_CONVERT_LUAFILE)
		{
			// 디버그용 파일 생성
			enchanterThis.SaveToFile(strScriptFilename+".convert.txt", strScript);
		}
	}
}

bool GScriptGroup_Field::HasSensorScript(int nFieldID, int nSensorID)
{
	string strTableName = MAKE_LUATABLE_FIELD(nFieldID);
	string strFuncName;
	
	strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_ENTER, nSensorID);
	if (WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))		return true;
	strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_LEAVE, nSensorID);
	if (WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))		return true;
	strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_INTERACT, nSensorID);
	if (WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))		return true;
	strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_USETALENT, nSensorID);
	if (WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))		return true;

	return false;
}

bool GScriptGroup_Field::OnCreate(GGlueField* pField)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	return LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_CREATE); 
}

bool GScriptGroup_Field::OnDestroy(GGlueField* pField)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	return LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_DESTROY); 
}

bool GScriptGroup_Field::OnTimeChanged(GGlueField* pField, int nNewTime)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	return LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_TIME_CHANGED, nNewTime); 
}

bool GScriptGroup_Field::OnWeatherChanged(GGlueField* pField, int nNextWeather)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	return LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_WEATHER_CHANGED, (int)nNextWeather);
}

bool GScriptGroup_Field::OnTimer(GGlueField* pField, int nTimerID)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	return LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_TIMER, nTimerID);
}

bool GScriptGroup_Field::OnSensorEnter(GGlueField* pField, int nSensorID, GGlueActor* pActor)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pActor, false);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_ENTER, nSensorID);
	return LuaCallback(pField, strFuncName, pActor);
}

bool GScriptGroup_Field::OnSensorLeave(GGlueField* pField, int nSensorID, GGlueActor* pActor)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pActor, false);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_LEAVE, nSensorID);
	return LuaCallback(pField, strFuncName, pActor);
}

bool GScriptGroup_Field::OnSensorInteract(GGlueField* pField, int nSensorID, GGluePlayer* pPlayer)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pPlayer, false);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_INTERACT, nSensorID);
	return LuaCallback(pField, strFuncName, pPlayer);
}

bool GScriptGroup_Field::OnSensorTalent(GGlueField* pField, int nSensorID, GGlueActor* pActor, int nTalentID)
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pActor, false);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_USETALENT, nSensorID);
	return LuaCallback(pField, strFuncName, pActor, nTalentID);
}

bool GScriptGroup_Field::OnMarkerArrive( GGlueField* pField, unsigned long nMarkerID, GGlueNPC* pNPC )
{

	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pNPC, false);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_MARKER_ARRIVE, nMarkerID);
	return LuaCallback(pField, strFuncName, pNPC);
}


bool GScriptGroup_Field::OnSpawn(GGlueField* pField, GGlueNPC* pNPC, int nSpawnID)
{
	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pNPC, false);
	MWLua::table t(WLUA->GetState());
	m_ScriptUtil.MakeSpawnTable(&t, nSpawnID, pNPC, pField);

	bool bRet = 
		LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_NPC_SPAWN, t);

	return bRet;
}

bool GScriptGroup_Field::OnDie(GGlueField* pField, GGlueNPC* pNPC, int nSpawnID)
{
	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pNPC, false);
	MWLua::table t(WLUA->GetState());
	m_ScriptUtil.MakeSpawnTable(&t, nSpawnID, pNPC, pField);

	bool bRet = 
		LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_NPC_DESPAWN, t);

	return bRet;
}

bool GScriptGroup_Field::OnEndCutscene( GGlueField* pField, GGluePlayer* pPlayer, int nCutsceneID )
{
	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pPlayer, false);
	return LuaCallback(pField, LUA_CALLBACK_FUNCNAME_FIELD_END_CUTSCENE, pPlayer, nCutsceneID); 
}

bool GScriptGroup_Field::OnEvent( GGlueField* pField, GGlueActor* pActor, const char* pszName )
{
	if (!PreCheck())	return false;
	VALID_RET(pField, false);
	VALID_RET(pActor, false);
	VALID_RET(pszName, false);
	string strFuncName = string(LUA_CALLBACK_FUNCNAME_FIELD_EVENT) + "_" + pszName;
	return LuaCallback(pField, strFuncName, pActor); 
}
