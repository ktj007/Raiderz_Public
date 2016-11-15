#include "stdafx.h"
#include "GNPCAILod1Updater.h"
#include "GNPCAI.h"
#include "GAIProfiler.h"

GNPCAILod1Updater::GNPCAILod1Updater(GNPCAI* pSubject)
: GNPCAIUpdater(pSubject)
{
	// do nothing
}

GNPCAILod1Updater::~GNPCAILod1Updater()
{
	// do nothing
}

void GNPCAILod1Updater::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_LOD_LEVEL_1, L"LOD 1");

	m_pSubject->UpdateBehaviorFSM(fDelta);	
	m_pSubject->UpdateJobMgr(fDelta);
}
