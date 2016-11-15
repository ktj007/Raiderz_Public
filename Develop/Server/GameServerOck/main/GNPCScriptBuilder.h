#ifndef _GNPCSCRIPT_BUILDER_H
#define _GNPCSCRIPT_BUILDER_H

#include "GLuaThisEnchanter.h"

static const char* pszLuaNPCFuncnames[]={
	  "Init"
	, "OnSpawn"
	, "OnDie"
	, "OnYell"
	, "OnDialogExit"
	, "OnAggro"
	, "OnTimer"
	, "OnUserdataChanged"
	, "OnHitCapsule"
	, "OnSwallowBegin"
	, "OnHitSwallowed"
	, "OnInteract"
	, "OnTryHit"
	, "OnChangeMode"
	, "OnTalentEnd"
	, "OnVictory"
	, "OnCombatBegin"
	, "OnCombatEnd"
	, "OnBPartRecover"
};

#define LUA_CALLBACK_FUNCNAME_NPC_INIT				pszLuaNPCFuncnames[0]
#define LUA_CALLBACK_FUNCNAME_NPC_SPAWN				pszLuaNPCFuncnames[1]
#define LUA_CALLBACK_FUNCNAME_NPC_DESPAWN			pszLuaNPCFuncnames[2]
#define LUA_CALLBACK_FUNCNAME_NPC_YELL				pszLuaNPCFuncnames[3]
#define LUA_CALLBACK_FUNCNAME_NPC_DIALOGEXIT		pszLuaNPCFuncnames[4]
#define LUA_CALLBACK_FUNCNAME_NPC_AGGRO				pszLuaNPCFuncnames[5]
#define LUA_CALLBACK_FUNCNAME_NPC_TIMER				pszLuaNPCFuncnames[6]
#define LUA_CALLBACK_FUNCNAME_NPC_USERDATACHANGED	pszLuaNPCFuncnames[7]
#define LUA_CALLBACK_FUNCNAME_NPC_HITCAPSULE		pszLuaNPCFuncnames[8]
#define LUA_CALLBACK_FUNCNAME_NPC_SWALLOW_BEGIN		pszLuaNPCFuncnames[9]
#define LUA_CALLBACK_FUNCNAME_NPC_HITSWALLOWED		pszLuaNPCFuncnames[10]
#define LUA_CALLBACK_FUNCNAME_NPC_INTERACT			pszLuaNPCFuncnames[11]
#define LUA_CALLBACK_FUNCNAME_NPC_TRYHIT			pszLuaNPCFuncnames[12]
#define LUA_CALLBACK_FUNCNAME_NPC_CHANGEMODE		pszLuaNPCFuncnames[13]
#define LUA_CALLBACK_FUNCNAME_NPC_TALENTEND			pszLuaNPCFuncnames[14]
#define LUA_CALLBACK_FUNCNAME_NPC_VICTORY			pszLuaNPCFuncnames[15]
#define LUA_CALLBACK_FUNCNAME_NPC_COMBAT_BEGIN		pszLuaNPCFuncnames[16]
#define LUA_CALLBACK_FUNCNAME_NPC_COMBAT_END		pszLuaNPCFuncnames[17]
#define LUA_CALLBACK_FUNCNAME_NPC_BPART_RECOVER		pszLuaNPCFuncnames[18]



class GNPCInfo;


TEST_FORWARD_F(TestScriptFunction_NPC, Fixture);

class GNPCScriptBuilder
{
	TEST_FRIEND_F(TestScriptFunction_NPC, Fixture);
public :
	GNPCScriptBuilder(GNPCInfo* pNPCInfo);
	~GNPCScriptBuilder();

	bool CheckNPCValid();
	void SetXmlLuaScript(const string& strStream);

	bool Build();


private :
	bool CreateNPCNameLuaTable();

	void BuildConvertFuncNameList();
	bool BuildFromLuaFile();
	bool BuildFromXmlLuaScript();
	bool BuildLuaSubScript();
	void MakeTableProperties();

private :
	GLuaThisEnchanter	m_LuaThisEnchanter;
	GNPCInfo*			m_pNPCInfo;	
	vector<string>		m_vecFuncName;
	string				m_strXmlLuaScript;
};


#endif