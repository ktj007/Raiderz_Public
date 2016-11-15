#include "stdafx.h"
#include "SClientConnectionKeeper.h"
#include "MCommandTable.h"
#include "SSystem.h"
#include "MLocale.h"

void SClientConnectionKeeper::TryToConnect()
{
	if (m_bConnected) return;
	if (!m_bActive) return;

	if (m_nTryTick > 0)
	{
		m_bTryReserved = true;
	}
	else
	{
		Post_Connect(m_strIP.c_str(), m_nPort, m_strNetworkCardIP.c_str());
	}
}

void SClientConnectionKeeper::Run()
{
	if (!m_bActive) return;
	if (m_nTryTick == 0) return;

	if (m_bTryReserved)
	{
		uint32 nNowTime = m_pSystem->GetNowTime();

		if (nNowTime - m_nLastTime >= m_nTryTick)
		{
			Post_Connect(m_strIP.c_str(), m_nPort, m_strNetworkCardIP.c_str());
			m_bTryReserved = false;
			m_nLastTime = nNowTime;
		}
	}

}

void SClientConnectionKeeper::OnConnect()
{
	m_bConnected = true;
}

void SClientConnectionKeeper::OnDisconnect()
{
	m_bConnected = false;
}

void SClientConnectionKeeper::OnNetError( int nErrorCode )
{
	m_bConnected = false;

	TryToConnect();
}

bool SClientConnectionKeeper::Post_Connect( const wchar_t* szIP, int nPort, const wchar_t* szNetworkCardIP )
{
	mlog("Try To Connect To Server(%s , %d)\n", MLocale::ConvUTF16ToAnsi(szIP).c_str(), nPort);

	MCommand* pNewCommand = m_pNetClient->NewLocalCommand(MC_LOCAL_NET_CONNECT_WITH_NIC);
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szIP).c_str()));
	pNewCommand->AddParameter(new MCommandParameterInt(nPort));
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szNetworkCardIP).c_str()));
	return m_pNetClient->Post(pNewCommand);
}

SClientConnectionKeeper::SClientConnectionKeeper( MNetClient* pNetClient, wstring strSeverIP, int nPort, SSystem* pSystem, int nTryTick/*=0*/, wstring strNetworkCardIP/*=""*/ ) 
: m_pNetClient(pNetClient)
, m_pSystem(pSystem)
, m_strIP(strSeverIP)
, m_nPort(nPort)
, m_nTryTick(nTryTick)
, m_strNetworkCardIP(strNetworkCardIP)
, m_bActive(true)
, m_nLastTime(0)
, m_bTryReserved(false)
{

}
