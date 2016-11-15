#pragma once

#include "GValidatorLib.h"
#include "GRuntimeValidatingLogger.h"

class GVALIDATOR_API GRuntimeLuaLogger: public GRuntimeValidatingLogger
{
public:
	GRuntimeLuaLogger(GRuntimeValidatingReporter& rReporter);
	virtual ~GRuntimeLuaLogger();

	virtual void Start(void);
	virtual void Stop(void);
};
