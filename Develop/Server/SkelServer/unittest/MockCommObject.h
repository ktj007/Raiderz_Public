#pragma once

#include "LCommObject.h"

class MockCommObject : public LCommObject
{
protected:

public:
	MockCommObject(MUID& uidObject) : LCommObject(uidObject) {}
	virtual ~MockCommObject() {}
};