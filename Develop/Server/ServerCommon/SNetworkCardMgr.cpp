#include "StdAfx.h"
#include "SNetworkCardMgr.h"
#include "MLocale.h"

#if (_MSC_VER >= 1900)
#include <WS2tcpip.h>
#endif

SNetworkCardMgr* SNetworkCardMgr::GetInstance()
{
	static SNetworkCardMgr NetworkCardMgr;
	return &NetworkCardMgr;
}


SNetworkCardMgr::SNetworkCardMgr(void)
{
}

SNetworkCardMgr::~SNetworkCardMgr(void)
{
}

void SNetworkCardMgr::Clear()
{
	m_listNetCardInfo_Reg.clear();
	m_listNetCardInfo_Unreg.clear();
}

bool SNetworkCardMgr::Init()
{
	char name[64] = {0,};
	gethostname(name, sizeof(name));

#if (_MSC_VER >= 1900)
	ADDRINFO* pAddrInfo;

	ADDRINFO AddrHints = { 0 };
	AddrHints.ai_family		= AF_INET;
	AddrHints.ai_protocol	= IPPROTO_TCP;
	AddrHints.ai_socktype	= SOCK_STREAM;

	if (getaddrinfo(name, NULL, &AddrHints, &pAddrInfo) != 0)
		return false;

	for (ADDRINFO* i = pAddrInfo; i; i = i->ai_next)
	{
		IN_ADDR iaddr = ((SOCKADDR_IN*)pAddrInfo->ai_addr)->sin_addr;
		char	szIP[64];

		if (!inet_ntop(AF_INET, &iaddr, szIP, 64))
			continue;

		m_listNetCardInfo_Unreg.push_back(SNetworkCardInfo(MLocale::ConvAnsiToTCHAR(szIP).c_str()));
	}
#else
	hostent* pHostEnt = gethostbyname(name);
	if (NULL	== pHostEnt->h_addr) return false;
	if (AF_INET	!= pHostEnt->h_addrtype) return false;
	
	for(int i = 0; pHostEnt->h_addr_list[i] != NULL; ++i)
	{
		in_addr iaddr;
		memcpy(&iaddr, pHostEnt->h_addr_list[i], sizeof(iaddr));
		
		tstring strAddr = MLocale::ConvAnsiToTCHAR(inet_ntoa(iaddr));

		m_listNetCardInfo_Unreg.push_back(SNetworkCardInfo(strAddr.c_str()));
	}
#endif

	return true;
}

bool SNetworkCardMgr::RegCard(wstring strIP, int nID)
{
	list<SNetworkCardInfo>::iterator itr = m_listNetCardInfo_Unreg.begin();
	for (; itr != m_listNetCardInfo_Unreg.end();)
	{
		SNetworkCardInfo& refNetworkCardInfo = (*itr);

		if (0 == refNetworkCardInfo.GetIP().compare(strIP.c_str()))
		{
			refNetworkCardInfo.SetID(nID);

			m_listNetCardInfo_Reg.push_back(refNetworkCardInfo);
			m_listNetCardInfo_Unreg.erase(itr);
			break;
		}

		++itr;
	}
	return true;
}

wstring SNetworkCardMgr::GetIP(int nNetworkCardID)
{
	list<SNetworkCardInfo>::iterator itrReg = m_listNetCardInfo_Reg.begin();
	for (; itrReg != m_listNetCardInfo_Reg.end(); ++itrReg)
	{
		SNetworkCardInfo& refNetworkCardInfo = (*itrReg);
		if (refNetworkCardInfo.GetID() == nNetworkCardID)
		{
			return refNetworkCardInfo.GetIP();
		}
	}

	return L"";
}
