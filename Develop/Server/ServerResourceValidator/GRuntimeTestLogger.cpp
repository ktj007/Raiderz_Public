#include "StdAfx.h"
#include "GRuntimeTestLogger.h"
#include "GGlobal.h"
#include "GTestSystem.h"

GRuntimeTestLogger::GRuntimeTestLogger(GRuntimeValidatingReporter& rReporter)
: GRuntimeValidatingLogger(rReporter)
{
}

GRuntimeTestLogger::~GRuntimeTestLogger(void)
{
	Stop();
}

void GRuntimeTestLogger::Start( void )
{
	gsys.pTestSystem->SetListener(this);
}

void GRuntimeTestLogger::Stop( void )
{
	gsys.pTestSystem->SetListener(NULL);
}