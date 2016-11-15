#include "StdAfx.h"
#include "GHateTableLocker.h"


GHateTableLocker::GHateTableLocker()
: m_bEnable(true)
{

}

bool GHateTableLocker::IsEnable()
{
	return m_bEnable;
}

void GHateTableLocker::Enable( bool bEnable )
{
	m_bEnable = bEnable;
}

void GHateTableLocker::Lock( MUID uidTarget, float fDurationTime )
{
	if (!m_bEnable)
		return;

	m_uidTarget = uidTarget;
	m_rgrReleaser.SetTickTime(fDurationTime);
	m_rgrReleaser.Start();
}

void GHateTableLocker::Unlock()
{
	if (!m_bEnable)
		return;

	m_uidTarget.SetInvalid();
	m_rgrReleaser.Stop();
}

bool GHateTableLocker::IsLock() const
{
	if (!m_bEnable)
		return false;

	return m_uidTarget.IsValid();
}

MUID GHateTableLocker::GetTarget() const
{
	return m_uidTarget;
}

void GHateTableLocker::Update( float fDelta )
{
	if (!m_bEnable)
		return;

	if (!m_rgrReleaser.IsReady(fDelta))
		return;

	Unlock();
}

