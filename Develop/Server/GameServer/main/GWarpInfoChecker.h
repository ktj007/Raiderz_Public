#pragma once

struct SENSOR_WARP_INFO;
class GEntityPlayer;

class GWarpInfoChecker : public MTestMemPool<GWarpInfoChecker>
{
public:
	enum Result
	{
		FULLHOUSE = -1,		// 해당 필드가 만원
		FAILURE = 0,		// 입장할 권한 없음
		SUCCESS,			// 입장할 권한 있음
	};

public:
	GWarpInfoChecker(const SENSOR_WARP_INFO& warpInfo);
	virtual ~GWarpInfoChecker();

	virtual bool CheckCondition(GEntityPlayer* pPlayer) const;
	virtual Result CheckQuestField(GEntityPlayer* pPlayer) const;	

	bool IsTrialWarp(void) const;
	bool IsSharedWarp(void) const;
	bool IsAutoWarp(void) const;
	bool IsManualWarp(void) const;
	bool IsAutoTrialWarp(void) const;
	bool IsManualTrialWarp(void) const;
	bool IsAutoSharedWarp(void) const;
	bool IsManualSharedWarp(void) const;
	bool IsSingleQuestWarp(void) const;
	int GetQuestLimitPersonnel(void) const;
	int GetFieldID(void) const;
	int GetFieldGroupID(void) const;
	int GetMarkerID(void) const;
	const SENSOR_WARP_INFO* GetSensorWarpInfo(void) const;

	bool SameFieldWarp(const GWarpInfoChecker* pChecker) const;	
	int GetAllowEnterPlayerCount(GEntityPlayer* pPlayer) const;

private:
	const SENSOR_WARP_INFO& m_warpInfo;	
};
