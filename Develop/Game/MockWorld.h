#pragma once

#include "XWorld.h"
#include "XZone.h"

class MockZone : public XZone
{
public:
	MockZone() : XZone() {}
	virtual ~MockZone() {}
	void InitForTest(RTerrain* pTerrain);
};

class MockWorld : public XWorld
{
public:
	MockWorld();
	virtual ~MockWorld();

	virtual bool IsTest() { return true; }

protected:
	virtual bool Init(XWorldInitParam& worldInitParam) override;
};
