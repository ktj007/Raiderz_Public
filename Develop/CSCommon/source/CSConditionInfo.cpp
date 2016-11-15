#include "stdafx.h"
#include "CSConditionInfo.h"
#include "AStlUtil.h"


CSCondition_Component::CSCondition_Component()
{

}

CSCondition_Component::~CSCondition_Component()
{

}

CSCondition_Leaf::CSCondition_Leaf()
: m_nCT(CT_NONE)
{

}


CSCondition_Leaf::~CSCondition_Leaf()
{

}

CONDITION_CLASS_TYPE CSCondition_Leaf::GetType()
{
	return CCT_LEAF;
}

int CSCondition_Leaf::GetMinLevel_For_CT_LEVEL()
{
	return (true == m_vecParam1.empty()) ? 0 : m_vecParam1[0];
}

int CSCondition_Leaf::GetMaxLevel_For_CT_LEVEL()
{
	return (true == m_vecParam2.empty()) ? INT_MAX : m_vecParam2[0];
}

bool CSCondition_Leaf::IsValid_For_CT_QUEST()
{
	if (true == m_vecParam1.empty()) return false;
	if (true == m_vecParam2.empty()) return false;

	return true;
}

int CSCondition_Leaf::GetQuestID_For_CT_QUEST()
{
	return m_vecParam1[0];
}

CONDITION_QUEST_STATE CSCondition_Leaf::GetQuestState_For_CT_QUEST()
{
	return static_cast<CONDITION_QUEST_STATE>(m_vecParam2[0]);
}

bool CSCondition_Leaf::IsValid_For_CT_QUEST_VAR()
{
	if (true == m_vecParam1.empty()) return false;
	if (true == m_vecParam2.empty()) return false;

	return true;
}

int CSCondition_Leaf::GetQuestID_For_CT_QUEST_VAR()
{
	return m_vecParam1[0];
}

int CSCondition_Leaf::GetVar_For_CT_QUEST_VAR()
{
	return m_vecParam2[0];
}
//////////////////////////////////////////////////////////////////////////
CSCondition_Composite::CSCondition_Composite( CONDITION_AND_OR nCAO )
: m_nCAO(nCAO)
{

}

CSCondition_Composite::~CSCondition_Composite()
{
	SAFE_DELETE_VECTOR(m_vecConditionComponent);
}

CONDITION_CLASS_TYPE CSCondition_Composite::GetType()
{
	return CCT_COMPOSITE;
}