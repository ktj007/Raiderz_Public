#pragma once

#include "CSConditionInfo.h"

class XConditionHelper
{
	static bool Level( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Quest( CSCondition_Leaf* pCondition, wstring& strText);
	static bool QuestVar( CSCondition_Leaf* pCondition, wstring& strText);
	static bool QuestObjective_Complete( CSCondition_Leaf* pCondition, wstring& strText);
	static bool QuestObjective_Incomplete( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Item( CSCondition_Leaf* pCondition, wstring& strText);
	static bool NoItem( CSCondition_Leaf* pCondition, wstring& strText);
	static bool FewItem( CSCondition_Leaf* pCondition, wstring& strText);
	static bool ManyItem( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Equip( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Race( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Sex( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Date( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Talent( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Talent_Style_TP( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Faction( CSCondition_Leaf* pCondition, wstring& strText);
	static bool Kill( CSCondition_Leaf* pCondition, wstring& strText);

public:
	static bool CheckItemCondition( CSCondition_Leaf* pCondition, wstring& strText );
};
