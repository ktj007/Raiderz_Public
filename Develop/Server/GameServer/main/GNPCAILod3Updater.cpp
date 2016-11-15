#include "stdafx.h"
#include "GNPCAILod3Updater.h"
#include "GNPCAI.h"
#include "GBehaviorStateMachine.h"
#include "GAIState.h"
#include "GAIProfiler.h"

GNPCAILod3Updater::GNPCAILod3Updater(GNPCAI* pSubject)
: GNPCAIUpdater(pSubject)
{
	// do nothing
}

GNPCAILod3Updater::~GNPCAILod3Updater()
{
	// do nothing
}

void GNPCAILod3Updater::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_LOD_LEVEL_3, L"LOD 3");

	m_pSubject->UpdateBehaviorFSM(fDelta);
	
	if (AI_BEHAVIOR_PEACE == m_pSubject->GetBehaviorFSM()->GetCurrentStateID() || 
		AI_BEHAVIOR_MISSION== m_pSubject->GetBehaviorFSM()->GetCurrentStateID() ||
		AI_BEHAVIOR_SESSION== m_pSubject->GetBehaviorFSM()->GetCurrentStateID())
	{
		m_pSubject->UpdateFindEnemy(fDelta);
	}
	
	m_pSubject->UpdateJobMgr(fDelta);
}
