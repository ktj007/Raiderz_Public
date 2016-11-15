#pragma once

class GServer;
class GCommonSystem;
class GPartySystem;
class GTradeSystem;
class GNPCShopSystem;
class GItemSystem;
class GQuestSystem;
class GTrainingSystem;
class GDialogSystem;
class GFieldSystem;
class GLootSystem;
class GPlayerSystem;
class GInteractionSystem;
class GConditionsSystem;
class GCombatCalculator;
class GAttackDamageCalculator;
class GTalentFactory;
class GValidateLogger;
class GSystem;
class GCommandCenter;
class GWorld;
class GFatigueSystem;
class GCraftSystem;
class GInnSystem;
class GPathFinder;
class GGuildSystem;
class GPaletteSystem;
class GMailSystem;
class GDBManager;
class GScriptManager;
class GSoulSystem;
class GTestSystem;
class GMasterServerFacade;
class GAppServerFacade;
class GFactionSystem;
class GMsgCommandFacade;
class GExpSystem;
class GMoneySystem;
class GPMSSystem;
class GRebirthSystem;
class GDBCacheSystem;
class GStorageSystem;
class GGameGuard;
class GServerDumper;

#define __GLOBAL_CHANGE(Type, Value)	Type* Change(Type* a) { Type* pPrev = Value; Value = a; return pPrev; }

class GGlobalSystem
{
public:	
	GServer*			pServer;
	GCommonSystem*		pCommonSystem;
	GPartySystem*		pPartySystem;
	GTradeSystem*		pTradeSystem;
	GNPCShopSystem*		pNPCShopSystem;
	GItemSystem*		pItemSystem;
	GQuestSystem*		pQuestSystem;
	GTrainingSystem*	pTrainingSystem;
	GDialogSystem*		pDialogSystem;
	GFieldSystem*		pFieldSystem;
	GLootSystem*		pLootSystem;
	GDBManager*			pDBManager;
	GScriptManager*		pScriptManager;
	GPlayerSystem*		pPlayerSystem;
	GInteractionSystem* pInteractionSystem;
	GConditionsSystem*	pConditionsSystem;
	GCombatCalculator*	pCombatCalculator;
	GAttackDamageCalculator*	pAttackDamageCalculator;
	GTalentFactory*		pTalentFactory;	
	GValidateLogger*	pValidateLogger;
	GSystem*			pSystem;
	GCommandCenter*		pCommandCenter;
	GWorld*				pWorld;
	GFatigueSystem*		pFatigueSystem;
	GCraftSystem*		pCraftSystem;
	GInnSystem*			pInnsystem;
	GPathFinder*		pPathFinder;
	GGuildSystem*		pGuildSystem;
	GPaletteSystem*		pPaletteSystem;
	GMailSystem*		pMailSystem;
	GSoulSystem*		pSoulSystem;
	GTestSystem*		pTestSystem;
	GMasterServerFacade*	pMasterServerFacade;
	GAppServerFacade*	pAppServerFacade;
	GFactionSystem*		pFactionSystem;
	GMsgCommandFacade*	pMsgCommandFacade;
	GExpSystem*			pExpSystem;
	GMoneySystem*		pMoneySystem;
	GPMSSystem*			pPMSSystem;
	GRebirthSystem*		pRebirthSystem;
	GDBCacheSystem*		pDBCacheSystem;
	GStorageSystem*		pStorageSystem;
	GGameGuard*			pGameGuard;	
	GServerDumper*		pServerDumper;
		
	GGlobalSystem()
	{
		SetNull();
	}
	virtual ~GGlobalSystem() {}

