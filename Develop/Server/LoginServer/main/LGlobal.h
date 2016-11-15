#pragma once

class LServer;
class LSystem;
class LCommandCenter;
class LDBManager;
class LPlayerLoginGameServerRetryer;
class LPMSSystem;

#define __GLOBAL_CHANGE(Type, Value)	Type* Change(Type* a) { Type* pPrev = Value; Value = a; return pPrev; }



class LGlobalSystem
{
public:	
	LServer*						pServer;
	LSystem*						pSystem;
	LCommandCenter*					pCommandCenter;
	LDBManager*						pDBManager;
	LPlayerLoginGameServerRetryer*	pPlayerLoginGameServerRetryer;
	LPMSSystem*						pPMS;

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
		pDBManager = NULL;
		pPlayerLoginGameServerRetryer = NULL;
		pPMS = NULL;
	}

	__GLOBAL_CHANGE(LServer,						pServer);
	__GLOBAL_CHANGE(LSystem,						pSystem);
	__GLOBAL_CHANGE(LCommandCenter, 				pCommandCenter);
	__GLOBAL_CHANGE(LDBManager,						pDBManager);
	__GLOBAL_CHANGE(LPlayerLoginGameServerRetryer,	pPlayerLoginGameServerRetryer);
	__GLOBAL_CHANGE(LPMSSystem,						pPMS);
};



class LPlayerObjectManager;
class LMasterServerMgr;
class LTimeoutManager;
class LPlayerAcceptManager;

class LGlobalManager
{
public:
	LPlayerObjectManager*		pPlayerObjectManager;
	LMasterServerMgr*			pMasterServerMgr;
	LTimeoutManager*			pTimeoutManager;
	LPlayerAcceptManager*		pPlayerAcceptManager;
	
	LGlobalManager()
	{
		SetNull();
	}
	virtual ~LGlobalManager() {}
		

	void SetNull()
	{
		pPlayerObjectManager = NULL;
		pMasterServerMgr = NULL;
		pTimeoutManager = NULL;
		pPlayerAcceptManager = NULL;
	}

	__GLOBAL_CHANGE(LPlayerObjectManager,			pPlayerObjectManager);
	__GLOBAL_CHANGE(LMasterServerMgr,				pMasterServerMgr);
	__GLOBAL_CHANGE(LTimeoutManager,				pTimeoutManager);
	__GLOBAL_CHANGE(LPlayerAcceptManager,			pPlayerAcceptManager);
};
		

extern LGlobalSystem gsys;
extern LGlobalManager gmgr;

