#ifndef _G_PLAYER_QUESTS_H_
#define _G_PLAYER_QUESTS_H_

#include "CSDef_Quest.h"

class GPlayerQuest;
class GEntityPlayer;

typedef map<int, GPlayerQuest*>	MAP_PLAYER_QUEST;	// 현재 진행중인 퀘스트
typedef map<int, int>			MAP_DONE_COUNT;		// 한번이라도 받은적이 있는 퀘스트의 완료 횟수가 저장된다.
													// 단, 완료를 한번도 한적이 없으면 완료 횟수는 0이다.
													// !!완료 횟수 저장안하기로 함. 횟수는 의미가 없음.
													
static const uint8 INVALID_QUSET_SLOT_ID	= 255;

class GPlayerQuests : public MTestMemPool<GPlayerQuests>
{
private:
	MAP_PLAYER_QUEST	m_mapDoingQuest;
	MAP_DONE_COUNT		m_mapDoneCount;
	GEntityPlayer*		m_pOwner;
	bool				m_bSlot[MAX_QUEST_INSTACNE_COUNT];

public:
	GPlayerQuests(GEntityPlayer* pOwner);
	virtual	~GPlayerQuests();

	void				Clear();
	void				ClearDoneCount();
	MAP_PLAYER_QUEST&	GetQuestContainer()		{ return m_mapDoingQuest; }
	MAP_DONE_COUNT&		GetDoneCountContainer()	{ return m_mapDoneCount; }

	bool			Has(int nQuestID);
	GPlayerQuest*	Get(int nQuestID);
	GPlayerQuest*	Insert(const uint8 nSlotID, int nQuestID, const wstring& strAcceptDt, const wstring& strExpiDt);
	void			Cancel(int nQuestID);	
	void			Update(float fDelta);
	void			Done(int nQuestID);

	bool			IsDoingQuest(int nQuestID);
	bool			IsDoingQuest(const vector<int>& vecQuestID);
	bool			IsCompleteQuest(int nQuestID);
	bool			IsDoneQuest(int nQuestID);
	bool			IsDBInsert(int nQuestID);

	int				GetQuestNeedItemQuantity(const vector<int>& vecQuestID, int nItemID);	

	int				GetDoneCount(int nQuestID);

	void			MakeActiveQuestID(vector<int>* pVecQuestID) const;
	uint8			FindAndMarkingEmptySlot();
	void			ResetSlot(const uint8 nSlotID);
	bool			IsValidSlotID(const uint8 nSlotID);
	bool			SlotMarking(const uint8 nSlotID);
};

#endif
