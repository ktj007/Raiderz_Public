#pragma once

#include "XModuleEntity.h"

class MockModuleEntity : public XModuleEntity
{
public:
	MockModuleEntity(XObject* pOwner=NULL);
	virtual ~MockModuleEntity() {}
};