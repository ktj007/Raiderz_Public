#pragma once

#include "MUID.h"
#include "MCommand.h"
#include "MCommandParameter.h"

using namespace minet;

MCommand* MakeNewCommand(int nCmdID, MUID TargetUID, int nParamCount, MCommandParameter* pCmdParam, ... );
MCommand* MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... );
