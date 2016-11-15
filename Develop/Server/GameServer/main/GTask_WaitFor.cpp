#include "StdAfx.h"
#include "GTask_WaitFor.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GNPCWaitFor.h"

GTask_WaitFor::GTask_WaitFor( GEntityNPC* pEntity, MUID uidTarget )
: GTask(pEntity)
, m_uidTarget(uidTarget)
{

}

GTask_WaitFor::~GTask_WaitFor(void)
{
}

bool GTask_WaitFor::OnStart()
{
	PFI_BLOCK_THISFUNC(1951);

	if (m_uidTarget == m_pOwner->GetUID())
		return false;	// 자기자신을 기다릴 수는 없음

	// 대기등록
	GField* pField = m_pOwner->GetField();
	VALID_RET(pField, false);

	GEntityNPC* pTarget = pField->FindNPC(m_uidTarget);
	VALID_RET(pTarget, false);

	pTarget->GetNPCWaitFor().AddWaiter(m_pOwner->GetUID());
	m_pOwner->GetNPCWaitFor().AddWaiting(pTarget->GetUID());

	// 틱설정
	m_rgrIntervalChecker.SetTickTime(1.0f);
	m_rgrIntervalChecker.Start();

	return true;
}

GTaskResult GTask_WaitFor::OnRun( float fDelta )
{
	PFI_BLOCK_THISFUNC(1804);

	if (!m_rgrIntervalChecker.IsReady(fDelta))
		return TR_RUNNING;

	if (m_pOwner->GetNPCWaitFor().IsWaiting())
		return TR_RUNNING;

	return TR_COMPLETED;
}