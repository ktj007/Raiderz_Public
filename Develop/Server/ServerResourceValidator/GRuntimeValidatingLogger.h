#pragma once

#include "GValidatorLib.h"
#include "CSLogListener.h"

class GRuntimeValidatingReporter;

class GVALIDATOR_API GRuntimeValidatingLogger: public CSLogListener
{
public:
	GRuntimeValidatingLogger(GRuntimeValidatingReporter& rReporter);
	virtual ~GRuntimeValidatingLogger();
	
	virtual void Start(void)	{}
	virtual void Stop(void)		{}

	virtual void OnLog(const char * szLog);

private:
	GRuntimeValidatingReporter& m_rReporter;
};
