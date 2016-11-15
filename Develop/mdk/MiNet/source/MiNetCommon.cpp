#include "stdafx.h"
#include "MiNetCommon.h"

namespace minet {

void SplitIAddress(char* szIP, int nIPBufferSize, int* pPort, const char* szAddress)
{
	*szIP = NULL;
	*pPort = 0;

	size_t nCopyBufferSize = strlen(szAddress)+2;
	char* szCopyAddress = new char[nCopyBufferSize];
	strcpy_s(szCopyAddress, nCopyBufferSize, szAddress);

	char seps[] =":\n";
	char* token;
	char* nextToken;
	 
	token = strtok_s(szCopyAddress, seps, &nextToken);
	if(token!=NULL) strcpy_s(szIP, nIPBufferSize, token);
	token = strtok_s(NULL, seps, &nextToken);
	if(token!=NULL) *pPort = atoi(token);

	delete[] szCopyAddress;
	szCopyAddress = NULL;
}

} // namespace minet