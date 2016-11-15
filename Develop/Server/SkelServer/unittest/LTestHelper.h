#pragma once

#include "MTypes.h"
#include "MUID.h"

class MockLink;

class ZTestHelper
{
protected:
	static MUID		m_uidGenerator;
public:
	// net ----------------
	static MockLink* NewLink(bool bIgnoreAllSendCommandID = false);
	static void ClearLinks();
	static MUID NewUID();
};