#pragma once

class GDBT_STORAGE_SERIALIZE;
class GDBT_STORAGE_UPDATE_MONEY;

class GStorageSystem : public MTestMemPool<GStorageSystem>
{
public:
	bool Show(GEntityPlayer* pPlayer);
	bool DepositMoney(GEntityPlayer* pPlayer, int nMoney);
	bool WithdrawMoney(GEntityPlayer* pPlayer, int nMoney);

public:
	void ShowForDBTask(GDBT_STORAGE_SERIALIZE& data);
	void DepositMoneyForDBTask(GDBT_STORAGE_UPDATE_MONEY& data);
	void WithdrawForDBTask(GDBT_STORAGE_UPDATE_MONEY& data);

private:
	void RouteShow(GEntityPlayer* pPlayer);
};
