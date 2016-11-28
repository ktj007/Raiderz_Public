#include "StdAfx.h"
#include "MTime.h"
#include "GJob_FleeFromEntity.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GModuleAI.h"

GJob_FleeFromEntity::GJob_FleeFromEntity(GModuleAI* pmAI, MUID& uidTarget) 
: GJob(pmAI)
, m_uidTarget(uidTarget)
{
}

GJob_FleeFromEntity::~GJob_FleeFromEntity(void)
{
}


bool GJob_FleeFromEntity::OnStart()
{
	PFI_BLOCK_THISFUNC(1902);
	return FleeFromEntity();
}

GJobResult GJob_FleeFromEntity::OnRun(float fDelta)
{
	PFI_BLOCK_THISFUNC(1700);

	vec3 vTarEntityPos;
	if (!GetTarEntityPos(vTarEntityPos))		
		return JR_CANCELED;

	return JR_RUNNING;
}

bool GJob_FleeFromEntity::FleeFromEntity()
{
	if (!IsInvalidTarget())
		return false;

	vec3 vTarEntityInversePos;
	if (!GetTarEntityInversePos(vTarEntityInversePos))		
		return false;

	GEntityActor* pTarget = GetOwnerNPC()->FindActor(m_uidTarget);
	if (!pTarget)											
		return false;

	SetRunningTask(m_TaskBroker.New_MoveDefinite(GetOwnerNPC(), vTarEntityInversePos, 0.0f));

	return true;
}

void GJob_FleeFromEntity::OnTaskComplete(GTaskID nTaskID)
{
	PFI_BLOCK_THISFUNC(9073);

	if (!FleeFromEntity())	
	{
		GetOwnerNPC()->RouteMoveStop();
	}
}

void GJob_FleeFromEntity::OnCancel()
{
	GetOwnerNPC()->RouteMoveStop();
}

bool GJob_FleeFromEntity::GetTarEntityPos(vec3& voutTarPos)
{
	GEntity* pTarget = GetOwnerNPC()->FindEntity(m_uidTarget);
	if (NULL == pTarget)	
		return false;

	voutTarPos = pTarget->GetPos();

	return true;
}

//최종 목적지를 얻어온다. - 특정 엔티티로 부터 도망간다.
bool GJob_FleeFromEntity::GetTarEntityInversePos(vec3& tar_pos)
{
	vec3 vTarEntityPos;
	if (false == GetTarEntityPos(vTarEntityPos)) return false;

	tar_pos =  GetOwnerNPC()->GetPos() - (vTarEntityPos - GetOwnerNPC()->GetPos());

	return true;
}

bool GJob_FleeFromEntity::IsInvalidTarget()
{
	if (NULL == GetOwnerNPC()->FindActor(m_uidTarget))	return false;

	return true;
}

string GJob_FleeFromEntity::GetParamString() const
{
	GEntityNPC* pTarget = GetOwnerNPC()->FindNPC(m_uidTarget);
	if (NULL == pTarget)	
		return "FromEntity: invalid_target";

	char buff[512];
	sprintf(buff, "FromEntity: %s(%I64d)", 
		MLocale::ConvUTF16ToAnsi(pTarget->GetName()).c_str(), pTarget->GetUID().Value);
	return buff;
}
