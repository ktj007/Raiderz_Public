#include "stdafx.h"
#include "LSelMyCharTimeoutHandler.h"
#include "LGlobal.h"
#include "LCommandCenter.h"
#include "SCmdRouter_Login.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"


void LSelMyCharTimeoutHandler::OnTimeout(MUID uidPlayer)
{
	LPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPlayer) return;

	pPlayer->OnSelectChar();

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseSelMyChar(uidPlayer, CR_FAIL_CHAR_TIMEOUT);
}
