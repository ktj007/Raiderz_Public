#include "stdafx.h"
#include "GRuntimeLuaLogger.h"
#include "CSLuaCallbackLogger.h"

GRuntimeLuaLogger::GRuntimeLuaLogger(GRuntimeValidatingReporter& rReporter)
: GRuntimeValidatingLogger(rReporter)
{
	// do nothing
}

GRuntimeLuaLogger::~GRuntimeLuaLogger()
{
	CSLuaCallbackLogger::SetListener(NULL);
}

void GRuntimeLuaLogger::Start(void)
{
	CSLuaCallbackLogger::SetListener(this);
}

void GRuntimeLuaLogger::Stop(void)
{
	CSLuaCallbackLogger::SetListener(NULL);
}
