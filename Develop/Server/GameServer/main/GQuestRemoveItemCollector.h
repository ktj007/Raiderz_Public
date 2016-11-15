#pragma once

class GQuestInfo;
class GEntityPlayer;
struct GITEM_STACK_AMT;

class GQuestRemoveItemCollector : public MTestMemPool<GQuestRemoveItemCollector>
{
public:
	void CollectForCancel(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt);
	bool CollectForReward(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt);
	void CollectForGive(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt);

private:
	void CollectDestroyItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt);		// 퀘스트 제거될때 삭제되는 아이템
	void CollectAddItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt);			// 퀘스트 시작할때 받은 아이템
	void CollectRemoveItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt);		// 퀘스트 시작할때 제거되는 아이템
	bool CollectQObjectiveItem(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GITEM_STACK_AMT>& vecItemStackAmt);	// 퀘스트 목적 아이템

	void FilterSameItem(vector<GITEM_STACK_AMT>& vecItemStackAmt);
	bool CheckAlreadyExistItem(vector<GITEM_STACK_AMT>& vecItemStackAmt, const int64 nIUID );
};
