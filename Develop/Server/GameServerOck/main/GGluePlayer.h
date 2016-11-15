#pragma once

#include "GGlueActor.h"

class GEntityPlayer;
class GGlueActor;
class GGlueNPC;

class GGluePlayer: public GGlueActor, public MTestMemPool<GGluePlayer>
{
public:
	GGluePlayer(GEntityPlayer* pDelegate);
	virtual ~GGluePlayer();

	GEntityPlayer* GetOwnerPlayer() const;

public:
	bool BindSoul(uint8 nSoulBindingID);
	void SetFatigue(int nPercent);
	void Tip(const char* szText);

	bool CheckCondition(int nConditionID);
	
	// 퀘스트관련 -------------------	
	bool UpdateQuestVar(int nQuestID, int nVar);
	int	GetQuestVar(int nQuestID);
	bool IsCompleteQuest(int nQuestID);
	bool AddQuest(int nQuestID);
	void RuinQuest(int nQuestID);

	// 아이템관련 -------------------
	bool AddMoney(int nMoney);
	bool RemoveMoney(int nMoney);
	int GetMoney();
	int GetItemQty(int nItemID);

	// 워프관련 -------------------
	bool GateToTrial(int nDynamicFieldGroupID, bool bSIngleEnter = false);
	bool GateToInnRoom(void);
	bool GateToMarker(int nFieldID, int nMarkerID);
	bool GateToActor(GGlueActor* pActorDelegator);
	bool Gate(int nFieldID, vec3 vPos);

	// 인터랙션관련  -------------------
	GGlueNPC* GetInteractNPC();

	// 투기장관련 -------------------
	bool ArenaRegister();

	// 컷신관련 -------------------
	bool Cutscene(int nCutsceneID);
	bool PartyCutscene(int nCutsceneID);

	// 팩션 관련
	void AddAmity(int nFactionID, int nQuantity);
	void RemoveAmity(int nFactionID, int nQuantity);
	void SetAmity(int nFactionID, int nQuantity);
};
