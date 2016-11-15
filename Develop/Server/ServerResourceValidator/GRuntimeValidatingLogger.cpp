#include "stdafx.h"
#include "GRuntimeValidatingLogger.h"
#include "GRuntimeValidatingReporter.h"
#include "MLocale.h"

GRuntimeValidatingLogger::GRuntimeValidatingLogger(GRuntimeValidatingReporter& rReporter)
: m_rReporter(rReporter)
{
	// do nothing
}

GRuntimeValidatingLogger::~GRuntimeValidatingLogger()
{
	// do nothing
}

void GRuntimeValidatingLogger::OnLog(const char* szLog)
{
	m_rReporter.Report(MLocale::ConvAnsiToUTF16(szLog));
}
