#include "stdafx.h"
#include "Winsock2.h"
#include "MInetUtil.h"
#include <iostream>

#if (_MSC_VER >= 1900)
#include <WS2tcpip.h>
#endif

void MConvertCompactIP(char* szOut, const char* szInputDottedIP)
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