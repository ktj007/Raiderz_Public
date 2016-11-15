#pragma once

#include "LPlayerObject.h"

class MockPlayerObject : public LPlayerObject
{
protected:

public:
	MockPlayerObject(MUID& uidObject) : LPlayerObject(uidObject) {}
	virtual ~MockPlayerObject() {}
};