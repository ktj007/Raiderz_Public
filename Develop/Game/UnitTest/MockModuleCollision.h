#pragma once

#include "XModuleCollision.h"

class MockModuleCollision : public XModuleCollision
{
private:
	virtual void			OnInitialized();

public:
	MockModuleCollision(XObject* pOwner=NULL);
	virtual ~MockModuleCollision() {}

};