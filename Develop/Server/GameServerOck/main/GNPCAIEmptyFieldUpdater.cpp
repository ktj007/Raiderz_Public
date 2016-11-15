#include "stdafx.h"
#include "GNPCAIEmptyFieldUpdater.h"
#include "GNPCAI.h"
#include "GBehaviorStateMachine.h"
#include "GAIState.h"
#include "GAIProfiler.h"

GNPCAIEmptyFieldUpdater::GNPCAIEmptyFieldUpdater(GNPCAI* pSubject)
: GNPCAIUpdater(pSubject)
{
	// do nothing
}

GNPCAIEmptyFieldUpdater::~GNPCAIEmptyFieldUpdater()
{
	// do nothing
}

void GNPCAIEmptyFieldUpdater::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_LOD_EMPTY_FIELD, L"LOD EmptyField");

	m_pSubject->UpdateBehaviorFSM(fDelta);
	m_pSubject->UpdateJobMgr(fDelta);
}
