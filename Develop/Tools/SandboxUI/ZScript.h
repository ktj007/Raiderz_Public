#pragma once

#include "MWLua.h"



class ZScript
{
	MWLua	m_Lua;

public:
	ZScript();
	~ZScript();

	MWLua* GetLua();

	bool Create();
	void Destroy();
};

inline MWLua* ZScript::GetLua()		{ return &m_Lua;	}
