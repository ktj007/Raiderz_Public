#pragma once

#include "PTestWrapper.h"
#include "PGameServerObjectManager.h"
#include "PChatChannelManager.h"
#include "SUnitTestUtil.h"
#include "PTestHelper.h"
#include "PGameServerObject.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"
#include "CTransData.h"

struct FBaseGameServer
{
	FBaseGameServer()
	{

	}
	virtual ~FBaseGameServer()
	{

	}

	MUID AddGameServerObject(int nServerID);

	PTestMgrWrapper<PGameServerObjectManager>	m_ServerObjectManagerWrapper;
};

//////////////////////////////////////////////////////////////////////////

struct FBasePlayer
{
	FBasePlayer()
	{

	}
	virtual ~FBasePlayer()
	{
		SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
		pManager->Clear();
	}

	int AddPlayerObject(int nGameServerID, int nCID = 0, wstring strName = L"");
};

//////////////////////////////////////////////////////////////////////////

struct FBaseChatChannel
{
	FBaseChatChannel()
	{

	}
	virtual ~FBaseChatChannel()
	{

	}

	MUID AddChatChannel_Private(CID cidOwnerPlayer, wstring strChannelName, int nPlayerGameServerID);

	PTestMgrWrapper<PChatChannelManager>	m_ChatChannelManagerWrapper;
};


//////////////////////////////////////////////////////////////////////////

class FBaseMail: public FBaseGameServer, public FBasePlayer
{
public:
	FBaseMail()
	{
		// Add GameServerObject
		int nServerID = SUnitTestUtil::NewID();

		MUID uidGameServer = AddGameServerObject(nServerID);			
		m_pGameServerObject = gmgr.pGameServerObjectManager->Find(uidGameServer);

		m_pLink = PTestHelper::NewLink(uidGameServer);


		// Add PlayerObject
		int nGameServerID = m_pGameServerObject->GetServerID();
		CID nCID = AddPlayerObject(nGameServerID);

		SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
		m_pProxyPlayer = pManager->FindPlayer(nCID);
	}

	virtual ~FBaseMail()
	{
		// do nothing
	}

	void InitMailInfo(TD_MAIL_MAILBOX_MAIL_INFO& info)
	{
		info.uidMail = 1;
		info.nType = 1;
		wcsncpy_s(info.strSenderName, L"TestSenderName", _TRUNCATE);
		wcsncpy_s(info.strMailTitle, L"TestMailTitle", _TRUNCATE);
		info.nAppendedItemID = 3;
		info.bIsRead = false;
		info.nExpiringTimeSec = 3600;
		info.bHasText = true;
		info.nAppendedMoney = 100;
	}

protected:	
	MockLink*	m_pLink;
	PGameServerObject* m_pGameServerObject;
	SProxyPlayer* m_pProxyPlayer;
};
