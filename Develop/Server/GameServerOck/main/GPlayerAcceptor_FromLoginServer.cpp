#include "stdafx.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GSystem.h"
#include "GCommandCenter.h"
#include "CCommandResultTable.h"
#include "GGlobal.h"
#include "CTransData.h"
#include "GCommand.h"
#include "MiNetHelper.h"
#include "GConfig.h"
#include "SCmdRouter_Login.h"
#include "GMasterServerFacade.h"
#include "GServerPlayerRouter.h"


void GPlayerAcceptor_FromLoginServer::ReserveLogin( TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, MUID uidConnectionKey, MUID uidPlayerOnLoginServer/*=MUID::ZERO*/ )
{
	m_nAID = pLoginAccountInfo->nAID;
	m_nCID = pLoginAccountInfo->nCID;
	m_strUserID = pLoginAccountInfo->szUserID;

	m_nEnterKey = pLoginAccountInfo->nEnterKey;

	m_uidConnectionKey = uidConnectionKey;

	m_uidPlayer_OnLoginServer = uidPlayerOnLoginServer;

	m_nReserveTime = gsys.pSystem->GetNowTime();
}

void GPlayerAcceptor_FromLoginServer::MakeFakeReserveLogin( MUID uidConnectionKey, TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo )
{
	ReserveLogin(pLoginAccountInfo, uidConnectionKey);
}

void GPlayerAcceptor_FromLoginServer::PostResponseSelMyChar( int nRet, MUID uidPlayer )
{
	wstring strIP = GConfig::m_strPublicIP;		

	TD_LOGIN_GAME_SERVER_INFO tdLoginGameServerInfo;
	wcsncpy_s(tdLoginGameServerInfo.strHostName, strIP.c_str(), _TRUNCATE);
	tdLoginGameServerInfo.nPort = (uint32)GConfig::m_nPort;
	tdLoginGameServerInfo.uidConnectionKey = m_uidConnectionKey;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseSelMyChar(uidPlayer, nRet);

	if (CR_SUCCESS == nRet)
	{
		cmdRouter.MoveToGameServer(uidPlayer, &tdLoginGameServerInfo);
	}
}

void GPlayerAcceptor_FromLoginServer::OnTimeout()
{
	GServerPlayerRouter* pRouter = gsys.pMasterServerFacade->GetPlayerRouter();
	pRouter->RouteDeletePlayer(m_nAID, m_uidPlayer_OnLoginServer);
}
