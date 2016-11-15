#pragma once

#include "GValidatorLib.h"
#include "CSLogListener.h"

class MMaietXmlTestReporter;

class GVALIDATOR_API GServerValidateReporter: public CSLogListener
{
public:
	GServerValidateReporter(std::ostream& ostream);
	virtual ~GServerValidateReporter();

	virtual void OnLog(const char* szLog);

public:
	void Report(void);

private:
	MMaietXmlTestReporter* m_pXmlReporter;
};
