#pragma once

class GNPCInfo;

class GExpSystem
{
public:
	virtual bool RewardExpByNPCKill(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, GNPCInfo* pNPCInfo);
	void AddExp(GEntityPlayer* pPlayer, int nEXP, int nNpcID);
	void AddExpForDB(GEntityPlayer* pPlayer, int nEXP);	
	void RouteAddExp(GEntityPlayer* pPlayer, int nEXP);

private:
	int CalculSumLevel(const vector<GEntityPlayer*>& vecSameServerBeneficiary);	
};
