#include "StdAfx.h"
#include "MTime.h"
#include "GJob_FleeToHome.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GModuleAI.h"

GJob_FleeToHome::GJob_FleeToHome(GModuleAI* pmAI) 
: GJob(pmAI)
{
}

GJob_FleeToHome::~GJob_FleeToHome(void)
{
}


bool GJob_FleeToHome::OnStart()
{
	PFI_BLOCK_THISFUNC(1903);
	InitFleeDir();

	return Flee();
}

GJobResult GJob_FleeToHome::OnRun(float fDelta)
{
	return JR_RUNNING;
}

vec3 GJob_FleeToHome::GetTarPos()
{
	return GetOwnerNPC()->GetPos() + (m_vFleeDir * 300.0f); // 3미터씩 이동
}


bool GJob_FleeToHome::Flee()
{
	SetRunningTask(m_TaskBroker.New_MoveDefinite(GetOwnerNPC(), GetTarPos(), 0.0f));

	return true;
}

void GJob_FleeToHome::OnTaskComplete(GTaskID nTaskID)
{
	PFI_BLOCK_THISFUNC(9075);

	Flee();
}

void GJob_FleeToHome::OnCancel()
{
}

void GJob_FleeToHome::InitFleeDir()
{
	vec3 vHomePoint = GetOwnerNPC()->GetHomePoint().vPoint;
	vec3 vOwnerPos = GetOwnerNPC()->GetPos();
	m_vFleeDir = vHomePoint - vOwnerPos;
	GMath::NormalizeZ0(m_vFleeDir);
}

string GJob_FleeToHome::GetParamString() const
{
	char buff[512];
	sprintf(buff, "TarPos: %.1f %.1f %.1f", 
		m_vFleeDir.x, m_vFleeDir.y, m_vFleeDir.z);
	return buff;
}
