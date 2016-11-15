#pragma once

#ifndef _PUBLISH
	#define AI_STAT_INIT(filedId,sectorSizeX,sectorSizeY,fieldName)	g_aiStatistics.InitStat(filedId, sectorSizeX, sectorSizeY, fieldName)
	#define AI_STAT_BATTLE_INC(fieldId,sectorX,sectorY)				g_aiStatistics.IncreaseBattleCount(fieldId, sectorX, sectorY)
	#define AI_STAT_NPC_MAX(fieldId,sectorX,sectorY,maxNpc)			g_aiStatistics.MaxNPCCount(fieldId, sectorX, sectorY, maxNpc)
#else
	#define AI_STAT_BATTLE_INC(fieldId,sectorX,sectorY)
	#define AI_STAT_NPC_MAX(fieldId,sectorX,sectorY,maxNpc)
	#define AI_STAT_INIT(filedId,sectorSizeX,sectorSizeY,fieldName)
#endif


struct SectorStat
{
	int m_nX;
	int m_nY;
	int m_nNPCCount;
	int m_nBattleCount;

	SectorStat()
		: m_nX(0), m_nY(0)
		, m_nNPCCount(0), m_nBattleCount(0)
	{}
};

struct FieldStat
{
	int m_nFieldID;
	wstring m_strFieldName;	
	int m_nSectorCount;
	int m_nSectorSizeX;
	int m_nSectorSizeY;
	int m_nTotalNPC;
	int m_nAverageNPC;	
	int m_nMaxNPC;	
	int m_nTotalBattle;
	int m_nAverageBattle;
	int m_nMaxBattle;	

	map<pair<int, int>, SectorStat> m_mapSectorStat;

	FieldStat()
		: m_nFieldID(0)
		, m_nSectorCount(0), m_nSectorSizeX(0), m_nSectorSizeY(0)
		, m_nTotalNPC(0), m_nAverageNPC(0), m_nMaxNPC(0)
		, m_nTotalBattle(0), m_nAverageBattle(0), m_nMaxBattle(0)
	{}
};

struct WorldStat
{
	int m_nFieldCount;
	int m_nSectorCount;
	int m_nAverageSectorCount;
	int m_nMaxSectorCount;	
	int m_nTotalNPC;	
	int m_nAverageNPC;
	int m_nMaxNPC;
	int m_nTotalBattle;
	int m_nAverageBattle;
	int m_nMaxBattle;

	map<int, FieldStat> m_mapFieldStats;

	WorldStat()
		: m_nFieldCount(0)
		, m_nSectorCount(0), m_nAverageSectorCount(0), m_nMaxSectorCount(0)
		, m_nTotalNPC(0), m_nAverageNPC(0), m_nMaxNPC(0)
		, m_nTotalBattle(0), m_nAverageBattle(0), m_nMaxBattle(0)
	{}
};


class GAIStatistics
{
public:
	struct TotalStatistics
	{
		WorldStat& m_statWorld;
		TotalStatistics(WorldStat& statWorld): m_statWorld(statWorld)	{}
		template <typename T> void operator() (T& pairFieldStat) const;
	};

public:
	void InitStat(int nFieldId, int sizeX, int sizeY, wstring strFieldName);
	void IncreaseBattleCount(int nFieldId, int x, int y);
	void MaxNPCCount(int nFieldId, int x, int y, int nCurrentNPCCount);
	void MakeTotalStatistics(void);
	const WorldStat& GetStatatistics(void);

private:
	WorldStat m_statWorld;
};

extern GAIStatistics g_aiStatistics;
