#pragma once

class XGlueGameQuestFunction
{
public:
	// Äù½ºÆ®
	int						GetQuestLevel(int nQuestID);
	int						GetQuestLimitPersonnel(int nQuestID);
	//const char*				GetQuestName(int nQuestID);
	//const char*				GetQuestDesc(int nQuestID);
	//bool					IsQuestManualShare(int nQuestID);

	//int						GetCountQuestObjectives( int nQuestID );
	//int						GetQuestObjectiveAchive(int nQuestID, unsigned int nCount);
	int						GetQuestObjectiveID(int nQuestID, unsigned int nCount);
	//const char*				GetQuestObjectiveDesc(int nQuestID, unsigned int nCount);
	const char*				GetQuestObjectiveZoneID(int nQuestID, unsigned int nCount);
	const char*				GetZoneID(int nFieldID);

	void					GiveUpQuest( int nQuestID );
	//void					ShareQuest( int nQuestID );

	int						GetRewardCount(int nQuestID);
	int						GetSelectableRewardCount(int nQuestID);
	const char*				GetRewardTypeNParam(int nQuestID, int index);
	const char*				GetSelectableRewardTypeNParam(int nQuestID, int index);
	const char*				GetTypeNParam(VEC_QUEST_REWARD& vecReward, int nQuestID, int index);

	void					QuestAccept(int nQuestID);
	void					QuestReject();
	void					QuestReward(int nQuestID, int nSelectedReward);

	const char*				QuestAbandonDestroyItem(int nQuestID );

	void					QuestObjectivePos(int nQuestID, int nObjectiveID);
	void					QuestInteractionCancelReq();
};
