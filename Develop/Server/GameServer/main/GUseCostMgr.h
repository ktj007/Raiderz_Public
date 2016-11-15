#pragma once

class GEntityActor;
class GTalentInfo;
class GBuffInfo;

class GUseCostMgr
{
public:
	GUseCostMgr(void);
	~GUseCostMgr(void);

	// 탤런트 사용 가능한지 여부 반환
	bool IsUseTalent(GEntityActor* pUser, GTalentInfo* pTalentInfo);
	// 버프 유지 가능한지 여부 반환
	bool IsMaintenanceBuff(GEntityActor* pUser, GBuffInfo* pTalentInfo);
	// 탤런트 사용 비용을 지불함, 지불이 불가능하면 false를 반환
	bool Pay_TalentCost(GEntityActor* pUser, GTalentInfo* pTalentInfo);
	// 탤런트 사용 비용을 강제로 지불함
	void Pay_TalentCostForce(GEntityActor* pUser, GTalentInfo* pTalentInfo);
	//	버프 유지 비용을 지불함, 지불이 불가능하면 false를 반환
	bool Pay_BuffMaintenanceCost(GEntityActor* pUser, GBuffInfo* pBuffInfo, int nTickCounter);
};
