#pragma once

class GEntityNPC;
class GEntityPlayer;

struct TD_ITEM_DROP;

TEST_FORWARD_FT(LootSystem, FLootSystem, RefreshItem);

class GLootModifyHandler
{
TEST_FRIEND_FT(LootSystem, FLootSystem, RefreshItem);

public:
	TEST_VIRTUAL ~GLootModifyHandler() {}

	TEST_VIRTUAL void HandleDropListModification(GEntityNPC* pNPC, set<int> setPreViewableCID);
	TEST_VIRTUAL void HandleUnviewablePlayer(set<int> setPreViewableCID, set<int> setPostViewableCID, GEntityNPC* pNPC);
	TEST_VIRTUAL void HandleViewablePlayer(set<int> setPreViewableCID, set<int> setPostViewableCID, GEntityNPC* pNPC);	
	TEST_VIRTUAL void HandleEmptyNPC(GEntityNPC* pNPC);

	void RouteInsertLootableNPC(GEntityNPC* pNPC);

private:
	TEST_VIRTUAL void RefreshItem(GEntityNPC* pNPC);	
	void RouteRefreshItem(GEntityPlayer* pPlayer, const vector<TD_ITEM_DROP>& vecTDDropItem);

	void RouteInsertLootableNPC(GEntityPlayer* pPlayer, const MUID& uidNPC);
	void RouteRemoveLootableNPC(GEntityPlayer* pPlayer, const MUID& uidNPC);
};
