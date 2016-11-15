#pragma once

#include "CSTriggerInfo.h"

class GWarpInfoChecker;

class GWarpInfoCollector : public MTestMemPool<GWarpInfoCollector>
{
public:
	typedef pair<GWarpInfoChecker*, bool>	pair_warpinfo;
	typedef vector<pair_warpinfo>			vec_warpinfo;	
	typedef vector<int>						vec_extract;

public:
	enum Result
	{
		FULLHOUSE = -1,		// 입장 권한이 있는 필드가 한개인데 만원
		NOTHING = 0,		// 입장 권한이 있는 필드가 없음
		SINGLE,				// 한개의 입장 권한이 있는 필드
		MULTY				// 둘 이상의 입장 권한이 있는 필드
	};

public:
	GWarpInfoCollector();
	virtual ~GWarpInfoCollector();

	Result CollectAutoWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer);
	Result CollectManualWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer);
	
	int GetWarpCount(void) const;
	int GetFieldGroupID(int nIndex) const;
	int GetFieldID(int nIndex) const;
	int GetMarkerID(int nIndex) const;
	bool IsTrialWarp(int nIndex) const;
	bool IsSharedWarp(int nIndex) const;	
	bool IsManualTrialWarp(int nIndex) const;
	bool IsSingleQuestWarp(int nIndex) const;
	const SENSOR_WARP_INFO* GetSensorWarpInfo(int nIndex) const;
	Result RecheckWarp(int nIndex, GEntityPlayer* pPlayer) const;
	int GetAllowFollowWarpCount(int nIndex, GEntityPlayer* pPlayer) const;

	virtual GWarpInfoChecker* NewWarpInfoChecker(const SENSOR_WARP_INFO& warpInfo);
	void AddWarpInfoChecker(GWarpInfoChecker* pWarpInfoChecker, bool isFullHouse);
	bool CollectAutoTrialWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer);
	bool CollectManualTrialWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer);
	bool CollectAutoSharedWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer);	
	bool CollectManualSharedWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer);	
	Result ExtractWarpField(void);
	void Clear(void);

private:
	void AddExtractField(int nWarpInfoIndex);	
	bool SameFieldChecker(int nWarpInfoIndex1, int nWarpInfoIndex2) const;
	Result GetExtractResult(void) const;
	const GWarpInfoChecker* GetWarpInfoChecker(int nWarpInfoIndex) const;

private:
	vec_warpinfo m_vecWarpInfo;
	vec_extract	m_vecExtract;
};
