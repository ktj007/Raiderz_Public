#include "stdafx.h"
#include "AddConsoleReporter.h"

AddConsoleReporter::AddConsoleReporter(std::ostream& ostream)
: GServerValidateReporter(ostream)
{
	// do nothing
}

AddConsoleReporter::~AddConsoleReporter()
{
	// do nothing
}

void AddConsoleReporter::OnLog(const char* szLog)
{
	GServerValidateReporter::OnLog(szLog);

	// and console output
	cout << szLog;

}
