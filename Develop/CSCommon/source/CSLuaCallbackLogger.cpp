#include "stdafx.h"
#include "CSLuaCallbackLogger.h"
#include "CSLogListener.h"
#include "MWLua.h"
#include "MLocale.h"


CSLogListener* CSLuaCallbackLogger::sm_pListener = NULL;

void CSLuaCallbackLogger::Log(const char* message)
{
	if (message == NULL || sm_pListener == NULL)
	{
		return;
	}

	sm_pListener->OnLog(message);
}

int CSLuaCallbackLogger::Exception(lua_State* L)
{
	std::tstring strError;

	if (lua_isstring(L, -1))
		strError += MLocale::ConvAnsiToTCHAR(lua_tostring(L, -1));
	else
		strError += _T("No Error Message.");

	CSLuaCallbackLogger::Log(MLocale::ConvTCHARToAnsi(strError.c_str()).c_str());

	strError = _T("\n") + strError + _T("\n");	// 기본 exceptionFunction과 동일한 출력문장 처리
	MWLua::GetWLua(L)->Log(MLocale::ConvTCHARToAnsi(strError.c_str()).c_str());

	lua_remove(L, -1);

	ShowCallStack(L, 0);

	// Clean Up
	MWLua::GetWLua(L)->InitCppCallInfo();

	mlog3(strError.c_str());

	return 0;
}
