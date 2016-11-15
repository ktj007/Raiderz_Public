#pragma once

class GEntityPlayer;
class GEntityNPC;

class GLootShower : public MTestMemPool<GLootShower>
{
public:
	bool Show(GEntityPlayer* pPlayer, const MUID& uidNPC);

private:
	bool Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC);
	bool Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC);
	void Route(GEntityPlayer* pPlayer, GEntityNPC* pNPC);
};

class GLootMoneyDistributor
{
public:
	bool Distribute(GEntityPlayer* pPlayer, GEntityNPC* pNPC);

private:
	int CalcDistributedMoney(GEntityNPC* pNPC, set<int> setMoneyAuthorizedCID);
	bool DB(set<int> setMoneyAuthorizedCID, int nDistributedMoney, int nNpcID);
	void Server(GEntityNPC* pNPC);
};


