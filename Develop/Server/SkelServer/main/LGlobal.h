#pragma once

class LServer;
class LSystem;
class LCommandCenter;

#define __GLOBAL_CHANGE(Type, Value)	Type* Change(Type* a) { Type* pPrev = Value; Value = a; return pPrev; }



class LGlobalSystem
{
public:	
	LServer*				pServer;
	LSystem*				pSystem;
	LCommandCenter*			pCommandCenter;
	

	LGlobalSystem()
	{
		SetNull();
	}
	virtual ~LGlobalSystem() {}

	void SetNull()
	{
		pServer = NULL;
		pSystem = NULL;
		pCommandCenter = NULL;
	}

	__GLOBAL_CHANGE(LServer,		pServer);
	__GLOBAL_CHANGE(LSystem,		pSystem);
	__GLOBAL_CHANGE(LCommandCenter, pCommandCenter);
};



class LCommObjectManager;

class LGlobalManager
{
public:
	LCommObjectManager*	pCommObjectManager;
	
	LGlobalManager()
	{
		SetNull();
	}
	virtual ~LGlobalManager() {}
		

	void SetNull()
	{
		pCommObjectManager = NULL;
	}

	__GLOBAL_CHANGE(LCommObjectManager,		pCommObjectManager);
};
		

extern LGlobalSystem gsys;
extern LGlobalManager gmgr;

