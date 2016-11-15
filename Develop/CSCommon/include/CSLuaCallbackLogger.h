#pragma once

#include "CSCommonLib.h"

struct lua_State;
class CSLogListener;

class CSCOMMON_API CSLuaCallbackLogger
{
public:
	static void Log(const char* message);
	static int Exception(lua_State* L);
	static void SetListener(CSLogListener* pListener) { sm_pListener = pListener; }

private:
	static CSLogListener* sm_pListener;
};
