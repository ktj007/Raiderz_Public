#ifndef _MOCK_PARTY_SYSTEM_H_
#define _MOCK_PARTY_SYSTEM_H_

#include "GPartyManager.h"
#include "MockParty.h"

class MockPartyManager : public GPartyManager
{
public:
	virtual GParty* NewParty(MUID uidParty)
	{		
		return new MockParty(uidParty);
	}
};

#include "GPartySystem.h"
class MockPartySystem : public GPartySystem
{
public:
	virtual void RoutePartyRefreshInfoAll(GParty* pParty) {}
	virtual GPartyManager* NewPartyManager(void)
	{
		return new MockPartyManager();
	}
};


#endif//_MOCK_PARTY_SYSTEM_H_
