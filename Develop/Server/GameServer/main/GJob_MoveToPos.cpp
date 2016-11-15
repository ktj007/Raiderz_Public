#include "stdafx.h"
#include "GJob_MoveToPos.h"
#include "GEntitySync.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GTask_Move.h"
#include "GScriptManager.h"
#include "GScriptGroup_Field.h"

GJob_MoveToPos::GJob_MoveToPos(GModuleAI* pmAI, vec3 vTarPos, vec3 vTarDir, float fLimitSumOfPathLength, unsigned long nMarkerID) 
: GJob(pmAI)
, m_vTarPos(vTarPos)
, m_vTarDir(vTarDir)
, m_fLimitSumOfPathLength(fLimitSumOfPathLength)
, m_nMarkerID(nMarkerID)
{
}

GJob_MoveToPos::~GJob_MoveToPos()
{
}

bool GJob_MoveToPos::OnStart()
{
	PFI_BLOCK_THISFUNC(1905);
	_ASSERT(GetOwnerNPC()->GetNPCInfo());
		
	GTask_Move* pTask=NULL;
	if (AT_SKY == GetOwnerNPC()->GetNPCInfo()->nAltitude)
	{
		pTask = static_cast<GTask_Move*>(m_TaskBroker.New_MoveStraight(GetOwnerNPC(), m_vTarPos, 0.0f));
	}
	else
	{
		pTask = static_cast<GTask_Move*>(m_TaskBroker.New_MoveDefinite(GetOwnerNPC(), m_vTarPos, 0.0f));
	}

	// 이동 완료시 설정될 방향 지정
	pTask->SetFinishDir(m_vTarDir);

	return SetRunningTask(pTask);
}

GJobResult GJob_MoveToPos::OnRun(float fDelta)
{
	return JR_RUNNING;
}

void GJob_MoveToPos::OnComplete()
{
//	GetOwnerNPC()->RouteMoveStop();

	CallScript();	
}

string GJob_MoveToPos::GetParamString() const
{
	char buff[512];
	sprintf(buff, "TarPos: %.1f %.1f %.1f", 
		m_vTarPos.x, m_vTarPos.y, m_vTarPos.z);
	return buff;
}

void GJob_MoveToPos::CallScript()
{
	if (0 == m_nMarkerID) return;
	GField* pField = GetOwnerNPC()->GetField();
	if (NULL == pField) return;

	GetScriptField()->OnMarkerArrive(pField->GetDelegator(), m_nMarkerID, GetOwnerNPC()->GetDelegator());
}

