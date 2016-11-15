#pragma once

class GEntityPlayer;
class GEntityNPC;
class GNPCInteractionElementInfo;
class GCondition_Leaf;
enum INTERACTION_TYPE;
enum ITRIGGER_TYPE;

enum UNGATHERABLE_REASON
{
	UR_NONE,
	UR_NPCTYPE,
	UR_TOOL,
	UR_TRAINING,
	UR_HIGHERTRAINING,

	UR_MAX
};

TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestBegin_ExistDoingQuest);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestBegin_ExistCompleteQuest);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestBegin_ExistDoneQuest);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestBegin);

TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestEnd_NotExistQuest);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestEnd_NotExistIncompleteDialog);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestEnd);

TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestInteract_NotExistQuest);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestInteract_ExistCompleteQObjective);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestInteract_ExistFailQuest);
TEST_FORWARD_FT(NPCIEChecker, Fixture, QuestInteract);

TEST_FORWARD_FT(NPCIEChecker, Fixture, NPCShop_BadFaction);
TEST_FORWARD_FT(NPCIEChecker, Fixture, NPCShop_GoodFaction);
TEST_FORWARD_FT(NPCIEChecker, Fixture, NPCShop_NotExistFaction);

TEST_FORWARD_FT(NPCIEChecker, Fixture, GuildStorage_NotJoinedGuild);
TEST_FORWARD_FT(NPCIEChecker, Fixture, GuildStorage);

class GNPCIEChecker : public MTestMemPool<GNPCIEChecker>
{
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestBegin_ExistDoingQuest);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestBegin_ExistCompleteQuest);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestBegin_ExistDoneQuest);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestBegin);	

TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestEnd_NotExistQuest);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestEnd_NotExistIncompleteDialog);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestEnd);

TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestInteract_NotExistQuest);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestInteract_ExistCompleteQObjective);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestInteract_ExistFailQuest);
TEST_FRIEND_FT(NPCIEChecker, Fixture, QuestInteract);

TEST_FRIEND_FT(NPCIEChecker, Fixture, NPCShop_BadFaction);
TEST_FRIEND_FT(NPCIEChecker, Fixture, NPCShop_GoodFaction);
TEST_FRIEND_FT(NPCIEChecker, Fixture, NPCShop_NotExistFaction);

TEST_FRIEND_FT(NPCIEChecker, Fixture, GuildStorage_NotJoinedGuild);
TEST_FRIEND_FT(NPCIEChecker, Fixture, GuildStorage);


public:
	vector<GNPCInteractionElementInfo*> FilterIElementInfo(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GNPCInteractionElementInfo*>& vecIElementInfo, ITRIGGER_TYPE nITrigType, MUID uidUseItem);

	bool Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo);
	bool Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo, bool& bAutoCheckPass, vector<GCondition_Leaf*>& vecFailConditionLeaf);	

	bool Gather(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nGatherTier, UNGATHERABLE_REASON& outnUR);
	bool Gather(GEntityPlayer* pPlayer, GEntityNPC* pNPC, INTERACTION_TYPE nIETypeGathering, UNGATHERABLE_REASON& outnUR);
	bool Gather(GEntityPlayer* pPlayer, GEntityNPC* pNPC, INTERACTION_TYPE nIETypeGathering);

private:
	bool QuestBegin(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIElementInfo);
	bool QuestEnd(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIElementInfo);

	bool QuestInteract(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo);
	bool NPCShop(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIElementInfo);	
	bool GuildStorage(GEntityPlayer* pPlayer);

	bool CheckAuto(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo);
	bool CheckITargetCondition(GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo);	
};
