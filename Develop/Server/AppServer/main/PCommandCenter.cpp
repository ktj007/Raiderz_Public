#include "stdafx.h"
#include "PCommandCenter.h"
#include "PChatChannel.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PGameServerObjectManager.h"
#include "PCommandCenter.h"
#include "SCommObjectManager.h"


PCommandCenter::PCommandCenter( minet::MNetServer* netserver)
: SCommandCenter(netserver)
{

}

void PCommandCenter::RouteToAll( MCommand* pCommand )
{
	int nCount = 0;

	gmgr.pGameServerObjectManager->Lock();	//----------- lock

	bool bFirstPlayer = true;
	for (SCommObjectManager::SCommObjectMap::iterator itor = gmgr.pGameServerObjectManager->GetObjects().begin();
		itor != gmgr.pGameServerObjectManager->GetObjects().end(); ++itor)
	{
		MUID uid = (*itor).first;
		nCount++;

		if (bFirstPlayer)
		{
			pCommand->SetReceiverUID(uid);
			bFirstPlayer=false;
		}
		else
		{
			pCommand->AddAdditiveReceiver(uid);
		}
	}

	gmgr.pGameServerObjectManager->Unlock();	//------ unlock

	if (nCount > 0) PostCommand(pCommand);
	else delete pCommand;
}

void PCommandCenter::RouteToChannel(PChatChannel* pChannel, minet::MCommand* pCommand, CID cidIgnorePlayer)
{
	vector<int > vecGameServerID;
	pChannel->GetPlayerGameServerIDVec(vecGameServerID);
	if (0 == vecGameServerID.size()) return;

	/// 찾아낸 GameServer 들에게 커맨드 전송
	bool bFirstReceiver = true;
	for(size_t i = 0; i < vecGameServerID.size(); ++i)
	{
		int nGameServerID = vecGameServerID[i];
		MUID uidGameServer = gmgr.pGameServerObjectManager->GetGameServerUID(nGameServerID);
		if (uidGameServer.IsInvalid()) continue;

		if (bFirstReceiver)
		{
			pCommand->SetReceiverUID(uidGameServer);
			bFirstReceiver = false;
		}
		else
		{
			pCommand->AddAdditiveReceiver(uidGameServer);
		}
	}

	if (0 == pCommand->GetReceiverCount()) return;

	gsys.pCommandCenter->PostCommand(pCommand);
}