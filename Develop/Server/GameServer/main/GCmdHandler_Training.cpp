#include "StdAfx.h"
#include "GCmdHandler_Training.h"
#include "GCommandTable.h"
#include "GTrainingSystem.h"
#include "GInteractionSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GPlayerInteraction.h"
#include "GGlobal.h"
#include "GNPCInteractor.h"

GCmdHandler_Training::GCmdHandler_Training(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_TRAINING_TRAIN_REQ,			OnRequestLearnTalent);
}

MCommandResult GCmdHandler_Training::OnRequestLearnTalent(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nTalentID;
	if (pCmd->GetParameter(&nTalentID,	0, MPT_INT)==false) return CR_FALSE;
	
	if (false == gsys.pTrainingSystem->Train(pPlayer, nTalentID))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);		
	}

	return CR_TRUE;
}
