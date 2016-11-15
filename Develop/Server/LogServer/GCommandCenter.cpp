#include "stdafx.h"
#include "GCommandCenter.h"
#include "GCommandTable.h"
#include "GGameServerMgr.h"

GCommandCenter::GCommandCenter( GNetServer& netserver ) : m_NetServer(netserver)
{
	IgnoreDebugCommandLog();
}

MCommand* GCommandCenter::NewCommand( int nCmdID, MUID TargetUID )
{
	return m_NetServer.NewCommand(nCmdID, TargetUID);
}

MCommand* GCommandCenter::NewCommand( int nCmdID )
{
	return m_NetServer.NewCommand(nCmdID, 0);
}

void GCommandCenter::PostCommand( MCommand* pCommand )
{
	if (NULL == pCommand) return;

	if (pCommand->IsNoReceiver())
	{
		SAFE_DELETE(pCommand);
		return;
	}

	m_NetServer.Post(pCommand);
}

void GCommandCenter::RouteToAll( MCommand* pCommand )
{
	int nCount = 0;

	gmgr.pGameServerMgr->Lock();	//----------- lock

	bool bFirstPlayer = true;
	for (GGameServerMgr::MAP_GAMESERVER::iterator itor = gmgr.pGameServerMgr->GetContainer().begin();
		itor != gmgr.pGameServerMgr->GetContainer().end(); ++itor)
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

	gmgr.pGameServerMgr->Unlock();	//------ unlock

	if (nCount > 0) PostCommand(pCommand);
	else delete pCommand;
}

void GCommandCenter::IgnoreDebugCommandLog()
{
	m_CommandLogger.IgnoreCommand(MC_DEBUG_STRING);
	m_CommandLogger.IgnoreCommand(MC_REQUEST_DEBUG_STRING);
}

void GCommandCenter::LogCommand( MCommand* pCmd )
{
#ifdef _PUBLISH
	return;
#endif

	m_CommandLogger.Log(pCmd);
}