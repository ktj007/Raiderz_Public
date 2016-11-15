#include "stdafx.h"
#include "GAIStatistics.h"
#include "GConfig.h"

GAIStatistics g_aiStatistics;

template <typename T>
void GAIStatistics::TotalStatistics::operator() (T& pairFieldStat) const
{
	FieldStat& rFieldStat = pairFieldStat.second;

	for each(map<pair<int, int>, SectorStat>::value_type pairSectorStats in rFieldStat.m_mapSectorStat)
	{
		rFieldStat.m_nTotalNPC += pairSectorStats.second.m_nNPCCount;
		rFieldStat.m_nTotalBattle += pairSectorStats.second.m_nBattleCount;
		rFieldStat.m_nMaxNPC = std::max<int>(rFieldStat.m_nMaxNPC, pairSectorStats.second.m_nNPCCount);
		rFieldStat.m_nMaxBattle = std::max<int>(rFieldStat.m_nMaxBattle, pairSectorStats.second.m_nBattleCount);
	}

	rFieldStat.m_nSectorCount = rFieldStat.m_mapSectorStat.size();
	if (rFieldStat.m_nSectorCount != 0)
	{
		rFieldStat.m_nAverageNPC = rFieldStat.m_nTotalNPC / rFieldStat.m_nSectorCount;
		rFieldStat.m_nAverageBattle = rFieldStat.m_nTotalBattle / rFieldStat.m_nSectorCount;
	}

	m_statWorld.m_nTotalNPC += rFieldStat.m_nTotalNPC;
	m_statWorld.m_nTotalBattle += rFieldStat.m_nTotalBattle;
	m_statWorld.m_nSectorCount += rFieldStat.m_nSectorCount;
	m_statWorld.m_nMaxNPC = std::max<int>(m_statWorld.m_nMaxNPC, rFieldStat.m_nTotalNPC);
	m_statWorld.m_nMaxBattle = std::max<int>(m_statWorld.m_nMaxBattle, rFieldStat.m_nTotalBattle);
	m_statWorld.m_nMaxSectorCount = std::max<int>(m_statWorld.m_nMaxSectorCount, rFieldStat.m_nSectorCount);
}

void GAIStatistics::InitStat(int nFieldId, int sizeX, int sizeY, wstring strFieldName)
{
	if (!GConfig::m_bAutoTestActive)
	{
		return;
	}

	map<int, FieldStat>::iterator itField = m_statWorld.m_mapFieldStats.find(nFieldId);

	if (itField == m_statWorld.m_mapFieldStats.end())
	{
		FieldStat statField;
		statField.m_nFieldID = nFieldId;
		statField.m_strFieldName = strFieldName;
		statField.m_nSectorSizeX = sizeX;
		statField.m_nSectorSizeY = sizeY;

		for(int y = 0; y < sizeY; y++)
		{
			for(int x = 0; x < sizeX; x++)
			{
				SectorStat statSector;
				statSector.m_nX = x;
				statSector.m_nY = y;

				statField.m_mapSectorStat.insert(make_pair(make_pair(x, y), statSector));
			}
		}

		m_statWorld.m_mapFieldStats.insert(make_pair(nFieldId, statField));
	}
}

void GAIStatistics::IncreaseBattleCount(int nFieldId, int x, int y)
{
	if (!GConfig::m_bAutoTestActive)
	{
		return;
	}

	map<int, FieldStat>::iterator itField = m_statWorld.m_mapFieldStats.find(nFieldId);

	if (itField != m_statWorld.m_mapFieldStats.end())
	{
		map<pair<int, int>, SectorStat>::iterator itSector = itField->second.m_mapSectorStat.find(make_pair(x, y));
		
		if (itSector != itField->second.m_mapSectorStat.end())
		{
			itSector->second.m_nBattleCount++;
		}
	}
}

void GAIStatistics::MaxNPCCount(int nFieldId, int x, int y, int nCurrentNPCCount)
{
	if (!GConfig::m_bAutoTestActive)
	{
		return;
	}

	map<int, FieldStat>::iterator itField = m_statWorld.m_mapFieldStats.find(nFieldId);

	if (itField != m_statWorld.m_mapFieldStats.end())
	{
		map<pair<int, int>, SectorStat>::iterator itSector = itField->second.m_mapSectorStat.find(make_pair(x, y));

		if (itSector != itField->second.m_mapSectorStat.end())
		{
			itSector->second.m_nNPCCount = std::max<int>(itSector->second.m_nNPCCount, nCurrentNPCCount);
		}
	}
}

void GAIStatistics::MakeTotalStatistics(void)
{
	if (!GConfig::m_bAutoTestActive)
	{
		return;
	}

	for_each(m_statWorld.m_mapFieldStats.begin(), m_statWorld.m_mapFieldStats.end(), TotalStatistics(m_statWorld));

	m_statWorld.m_nFieldCount = m_statWorld.m_mapFieldStats.size();
	if (m_statWorld.m_nFieldCount != 0)
	{
		m_statWorld.m_nAverageNPC = m_statWorld.m_nTotalNPC / m_statWorld.m_nFieldCount;
		m_statWorld.m_nAverageBattle = m_statWorld.m_nTotalBattle / m_statWorld.m_nFieldCount;
		m_statWorld.m_nAverageSectorCount = m_statWorld.m_nSectorCount / m_statWorld.m_nFieldCount;
	}
}

const WorldStat& GAIStatistics::GetStatatistics(void)
{
	return m_statWorld;
}
