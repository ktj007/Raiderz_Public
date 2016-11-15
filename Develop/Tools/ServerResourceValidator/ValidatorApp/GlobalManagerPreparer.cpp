#include "stdafx.h"
#include "GlobalManagerPreparer.h"
#include "GGlobal.h"
#include "GValidateLogger.h"
#include "GItemManager.h"
#include "GNPCInfoMgr.h"
#include "GTalentInfo.h"
#include "GQuestInfoMgr.h"
#include "GSoulBindingInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GItemConditionsInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GTradeInfoMgr.h"
#include "GLootInfoMgr.h"
#include "GTalentInfoMgr.h"

GlobalManagerPreparer::GlobalManagerPreparer()
{
	// do nothing
}

GlobalManagerPreparer::~GlobalManagerPreparer()
{
	SAFE_DELETE(gsys.pValidateLogger);

	SAFE_DELETE(gmgr.pNPCInfoMgr);
	SAFE_DELETE(gmgr.pTalentInfoMgr);
	SAFE_DELETE(gmgr.pCondtionsInfoMgr);
	SAFE_DELETE(gmgr.pLootInfoMgr);
	SAFE_DELETE(gmgr.pQuestInfoMgr);
	SAFE_DELETE(gmgr.pDialogInfoMgr);
	SAFE_DELETE(gmgr.pTradeInfoMgr);
	SAFE_DELETE(gmgr.pSoulBindingInfoMgr);
	SAFE_DELETE(gmgr.pItemManager);
	SAFE_DELETE(gmgr.pItemConditionsInfoMgr);
}

bool GlobalManagerPreparer::Prepare(void)
{
	gsys.pValidateLogger = new GValidateLogger;

	gmgr.pNPCInfoMgr = new GNPCInfoMgr;
	gmgr.pTalentInfoMgr = new GTalentInfoMgr;	
	gmgr.pCondtionsInfoMgr = new GConditionsInfoMgr;
	gmgr.pLootInfoMgr = new GLootInfoMgr;
	gmgr.pQuestInfoMgr = new GQuestInfoMgr;
	gmgr.pDialogInfoMgr = new GDialogInfoMgr;
	gmgr.pTradeInfoMgr = new GTradeInfoMgr;
	gmgr.pSoulBindingInfoMgr = new GSoulBindingInfoMgr;
	gmgr.pItemManager = new GItemManager;
	gmgr.pItemConditionsInfoMgr = new GItemConditionsInfoMgr;

	return true;
}