	void SetNull()
	{
		pServer = NULL;
		pCommonSystem = NULL;
		pPartySystem = NULL;
		pTradeSystem = NULL;
		pNPCShopSystem = NULL;
		pItemSystem = NULL;
		pQuestSystem = NULL;
		pTrainingSystem = NULL;
		pDialogSystem = NULL;
		pFieldSystem = NULL;
		pLootSystem = NULL;
		pDBManager = NULL;
		pScriptManager = NULL;
		pPlayerSystem = NULL;
		pInteractionSystem = NULL;
		pConditionsSystem = NULL;
		pCombatCalculator = NULL;
		pAttackDamageCalculator = NULL;
		pTalentFactory = NULL;
		pValidateLogger = NULL;
		pSystem = NULL;
		pCommandCenter = NULL;
		pWorld = NULL;
		pFatigueSystem = NULL;
		pCraftSystem = NULL;
		pInnsystem = NULL;
		pPathFinder = NULL;
		pGuildSystem = NULL;
		pPaletteSystem = NULL;
		pMailSystem = NULL;
		pSoulSystem = NULL;
		pTestSystem = NULL;
		pMasterServerFacade = NULL;
		pFactionSystem = NULL;
		pMsgCommandFacade = NULL;
		pExpSystem = NULL;
		pPMSSystem = NULL;
		pMoneySystem = NULL;
		pRebirthSystem = NULL;
		pDBCacheSystem = NULL;
		pGameGuard = NULL;
		pServerDumper = NULL;
	}

	__GLOBAL_CHANGE(GServer, pServer);
	__GLOBAL_CHANGE(GCommonSystem, pCommonSystem);
	__GLOBAL_CHANGE(GPartySystem, pPartySystem);
	__GLOBAL_CHANGE(GTradeSystem, pTradeSystem);
	__GLOBAL_CHANGE(GNPCShopSystem, pNPCShopSystem);
	__GLOBAL_CHANGE(GItemSystem, pItemSystem);
	__GLOBAL_CHANGE(GQuestSystem, pQuestSystem);
	__GLOBAL_CHANGE(GTrainingSystem, pTrainingSystem);
	__GLOBAL_CHANGE(GDialogSystem, pDialogSystem);
	__GLOBAL_CHANGE(GFieldSystem, pFieldSystem);
	__GLOBAL_CHANGE(GLootSystem, pLootSystem);
	__GLOBAL_CHANGE(GDBManager, pDBManager);
	__GLOBAL_CHANGE(GScriptManager, pScriptManager);
	__GLOBAL_CHANGE(GPlayerSystem, pPlayerSystem);
	__GLOBAL_CHANGE(GInteractionSystem, pInteractionSystem);
	__GLOBAL_CHANGE(GConditionsSystem, pConditionsSystem);
	__GLOBAL_CHANGE(GCombatCalculator, pCombatCalculator);
	__GLOBAL_CHANGE(GAttackDamageCalculator, pAttackDamageCalculator);
	__GLOBAL_CHANGE(GTalentFactory, pTalentFactory);
	__GLOBAL_CHANGE(GValidateLogger, pValidateLogger);
	__GLOBAL_CHANGE(GSystem, pSystem);
	__GLOBAL_CHANGE(GCommandCenter, pCommandCenter);
	__GLOBAL_CHANGE(GWorld, pWorld);
	__GLOBAL_CHANGE(GFatigueSystem, pFatigueSystem);
	__GLOBAL_CHANGE(GCraftSystem, pCraftSystem);
	__GLOBAL_CHANGE(GInnSystem, pInnsystem);
	__GLOBAL_CHANGE(GPathFinder, pPathFinder);
	__GLOBAL_CHANGE(GGuildSystem, pGuildSystem);
	__GLOBAL_CHANGE(GPaletteSystem, pPaletteSystem);
	__GLOBAL_CHANGE(GMailSystem, pMailSystem);
	__GLOBAL_CHANGE(GSoulSystem, pSoulSystem);
	__GLOBAL_CHANGE(GTestSystem, pTestSystem);
	__GLOBAL_CHANGE(GMasterServerFacade, pMasterServerFacade);
	__GLOBAL_CHANGE(GAppServerFacade, pAppServerFacade);
	__GLOBAL_CHANGE(GFactionSystem, pFactionSystem);
	__GLOBAL_CHANGE(GMsgCommandFacade, pMsgCommandFacade);
	__GLOBAL_CHANGE(GExpSystem, pExpSystem);
	__GLOBAL_CHANGE(GMoneySystem, pMoneySystem);
	__GLOBAL_CHANGE(GPMSSystem, pPMSSystem);
	__GLOBAL_CHANGE(GRebirthSystem, pRebirthSystem);
	__GLOBAL_CHANGE(GDBCacheSystem, pDBCacheSystem);
	__GLOBAL_CHANGE(GStorageSystem, pStorageSystem);
	__GLOBAL_CHANGE(GGameGuard, pGameGuard);
	__GLOBAL_CHANGE(GServerDumper, pServerDumper);
	
};


