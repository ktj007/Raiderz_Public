#pragma once

#include "XBaseValidator.h"

class XIsAnimationValidator : public XBaseValidator
{
private:
	bool IsLootAni();
	bool IsBuffAni();
	bool IsMaleDummyloc();
	bool IsFeMaleDummyloc();

public:
	virtual bool Check();
};
