#include "stdafx.h"
#include "GServerSystems.h"
#include "GCommonSystem.h"
#include "GDBManager.h"
#include "GPartySystem.h"
#include "GTradeSystem.h"
#include "GNPCShopSystem.h"
#include "GItemSystem.h"
#include "GQuestSystem.h"
#include "GTrainingSystem.h"
#include "GDialogSystem.h"
#include "GFieldSystem.h"
#include "GLootSystem.h"
#include "GPlayerSystem.h"
#include "GInteractionSystem.h"
#include "GConditionsSystem.h"
#include "GCombatCalculator.h"
#include "GAttackDamageCalculator.h"
#include "GTalentFactory.h"
#include "GValidateLogger.h"
#include "GServer.h"
#include "GWorld.h"
#include "GFatigueSystem.h"
#include "GCraftSystem.h"
#include "GInnSystem.h"
#include "GPathFinder.h"
#include "GGuildSystem.h"
#include "GPaletteSystem.h"
#include "GMailSystem.h"
#include "GSoulSystem.h"
#include "GTDMaker.h"
#include "GSystem.h"
#include "GTestSystem.h"
#include "GFactionSystem.h"
#include "GMsgCommandFacade.h"
#include "GPickingZTable.h"
#include "GExpSystem.h"
#include "GMoneySystem.h"
#include "GPMSSystem.h"
#include "GRebirthSystem.h"
#include "GDBCacheSystem.h"
#include "GStorageSystem.h"
#include "GFakeDBManager.h"
#include "GConfig.h"
#include "GGameGuard.h"
#include "GConst.h"
#include "GServerDumper.h"



GServerSystems::GServerSystems() : GGlobalSystem()
{
}

GServerSystems::~GServerSystems()
{

}

void GServerSystems::Create()
{
	pServer				= new GServer();
	pCommonSystem		= new GCommonSystem();
	pPartySystem		= new GPartySystem();
	pTradeSystem		= new GTradeSystem();
	pNPCShopSystem		= new GNPCShopSystem();
	pItemSystem			= new GItemSystem();
	pQuestSystem		= new GQuestSystem();
	pTrainingSystem		= new GTrainingSystem();
	pDialogSystem		= new GDialogSystem();
	pFieldSystem		= new GFieldSystem();
	pLootSystem			= new GLootSystem();

	if (GConfig::m_bNoDB)
	{
		pDBManager			= new GFakeDBManager();
		mlog("Fake DB Manager Created\n");
	}
	else
	{
		pDBManager			= new GDBManager();
	}

	pScriptManager		= new GScriptManager();
	pPathFinder			= new GPathFinder();
	pPlayerSystem		= new GPlayerSystem;
	pInteractionSystem	= new GInteractionSystem();
	pConditionsSystem	= new GConditionsSystem();
	pCombatCalculator	= new GCombatCalculator();
	pAttackDamageCalculator	= new GAttackDamageCalculator();
	pTalentFactory		= new GTalentFactory();
	pValidateLogger		= new GValidateLogger();
	pSystem				= new GSystem();
	pWorld				= new GWorld();
	pFatigueSystem		= new GFatigueSystem();
	pCraftSystem		= new GCraftSystem();
	pInnsystem			= new GInnSystem();
	pGuildSystem		= new GGuildSystem();
	pPaletteSystem		= new GPaletteSystem();
	pMailSystem			= new GMailSystem();
	pSoulSystem			= new GSoulSystem();
	pTestSystem			= new GTestSystem();
	pFactionSystem		= new GFactionSystem;
	pMsgCommandFacade	= new GMsgCommandFacade();
	pExpSystem			= new GExpSystem;
	pMoneySystem		= new GMoneySystem;
	pPMSSystem			= new GPMSSystem();
	pRebirthSystem		= new GRebirthSystem;
	pDBCacheSystem		= new GDBCacheSystem;
	pStorageSystem		= new GStorageSystem;
	pGameGuard			= new GGameGuard;
	pServerDumper		= new GServerDumper;

	gsys.pServer			= pServer;
	gsys.pCommonSystem		= pCommonSystem;
	gsys.pPartySystem		= pPartySystem;
	gsys.pTradeSystem		= pTradeSystem;
	gsys.pNPCShopSystem	= pNPCShopSystem;
	gsys.pItemSystem		= pItemSystem;
	gsys.pQuestSystem		= pQuestSystem;
	gsys.pTrainingSystem	= pTrainingSystem;
	gsys.pDialogSystem		= pDialogSystem;
	gsys.pFieldSystem		= pFieldSystem;
	gsys.pLootSystem		= pLootSystem;
	gsys.pDBManager			= pDBManager;
	gsys.pScriptManager		= pScriptManager;
	gsys.pPlayerSystem		= pPlayerSystem;
	gsys.pInteractionSystem	= pInteractionSystem;
	gsys.pConditionsSystem	= pConditionsSystem;
	gsys.pCombatCalculator	= pCombatCalculator;
	gsys.pAttackDamageCalculator	= pAttackDamageCalculator;
	gsys.pTalentFactory		= pTalentFactory;
	gsys.pValidateLogger	= pValidateLogger;
	gsys.pSystem			= pSystem;
	gsys.pWorld				= pWorld;
	gsys.pFatigueSystem		= pFatigueSystem;
	gsys.pCraftSystem		= pCraftSystem;
	gsys.pInnsystem			= pInnsystem;
	gsys.pPathFinder		= pPathFinder;
	gsys.pGuildSystem		= pGuildSystem;
	gsys.pPaletteSystem		= pPaletteSystem;
	gsys.pMailSystem		= pMailSystem;
	gsys.pSoulSystem		= pSoulSystem;	
	gsys.pTestSystem		= pTestSystem;
	gsys.pFactionSystem		= pFactionSystem;
	gsys.pMsgCommandFacade	= pMsgCommandFacade;
	gsys.pExpSystem			= pExpSystem;
	gsys.pMoneySystem		= pMoneySystem;
	gsys.pPMSSystem			= pPMSSystem;
	gsys.pRebirthSystem		= pRebirthSystem;
	gsys.pDBCacheSystem		= pDBCacheSystem;
	gsys.pStorageSystem		= pStorageSystem;
	gsys.pGameGuard			= pGameGuard;
	gsys.pServerDumper		= pServerDumper;
}

