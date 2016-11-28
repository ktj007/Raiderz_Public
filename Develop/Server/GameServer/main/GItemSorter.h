#pragma once

class GItem;
class GDBT_ITEM_SORT;
class GDBT_ITEM_SORT_SLOT;
class GDBT_ITEM_MERGE_ALL_SLOT;
struct TD_ITEM;
struct TD_ITEM_INVENTORY_SORT;

class GItemSorter : public MTestMemPool<GItemSorter>
{
public:
	bool Sort(GEntityPlayer* pPlayer);
	bool SortByTD(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<TD_ITEM_INVENTORY_SORT>& vectdItemSort);
	void SortForDBTask(const GDBT_ITEM_SORT& data);

private:
	bool SortByTD_Check(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<TD_ITEM_INVENTORY_SORT>& vectdItemSort) const;

	vector<GItem*> GetSortedAllItem(const vector<GItem*>& vecAllItem);

	vector<GItem*>	SelectEquipItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectUsableItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectQuestItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectMaterialItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectEtcItemForSort(set<GItem*>& setItem);

	bool MakeDBTaskData(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<GItem*>& vecSortedAllItem, int nStartSlotIndex, GDBT_ITEM_SORT& data);
	vector<GDBT_ITEM_MERGE_ALL_SLOT> MakeDBTaskData_MergeAll(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, const vector<GDBT_ITEM_SORT_SLOT>& vecSortSlot);

	int SumStackAmount(const vector<pair<const GDBT_ITEM_SORT_SLOT&, GItem*>>& vecItem) const;

	void Route(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nSlotType, int nInvenIndex, const vector<TD_ITEM>& vectdItem);
	vector<TD_ITEM> MakeTD(const map<int, GItem*>& mapSortedItem);
};
