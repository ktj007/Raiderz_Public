#pragma once

class GEntityNPC;
class GDropItem;

enum UNGETTABLE_ITEM_REASON;

TEST_FORWARD_FT(LootSystem, FLootSystem, HandleUngettableItem_OtherServer);
TEST_FORWARD_FT(LootSystem, FLootSystem, HandleUngettableItem_OtherField);

class GLootItemGetter : public MTestMemPool<GLootItemGetter>
{
TEST_FRIEND_FT(LootSystem, FLootSystem, HandleUngettableItem_OtherServer);
TEST_FRIEND_FT(LootSystem, FLootSystem, HandleUngettableItem_OtherField);

public:
	TEST_VIRTUAL ~GLootItemGetter() {}

	TEST_VIRTUAL UNGETTABLE_ITEM_REASON GetItem(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);
	void GetItemForDBTask(const MUID& nPlayerUID, const MUID& nNPCUID, const MUID& nDropItemUID, int16 nSlotID, int16 nToStackAmt, int16 nStackModAmt, int64 nIUID);

private:
	void _GetItem(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);
	void DB(GEntityPlayer* pPlayer, GEntityNPC* pNPC,  const vector<GDropItem*>& vecDropItem);
	void RemoveDropItem(GEntityNPC* pNPC,  const vector<GDropItem*>& vecDropItem);
	void Route(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);

	map<int,int> CompressDropItem(const vector<GDropItem*>& vecDropItem);
		
	void HandleUngettableItem(UNGETTABLE_ITEM_REASON nUIR, int nCID, const vector<GDropItem*>& vecDropItem);
	UNGETTABLE_ITEM_REASON CheckGettable(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);	
};