void GServerSystems::Destroy()
{
	SAFE_DELETE(pWorld);					gsys.pWorld = NULL;
	SAFE_DELETE(pServer);					gsys.pServer = NULL;
	SAFE_DELETE(pConditionsSystem);			gsys.pConditionsSystem = NULL;
	SAFE_DELETE(pInteractionSystem);		gsys.pInteractionSystem = NULL;
	SAFE_DELETE(pPlayerSystem);				gsys.pPlayerSystem = NULL;
	SAFE_DELETE(pPartySystem);				gsys.pPartySystem = NULL;
	SAFE_DELETE(pTradeSystem);				gsys.pTradeSystem = NULL;
	SAFE_DELETE(pNPCShopSystem);			gsys.pNPCShopSystem = NULL;
	SAFE_DELETE(pItemSystem);				gsys.pItemSystem = NULL;
	SAFE_DELETE(pQuestSystem);				gsys.pQuestSystem = NULL;
	SAFE_DELETE(pTrainingSystem);			gsys.pTrainingSystem = NULL;
	SAFE_DELETE(pDialogSystem);				gsys.pDialogSystem = NULL;
	SAFE_DELETE(pFieldSystem);				gsys.pFieldSystem = NULL;
	SAFE_DELETE(pLootSystem);				gsys.pLootSystem = NULL;
	SAFE_DELETE(pDBManager);				gsys.pDBManager = NULL;
	SAFE_DELETE(pScriptManager);			gsys.pScriptManager = NULL;
	SAFE_DELETE(pCommonSystem);				gsys.pCommonSystem = NULL;
	SAFE_DELETE(pCombatCalculator);			gsys.pCombatCalculator = NULL;
	SAFE_DELETE(pAttackDamageCalculator);	gsys.pAttackDamageCalculator = NULL;
	SAFE_DELETE(pTalentFactory);			gsys.pTalentFactory = NULL;
	SAFE_DELETE(pValidateLogger);			gsys.pValidateLogger = NULL;
	SAFE_DELETE(pSystem);					gsys.pSystem = NULL;
	SAFE_DELETE(pFatigueSystem);			gsys.pFatigueSystem = NULL;
	SAFE_DELETE(pCraftSystem);				gsys.pCraftSystem = NULL;
	SAFE_DELETE(pInnsystem);				gsys.pInnsystem = NULL;
	SAFE_DELETE(pPathFinder);				gsys.pPathFinder = NULL;
	SAFE_DELETE(pGuildSystem);				gsys.pGuildSystem = NULL;
	SAFE_DELETE(pPaletteSystem);			gsys.pPaletteSystem = NULL;
	SAFE_DELETE(pMailSystem);				gsys.pMailSystem = NULL;
	SAFE_DELETE(pSoulSystem);				gsys.pSoulSystem = NULL;
	SAFE_DELETE(pTestSystem);				gsys.pTestSystem = NULL;
	SAFE_DELETE(pFactionSystem);			gsys.pFactionSystem = NULL;
	SAFE_DELETE(pMsgCommandFacade);			gsys.pMsgCommandFacade= NULL;
	SAFE_DELETE(pExpSystem);				gsys.pExpSystem= NULL;
	SAFE_DELETE(pMoneySystem);				gsys.pMoneySystem = NULL;
	SAFE_DELETE(pPMSSystem);				gsys.pPMSSystem = NULL;
	SAFE_DELETE(pRebirthSystem);			gsys.pRebirthSystem = NULL;
	SAFE_DELETE(pDBCacheSystem);			gsys.pDBCacheSystem = NULL;
	SAFE_DELETE(pStorageSystem);			gsys.pStorageSystem = NULL;
	SAFE_DELETE(pGameGuard);				gsys.pGameGuard = NULL;
	SAFE_DELETE(pServerDumper);				gsys.pServerDumper = NULL;
			
	gsys.SetNull();
}

