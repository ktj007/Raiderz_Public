#pragma once

#include "MTypes.h"
#include "MUID.h"

class MockLink;

class PTestHelper
{
public:
	// net ----------------
	static MockLink* NewLink(MUID uidNew=MUID::Invalid(), bool bIgnoreAllSendCommandID = false);
	static void ClearLinks();	
};
