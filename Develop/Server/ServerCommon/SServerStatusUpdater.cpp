#include "StdAfx.h"
#include "SServerStatusUpdater.h"
#include "MTime.h"


SServerStatusUpdater::SServerStatusUpdater()
{
	m_pServerStatusUpdateRglt = new MRegulator(MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC);
}

SServerStatusUpdater::~SServerStatusUpdater()
{
	SAFE_DELETE(m_pServerStatusUpdateRglt);
}

void SServerStatusUpdater::Update(const float fDelta)
{
	if (!m_pServerStatusUpdateRglt->IsReady(fDelta))
		return;

	OnUpdate(fDelta);
}