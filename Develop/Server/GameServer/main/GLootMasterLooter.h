#pragma once

class GEntityPlayer;
class GEntityNPC;
class GDropItem;

class GLootMasterLooter : public MTestMemPool<GLootMasterLooter>
{
public:
	bool MasterLootItem(GEntityPlayer* pPlayer, const MUID& uidNPC, const MUID& nDropItemUID, uint8 nSelectedIndex, bool bRandom);

private:
	bool Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GDropItem* pDropItem, uint8 nSelectedIndex, bool bRandom);
	void Apply(GEntityNPC* pNPC, GDropItem* pDropItem, uint8 nSelectedIndex, bool bRandom);

	int SelectAssignCID(GEntityNPC* pNPC, uint8 nSelectedIndex, bool bRandom);
};
