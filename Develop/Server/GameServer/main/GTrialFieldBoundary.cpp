#include "StdAfx.h"
#include "GTrialFieldBoundary.h"
#include "GTrialField.h"
#include "GFieldGridRouter.h"
#include "CCommandTable.h"
#include "GCommand.h"

static const float TRIALFIELD_BOUNDARY_EXPIRED_TIME = 30.0f;

GTrialFieldBoundary::GTrialFieldBoundary(GTrialField* pOwner)
: m_pOwner(pOwner)
, m_bExpired(false)
{
	m_pOwner->GetGrid().CreateBoundChecker(this);
	m_rgrExpired.SetTickTime(TRIALFIELD_BOUNDARY_EXPIRED_TIME);
	m_rgrExpired.Stop();
}

GTrialFieldBoundary::~GTrialFieldBoundary(void)
{
}

void GTrialFieldBoundary::OnChangeOutters( int nPlayerQtyInBoundary )
{
	if (nPlayerQtyInBoundary > 0)
	{
		StartExpired();
	}
	else
	{
		StopExpired();
	}
}

void GTrialFieldBoundary::Update( float fDelta )
{
	Update_Expired(fDelta);
}

void GTrialFieldBoundary::StartExpired()
{
	if (!m_rgrExpired.IsActive())
	{
		m_rgrExpired.Start();
	}
}

void GTrialFieldBoundary::StopExpired()
{
	m_rgrExpired.Reset();
	m_rgrExpired.Stop();
}

void GTrialFieldBoundary::Update_Expired( float fDelta )
{
	if (!m_rgrExpired.IsReady(fDelta))
		return;

	m_bExpired = true;
}

bool GTrialFieldBoundary::IsExpired() const
{
	return m_bExpired;
}

