#include "StdAfx.h"
#include "GPlayerObjectManager.h"
#include "GDBManager.h"
#include "GScriptManager.h"
#include "GItemManager.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "GFieldMgr.h"
#include "GTalentInfo.h"
#include "GPlayerDB.h"
#include "GQuestInfoMgr.h"
#include "GFieldInfo.h"

#include "GPartySystem.h"
#include "GTradeSystem.h"
#include "GTradeLogic.h"
#include "GNPCTradeSystem.h"
#include "GItemSystem.h"
#include "GQuestSystem.h"
#include "GTrainingSystem.h"
#include "GDialogSystem.h"
#include "GChainSystem.h"
#include "GFieldSystem.h"
#include "GLootSystem.h"

//GMgrAndSys gMgrAndSys;

GMgrAndSys::GMgrAndSys()
{
	Clear();
}

GMgrAndSys::~GMgrAndSys()
{
}

void GMgrAndSys::Clear()
{
	m_pDBMgr = NULL;
	m_pPlayerObjectMgr = NULL;	
	m_pScriptMgr = NULL;
	m_pItemMgr = NULL;
	m_pNPCInfoMgr = NULL;
	m_pNPCMgr = NULL;
	m_pFieldMgr = NULL;
	m_pTalentInfoMgr = NULL;	
	m_pQuestInfoMgr = NULL;
	m_pFieldInfoMgr = NULL;
	m_pEnvMgr = NULL;	
	m_pPartySys = NULL;

	m_pPlayerDB = NULL;
	m_pTradeSys = NULL;
	m_pTradeLogic = NULL;
	m_pNPCTradeSys = NULL;
	m_pItemSys = NULL;
	m_pQuestSys = NULL;
	m_pTrainingSys = NULL;
	m_pDialogSys = NULL;
	m_pChainSys = NULL;
	m_pFieldSys = NULL;
	m_pLootSys = NULL;
}

bool GMgrAndSys::IsValid()
{
	if (NULL == m_pDBMgr) return false;
	if (NULL == m_pPlayerObjectMgr) return false;	
	if (NULL == m_pScriptMgr) return false;
	if (NULL == m_pItemMgr) return false;
	if (NULL == m_pNPCInfoMgr) return false;
	if (NULL == m_pNPCMgr) return false;
	if (NULL == m_pFieldMgr) return false;
	if (NULL == m_pTalentInfoMgr) return false;	
	if (NULL == m_pQuestInfoMgr) return false;
	if (NULL == m_pFieldInfoMgr) return false;
	if (NULL == m_pEnvMgr) return false;	
	if (NULL == m_pPartySys) return false;
	

	if (NULL == m_pPlayerDB) return false;
	if (NULL == m_pTradeSys) return false;
	if (NULL == m_pTradeLogic) return false;
	if (NULL == m_pNPCTradeSys) return false;
	if (NULL == m_pItemSys) return false;
	if (NULL == m_pQuestSys) return false;
	if (NULL == m_pTrainingSys) return false;
	if (NULL == m_pDialogSys) return false;
	if (NULL == m_pChainSys) return false;
	if (NULL == m_pFieldSys) return false;
	if (NULL == m_pLootSys) return false;

	return true;
}