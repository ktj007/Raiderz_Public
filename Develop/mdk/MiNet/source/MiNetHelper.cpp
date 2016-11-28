#include "stdafx.h"
#include "MiNetHelper.h"
#include "Winsock2.h"
#include "MInetUtil.h"
#include <iostream>

#if (_MSC_VER >= 1900)
#include <WS2tcpip.h>
#endif

namespace minet {


void MiNetHelper::ConvertCompactIP(char* szOut, const char* szInputDottedIP)
{
	in_addr addr = { 0 };
#if (_MSC_VER >= 1900)
	inet_pton(AF_INET, szInputDottedIP, &addr);
#else
	addr.S_un.S_addr = inet_addr(szInputDottedIP);
#endif
	sprintf(szOut, "%03u%03u%03u%03u", addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, 
		addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
}

unsigned long MiNetHelper::ConvertIPAddress( const char* szIP )
{
#if (_MSC_VER >= 1900)
	DWORD	dwAddr = INADDR_NONE;
	IN_ADDR inAddr;

	if (inet_pton(AF_INET, szIP, &inAddr))
		dwAddr = inAddr.s_addr;
#else
	DWORD dwAddr = inet_addr(szIP);
#endif

	if (dwAddr != INADDR_NONE)
	{
		return dwAddr;
	}
	else 
	{
#if (_MSC_VER >= 1900)
		ADDRINFO* pAddrInfo;

		ADDRINFO AddrHints = { 0 };
		AddrHints.ai_family		= AF_INET;
		AddrHints.ai_protocol	= IPPROTO_TCP;
		AddrHints.ai_socktype	= SOCK_STREAM;

		if (getaddrinfo(szIP, NULL, &AddrHints, &pAddrInfo) == 0)
			return ((SOCKADDR_IN*)pAddrInfo->ai_addr)->sin_addr.s_addr;
#else
		HOSTENT* pHost = gethostbyname(szIP);
		
		if (pHost != NULL)
		{	
			return ((IN_ADDR*)pHost->h_addr)->s_addr;
		}		
#endif
	}

	return INADDR_NONE;
}

char* MiNetHelper::ConvertIPAddress( unsigned long nIP )
{
	in_addr addr;
	addr.s_addr = nIP;
#if (_MSC_VER >= 1900)
	static char szIP[64];
	inet_ntop(AF_INET, &addr, szIP, 64);
	return szIP;
#else
	return inet_ntoa(addr);
#endif
}

string MiNetHelper::GetHostIPString()
{
	char name[256];
	string strIP;

	if (gethostname(name, sizeof(name)) == 0)
	{
#if (_MSC_VER >= 1900)
		ADDRINFO* pAddrInfo;

		ADDRINFO AddrHints = { 0 };
		AddrHints.ai_family = AF_INET;
		AddrHints.ai_protocol = IPPROTO_TCP;
		AddrHints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(name, NULL, &AddrHints, &pAddrInfo) == 0)
		{
			IN_ADDR inAddr = ((SOCKADDR_IN*)pAddrInfo->ai_addr)->sin_addr;
			char	szIP[64];

			if (!inet_ntop(AF_INET, &inAddr, szIP, 64))
				strIP = szIP;
		}
#else
		PHOSTENT hostinfo = gethostbyname(name);
		if (hostinfo != NULL)
		{
			strIP = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
		}
#endif
	}	

	return strIP;
}








} // namespace minet