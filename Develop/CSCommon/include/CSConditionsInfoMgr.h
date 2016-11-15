#pragma once

#include "MXml.h"
#include "CSCommonLib.h"
#include <map>
using std::map;

class CSConditionsInfo;
class CSCondition_Component;
class CSCondition_Leaf;
class CSCondition_Composite;
enum CONDITION_AND_OR;

#define CONDITIONS_XML_TAG_MAIET			"maiet"
#define CONDITIONS_XML_TAG_CONDITIONS		"CONDITIONS"
#define CONDITIONS_XML_TAG_CONDITION		"CONDITION"
#define CONDITIONS_XML_TAG_AND				"AND"
#define CONDITIONS_XML_TAG_OR				"OR"
#define CONDITIONS_XML_ATTR_ID				"id"
#define CONDITIONS_XML_ATTR_VALIDATION		"validation"
#define CONDITIONS_XML_ATTR_TYPE			"type"
#define CONDITIONS_XML_ATTR_PARAM1			"param1"
#define CONDITIONS_XML_ATTR_PARAM2			"param2"

typedef map<int, CSConditionsInfo*> MAP_CONDITIONSINFO;

class CSCOMMON_API CSConditionsInfoMgr
{
protected:
	MAP_CONDITIONSINFO	m_mapConditionsInfo;

	virtual CSCondition_Leaf* New_Condition_Leaf() = 0;
	virtual CSConditionsInfo* New_ConditionsInfo() = 0;
	virtual CSCondition_Composite* New_Condition_Composite(CONDITION_AND_OR nCAO) = 0;
private:
	void ParseConditions(MXmlElement* pElement, MXml* pXml);

	void ParseAnd(MXmlElement* pElement, MXml* pXml, CSConditionsInfo* pConditionsInfo, vector<CSCondition_Component*>& vecConditionComponent);
	void ParseOr(MXmlElement* pElement, MXml* pXml, CSConditionsInfo* pConditionsInfo, vector<CSCondition_Component*>& vecConditionComponent);
	void ParseCondition(MXmlElement* pElement, MXml* pXml, CSConditionsInfo* pConditionsInfo, vector<CSCondition_Component*>& vecConditionComponent);
	void ParseCondition_Level(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Quest(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_QuestVar(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_QuestObjective_Complete(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_QuestObjective_Incomplete(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Item(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_NoItem(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_FewItem(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_ManyItem(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Equip(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);	
	void ParseCondition_Race(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Sex(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Faction(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Title(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Date(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Talent(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Talent_Style_TP(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_Kill(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);
	void ParseCondition_QPVP_Team(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf);

public:
	CSConditionsInfoMgr();
	virtual ~CSConditionsInfoMgr();

	void Clear();

	bool Load(const TCHAR* szFileName);
	virtual void Insert(CSConditionsInfo* pConditionInfo);
	CSConditionsInfo* Get(int nID);
	bool IsConditionsID(int nID);
	MAP_CONDITIONSINFO& GetContainer();
	
};


