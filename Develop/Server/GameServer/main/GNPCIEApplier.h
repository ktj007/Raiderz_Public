#ifndef _G_IE_APPLIER_H_
#define _G_IE_APPLIER_H_

class GEntityPlayer;
class GNPCInteractionElementInfo;
enum INTERACTION_TYPE;

class GNPCIEApplier : public MTestMemPool<GNPCIEApplier>
{
private:
	bool Dialog(GEntityPlayer* pPlayer, int nDialogID);
	bool Trade(GEntityPlayer* pPlayer, int nTradeID);
	bool QuestBegin(GEntityPlayer* pPlayer, int nQuestID);
	bool QuestEnd(GEntityPlayer* pPlayer, int nQuestID);
	bool QuestInteractMortal(GEntityPlayer* pPlayer, int nQuestID);
	bool QuestInteractImmortal(GEntityPlayer* pPlayer, int nQuestID);
	bool LootMortal(GEntityPlayer* pPlayer, int nLootID);
	bool LootImmortal(GEntityPlayer* pPlayer, int nLootID);
	bool Craft(GEntityPlayer* pPlayer, int nCraftID);
	bool InnSleep(GEntityPlayer* pPlayer, int nDialogID);
	bool SoulBinding(GEntityPlayer* pPlayer, uint8 nSoulBindingID);
	bool ChallengerQuest(GEntityPlayer* pPlayer);
	bool Gather(GEntityPlayer* pPlayer, INTERACTION_TYPE nGatheringTierIndex, int nLootID);
	bool Mailbox(GEntityPlayer* pPlayer);
	bool Trigger(GEntityPlayer* pPlayer, const vector<int>& vecAct);
	bool GuildCreate(GEntityPlayer* pPlayer);
	bool GuildDestroy(GEntityPlayer* pPlayer);
	bool Storage(GEntityPlayer* pPlayer);
	bool GuildStorage(GEntityPlayer* pPlayer);

public:
	bool Apply(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIEInfo);
};

#endif
