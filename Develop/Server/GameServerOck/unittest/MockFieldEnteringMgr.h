#pragma once

#include "GFieldEnteringMgr.h"

class MockFieldEnteringMgr : public GFieldEnteringMgr
{
public:
	virtual void Add(MUID uidPlayer) override
	{
		EnterPlayerWhoFieldLoadingComplete(uidPlayer);
	}
};
