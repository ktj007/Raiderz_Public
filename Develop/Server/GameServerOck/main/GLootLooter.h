#pragma once

class GEntityPlayer;
class GEntityNPC;
class GDropItem;

struct TD_LOOT_ROLL_ITEM;
struct TD_LOOT_ROLL_RESULT;

enum UNGETTABLE_ITEM_REASON;

typedef map<int, vector<GDropItem*>> MAP_ROLLWINNERITEM;

TEST_FORWARD_FT(LootSystem, FLootSystem, PrepareMasterLootItem);
TEST_FORWARD_FT(LootSystem, FLootSystem, RollItem);
TEST_FORWARD_FT(LootSystem, FLootSystem, PrepareMasterLootItem_NotExistMasterInSameField);
TEST_FORWARD_FT(LootSystem, FLootSystem, RollItem_RouteForOtherServer);

class GLootLooter : public MTestMemPool<GLootLooter>
{
TEST_FRIEND_FT(LootSystem, FLootSystem, PrepareMasterLootItem);
TEST_FRIEND_FT(LootSystem, FLootSystem, RollItem);
TEST_FRIEND_FT(LootSystem, FLootSystem, PrepareMasterLootItem_NotExistMasterInSameField);	
TEST_FRIEND_FT(LootSystem, FLootSystem, RollItem_RouteForOtherServer);

public:
	TEST_VIRTUAL ~GLootLooter() {}

	bool LootItem(GEntityPlayer* pPlayer, const MUID& uidNPC, const vector<MUID>& vecDropItemUID);
	UNGETTABLE_ITEM_REASON GetItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);

private:
	bool LootItem_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<MUID>& vecDropItemUID);
	void LootItem_Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<MUID>& vecDropItemUID);

	TEST_VIRTUAL void RollItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);
	void RollItem_MakeData(GEntityNPC* pNPC, const vector<GDropItem*> &vecDropItem, const set<int>& setBeneficiaryCID, vector<TD_LOOT_ROLL_RESULT> &outvecTDRollResult, vector<TD_LOOT_ROLL_ITEM> &outvecTDRollItem, MAP_ROLLWINNERITEM& outmapRollWinnerItem);
	void RollItem_Route( const set<int>& setBeneficiaryCID, const vector<TD_LOOT_ROLL_ITEM>& vecTDRollItem, const vector<TD_LOOT_ROLL_RESULT>& vecTDRollResult );
	void RollItem_Apply(GEntityNPC* pNPC, const MAP_ROLLWINNERITEM& mapRollWinnerItem);

	TEST_VIRTUAL void PrepareMasterLootItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);

	void PreapareMasterLootItem_Apply( const vector<GDropItem*>& vecDropItem );
	void PrepareMasterLootItem_Route(int nMasterCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem);
};
