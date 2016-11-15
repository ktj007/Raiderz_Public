#include "stdafx.h"
#include "ZScriptManager.h"
#include "ZDef.h"
#include "ZSystem.h"
#include "ZConst.h"
#include "CSLuaCallbackLogger.h"

static void MaietWLuaLog(const char* text)
{
	if (ZConst::SHOW_LUA_ERROR_LOG)
	{
		mlog3(text);
	}
	else
	{
		MGetDefaultLogger()->Output(MLOG_FILE, text, MLOG_LEVEL_DEBUG);
	}
}


ZScriptManager::ZScriptManager() 
: m_bRunning(false)
{

}

ZScriptManager::~ZScriptManager()
{
	_ASSERT(!IsRunning() && "ZScriptManager::Fini()을 명시적으로 호출해야합니다.");
}

bool ZScriptManager::Init(bool bUseCallbackLogger)
{
	if (m_bRunning)
	{
		Fini();
	}

	m_Lua.Open();
	m_Lua.SetLogFunc(MaietWLuaLog);

	if (true == bUseCallbackLogger)
	{
		m_Lua.SetExceptionFunc(CSLuaCallbackLogger::Exception);
	}

	USES_CONVERSION_EX;
	WRUNFILE(&m_Lua, W2A_EX(FILENAME_SCRIPT_ENTRY, 0));

	m_bRunning = true;


	return true;
}

void ZScriptManager::Fini()
{
	if (!m_bRunning)
		return;

	m_Lua.Close();


	m_bRunning = false;
}

//////////////////////////////////////////////////////////////////////////


ZScriptManager* GetScriptManager()
{
	return gsys.pScriptManager;
}
