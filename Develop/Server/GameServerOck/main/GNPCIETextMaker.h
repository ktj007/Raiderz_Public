#ifndef _G_NPC_IE_TEXT_MAKER_H_
#define _G_NPC_IE_TEXT_MAKER_H_

class GEntityPlayer;
enum INTERACTION_TYPE;

class GNPCIETextMaker : public MTestMemPool<GNPCIETextMaker>
{
private:
	wstring Dialog(int nIEAct);
	wstring NPCShop(int nIEAct);
	wstring QuestBegin(GEntityPlayer* pPlayer, int nIEAct);
	wstring QuestEnd(GEntityPlayer* pPlayer, int nIEAct);
	wstring Craft(int nIEAct);
	wstring InnSleep(int nIEAct);
	wstring SoulBinding(int nIEAct);
	wstring ChallengeQuest(int nIEAct);

public:
	wstring Make(GEntityPlayer* pPlayer, INTERACTION_TYPE nIEType, int nIEAct);
};

#endif
