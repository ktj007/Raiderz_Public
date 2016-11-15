#include "StdAfx.h"
#include "GFakeLoginPlayerAcceptor_FromGameServerToLoginServer.h"
#include "GGlobal.h"
#include "GSystem.h"

GFakeLoginPlayerAcceptor_FromGameServerToLoginServer::GFakeLoginPlayerAcceptor_FromGameServerToLoginServer(void)
{
}

GFakeLoginPlayerAcceptor_FromGameServerToLoginServer::~GFakeLoginPlayerAcceptor_FromGameServerToLoginServer(void)
{
}

void GFakeLoginPlayerAcceptor_FromGameServerToLoginServer::ReserveLogin(MUID uidConnectionKey, AID nAID, wstring strUserID)
{
	m_uidConnectionKey = uidConnectionKey;
	m_nAID = nAID;
	m_strUserID = strUserID;

	m_nReserveTime = gsys.pSystem->GetNowTime();
}

void GFakeLoginPlayerAcceptor_FromGameServerToLoginServer::OnTimeout()
{

}