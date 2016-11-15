#pragma once

enum SH_ITEM_SLOT_TYPE;
enum SH_ITEM_SLOT;

struct GDBT_ITEM_EQUIP;
class GEntityPlayer;
class GItemHolder;
class GItem;

class GItemEquiperForDBTask : public MTestMemPool<GItemEquiperForDBTask>
{
public:
	void Equip(GDBT_ITEM_EQUIP& data);	
	void Fail(GDBT_ITEM_EQUIP& data);

private:
	void EquipItem(GEntityPlayer* pPlayer, GItemHolder* pItemHolder, GItem* pEquipItem, const GDBT_ITEM_EQUIP &data, bool bRouteToMe=true);
	void UnequipItem1(GEntityPlayer* pPlayer,  GItemHolder* pItemHolder, const GDBT_ITEM_EQUIP &data, bool bRoute=true);
	void UnequipItem2(GEntityPlayer* pPlayer, GItemHolder* pItemHolder, const GDBT_ITEM_EQUIP &data);

	void Bind(GItem* pEquipItem);

	void RouteToMe(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotTypeFrom, int nSlotIDFrom, SH_ITEM_SLOT nSlotIDTo);
	void RouteToOther(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotTypeFrom, int nSlotIDFrom, SH_ITEM_SLOT nSlotIDTo, int nItemID, int nColor, int nEnchantBufFID);
	void RouteEquipSwapToInven(GEntityPlayer* pPlyaer, const GDBT_ITEM_EQUIP &data);
};
