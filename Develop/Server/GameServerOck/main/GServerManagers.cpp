#include "stdafx.h"
#include "GDef.h"
#include "GServerManagers.h"
#include "GBattleArenaMgr.h"
#include "GPlayerObjectManager.h"
#include "GScriptManager.h"
#include "GItemManager.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "GFieldMgr.h"
#include "GTalentInfo.h"
#include "GBuffInfo.h"
#include "GQuestInfoMgr.h"
#include "GFieldInfo.h"
#include "GEnvManager.h"
#include "GSoulBindingInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GItemConditionsInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GNPCShopInfoMgr.h"
#include "GLootInfoMgr.h"
#include "GRecipeInfoMgr.h"
#include "GCraftInfoMgr.h"
#include "GInnRoomInfoMgr.h"
#include "GGuildMgr.h"
#include "GDuelMgr.h"
#include "GTalentInfoMgr.h"
#include "GFieldInfoMgr.h"
#include "GAIEventMgr.h"
#include "GFactionInfoMgr.h"
#include "GFactionRelationInfoMgr.h"
#include "GCutsceneInfoMgr.h"
#include "GEmblemInfoMgr.h"
#include "GChallengerQuestMgr.h"
#include "GReserveCmdManager.h"
#include "GPlayerDBCacheMgr.h"
#include "GAIRoomMgr.h"
#include "GQPEventInfoMgr.h"
#include "GPresetInfoMgr.h"
#include "GExportDBStringTable.h"

GServerManagers::GServerManagers() : GGlobalManager()
{
}

GServerManagers::~GServerManagers()
{

}

void GServerManagers::Create()
{
	pPlayerObjectManager	= new GPlayerObjectManager();
	pItemManager			= new GItemManager();
	pNPCInfoMgr				= new GNPCInfoMgr();
	pNPCMgr					= new GNPCMgr();
	pFieldMgr				= new GFieldMgr();
	pTalentInfoMgr			= new GTalentInfoMgr();
	pBuffInfoMgr			= new GBuffInfoMgr();
	pQuestInfoMgr			= new GQuestInfoMgr();
	pFieldInfoMgr			= new GFieldInfoMgr();
	pEnvManager				= new GEnvManager();	
	pSoulBindingInfoMgr		= new GSoulBindingInfoMgr();
	
	pCondtionsInfoMgr		= new GConditionsInfoMgr();
	pItemConditionsInfoMgr	= new GItemConditionsInfoMgr();

	pDialogInfoMgr			= new GDialogInfoMgr();
	pNPCShopInfoMgr			= new GNPCShopInfoMgr();
	pLootInfoMgr			= new GLootInfoMgr();
	pRecipeInfoMgr			= new GRecipeInfoMgr();
	pCraftInfoMgr			= new GCraftInfoMgr();
	pInnRoomInfoMgr			= new GInnRoomInfoMgr();
	pGuildMgr				= new GGuildMgr();
	pBattleArenaMgr			= new GBattleArenaMgr();
	pAIEventMgr				= new GAIEventMgr();
	pFactionInfoMgr			= new GFactionInfoMgr();
	pFactionRelationInfoMgr	= new GFactionRelationInfoMgr();
	pCutsceneInfoMgr		= new GCutsceneInfoMgr();
	pEmblemInfoMgr			= new GEmblemInfoMgr();
	pChallengerQuestMgr		= new GChallengerQuestMgr();
	pReserveCmdManager		= new GReserveCmdManager();
	pPlayerDBCacheMgr		= new GPlayerDBCacheMgr();
	pAIRoomMgr				= new GAIRoomMgr();
	pSharedMemMUIDPool		= new GVectorMUIDPool(SHARED_MEM_MUID_POOL_SIZE, SHARED_MEM_MUID_VECOTR_SIZE);
	pQPEventInfoMgr			= new GQPEventInfoMgr();
	pPresetInfoMgr			= new GPresetInfoMgr();
	pExportDBStringTable	= new GExportDBStringTable();

	gmgr.pPlayerObjectManager	= pPlayerObjectManager;
	gmgr.pItemManager			= pItemManager;
	gmgr.pNPCInfoMgr			= pNPCInfoMgr;
	gmgr.pNPCMgr				= pNPCMgr;
	gmgr.pFieldMgr				= pFieldMgr;
	gmgr.pTalentInfoMgr			= pTalentInfoMgr;
	gmgr.pBuffInfoMgr			= pBuffInfoMgr;
	gmgr.pQuestInfoMgr			= pQuestInfoMgr;
	gmgr.pFieldInfoMgr			= pFieldInfoMgr;
	gmgr.pEnvManager			= pEnvManager;
	gmgr.pSoulBindingInfoMgr	= pSoulBindingInfoMgr;
	
	gmgr.pCondtionsInfoMgr		= pCondtionsInfoMgr;
	gmgr.pItemConditionsInfoMgr	= pItemConditionsInfoMgr;

	gmgr.pDialogInfoMgr			= pDialogInfoMgr;
	gmgr.pNPCShopInfoMgr		= pNPCShopInfoMgr;
	gmgr.pLootInfoMgr			= pLootInfoMgr;
	gmgr.pRecipeInfoMgr			= pRecipeInfoMgr;
	gmgr.pCraftInfoMgr			= pCraftInfoMgr;
	gmgr.pInnRoomInfoMgr		= pInnRoomInfoMgr;
	gmgr.pGuildMgr				= pGuildMgr;
	gmgr.pBattleArenaMgr		= pBattleArenaMgr;
	gmgr.pAIEventMgr			= pAIEventMgr;
	gmgr.pFactionInfoMgr		= pFactionInfoMgr;
	gmgr.pFactionRelationInfoMgr= pFactionRelationInfoMgr;
	gmgr.pCutsceneInfoMgr		= pCutsceneInfoMgr;
	gmgr.pEmblemInfoMgr			= pEmblemInfoMgr;
	gmgr.pChallengerQuestMgr	= pChallengerQuestMgr;
	gmgr.pReserveCmdManager		= pReserveCmdManager;
	gmgr.pPlayerDBCacheMgr		= pPlayerDBCacheMgr;
	gmgr.pAIRoomMgr				= pAIRoomMgr;
	gmgr.pSharedMemMUIDPool		= pSharedMemMUIDPool;
	gmgr.pQPEventInfoMgr		= pQPEventInfoMgr;
	gmgr.pPresetInfoMgr			= pPresetInfoMgr;
	gmgr.pExportDBStringTable	= pExportDBStringTable;

}

