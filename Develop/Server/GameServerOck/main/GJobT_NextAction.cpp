#include "StdAfx.h"
#include "GJobT_NextAction.h"
#include "GNPCWaitFor.h"
#include "GEntityNPC.h"

GJobT_NextAction::GJobT_NextAction( GModuleAI* pmAI )
: GInstantJob(pmAI)
{
}

GJobT_NextAction::~GJobT_NextAction(void)
{
}

bool GJobT_NextAction::OnStart()
{
	GetNPC()->GetNPCWaitFor().RunNextAction();

	return true;
}

string GJobT_NextAction::GetParamString() const
{
	return "";
}