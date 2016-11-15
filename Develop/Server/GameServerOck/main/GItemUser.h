#pragma once

class GEntityPlayer;
class GEntityNPC;
class GItem;
class GItemData;
class GCondition_Leaf;



class GItemUser : public MTestMemPool<GItemUser>
{
public:
	bool Use(GEntityPlayer* pPlayer, int nInvenSlotID, const MUID uidNPC = MUID(0, 0));
	void ConsumeUsableItem(GEntityPlayer* pPlayer, MUID nUsedTalentItemUID);
	void ConsumeUsableItem(GEntityPlayer* pPlayer, GItem* pItem);
	void RouteFail(GEntityPlayer* pPlayer, int nInvenSlotID, GCondition_Leaf* pFailConditionLeaf=NULL);

private:
	bool Check(GEntityPlayer* pPlayer, GItem* pItem, GCondition_Leaf*& pFailConditionLeaf);

	bool Apply(GEntityPlayer* pPlayer, GItem* pItem, const MUID uidNPC);	
	bool Apply_Equip(GEntityPlayer* pPlayer, GItem* pItem);
	bool Apply_Usable(GEntityPlayer* pPlayer, GItem* pItem, const MUID uidNPC);

	bool Apply_Interaction(GEntityPlayer* pPlayer, GItem* pItem, const MUID uidNPC);
	bool Apply_TalentUse(GEntityPlayer* pPlayer, GItem* pItem);
	bool Apply_QuestAdd(GEntityPlayer* pPlayer, GItem* pItem);
	bool Apply_TalentTrain(GEntityPlayer* pPlayer, GItem* pItem);
	bool Apply_TalentUntrain(GEntityPlayer* pPlayer, GItem* pItem);


	// 아이템 인터랙션
	GEntityNPC* GetInteractionNPC(GEntityPlayer* pPlayer, GItem* pItem, const MUID& uidNPC, const bool IsLive);	
};
