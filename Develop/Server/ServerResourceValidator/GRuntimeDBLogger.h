#pragma once

#include "GValidatorLib.h"
#include "GRuntimeValidatingLogger.h"

class GVALIDATOR_API GRuntimeDBLogger: public GRuntimeValidatingLogger
{
public:
	GRuntimeDBLogger(GRuntimeValidatingReporter& rReporter);
	virtual ~GRuntimeDBLogger();

	virtual void Start(void);
	virtual void Stop(void);
};
