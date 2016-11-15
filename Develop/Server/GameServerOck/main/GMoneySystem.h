#pragma once

class GMoneySystem : public MTestMemPool<GMoneySystem>
{
public:
	GMoneySystem(void);
	~GMoneySystem(void);

	bool Increase(GEntityPlayer* pPlayer, int nMoney);
	bool Decrease(GEntityPlayer* pPlayer, int nMoney);

	void IncreaseForDBTask(GEntityPlayer* pPlayer, int nMoney);	
	void DecreaseForDBTask(GEntityPlayer* pPlayer, int nMoney);	

	void RouteUpdateMoney( GEntityPlayer* pPlayer);
};
