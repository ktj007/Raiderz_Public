#pragma once

#include "LTimeoutHandler.h"

class LSelMyCharTimeoutHandler: public LTimeoutHandler
{
public:
	virtual void OnTimeout(MUID uidPlayer);
};
