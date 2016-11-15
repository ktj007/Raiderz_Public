#include "stdafx.h"
#include "GNPCAIDefaultUpdater.h"
#include "GNPCAI.h"
#include "GBehaviorStateMachine.h"
#include "GAIState.h"
#include "GAIProfiler.h"

GNPCAIDefaultUpdater::GNPCAIDefaultUpdater(GNPCAI* pSubject)
: GNPCAIUpdater(pSubject)
, m_fTargetCumulateDeltaSeconds(0.0f)
{
	// do nothing
}

GNPCAIDefaultUpdater::~GNPCAIDefaultUpdater()
{
	// do nothing
}

void GNPCAIDefaultUpdater::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_LOD_DEFAULT, L"LOD Default");

	bool isTarget = false;

	if (AI_BEHAVIOR_PEACE == m_pSubject->GetBehaviorFSM()->GetCurrentStateID())
	{
		m_TargetUpdateRegulator.SetTickTime(1.0f);
		isTarget = m_TargetUpdateRegulator.IsReady(fDelta);
	}
	else if (AI_BEHAVIOR_MISSION== m_pSubject->GetBehaviorFSM()->GetCurrentStateID() ||
			AI_BEHAVIOR_SESSION== m_pSubject->GetBehaviorFSM()->GetCurrentStateID())
	{
		m_TargetUpdateRegulator.SetTickTime(0.5f);
		isTarget = m_TargetUpdateRegulator.IsReady(fDelta);
	}

	m_pSubject->UpdateBehaviorFSM(fDelta);
	
	m_fTargetCumulateDeltaSeconds += fDelta;
	if (isTarget)	m_pSubject->UpdateFindEnemy(m_fTargetCumulateDeltaSeconds);
	
	m_pSubject->UpdateJobMgr(fDelta);

	if (isTarget)	m_fTargetCumulateDeltaSeconds = 0.0f;
}
