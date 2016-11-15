#pragma once

#include "MTypes.h"
#include "MUID.h"

class MockLink;
class MockPlayerObject;

class LTestHelper
{
protected:
	static MUID		m_uidGenerator;
	static int		m_IDGenerator;
	static int		m_nName;
public:
	// net ----------------
	static MockLink* NewLink(MUID uidNew=MUID::Invalid(), bool bIgnoreAllSendCommandID = false);
	static void ClearLinks();
	static MUID NewUID();
	static int NewID();
	static tstring NewName();

	// PlayerObject
	static MockPlayerObject* NewMockPlayerObject(MUID uidNew=MUID::ZERO);
};