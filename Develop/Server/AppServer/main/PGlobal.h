#pragma once

#define __GLOBAL_CHANGE(Type, Value)	Type* Change(Type* a) { Type* pPrev = Value; Value = a; return pPrev; }


class PServer;
class PSystem;
class PCommandCenter;
class PDBManager;
class PLoginLogic;
class PMsgCommandFacade;
class PMasterServerFacade;
class PPostOfficeSystem;
class PTradeMarketService;
class PTradeMarketServiceConsumer;

class PGlobalSystem
{
public:	
	PServer*						pServer;
	PSystem*						pSystem;
	PCommandCenter*					pCommandCenter;
	PDBManager*						pDBManager;
	PLoginLogic*					pLoginLogic;
	PMsgCommandFacade*				pMsgCommandFacade;
	PMasterServerFacade*			pMasterServerFacade;
	PPostOfficeSystem*				pPostOfficeSystem;
	PTradeMarketService*			pTradeMarketService;
	PTradeMarketServiceConsumer*	pTradeMarketServiceConsumer;

	PGlobalSystem()
	{
		SetNull();
	}
	virtual ~PGlobalSystem() {}

	void SetNull()
	{
		pServer = NULL;
		pSystem = NULL;
		pCommandCenter = NULL;
		pDBManager = NULL;
		pLoginLogic = NULL;
		pMsgCommandFacade = NULL;
		pMasterServerFacade = NULL;
		pPostOfficeSystem = NULL;
		pTradeMarketService = NULL;
		pTradeMarketServiceConsumer = NULL;
	}

	__GLOBAL_CHANGE(PServer,						pServer);
	__GLOBAL_CHANGE(PSystem,						pSystem);
	__GLOBAL_CHANGE(PCommandCenter, 				pCommandCenter);
	__GLOBAL_CHANGE(PDBManager,						pDBManager);
	__GLOBAL_CHANGE(PLoginLogic,					pLoginLogic);
	__GLOBAL_CHANGE(PMsgCommandFacade,				pMsgCommandFacade);
	__GLOBAL_CHANGE(PMasterServerFacade,			pMasterServerFacade);
	__GLOBAL_CHANGE(PPostOfficeSystem,				pPostOfficeSystem);
	__GLOBAL_CHANGE(PTradeMarketService,			pTradeMarketService);
	__GLOBAL_CHANGE(PTradeMarketServiceConsumer,	pTradeMarketServiceConsumer);
};


class PGameServerObjectManager;
class PChatChannelManager;
class PFieldInfoManager;

class PGlobalManager
{
public:
	PGameServerObjectManager*		pGameServerObjectManager;	
	PChatChannelManager*			pChatChannelManager;
	PFieldInfoManager*				pFieldInfoManager;
	
	PGlobalManager()
	{
		SetNull();
	}
	virtual ~PGlobalManager() {}
		

	void SetNull()
	{
		pGameServerObjectManager = NULL;		
		pChatChannelManager = NULL;
		pFieldInfoManager = NULL;
	}

	__GLOBAL_CHANGE(PGameServerObjectManager,		pGameServerObjectManager);	
	__GLOBAL_CHANGE(PChatChannelManager,			pChatChannelManager);
	__GLOBAL_CHANGE(PFieldInfoManager,				pFieldInfoManager);
};
		

extern PGlobalSystem gsys;
extern PGlobalManager gmgr;

