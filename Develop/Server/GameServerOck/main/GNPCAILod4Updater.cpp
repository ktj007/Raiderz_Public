#include "stdafx.h"
#include "GNPCAILod4Updater.h"
#include "GNPCAI.h"
#include "GAIProfiler.h"

GNPCAILod4Updater::GNPCAILod4Updater(GNPCAI* pSubject)
: GNPCAIUpdater(pSubject)
, m_fCumulateDeltaSeconds(0.0f)
{
	m_UpdateRagulator.SetTickTime(3.0f);
}

GNPCAILod4Updater::~GNPCAILod4Updater()
{
	// do nothing
}

void GNPCAILod4Updater::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_LOD_LEVEL_4, L"LOD 4");

	m_fCumulateDeltaSeconds += fDelta;

	if (m_UpdateRagulator.IsReady(fDelta))
	{
		m_pSubject->UpdateBehaviorFSM(m_fCumulateDeltaSeconds);
		m_pSubject->UpdateJobMgr(m_fCumulateDeltaSeconds);

		m_fCumulateDeltaSeconds = 0.0f;
	}
}

float GNPCAILod4Updater::ResetRemainDelta(void)
{
	float fRemainDelta = m_fCumulateDeltaSeconds;	
	m_fCumulateDeltaSeconds = 0.0f;
	
	return fRemainDelta;
}
