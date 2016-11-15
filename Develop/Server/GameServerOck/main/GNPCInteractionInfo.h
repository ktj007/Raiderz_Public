#ifndef _G_INTERACTION_INFO_H_
#define _G_INTERACTION_INFO_H_

class GConditionsInfo;
enum CONDITION_TYPE;
enum ITRIGGER_TYPE;
enum INTERACTION_TYPE;
enum ITARGET_CONDITION_TYPE;

bool IsIEGatherType(INTERACTION_TYPE eType);

class GNPCInteractionElementInfo : public MTestMemPool<GNPCInteractionElementInfo>
{
public:
	int m_nID;
	GConditionsInfo*		m_pConditionsInfo;
	INTERACTION_TYPE		m_nType;
	ITARGET_CONDITION_TYPE	m_nTargetCondition;
	vector<int> m_vecAct;
	int m_nUseItemID;

public:
	GNPCInteractionElementInfo();
	~GNPCInteractionElementInfo();

	int GetSinlgeAct();
	ITRIGGER_TYPE GetTriggerType();

};

typedef vector<GNPCInteractionElementInfo*> VEC_NPC_IEINFO;

class GNPCInteractionInfo : public MTestMemPool<GNPCInteractionInfo>
{
public:
	VEC_NPC_IEINFO					m_vecNPCIEInfo;

public:
	GNPCInteractionInfo();
	~GNPCInteractionInfo();

	void Clear();

	bool HasCondition(CONDITION_TYPE nConditionType);
	bool HasQuestIE();
	bool HasIE(INTERACTION_TYPE nIEType);

	void SortByInteractionType();	
};


#endif//_G_NPC_INTERACTION_INFO_H_
