#include "stdafx.h"
#include "GNPCScriptBuilder.h"
#include "GNPCInfo.h"
#include "GDef.h"
#include "GScriptMacro.h"
#include "GScriptManager.h"
#include "GConst.h"
#include "GGlobal.h"


GNPCScriptBuilder::GNPCScriptBuilder(GNPCInfo* pNPCInfo) : m_pNPCInfo(pNPCInfo)
{	
}


GNPCScriptBuilder::~GNPCScriptBuilder()
{

}


bool GNPCScriptBuilder::Build()
{
	if (!CheckNPCValid())
		return false;

	BuildConvertFuncNameList();

	if (!CreateNPCNameLuaTable())
		return false;

	MakeTableProperties();
	BuildLuaSubScript();
	BuildFromLuaFile();
	BuildFromXmlLuaScript();		

	return true;
}


void GNPCScriptBuilder::SetXmlLuaScript(const string& strXmlLuaScript)
{
	m_strXmlLuaScript = strXmlLuaScript;
}


bool GNPCScriptBuilder::BuildFromLuaFile()
{
	if (!CheckNPCValid())
		return false;

	string strScript;

	if (!m_LuaThisEnchanter.ConvertFromFile(MAKE_LUAFILE(MLocale::ConvUTF16ToAnsi(PATH_SCRIPT_NPC_LUA).c_str(), MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua.c_str()).c_str()), strScript))
		return false;

	if (!WRUNSTRING(WLUA, strScript.c_str()))
	{
		mlog3("NPC script syntax error occurred. File: %s.lua, ID:%d(%s)\n", 
			MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua).c_str(), 
			m_pNPCInfo->nID, 
			MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strName).c_str());
		return false;
	}

	if (IsRunForTest())
	{
		if (GConst::DEBUG_CREATE_CONVERT_LUAFILE)
		{
			// 디버그용 파일 생성
			m_LuaThisEnchanter.SaveToFile(MAKE_LUAFILE(MLocale::ConvUTF16ToAnsi(PATH_SCRIPT_NPC_LUA).c_str(), MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua.c_str()).c_str())+".convert.txt", strScript);
		}
	}

	return true;
}


bool GNPCScriptBuilder::BuildFromXmlLuaScript()
{
	if (!CheckNPCValid())
		return false;

	if (m_strXmlLuaScript.empty())
		return false;

	string strScript;

	if (!m_LuaThisEnchanter.ConvertFromStream(m_strXmlLuaScript, strScript))
		return false;

	if (!WRUNSTRING(WLUA, strScript.c_str()))
	{
		mlog3("NPC script syntax error occurred. File: %s.xml, ID:%d(%s)\n", 
			MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua).c_str(), 
			m_pNPCInfo->nID, 
			MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strName).c_str());
		return false;
	}

	if (IsRunForTest())
	{
		if (GConst::DEBUG_CREATE_CONVERT_LUAFILE)
		{
			// 디버그용 파일 생성
			m_LuaThisEnchanter.SaveToFile(MAKE_LUAFILE(MLocale::ConvUTF16ToAnsi(PATH_SCRIPT_NPC_COLT).c_str(), MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua.c_str()).c_str())+".convert.txt", strScript);
		}
	}

	return true;
}


bool GNPCScriptBuilder::CreateNPCNameLuaTable()
{
	return MAKE_TABLE(MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua.c_str()).c_str());
}

void GNPCScriptBuilder::MakeTableProperties()
{
	// 멤버 변수 추가
	gsys.pScriptManager->GetLua()->SetMemberVar(MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua.c_str()).c_str(), "MaxHP", m_pNPCInfo->nMaxHP);
}


bool GNPCScriptBuilder::CheckNPCValid()
{
	VALID_RET(m_pNPCInfo, false);
	VALID_RET(m_pNPCInfo->strNameInLua.length() > 0, false);

	return true;
}


void GNPCScriptBuilder::BuildConvertFuncNameList()
{
	for (size_t i=1; i<ArraySize(pszLuaNPCFuncnames); ++i)	// Info: 1부터 시작하는 이유는 NPC:Init() 콜백은 this가 필요하지 않기 때문.
		m_LuaThisEnchanter.AddMatchMemFunc(pszLuaNPCFuncnames[i]);
}


bool GNPCScriptBuilder::BuildLuaSubScript()
{
	VALID_RET(m_pNPCInfo, false);
	VALID_RET(m_pNPCInfo->strNameInLua.length() > 0, false);

	char szID[128];
	_itoa_s(m_pNPCInfo->nID, szID, 128, 10);

	// SubScribeAI 추가
	const string strScript = "function " + MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua.c_str()) + ":RegisterTeachableTalentID(id)\n"\
		"RegisterTeachableTalentID(\"" + string(szID) + "\", id)\n"\
		"end\n"\
		"function " + MLocale::ConvUTF16ToAnsi(m_pNPCInfo->strNameInLua.c_str()).c_str() + ":RegisterSellItemID(id)\n"\
		"RegisterSellItemID(\"" + szID + "\", id)\n"\
		"end\n";

	return WRUNSTRING(WLUA, strScript.c_str());
}
