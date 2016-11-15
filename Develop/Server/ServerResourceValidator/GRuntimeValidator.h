#pragma once

#include "GValidatorLib.h"

class GRuntimeValidatingReporter;
class GRuntimeValidatingLogger;

class GVALIDATOR_API GRuntimeValidator
{
public:
	GRuntimeValidator();
	virtual ~GRuntimeValidator();
	
	virtual void Start(void);
	virtual void Stop(void);

private:
	GRuntimeValidatingReporter* m_pReporter;
	vector<GRuntimeValidatingLogger*> m_vecLoggers;
};
