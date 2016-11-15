#pragma once

#include "LTimeoutHandler.h"

class LMoveServerTimeoutHandler: public LTimeoutHandler
{
public:
	virtual void OnTimeout(MUID uidPlayer);
};
