#pragma once

#include "GValidatorLib.h"
#include "GRuntimeValidatingLogger.h"

class GVALIDATOR_API GRuntimeTestLogger: public GRuntimeValidatingLogger
{
public:
	GRuntimeTestLogger(GRuntimeValidatingReporter& rReporter);
	virtual ~GRuntimeTestLogger(void);

	virtual void Start(void);
	virtual void Stop(void);
};
