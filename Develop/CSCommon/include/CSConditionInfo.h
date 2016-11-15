#pragma once

#include "CSCommonLib.h"

#include <vector>
using std::vector;

enum CONDITION_QUEST_STATE
{
	CQS_NONE	= 0,
	CQS_NOTEXIST,
	CQS_EXIST,
	CQS_NOTCOMPLETE,
	CQS_COMPLETE,
	CQS_NOTDONE,
	CQS_DONE,
	CQS_ACTIVE,	

	CQS_MAX
};

enum CONDITION_TYPE
{
	CT_NONE = 0,
	CT_LEVEL,
	CT_QUEST,
	CT_QUEST_VAR,
	CT_QUEST_OBJECTIVE_COMPLETE,
	CT_QUEST_OBJECTIVE_INCOMPLETE,
	CT_ITEM,
	CT_NOITEM,
	CT_FEWITEM,
	CT_MANYITEM,
	CT_EQUIP,
	CT_RACE,
	CT_SEX,
	CT_FACTION,
	CT_TITLE,
	CT_DATE,
	CT_TALENT,
	CT_TALENT_STYLE_TP,
	CT_KILL,
	CT_QPVP_TEAM,

	CT_MAX
};

enum CONDITION_AND_OR
{
	CAO_NONE = 0,
	CAO_AND,
	CAO_OR,

	CAO_MAX
};

enum CONDITION_CLASS_TYPE
{
	CCT_NONE = 0,
	CCT_LEAF,
	CCT_COMPOSITE,

	CCT_MAX
};

class CSCOMMON_API CSCondition_Component
{
public:
	CSCondition_Component();
	virtual ~CSCondition_Component();

	virtual CONDITION_CLASS_TYPE GetType() = 0;
};

class CSCOMMON_API CSCondition_Leaf : public CSCondition_Component
{
public:
	CONDITION_TYPE	m_nCT;
	vector<INT>		m_vecParam1;
	vector<INT>		m_vecParam2;

public:
	CSCondition_Leaf();
	virtual ~CSCondition_Leaf();

	CONDITION_CLASS_TYPE GetType();

	// for CT_LEVEL
	bool IsValid_For_CT_LEVEL() { return true; }
	int GetMinLevel_For_CT_LEVEL();
	int GetMaxLevel_For_CT_LEVEL();

	// for CT_QUEST
	bool IsValid_For_CT_QUEST();
	int GetQuestID_For_CT_QUEST();
	CONDITION_QUEST_STATE GetQuestState_For_CT_QUEST();

	// for CT_QUEST_VAR
	bool IsValid_For_CT_QUEST_VAR();
	int GetQuestID_For_CT_QUEST_VAR();
	int GetVar_For_CT_QUEST_VAR();
};


class CSCOMMON_API CSCondition_Composite : public CSCondition_Component
{
public:
	vector<CSCondition_Component*> m_vecConditionComponent;
	CONDITION_AND_OR m_nCAO;

public:
	CSCondition_Composite(CONDITION_AND_OR nCAO);
	virtual ~CSCondition_Composite();

	CONDITION_CLASS_TYPE GetType();
};