void GServerManagers::Destroy()
{
	SAFE_DELETE(pExportDBStringTable);			gmgr.pExportDBStringTable = NULL;
	SAFE_DELETE(pSharedMemMUIDPool);			gmgr.pSharedMemMUIDPool = NULL;
	SAFE_DELETE(pPlayerObjectManager);			gmgr.pPlayerObjectManager = NULL;
	SAFE_DELETE(pFieldMgr);						gmgr.pFieldMgr = NULL;
	SAFE_DELETE(pNPCShopInfoMgr);				gmgr.pNPCShopInfoMgr = NULL;
	SAFE_DELETE(pDialogInfoMgr);				gmgr.pDialogInfoMgr = NULL;
	SAFE_DELETE(pCondtionsInfoMgr);				gmgr.pCondtionsInfoMgr = NULL;
	SAFE_DELETE(pItemConditionsInfoMgr);		gmgr.pItemConditionsInfoMgr = NULL;
	SAFE_DELETE(pSoulBindingInfoMgr);			gmgr.pSoulBindingInfoMgr = NULL;
	SAFE_DELETE(pEnvManager);					gmgr.pEnvManager = NULL;
	SAFE_DELETE(pFieldInfoMgr);					gmgr.pFieldInfoMgr = NULL;
	SAFE_DELETE(pQuestInfoMgr);					gmgr.pQuestInfoMgr = NULL;
	SAFE_DELETE(pTalentInfoMgr);				gmgr.pTalentInfoMgr = NULL;
	SAFE_DELETE(pBuffInfoMgr);					gmgr.pBuffInfoMgr = NULL;
	SAFE_DELETE(pNPCMgr);						gmgr.pNPCMgr = NULL;
	SAFE_DELETE(pNPCInfoMgr);					gmgr.pNPCInfoMgr = NULL;
	SAFE_DELETE(pItemManager);					gmgr.pItemManager = NULL;
	SAFE_DELETE(pLootInfoMgr);					gmgr.pLootInfoMgr = NULL;
	SAFE_DELETE(pRecipeInfoMgr);				gmgr.pRecipeInfoMgr = NULL;
	SAFE_DELETE(pCraftInfoMgr);					gmgr.pCraftInfoMgr = NULL;
	SAFE_DELETE(pInnRoomInfoMgr);				gmgr.pInnRoomInfoMgr = NULL;
	SAFE_DELETE(pGuildMgr);						gmgr.pGuildMgr = NULL;
	SAFE_DELETE(pBattleArenaMgr);				gmgr.pBattleArenaMgr = NULL;
	SAFE_DELETE(pAIEventMgr);					gmgr.pAIEventMgr = NULL;
	SAFE_DELETE(pFactionInfoMgr);				gmgr.pFactionInfoMgr = NULL;
	SAFE_DELETE(pFactionRelationInfoMgr);		gmgr.pFactionRelationInfoMgr = NULL;
	SAFE_DELETE(pCutsceneInfoMgr);				gmgr.pCutsceneInfoMgr = NULL;
	SAFE_DELETE(pEmblemInfoMgr);				gmgr.pEmblemInfoMgr = NULL;
	SAFE_DELETE(pChallengerQuestMgr);			gmgr.pChallengerQuestMgr = NULL;
	SAFE_DELETE(pReserveCmdManager);			gmgr.pReserveCmdManager = NULL;
	SAFE_DELETE(pPlayerDBCacheMgr);				gmgr.pPlayerDBCacheMgr = NULL;
	SAFE_DELETE(pAIRoomMgr);					gmgr.pAIRoomMgr = NULL;
	SAFE_DELETE(pQPEventInfoMgr);				gmgr.pQPEventInfoMgr = NULL;
	SAFE_DELETE(pPresetInfoMgr);				gmgr.pPresetInfoMgr = NULL;
	
	gmgr.SetNull();
}
