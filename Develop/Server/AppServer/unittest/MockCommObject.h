#pragma once

#include "PGameServerObject.h"

class MockCommObject : public PGameServerObject
{
protected:

public:
	MockCommObject(MUID& uidObject, int nServerID) : PGameServerObject(uidObject, nServerID) {}
	virtual ~MockCommObject() {}
};