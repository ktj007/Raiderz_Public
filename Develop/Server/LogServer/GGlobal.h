#pragma once

class GServer;
class GSystem;
class GCommandCenter;
class GDBManager;

#define __GLOBAL_CHANGE(Type, Value)	Type* Change(Type* a) { Type* pPrev = Value; Value = a; return pPrev; }

class GGlobalSystem
{
public:	
	GServer*				pServer;
	GSystem*				pSystem;
	GCommandCenter*			pCommandCenter;
	GDBManager*				pDBManager;
	

	GGlobalSystem()
	{
		SetNull();
	}
	virtual ~GGlobalSystem() {}

	void SetNull()
	{
		pServer = NULL;
		pSystem = NULL;
		pCommandCenter = NULL;
		pDBManager = NULL;
		
	}
};



class GGameServerMgr;

class GGlobalManager
{
public:
	GGameServerMgr*	pGameServerMgr;
	
	GGlobalManager()
	{
		SetNull();
	}
	virtual ~GGlobalManager() {}
		

	void SetNull()
	{
		pGameServerMgr = NULL;
	}
};
		

extern GGlobalSystem gsys;
extern GGlobalManager gmgr;

