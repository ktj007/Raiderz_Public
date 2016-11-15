#pragma once

#include "XModuleAction.h"

class MockModuleAction : public XModuleAction
{
private:
	virtual void			OnInitialized();

public:
	MockModuleAction(XObject* pOwner=NULL);
	virtual ~MockModuleAction() {}

	void					Init(XModuleMotion* pModuleMotion = NULL, XModuleMovable* pModuleMovable = NULL);

};