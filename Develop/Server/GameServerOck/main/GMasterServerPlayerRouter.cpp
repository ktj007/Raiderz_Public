#include "stdafx.h"
#include "GMasterServerPlayerRouter.h"
#include "GCommand.h"
#include "CCommandTable_Master.h"
#include "GConfig.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"


GMasterServerPlayerRouter::GMasterServerPlayerRouter()
{
	// do nothing
}

GMasterServerPlayerRouter::~GMasterServerPlayerRouter()
{
	// do nothing
}

void GMasterServerPlayerRouter::RouteDeletePlayer(int nAID, const MUID& uidPlayer)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PLAYER_DELETE_PLAYER,
		2,
		NEW_INT(nAID),
		NEW_UID(uidPlayer)
		);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerPlayerRouter::RoutePlayerState_InWorldReq(const GEntityPlayer* pEntityPlayer)
{
	_ASSERT(pEntityPlayer != NULL);

	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(pEntityPlayer->GetUID());
	if (NULL == pPlayerObject) return;

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PLAYER_STATE_IN_WORLD_REQ,
		5,
		NEW_INT((int)pPlayerObject->GetAccountInfo().nAID),
		NEW_INT(pEntityPlayer->GetCID()),
		NEW_UID(pEntityPlayer->GetUID()),
		NEW_WSTR(pEntityPlayer->GetName()),
		NEW_INT(pEntityPlayer->GetGID())
		);

	gsys.pMasterServerFacade->Route(pNewCmd);
}
void GMasterServerPlayerRouter::RoutePlayerInFieldNotify(const GEntityPlayer* pEntityPlayer, MUID uidField, int nFieldID)
{
	_ASSERT(pEntityPlayer != NULL);

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_PLAYER_IN_FIELD_NOTIFY,
																	3,
																	NEW_INT(pEntityPlayer->GetCID()),
																	NEW_UID(uidField),
																	NEW_INT(nFieldID)
																	);

	gsys.pMasterServerFacade->Route(pNewCmd);
}