#ifndef _G_CONFIG_LUA_H
#define _G_CONFIG_LUA_H

#include "MTypes.h"
#include "GDef.h"
#include "GFrameworkLib.h"

/// 루아 설정
class GFRAMEWORK_API GConfig_Lua
{
public:
	bool		bAutoTestActive;
	wstring		strAutoTestType;
	GAITestType	nAITestType;	
	int			nRepeat;
	DWORD		dwTime;	
	int			nSpeed;

	float		fLimitFrame;

	int			nNPCCount;
	int			nFieldID;
	int			nQPVPEventID;


public:
	GConfig_Lua();
	void Create();
};

#endif
