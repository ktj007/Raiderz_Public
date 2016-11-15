#pragma once

#include "PmLib.h"

class PmLoginTimeChecker;
class PmAuthenticator;
struct PmUserData;

class PMANG_API PmLoginFacade
{
public:
	PmLoginFacade(wstring strServerKey, uint32 nToleranceSeconds);
	virtual ~PmLoginFacade();

	bool Login(wstring strLoginStream, PmUserData* poutUserData);

private:
	PmLoginTimeChecker* m_pTimeChecker;
	PmAuthenticator* m_pAuthenticator;
};
