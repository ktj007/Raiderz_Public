#pragma once

#include "MWLua.h"

class GScript
{
protected:
	MWLua					m_Lua;

public:
	GScript();
	virtual ~GScript();
	bool Create();
	void Destroy();

	// 게임 이벤트
	void Event(const char* szEvent);
	void Event(const char* szEvent, const char* str1, ...);
	void Event(const char* szEvent, double fArg1, ...);

	MWLua* GetLua()		{ return &m_Lua; }
};
