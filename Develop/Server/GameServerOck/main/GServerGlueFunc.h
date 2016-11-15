#pragma once

class GModule;
class GModuleAI;
class GServerGlueFunc
{
public:
	static void DeclGlueFunc();
	
	void LuaTraceNum(double d);
	void LuaTraceStr(char* strTrace);

};

extern GServerGlueFunc g_ServerGlueFunc;
