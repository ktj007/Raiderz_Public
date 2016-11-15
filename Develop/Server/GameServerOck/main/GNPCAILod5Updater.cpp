#include "stdafx.h"
#include "GNPCAILod5Updater.h"
#include "GNPCAI.h"
#include "GAIProfiler.h"

GNPCAILod5Updater::GNPCAILod5Updater(GNPCAI* pSubject)
: GNPCAIUpdater(pSubject)
{
	// do nothing
}

GNPCAILod5Updater::~GNPCAILod5Updater()
{
	// do nothing
}

void GNPCAILod5Updater::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_LOD_LEVEL_5, L"LOD 5");
	// do nothing
}
