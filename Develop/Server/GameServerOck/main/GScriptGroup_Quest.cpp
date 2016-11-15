#include "StdAfx.h"
#include "GScriptGroup_Quest.h"
#include "GFieldInfo.h"
#include "GField.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GPlayerQuest.h"
#include "GQuestInfo.h"
#include "GNullGluePlayer.h"
#include "GNullGlueNPC.h"
#include "GNullGlueQuest.h"
#include "GLuaThisEnchanter.h"
#include "GConst.h"
#include "GGlueQuest.h"
#include "GScriptUtil.h"

const char* pszLuaQuestFuncnames[]={
	"OnBegin",
	"OnEnd",
	"OnComplete",
	"OnFail",
	"OnObjComplete",
	"OnObjUpdate",
	"OnObjProgress",
	"OnObjInteract",
};

#define LUA_CALLBACK_FUNCNAME_QUEST_BEGIN						pszLuaQuestFuncnames[0]
#define LUA_CALLBACK_FUNCNAME_QUEST_END							pszLuaQuestFuncnames[1]
#define LUA_CALLBACK_FUNCNAME_QUEST_COMPLETE					pszLuaQuestFuncnames[2]
#define LUA_CALLBACK_FUNCNAME_QUEST_FAIL						pszLuaQuestFuncnames[3]
#define LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_COMPLETE			pszLuaQuestFuncnames[4]
#define LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_UPDATE			pszLuaQuestFuncnames[5]
#define LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_PROGRESS			pszLuaQuestFuncnames[6]
#define LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_INTERACT			pszLuaQuestFuncnames[7]



GScriptGroup_Quest::GScriptGroup_Quest(void)
{
}

GScriptGroup_Quest::~GScriptGroup_Quest(void)
{
}

void GScriptGroup_Quest::RegisterGlues()
{
	MWLua::class_<GGlueQuest>(WLUA->GetState(), "PlayerQuest")
		.def(MWLua::constructor<GPlayerQuest*>())
		.def("UpdateVar",	 &GGlueQuest::UpdateQuestVar)
		.def("GetVar",		 &GGlueQuest::GetQuestVar)
		;
}

bool GScriptGroup_Quest::PreCheck()
{
	return GetScriptManager()->IsRunning();
}

void GScriptGroup_Quest::Load(int nQuestID)
{
	string strFileName = MAKE_LUAFILE_QUEST(nQuestID);
	string strTableName = MAKE_LUATABLE_QUEST(nQuestID);
	MAKE_TABLE(strTableName.c_str());

	// 스크립트 전처리
	string strScript;
	string strScriptFilename = MAKE_LUAFILE(MLocale::ConvUTF16ToAnsi(PATH_SCRIPT_QUEST).c_str(), strFileName);
	GLuaThisEnchanter enchanterThis;
	
	for (size_t i=0; i<ArraySize(pszLuaQuestFuncnames); ++i)
		enchanterThis.AddMatchMemFunc(pszLuaQuestFuncnames[i]);

	if (!enchanterThis.ConvertFromFile(strScriptFilename, strScript))
		return;

	if (strScript.length() == 0)
		return;

	// 스크립트 실행
	MLog1(MLOG_FILE, "QuestScriptLoad (id: %d)\n", nQuestID);

	WRUNSTRING(WLUA, strScript);

	if (IsRunForTest())
	{
		if (GConst::DEBUG_CREATE_CONVERT_LUAFILE)
		{
			// 디버그용 파일 생성
			enchanterThis.SaveToFile(strScriptFilename+".convert.txt", strScript);
		}
	}
}

bool GScriptGroup_Quest::OnBegin(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, GGlueNPC* pNPC)
{
	if (!PreCheck())	return false;
	
	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_BEGIN, pPlayer, pNPC); 
}

bool GScriptGroup_Quest::OnEnd(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, GGlueNPC* pNPC)
{
	if (!PreCheck())	return false;

	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_END, pPlayer, pNPC); 
}

bool GScriptGroup_Quest::OnComplete(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer)
{
	if (!PreCheck())	return false;

	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_COMPLETE, pPlayer); 
}

bool GScriptGroup_Quest::OnFail(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer)
{
	if (!PreCheck())	return false;

	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_FAIL, pPlayer); 
}

bool GScriptGroup_Quest::OnObjectiveComplete(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger)
{
	if (!PreCheck())	return false;

	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_COMPLETE, pPlayer, nObjectiveID, bTrigger); 
}

bool GScriptGroup_Quest::OnObjectiveUpdate(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger, int nProgress)
{
	if (!PreCheck())	return false;

	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_UPDATE, pPlayer, nObjectiveID, bTrigger, nProgress);
}

bool GScriptGroup_Quest::OnObjectiveProgress( GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger)
{
	if (!PreCheck())	return false;

	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_PROGRESS, pPlayer, nObjectiveID, bTrigger);
}

bool GScriptGroup_Quest::OnObjectiveInteract( GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger, int nProgress, GGlueNPC* pNPC )
{
	if (!PreCheck())	return false;

	VALID_RET(pPlayerQuest, false);
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	return LuaCallback(pPlayerQuest, LUA_CALLBACK_FUNCNAME_QUEST_OBJECTIVE_INTERACT, pPlayer, nObjectiveID, bTrigger, nProgress, pNPC);
}