#pragma once

#include "XBaseValidator.h"

class XAnimationEventValidator : public XBaseValidator
{
private:
	bool CheckEvent();

public:
	virtual bool Check();
};
