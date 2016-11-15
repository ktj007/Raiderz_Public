#pragma once

#include "PmLib.h"
#include "MTstring.h"

class PMANG_API PmAuthenticator
{
public:
	PmAuthenticator(wstring strServerKey);
	
	bool Verify(wstring strExpected, wstring strLoginStream) const;

private:
	wstring m_strServerKey;
};
