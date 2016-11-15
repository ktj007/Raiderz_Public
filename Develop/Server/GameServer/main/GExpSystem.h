#pragma once

class GNPCInfo;

class GExpSystem
{
public:
	virtual bool RewardExpByNPCKill(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, GNPCInfo* pNPCInfo);
	void AddExp(GEntityPlayer* pPlayer, int nEXP, int nNpcID);
	void AddItemExp(GEntityPlayer* pPlayer, int nEXP, int nNpcID);
	void AddExpForDB(GEntityPlayer* pPlayer, int nEXP);	
	void AddItemExpForDB(GEntityPlayer* pPlayer, int nEXP, int nNextAttuneXP);
	void RouteAddExp(GEntityPlayer* pPlayer, int nEXP);
	void RouteItemAddExp(GEntityPlayer* pPlayer, int nEXP, int nNextAttuneXP);

private:
	int CalculSumLevel(const vector<GEntityPlayer*>& vecSameServerBeneficiary);	
};
