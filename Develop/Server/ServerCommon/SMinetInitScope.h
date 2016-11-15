#pragma once

#include "ServerCommonLib.h"
#include "MiNet.h"

class SMinetInitScope
{
public:
	SMinetInitScope() 
	{
		minet::Init(SH_COMMAND_VERSION);
	}
	virtual ~SMinetInitScope() 
	{
		minet::Fini();
	}
};
