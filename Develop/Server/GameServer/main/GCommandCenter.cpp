#include "stdafx.h"
#include "GCommandCenter.h"
#include "GPlayerObjectManager.h"
#include "GCommandTable.h"
#include "GCommandLogger.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GProxyChatChannel.h"
#include "GAppServerFacade.h"
#include "GConfig.h"
#include "GConst.h"

GCommandCenter::GCommandCenter( MNetServer* pNetServer )
: SCommandCenter(pNetServer)
{

}

void GCommandCenter::SetLogger(GCommandLogger* pCommandLogger)
{
	if (NULL == pCommandLogger) return;

	SAFE_DELETE(m_pCommandLogger);
	m_pCommandLogger = pCommandLogger;
}

void GCommandCenter::RouteToAll( MCommand* pCommand )
{
	int nCount = 0;

	bool bFirstPlayer = true;
	for (GPlayerObjectManager::GPlayerObjectMap::iterator itor = gmgr.pPlayerObjectManager->GetObjects().begin();
		itor != gmgr.pPlayerObjectManager->GetObjects().end(); ++itor)
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

	if (nCount > 0) PostCommand(pCommand);
	else
	{
		SAFE_DELETE(pCommand);
	}
}

void GCommandCenter::RouteToChatChannel(MUID uidChannel, MCommand* pCommand)
{
	GProxyChatChannelManager* pManager = gsys.pAppServerFacade->GetChatChannelManager();
	GProxyChatChannel* pChannel = pManager->FindChannel(uidChannel);

	if (pChannel != NULL)
	{
		for(player_map::iterator it = pChannel->BeginPlayer(); it != pChannel->EndPlayer(); it++)
		{
			GProxyChatPlayer* pPlayer = it->second;

			if (pPlayer->GetGameServerID() == GConfig::m_nMyServerID)
			{
				MUID uidPlayer = pPlayer->GetUID();
				pCommand->AddReceiver(uidPlayer);
			}
		}
	}
	
	PostCommand(pCommand);
}

void GCommandCenter::NetDebugString( const wchar_t* szKey, int nParam, const wchar_t* szText )
{
	if (IsRunForTest())
	{
		MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, 4, NEW_WSTR((szKey==NULL) ? L"" : szKey), NEW_INT(nParam), NEW_WSTR(szText), NEW_BLOB());
		RouteToAll(pNewCommand);
	}
}