class GPlayerObjectManager;
class GDBManager;
class GScriptManager;
class GItemManager;
class GNPCInfoMgr;
class GNPCMgr;
class GFieldMgr;
class GTalentInfoMgr;
class GQuestInfoMgr;
class GFieldInfoMgr;
class GEnvManager;
class GSoulBindingInfoMgr;
class GBuffInfoMgr;
class GConditionsInfoMgr;
class GItemConditionsInfoMgr;
class GDialogInfoMgr;
class GNPCShopInfoMgr;
class GTrainingInfoMgr;
class GLootInfoMgr;
class GRecipeInfoMgr;
class GCraftInfoMgr;
class GInnRoomInfoMgr;
class GGuildMgr;
class GBattleArenaMgr;
class GAIEventMgr;
class GFactionInfoMgr;
class GFactionRelationInfoMgr;
class GCutsceneInfoMgr;
class GEmblemInfoMgr;
class GChallengerQuestMgr;
class GReserveCmdManager;
class GPlayerDBCacheMgr;
class GAIRoomMgr;
class GVectorMUIDPool;
class GQPEventInfoMgr;
class GPresetInfoMgr;
class GExportDBStringTable;

class GGlobalManager
{
public:
	GPlayerObjectManager*	pPlayerObjectManager;
	GItemManager*			pItemManager;
	GNPCInfoMgr*			pNPCInfoMgr;
	GNPCMgr*				pNPCMgr;
	GFieldMgr*				pFieldMgr;
	GTalentInfoMgr*			pTalentInfoMgr;	
	GBuffInfoMgr*			pBuffInfoMgr;
	GQuestInfoMgr*			pQuestInfoMgr;
	GFieldInfoMgr*			pFieldInfoMgr;		
	GEnvManager*			pEnvManager;
	GSoulBindingInfoMgr*	pSoulBindingInfoMgr;
	GConditionsInfoMgr*		pCondtionsInfoMgr;
	GItemConditionsInfoMgr* pItemConditionsInfoMgr;
	GDialogInfoMgr*			pDialogInfoMgr;
	GNPCShopInfoMgr*		pNPCShopInfoMgr;
	GLootInfoMgr*			pLootInfoMgr;
	GRecipeInfoMgr*			pRecipeInfoMgr;
	GCraftInfoMgr*			pCraftInfoMgr;
	GInnRoomInfoMgr*		pInnRoomInfoMgr;
	GGuildMgr*				pGuildMgr;
	GBattleArenaMgr*		pBattleArenaMgr;
	GAIEventMgr*			pAIEventMgr;
	GFactionInfoMgr*		pFactionInfoMgr;
	GFactionRelationInfoMgr*pFactionRelationInfoMgr;
	GCutsceneInfoMgr*		pCutsceneInfoMgr;
	GEmblemInfoMgr*			pEmblemInfoMgr;
	GChallengerQuestMgr*	pChallengerQuestMgr;
	GReserveCmdManager*		pReserveCmdManager;
	GPlayerDBCacheMgr*		pPlayerDBCacheMgr;
	GAIRoomMgr*				pAIRoomMgr;
	GVectorMUIDPool*		pSharedMemMUIDPool;
	GQPEventInfoMgr*		pQPEventInfoMgr;
	GPresetInfoMgr*			pPresetInfoMgr;
	GExportDBStringTable*	pExportDBStringTable;

		
	GGlobalManager()
	{
		SetNull();
	}
	virtual ~GGlobalManager() {}
		

