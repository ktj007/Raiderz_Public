#pragma once

#include "MWLua.h"

/// lua에서 사용하는 C 함수 모음
class XScriptFunc
{
public:
	static void Register(MWLua* pLua);

	// Debug ---------
	static void Debug_AddLogFilter(const char* szFilter);
	static void Debug_Console(const char* szCommand);
};
