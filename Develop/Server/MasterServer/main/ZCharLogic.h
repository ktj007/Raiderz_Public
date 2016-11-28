#pragma once

class ZCharLogic
{
public:
	ZCharLogic(void);
	~ZCharLogic(void);

	void NPCDieReward(const vector<CID>& vecBeneficiaryCID, int nAllBeneficiaryCount, int nNPCID);
};
