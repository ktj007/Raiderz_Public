#pragma once

class GEntityPlayer;
class GParty;
class GQuestInfo;
enum QUEST_SHARE_TYPE;
enum QUEST_SHARE_RESULT;

TEST_FORWARD_FT(QuestSharer, Fixture, SelectShareResult_Distance);
TEST_FORWARD_FT(QuestSharer, Fixture, SelectShareResult_Condition);
TEST_FORWARD_FT(QuestSharer, Fixture, SelectShareResult_Exist);
TEST_FORWARD_FT(QuestSharer, Fixture, SelectShareResult_Done);

class GQuestSharer : public MTestMemPool<GQuestSharer>
{
	TEST_FRIEND_FT(QuestSharer, Fixture, SelectShareResult_Distance);
	TEST_FRIEND_FT(QuestSharer, Fixture, SelectShareResult_Condition);
	TEST_FRIEND_FT(QuestSharer, Fixture, SelectShareResult_Exist);
	TEST_FRIEND_FT(QuestSharer, Fixture, SelectShareResult_Done);

public:
	bool Share(GEntityPlayer* pPlayer, int nQuestID);	
	bool IsSharableQuest(GEntityPlayer* pPlayer, int nQuestID);

private:
	bool Check(GEntityPlayer* pPlayer, int nQuestID);
	void Route(GEntityPlayer* pPlayer, GParty* pParty, GQuestInfo* pQuestInfo);
	QUEST_SHARE_RESULT SelectShareResult( GEntityPlayer* pPlayer, GEntityPlayer* pMember, GQuestInfo* pQuestInfo );

	bool HaveQuestSharableMember(GEntityPlayer* pPlayer, int nQuestID);
};
