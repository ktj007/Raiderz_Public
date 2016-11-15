#include "stdafx.h"
#include "PmRegionStat.h"
#include "MDebug.h"

PmRegionStat::PmRegionStat()
: m_vecStat(MAX_PRC * MAX_PAC * MAX_PGC)
, m_nTotalUserCount(0)
, m_vecStatPC(MAX_PRC * MAX_PAC * MAX_PGC)
{
	m_mapTotalUserCountPC.insert(make_pair(0, 0));
}

void PmRegionStat::AddUser(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender, int nPCCafeID)
{
	if (!IsValid(nRegion, nAge, nGender))	return;


	size_t nIndex = CalcIndex(nRegion, nAge, nGender);

	VALID(nIndex < m_vecStat.size());

	if (m_vecStat[nIndex] < LONG_MAX)
	{
		m_vecStat[nIndex]++;
		m_nTotalUserCount++;
	}

	if (0 < nPCCafeID)
	{
		if (m_vecStatPC[nIndex] < LONG_MAX)
		{
			m_vecStatPC[nIndex]++;
			IncreasePCCafeUserCount(nPCCafeID);
		}
	}
}

void PmRegionStat::RemoveUser(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender, int nPCCafeID)
{
	if (!IsValid(nRegion, nAge, nGender))	return;


	size_t nIndex = CalcIndex(nRegion, nAge, nGender);

	VALID(nIndex < m_vecStat.size());

	if (0 <	m_vecStat[nIndex])
	{
		m_vecStat[nIndex]--;
		m_nTotalUserCount--;
	}

	if (0 < nPCCafeID)
	{
		if (0 < m_vecStatPC[nIndex])
		{
			m_vecStatPC[nIndex]--;
			DecreasePCCafeUserCount(nPCCafeID);			
		}
	}
}

size_t PmRegionStat::CalcIndex(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender) const
{
	size_t nIndex =	MAX_PGC * MAX_PAC * nRegion
					+ MAX_PGC * nAge
					+ nGender;

	return nIndex;
}

bool PmRegionStat::IsValid(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender) const
{
	if (nRegion < PRC_GYEONGGI	|| MAX_PRC <= nRegion)	return false;
	if (nAge	< PAC_0TO13		|| MAX_PAC <= nAge)		return false;
	if (nGender < PGC_MALE		|| MAX_PGC <= nGender)	return false;

	return true;
}

int PmRegionStat::GetTotalUserCount() const
{
	return m_nTotalUserCount;
}

VLONG* PmRegionStat::GetRegionStat()
{
	return &m_vecStat;
}

VLONG* PmRegionStat::GetRegionStatPC()
{
	return &m_vecStatPC;
}

map<int, int>& PmRegionStat::GetTotalUserCountPC()
{
	return m_mapTotalUserCountPC;
}

void PmRegionStat::IncreasePCCafeUserCount(int nPCCafeID)
{
	map<int, int>::iterator it = m_mapTotalUserCountPC.find(nPCCafeID);
	
	if (it == m_mapTotalUserCountPC.end())
	{
		m_mapTotalUserCountPC.insert(make_pair(nPCCafeID, 0));
	}

	if (m_mapTotalUserCountPC[0] < INT_MAX)
	{
		m_mapTotalUserCountPC[0]++;
		m_mapTotalUserCountPC[nPCCafeID]++;
	}
}

void PmRegionStat::DecreasePCCafeUserCount(int nPCCafeID)
{
	map<int, int>::iterator it = m_mapTotalUserCountPC.find(nPCCafeID);

	if (it != m_mapTotalUserCountPC.end())
	{
		if (0 < m_mapTotalUserCountPC[nPCCafeID])
		{
			m_mapTotalUserCountPC[nPCCafeID]--;
		}
	}

	if (0 < m_mapTotalUserCountPC[0])
	{
		m_mapTotalUserCountPC[0]--;		
	}
}
