#pragma once

class GItem;
class GDBT_ITEM_SORT;

class GItemSorter : public MTestMemPool<GItemSorter>
{
public:
	bool Sort(GEntityPlayer* pPlayer);
	void SortForDBTask(const GDBT_ITEM_SORT& data);

private:
	vector<GItem*> GetSortedAllItem(const vector<GItem*>& vecAllItem);

	vector<GItem*>	SelectEquipItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectUsableItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectQuestItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectMaterialItemForSort(set<GItem*>& setItem);
	vector<GItem*>	SelectEtcItemForSort(set<GItem*>& setItem);

	bool MakeDBTaskData(GEntityPlayer* pPlayer, const vector<GItem*>& vecSortedAllItem, GDBT_ITEM_SORT& data);

	void Route(GEntityPlayer* pPlayer);
};
