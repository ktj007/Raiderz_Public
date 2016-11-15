#pragma once

#include "SServableChecker.h"

class ZServableChecker : public SServableChecker
{
protected:
	bool Check(const float fDelta) override;

private:
	bool Check_RequiredServerCount();
	bool Check_CurrntServerCount();

};
