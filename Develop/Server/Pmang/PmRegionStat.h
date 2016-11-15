#pragma once

#include "PmLib.h"
#include "PmDef.h"

class PMANG_API PmRegionStat
{
public:
	PmRegionStat();

	void AddUser(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender, int nPCCafeID);
	void RemoveUser(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender, int nPCCafeID);

	// 전체 (PC방 포함)
	int GetTotalUserCount() const;
	VLONG* GetRegionStat();
	
	// PC방
	map<int, int>& GetTotalUserCountPC();
	VLONG* GetRegionStatPC();

private:
	size_t CalcIndex(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender) const;
	bool IsValid(PmRegionCode nRegion, PmAgeCode nAge, PmGenderCode nGender) const;

	void IncreasePCCafeUserCount(int nPCCafeID);
	void DecreasePCCafeUserCount(int nPCCafeID);

private:
	int m_nTotalUserCount;
	VLONG m_vecStat;

	VLONG m_vecStatPC;	
	map<int, int> m_mapTotalUserCountPC;
};
