#pragma once

class ZServer;
class ZSystem;
class ZCommandCenter;
class ZDBManager;
class ZPartyLogic;
class ZScriptManager;
class ZLootLogic;
class ZCharLogic;
class ZPMSSystem;
class ZGuildLogic;

#define __GLOBAL_CHANGE(Type, Value)	Type* Change(Type* a) { Type* pPrev = Value; Value = a; return pPrev; }



class ZGlobalSystem
{
public:	
	ZServer*				pServer;
	ZSystem*				pSystem;
	ZCommandCenter*			pCommandCenter;
	ZDBManager*				pDBManager;
	ZPartyLogic*			pPartyLogic;
	ZScriptManager*			pScriptManager;
	ZLootLogic*				pLootLogic;
	ZCharLogic*				pCharLogic;
	ZPMSSystem*				pPMS;
	ZGuildLogic*			pGuildLogic;

	ZGlobalSystem()
	{
		SetNull();
	}
	virtual ~ZGlobalSystem() {}

	void SetNull()
	{
		pServer = NULL;
		pSystem = NULL;
		pCommandCenter = NULL;
		pDBManager = NULL;
		pPartyLogic = NULL;
		pScriptManager = NULL;
		pLootLogic = NULL;
		pCharLogic = NULL;
		
		pPMS = NULL;
	}

	__GLOBAL_CHANGE(ZServer,			pServer);
	__GLOBAL_CHANGE(ZSystem,			pSystem);
	__GLOBAL_CHANGE(ZCommandCenter, 	pCommandCenter);
	__GLOBAL_CHANGE(ZDBManager,			pDBManager);
	__GLOBAL_CHANGE(ZPartyLogic,		pPartyLogic);
	__GLOBAL_CHANGE(ZScriptManager,		pScriptManager);
	__GLOBAL_CHANGE(ZLootLogic,			pLootLogic);
	__GLOBAL_CHANGE(ZCharLogic,			pCharLogic);
	__GLOBAL_CHANGE(ZPMSSystem,			pPMS);
	__GLOBAL_CHANGE(ZGuildLogic,		pGuildLogic);
};



class ZGameServerObjectManager;
class ZGameServerInfoManager;
class ZLoginServerFacade;
class ZLoginServerInfoManager;
class ZAppServerObjectManager;
class ZAppServerInfoManager;
class ZFieldInfoManager;
class ZFieldManager;
class ZPlayerManager;
class ZDuplicationLoginManager;
class ZPartyManager;
class ZFixedPartyInfoManager;

class ZGlobalManager
{
public:
	ZGameServerInfoManager*			pGameServerInfoManager;
	ZGameServerObjectManager*		pGameServerObjectManager;

	ZLoginServerInfoManager*		pLoginServerInfoManager;
	ZLoginServerFacade*				pLoginServerFacade;

	ZAppServerInfoManager*			pAppServerInfoManager;
	ZAppServerObjectManager*		pAppServerObjectManager;

	ZFieldInfoManager*				pFieldInfoManager;
	ZFieldManager*					pFieldManager;

	ZPlayerManager*					pPlayerManager;
	ZDuplicationLoginManager*		pDuplicationLoginManager;

	ZPartyManager*					pPartyManager;
	ZFixedPartyInfoManager*			pFixedPartyInfoManager;

	ZGlobalManager()
	{
		SetNull();
	}
	virtual ~ZGlobalManager() {}
		

	void SetNull()
	{
		pGameServerInfoManager = NULL;
		pGameServerObjectManager = NULL;

		pLoginServerInfoManager = NULL;
		pLoginServerFacade = NULL;

		pAppServerInfoManager = NULL;
		pAppServerObjectManager = NULL;

		pFieldInfoManager = NULL;
		pFieldManager = NULL;

		pPlayerManager = NULL;
		pDuplicationLoginManager = NULL;
		
		pPartyManager = NULL;
		pFixedPartyInfoManager = NULL;
	}

	__GLOBAL_CHANGE(ZGameServerInfoManager,			pGameServerInfoManager);
	__GLOBAL_CHANGE(ZGameServerObjectManager,		pGameServerObjectManager);

	__GLOBAL_CHANGE(ZLoginServerInfoManager,		pLoginServerInfoManager);
	__GLOBAL_CHANGE(ZLoginServerFacade,				pLoginServerFacade);

	__GLOBAL_CHANGE(ZAppServerInfoManager,			pAppServerInfoManager);
	__GLOBAL_CHANGE(ZAppServerObjectManager,		pAppServerObjectManager);

	__GLOBAL_CHANGE(ZFieldInfoManager,				pFieldInfoManager);
	__GLOBAL_CHANGE(ZFieldManager,					pFieldManager);

	__GLOBAL_CHANGE(ZPlayerManager,					pPlayerManager);
	__GLOBAL_CHANGE(ZDuplicationLoginManager,		pDuplicationLoginManager);

	__GLOBAL_CHANGE(ZPartyManager,					pPartyManager);
	__GLOBAL_CHANGE(ZFixedPartyInfoManager,			pFixedPartyInfoManager);
};
		

extern ZGlobalSystem gsys;
extern ZGlobalManager gmgr;

