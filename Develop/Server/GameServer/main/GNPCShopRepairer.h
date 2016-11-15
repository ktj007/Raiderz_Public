#pragma once

class GTestSystem;
class GEntityPlayer;
class GItem;
class GNPCShopInfo;

class GNPCShopRepairer : public MTestMemPool<GNPCShopRepairer>
{
friend GTestSystem;

public:
	bool RepairOne(GEntityPlayer* pPlayer, int nNPCShopID, int8 slotType, int nSlotID);
	bool RepairAll(GEntityPlayer* pPlayer, int nNPCShopID);	
	bool RepairAllForGM(GEntityPlayer* pPlayer);

	void Route(GEntityPlayer* pPlayer, const vector<pair<int,int>>& vecSlotInfo);

private:
	bool RepairOne_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItem* pItem, int& outnRepairPrice);	
	bool RepairAll_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, const vector<GItem*>& vecItem, int& outnRepairPrice);
	
	bool CheckRepairableItem( GItem* pItem, GEntityPlayer* pPlayer );
	bool CheckRepairableNPC(GNPCShopInfo* pNPCShopInfo);
	bool CheckMoney(GEntityPlayer* pPlayer, int nRepairPrice);

	vector<GItem*> CollectRepairableItem(GEntityPlayer* pPlayer);

	int CalcRepairPrice(GItem* pItem);
	int CalcRepairPrice(vector<GItem*> vecItem);
};
