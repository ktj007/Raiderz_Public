#pragma once

#include "GServerValidateReporter.h"

class AddConsoleReporter: public GServerValidateReporter
{
public:
	AddConsoleReporter(std::ostream& ostream);
	virtual ~AddConsoleReporter();

	virtual void OnLog(const char* szLog);
};
