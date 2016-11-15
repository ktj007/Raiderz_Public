#pragma once

#include "XModuleMotion.h"

class MockModuleMotion : public XModuleMotion
{
private:
	virtual void			OnInitialized();

public:
	MockModuleMotion(XObject* pOwner=NULL);
	virtual ~MockModuleMotion() {}

	void					Init(XMotionLogic* pMotionLogic, XUpperMotionLogic* pUpperMotionLogic);
};