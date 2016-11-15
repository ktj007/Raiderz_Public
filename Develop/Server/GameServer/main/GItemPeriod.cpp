#include "stdafx.h"
#include "GItemPeriod.h"


GItemPeriod::GItemPeriod() : m_bPeriod(false), m_nPeriodSec(0), m_nUsageStartCharPlayTimeSec(0), m_bStartCalculatingUsagePeriod(false)
{

}

GItemPeriod::~GItemPeriod()
{

}

void GItemPeriod::Set( const bool bPeriod, const bool bStartCalculatingUsagePeriod, const int nPeriodSec, const int nUsageStartCharPlayTimeSec )
{
	m_bStartCalculatingUsagePeriod		= bStartCalculatingUsagePeriod;
	m_bPeriod							= bPeriod;
	m_nPeriodSec						= nPeriodSec;
	m_nUsageStartCharPlayTimeSec		= nUsageStartCharPlayTimeSec;
}

bool GItemPeriod::IsExpired( const int nCharPlayTimeSec ) const
{
	if (!m_bPeriod)
		return false;

	if (!m_bStartCalculatingUsagePeriod)
		return false;

	if (0 < GetRemainTimeSec(nCharPlayTimeSec))
		return false;

	return true;
}

bool GItemPeriod::IsStartedCalculatingUsagePeriod()
{
	return m_bStartCalculatingUsagePeriod;
}

int GItemPeriod::GetRemainTimeSec(const int nCharPlayTimeSec) const
{
	return m_nPeriodSec - (nCharPlayTimeSec - m_nUsageStartCharPlayTimeSec);
}

int GItemPeriod::GetPeriodSec() const
{
	return m_nPeriodSec;
}

bool GItemPeriod::IsPeriod() const
{
	return m_bPeriod;
}

void GItemPeriod::StartCalculatingUsagePeriod()
{
	m_bStartCalculatingUsagePeriod = true;
}

int GItemPeriod::GetUsageStartCharPlayTimeSec()
{
	return m_nUsageStartCharPlayTimeSec;
}
