#include "stdafx.h"
#include "GPlayerAcceptor_FromGameServer.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GMasterServerFacade.h"
#include "GServerPlayerRouter.h"


void GPlayerAcceptor_FromGameServer::ReserveLogin( minet::MCommand* pCommand )
{
	bool bRet = m_PlayerGameData.PackFromCommand(pCommand);
	_ASSERT(bRet);
	_ASSERT(m_PlayerGameData.IsPacked());

	if (bRet == false) return;

	MUID uidConnectionKey;
	MUID uidPlayer;

	pCommand->GetParameter(&uidConnectionKey,		0,	MPT_UID);
	pCommand->GetParameter(&uidPlayer,				2,	MPT_UID);

	m_nAID = m_PlayerGameData.GetAID();
	m_strUserID = m_PlayerGameData.GetUserID();	
	
	m_nEnterKey = m_PlayerGameData.GetEnterKey();

	m_uidPlayerUID = uidPlayer;
	m_uidConnectionKey = uidConnectionKey;
	m_nReserveTime = gsys.pSystem->GetNowTime();	
}

bool GPlayerAcceptor_FromGameServer::SerializePlayerGameData( GEntityPlayer* pEntityPlayer )
{
	if (!m_PlayerGameData.IsPacked() || pEntityPlayer == NULL)
	{
		_ASSERT(0);
		return false;
	}

	m_PlayerGameData.UnpackToEntityPlayer(pEntityPlayer);

	return true;
}

void GPlayerAcceptor_FromGameServer::OnTimeout()
{
	GServerPlayerRouter* pRouter = gsys.pMasterServerFacade->GetPlayerRouter();
	pRouter->RouteDeletePlayer(m_nAID, m_uidPlayerUID);
}