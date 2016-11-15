#include "StdAfx.h"
#include "ZReceiveGameServerUIDSelector.h"
#include "ZPlayer.h"
#include "ZPlayerManager.h"
#include "ZGameServerObjectManager.h"
#include "AStlUtil.h"

void ZReceiveGameServerUIDSelector::Select(const vector<int>& vecCID, MAP_RECEIEVE_GAMESERVER_UID& outmapReceiveServerUID)
{
	for each (int nCID in vecCID)
	{
		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByCID(nCID);
		if (NULL == pPlayer) continue;

		if (true == pPlayer->IsInWorld())
		{
			int nGameServerID = pPlayer->GetGameServerID();
			MUID nGameServerUID = gmgr.pGameServerObjectManager->IDToUID(nGameServerID);

			MapSecondVectorInserter(outmapReceiveServerUID, nGameServerUID, nCID);
		}
		else if (true == pPlayer->IsReadyMoveGameServer() || true == pPlayer->IsMoveGameServer())
		{
			int nGameServerID = pPlayer->GetReservedGameServerID();
			MUID nGameServerUID = gmgr.pGameServerObjectManager->IDToUID(nGameServerID);

			MapSecondVectorInserter(outmapReceiveServerUID, nGameServerUID, nCID);
		}
	}
}

void ZReceiveGameServerUIDSelector::Select(int nCID, MAP_RECEIEVE_GAMESERVER_UID& outmapReceiveServerUID)
{
	vector<int> vecCID;
	vecCID.push_back(nCID);

	return Select(vecCID, outmapReceiveServerUID);
}
