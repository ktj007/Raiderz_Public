#include "stdafx.h"
#include "GRuntimeDBLogger.h"
#include "GGlobal.h"
#include "GDBManager.h"

GRuntimeDBLogger::GRuntimeDBLogger(GRuntimeValidatingReporter& rReporter)
: GRuntimeValidatingLogger(rReporter)
{
	// do nothing
}

GRuntimeDBLogger::~GRuntimeDBLogger()
{
	gsys.pDBManager->SetListener(NULL);
}

void GRuntimeDBLogger::Start(void)
{
	gsys.pDBManager->SetListener(this);
}

void GRuntimeDBLogger::Stop(void)
{
	gsys.pDBManager->SetListener(NULL);
}
