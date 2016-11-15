#include "StdAfx.h"
#include "GInstantJob.h"
#include "GModuleAI.h"

GInstantJob::GInstantJob( GModuleAI* pmAI )
: GJob(pmAI)
{

}

GEntityNPC* GInstantJob::GetNPC()
{
	return m_pOwner->GetOwnerNPC();
}