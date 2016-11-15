#include "StdAfx.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "LGlobal.h"
#include "LSystem.h"
#include "LCmdRouter_Player.h"


LPlayerAcceptor_FromGameServer::LPlayerAcceptor_FromGameServer(void)
{
}

LPlayerAcceptor_FromGameServer::~LPlayerAcceptor_FromGameServer(void)
{
}

void LPlayerAcceptor_FromGameServer::ReserveAccept(MUID uidConnectionKey, AID nAID, const wstring& strUserID, MUID uidPlayerFromGameServer)
{
	m_uidConnectionKey = uidConnectionKey;
	m_nAID = nAID;
	m_strUserID = strUserID;
	m_uidPlayerFromGameServer = uidPlayerFromGameServer;

	m_nReserveTime = gsys.pSystem->GetNowTime();
}

void LPlayerAcceptor_FromGameServer::OnTimeout()
{
	LCmdRouter_Player::PostDeletePlayer(m_nAID, MUID::ZERO);
}