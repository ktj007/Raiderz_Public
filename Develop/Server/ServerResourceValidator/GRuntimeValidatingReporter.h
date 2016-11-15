#pragma once

#include "GValidatorLib.h"

class GVALIDATOR_API GRuntimeValidatingReporter
{
public:
	GRuntimeValidatingReporter();
	virtual ~GRuntimeValidatingReporter();

	void Report(wstring strMessage);
};
