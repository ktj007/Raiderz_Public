#pragma once

#include "GAIEvent.h"

class GAIEventHandler
{
public:	
	virtual void OnNotify(GAIEvent aiEvent) = 0;
};
