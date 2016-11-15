#include "stdafx.h"
#include "PFixtureHelper.h"
#include "PTestHelper.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"
#include "STransData.h"
#include "PChatChannel.h"
#include "SUnitTestUtil.h"

//////////////////////////////////////////////////////////////////////////

MUID FBaseGameServer::AddGameServerObject(int nServerID)
{
	MUID uidNew = SUnitTestUtil::NewUID();
	SCommObject* pCommObject = gmgr.pGameServerObjectManager->NewObject(uidNew, nServerID);

	gmgr.pGameServerObjectManager->AddObject(pCommObject);
	return uidNew;
}

//////////////////////////////////////////////////////////////////////////

int FBasePlayer::AddPlayerObject(int nGameServerID, int nCID, wstring strName)
{
	if (0 == nCID) nCID = SUnitTestUtil::NewID();
	if (0 == strName.size()) strName = SUnitTestUtil::NewName();

	MUID uidNew = SUnitTestUtil::NewUID();

	TD_PROXY_PLAYER_INFO info;
	info.nCID = nCID;
	info.nGameServerID = nGameServerID;
	wcsncpy_s(info.szName, strName.c_str(), _TRUNCATE);
	
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
	pManager->AddPlayer(info);

	return nCID;
}

//////////////////////////////////////////////////////////////////////////

MUID FBaseChatChannel::AddChatChannel_Private(CID cidOwnerPlayer, wstring strChannelName, int nPlayerGameServerID)
{
	MUID uidNew = SUnitTestUtil::NewUID();
	PChatChannel* pChatChannel = new PChatChannel(uidNew, CCT_PRIVATE);
	pChatChannel->Init(cidOwnerPlayer, strChannelName);

	gmgr.pChatChannelManager->AddChannel(pChatChannel);

	gmgr.pChatChannelManager->EnterChannel(pChatChannel, cidOwnerPlayer, nPlayerGameServerID);

	return pChatChannel->GetUID();
}