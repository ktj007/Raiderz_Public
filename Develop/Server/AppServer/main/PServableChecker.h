#pragma once

#include "SServableChecker.h"

class PServableChecker : public SServableChecker
{
protected:
	bool Check(const float fDelta) override;

private:
	bool Check_IsConnectedMasterServer();

};
