#include "StdAfx.h"
#include ".\gscript.h"

GScript::GScript()
{
}

GScript::~GScript()
{
}

bool GScript::Create()
{
	if(!m_Lua.Open())
	{
		return true;
	}

	return false;
}

void GScript::Destroy()
{
	m_Lua.Close();
}

void GScript::Event(const char* szEvent)
{
	lua_getglobal(m_Lua.GetState(), szEvent);

//	m_Lua.RunCall(0,0);
}

void GScript::Event(const char* szEvent, const char *str, ...)
{
	lua_getglobal(m_Lua.GetState(), szEvent);

	char* pStr = const_cast<char*>(str);
	va_list args;
	int nCount = 0;

	va_start(args, str);
	while(pStr != NULL)
	{
//		m_Lua.PushString(pStr);
		pStr = va_arg(args, char*);
		nCount++;
	}
	va_end(args);

//	m_Lua.RunCall(nCount, 0);
}

void GScript::Event(const char* szEvent, double dArg, ...)
{
	lua_getglobal(m_Lua.GetState(), szEvent);

	double arg = dArg;
	va_list args;
	int nCount = 0;

	va_start(args, dArg);
	while(dArg != 0)
	{
//		m_Lua.PushNumber(arg);
		arg = va_arg(args, double);
		nCount++;
	}
	va_end(args);

//	m_Lua.RunCall(nCount, 0);
}