	void SetNull()
	{
		pPlayerObjectManager = NULL;
		pItemManager = NULL;
		pNPCInfoMgr = NULL;
		pNPCMgr = NULL;
		pFieldMgr = NULL;
		pTalentInfoMgr = NULL;
		pBuffInfoMgr = NULL;
		pQuestInfoMgr = NULL;
		pFieldInfoMgr = NULL;
		pEnvManager = NULL;
		pSoulBindingInfoMgr = NULL;
		pCondtionsInfoMgr = NULL;
		pItemConditionsInfoMgr = NULL;
		pDialogInfoMgr = NULL;
		pNPCShopInfoMgr = NULL;
		pLootInfoMgr = NULL;
		pRecipeInfoMgr = NULL;
		pCraftInfoMgr = NULL;
		pInnRoomInfoMgr = NULL;
		pGuildMgr = NULL;
		pBattleArenaMgr = NULL;
		pAIEventMgr = NULL;
		pFactionInfoMgr = NULL;
		pFactionRelationInfoMgr = NULL;
		pCutsceneInfoMgr = NULL;
		pEmblemInfoMgr = NULL;
		pChallengerQuestMgr = NULL;
		pReserveCmdManager = NULL;
		pPlayerDBCacheMgr = NULL;
		pAIRoomMgr = NULL;
		pSharedMemMUIDPool = NULL;
		pQPEventInfoMgr = NULL;
		pPresetInfoMgr = NULL;
		pExportDBStringTable = NULL;
	}

	__GLOBAL_CHANGE(GPlayerObjectManager, pPlayerObjectManager);
	__GLOBAL_CHANGE(GItemManager, pItemManager);
	__GLOBAL_CHANGE(GNPCInfoMgr, pNPCInfoMgr);
	__GLOBAL_CHANGE(GNPCMgr, pNPCMgr);
	__GLOBAL_CHANGE(GFieldMgr, pFieldMgr);
	__GLOBAL_CHANGE(GTalentInfoMgr, pTalentInfoMgr);
	__GLOBAL_CHANGE(GBuffInfoMgr, pBuffInfoMgr);
	__GLOBAL_CHANGE(GQuestInfoMgr, pQuestInfoMgr);
	__GLOBAL_CHANGE(GFieldInfoMgr, pFieldInfoMgr);
	__GLOBAL_CHANGE(GEnvManager, pEnvManager);
	__GLOBAL_CHANGE(GSoulBindingInfoMgr, pSoulBindingInfoMgr);
	__GLOBAL_CHANGE(GConditionsInfoMgr, pCondtionsInfoMgr);
	__GLOBAL_CHANGE(GItemConditionsInfoMgr, pItemConditionsInfoMgr);
	__GLOBAL_CHANGE(GDialogInfoMgr, pDialogInfoMgr);
	__GLOBAL_CHANGE(GNPCShopInfoMgr, pNPCShopInfoMgr);
	__GLOBAL_CHANGE(GLootInfoMgr, pLootInfoMgr);
	__GLOBAL_CHANGE(GRecipeInfoMgr, pRecipeInfoMgr);
	__GLOBAL_CHANGE(GCraftInfoMgr, pCraftInfoMgr);	
	__GLOBAL_CHANGE(GInnRoomInfoMgr, pInnRoomInfoMgr);
	__GLOBAL_CHANGE(GGuildMgr, pGuildMgr);
	__GLOBAL_CHANGE(GBattleArenaMgr, pBattleArenaMgr);
	__GLOBAL_CHANGE(GAIEventMgr, pAIEventMgr);
	__GLOBAL_CHANGE(GFactionInfoMgr, pFactionInfoMgr);
	__GLOBAL_CHANGE(GFactionRelationInfoMgr, pFactionRelationInfoMgr);
	__GLOBAL_CHANGE(GCutsceneInfoMgr, pCutsceneInfoMgr)
	__GLOBAL_CHANGE(GEmblemInfoMgr, pEmblemInfoMgr)
	__GLOBAL_CHANGE(GChallengerQuestMgr, pChallengerQuestMgr)
	__GLOBAL_CHANGE(GReserveCmdManager, pReserveCmdManager)
	__GLOBAL_CHANGE(GPlayerDBCacheMgr, pPlayerDBCacheMgr)	
	__GLOBAL_CHANGE(GVectorMUIDPool, pSharedMemMUIDPool)
	__GLOBAL_CHANGE(GQPEventInfoMgr, pQPEventInfoMgr)
	__GLOBAL_CHANGE(GPresetInfoMgr, pPresetInfoMgr)
	__GLOBAL_CHANGE(GExportDBStringTable, pExportDBStringTable);
};
		

extern GGlobalSystem gsys;
extern GGlobalManager gmgr;

inline GPartySystem*	GetPartySys()		{	return gsys.pPartySystem;	}
