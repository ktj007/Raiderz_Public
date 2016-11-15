#include "stdafx.h"
#include "GJob_Talent.h"
#include "GEntitySync.h"
#include "GMessageID.h"
#include "GCommandTable.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GTargetObserveRotater.h"
#include "GGlobal.h"
#include "GTalentInfoMgr.h"

GJob_Talent::GJob_Talent(GModuleAI* pmAI, MUID uidTarget)
:GJob(pmAI)
,m_uidTargetActor(uidTarget)
,m_nTalentID(INVALID_TALENT_ID)
,m_bGainStress(false)
{
	;
}

string GJob_Talent::GetParamString() const
{
	string strName = "unknown";
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(m_nTalentID);
	if (pTalentInfo)
	{
		strName = MLocale::ConvUTF16ToAnsi(pTalentInfo->m_szName);
	}

	char buff[512];
	sprintf(buff, "%s(%d)", strName.c_str(), m_nTalentID);
	return buff;
}

GJob_UseTalent::GJob_UseTalent(GModuleAI* pmAI, GTalentInfo* pTalentInfo, MUID uidTarget)
: GJob_Talent(pmAI, uidTarget), m_pTalentInfo(pTalentInfo)
,m_bRunMainTalent(false)
, m_bRotatable(true)
{
	_ASSERT(pTalentInfo);
	m_nTalentID = pTalentInfo->m_nID;
}

GJob_UseTalent::~GJob_UseTalent()
{
	;
}

bool GJob_UseTalent::OnStart()
{
	UseTalent();

	return true;
}

GJobResult GJob_UseTalent::OnRun(float fDelta)
{
	return JR_RUNNING;
}

void GJob_UseTalent::OnComplete()
{
}

void GJob_UseTalent::OnCancel()
{
}

void GJob_UseTalent::OnTaskComplete(GTaskID nTaskID)
{
	PFI_B_THISFUNC(9078);

	if (!m_bRunMainTalent)
	{
		UseTalent();
	}
}

void GJob_UseTalent::UseTalent()
{
	GEntityNPC* pOwnerNPC = GetOwnerNPC();

	// 회전 탤런트 사용이 필요한 경우
	GTargetObserveRotater rotater;
	bool bNeedRotate = rotater.IsNeedRotate(pOwnerNPC);

	if (m_bRotatable &&
		bNeedRotate &&
		m_pTalentInfo->m_bSetDirection)
	{
		GTask* pMoveTask = rotater.GetRotateTask(pOwnerNPC);
		if (pMoveTask)
		{
			SetRunningTask(pMoveTask);
			return;
		}
	}

	UseMainTalent();

}

void GJob_UseTalent::UseMainTalent()
{
	SetRunningTask(m_TaskBroker.New_UseTalent(GetOwnerNPC(), m_pTalentInfo, m_uidTargetActor, m_bGainStress, m_bRotatable));
	m_bRunMainTalent = true;
}

//////////////////

GJob_Guard::GJob_Guard(GModuleAI* pmAI, int nGuardTalentID, float fGuardingTime): 
GJob_Talent(pmAI), m_fGuardingTime(fGuardingTime)
{
	m_nTalentID = nGuardTalentID;
}

GJob_Guard::~GJob_Guard()
{
	;
}

bool GJob_Guard::OnStart()
{
	SetRunningTask(m_TaskBroker.New_Guard(GetOwnerNPC(), m_nTalentID, m_fGuardingTime));

	return true;
}

GJobResult GJob_Guard::OnRun(float fDelta)
{
	return JR_RUNNING;
}

