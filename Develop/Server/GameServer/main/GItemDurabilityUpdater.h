#ifndef _G_ITEM_DURABILITY_UPDATER_H_
#define _G_ITEM_DURABILITY_UPDATER_H_

#define MAX_DURABILITY_LOST_RATE_WEAPON			2.0f	/// 무기의 최대 내구도 손상확률
#define MAX_DURABILITY_LOST_RATE_EQUIPMENT		2.5f	/// 방어구의 최대 내구도 손상확률
#define MAX_DURABILITY_LOST_RATE_EQUIPMENT_CRI	5.0f	/// 크리에 의한 방어구의 최대 내구도 손상확률


class GItem;
class GEntityPlayer;
class GEntityActor;

class GItemDurabilityUpdater : public MTestMemPool<GItemDurabilityUpdater>
{
private:
	bool LostWeaponDurabilityByCombat(GEntityPlayer* pAttacker, int nLevelFactor);
	bool LostEquipmentDurabilityByCombat(GEntityPlayer* pDefender, int nLevelFactor, bool bCritical, bool bTryGuard);

	bool Check(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability);
	bool ApplyDB(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability);
	bool ApplyServer(const vector<GItem*>& vecItem, const vector<int>& vecDurability);
	void Route(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem);

public:
	bool LostDurabilityByCombat(GEntityActor* pAttackerActor, GEntityActor* pDefenderActor, bool bCritical, bool bTryGuard);
	bool LostDurabilityByDie(GEntityPlayer* pVictim);	

	bool Update(GEntityPlayer* pPlayer, GItem* pItem, int nDurability);
	bool Update(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability);
	bool ApplyServerAndRoute(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability);
};

#endif
