#include "stdafx.h"
#include "MiNetHelper.h"
#include "Winsock2.h"
#include "MInetUtil.h"
#include <iostream>

namespace minet {


void MiNetHelper::ConvertCompactIP(char* szOut, const char* szInputDottedIP)
{
	in_addr addr;
	addr.S_un.S_addr = inet_addr(szInputDottedIP);
	sprintf(szOut, "%03u%03u%03u%03u", addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, 
		addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
}

unsigned long MiNetHelper::ConvertIPAddress( const char* szIP )
{
	DWORD dwAddr = inet_addr(szIP);

	if (dwAddr != INADDR_NONE)
	{
		return dwAddr;
	}
	else 
	{
		HOSTENT* pHost = gethostbyname(szIP);
		
		if (pHost != NULL)
		{	
			return ((IN_ADDR*)pHost->h_addr)->s_addr;
		}		
	}

	return INADDR_NONE;
}

char* MiNetHelper::ConvertIPAddress( unsigned long nIP )
{
	in_addr addr;
	addr.s_addr = nIP;
	return inet_ntoa(addr);
}

string MiNetHelper::GetHostIPString()
{
	char name[256];
	string strIP;

	if (gethostname(name, sizeof(name)) == 0)
	{
		PHOSTENT hostinfo = gethostbyname(name);
		if (hostinfo != NULL)
		{
			strIP = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
		}
	}	

	return strIP;
}








} // namespace minet