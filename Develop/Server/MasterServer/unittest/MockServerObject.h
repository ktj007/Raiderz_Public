#pragma once

#include "ZGameServerObject.h"

class MockServerObject : public ZGameServerObject
{
protected:

public:
	MockServerObject(MUID uidObject, ZGameServerInfo* pGameServerInfo) : ZGameServerObject(uidObject, pGameServerInfo) {}
	virtual ~MockServerObject() {}
};
