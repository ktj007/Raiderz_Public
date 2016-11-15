#pragma once

#include "CSConditionInfo.h"

class CSCondition_Leaf;
class CSCondition_Composite;
class CSCondition_Component;

class XConditionStringMaker
{
protected:
	CONDITION_AND_OR	m_nCAO;

	bool Level( CSCondition_Leaf* pCondition, wstring& strText );
	bool Quest( CSCondition_Leaf* pCondition, wstring& strText );
	bool QuestVar( CSCondition_Leaf* pCondition, wstring& strText );
	bool QuestObjective_Complete( CSCondition_Leaf* pCondition, wstring& strText );
	bool QuestObjective_Incomplete( CSCondition_Leaf* pCondition, wstring& strText );
	bool Item( CSCondition_Leaf* pCondition, wstring& strText );
	bool NoItem( CSCondition_Leaf* pCondition, wstring& strText );
	bool FewItem( CSCondition_Leaf* pCondition, wstring& strText );
	bool ManyItem( CSCondition_Leaf* pCondition, wstring& strText );
	bool Equip( CSCondition_Leaf* pCondition, wstring& strText );
	bool Race( CSCondition_Leaf* pCondition, wstring& strText );
	bool Sex( CSCondition_Leaf* pCondition, wstring& strText );
	bool Date( CSCondition_Leaf* pCondition, wstring& strText );
	bool Talent( CSCondition_Leaf* pCondition, wstring& strText );
	bool Talent_Style_TP( CSCondition_Leaf* pCondition, wstring& strText );
	bool Faction( CSCondition_Leaf* pCondition, wstring& strText );
	bool Kill( CSCondition_Leaf* pCondition, wstring& strText );

	bool CheckLeaf( CSCondition_Leaf* pCondition, wstring& strText );
	bool CheckComposite( CSCondition_Composite* pCondition, wstring& strText);

public:
	bool Check( CSCondition_Component* pCondition, wstring& strText);

	void Init()	{ m_nCAO = CAO_NONE;}
};
