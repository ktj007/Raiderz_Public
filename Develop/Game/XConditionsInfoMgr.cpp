#include "StdAfx.h"
#include "XConditionsInfoMgr.h"
#include "XConditionsInfo.h"
#include "XConditionInfo.h"

XConditionsInfoMgr::XConditionsInfoMgr()
{
}

XConditionsInfoMgr::~XConditionsInfoMgr()
{

}

CSCondition_Leaf* XConditionsInfoMgr::New_Condition_Leaf()
{
	return new XCondition_Leaf();
}

CSConditionsInfo* XConditionsInfoMgr::New_ConditionsInfo()
{
	return new XConditionsInfo();
}

CSCondition_Composite* XConditionsInfoMgr::New_Condition_Composite( CONDITION_AND_OR nCAO )
{
	return new XCondition_Composite(nCAO);
}

XConditionsInfo* XConditionsInfoMgr::Get( int nID )
{
	return static_cast<XConditionsInfo*>(CSConditionsInfoMgr::Get(nID));
}

int XConditionsInfoMgr::GetMaxLevel_For_CT_LEVEL( int nID )
{
	XConditionsInfo* pConds = Get(nID);
	if (pConds && pConds->m_pConditionComposite)
	{
		CSCondition_Leaf* pCondLeaf = dynamic_cast<CSCondition_Leaf*>(GetFirstLeaf(pConds->m_pConditionComposite, CT_LEVEL));
		if (pCondLeaf)
		{
			return pCondLeaf->GetMaxLevel_For_CT_LEVEL();
		}
	}

	return INT_MAX;
}

int XConditionsInfoMgr::GetMinLevel_For_CT_LEVEL( int nID )
{
	XConditionsInfo* pConds = Get(nID);
	if (pConds && pConds->m_pConditionComposite)
	{
		CSCondition_Leaf* pCondLeaf = dynamic_cast<CSCondition_Leaf*>(GetFirstLeaf(pConds->m_pConditionComposite, CT_LEVEL));
		if (pCondLeaf)
		{
			return pCondLeaf->GetMinLevel_For_CT_LEVEL();
		}
	}
	return 0;
}

CSCondition_Leaf* XConditionsInfoMgr::GetFirstLeaf( CSCondition_Composite* pCondition_Composite, CONDITION_TYPE nCondType )
{
	for (size_t i = 0; i < pCondition_Composite->m_vecConditionComponent.size(); i++)
	{
		CSCondition_Component* pCond = pCondition_Composite->m_vecConditionComponent[i];
		if (pCond->GetType() == CCT_LEAF)
		{
			CSCondition_Leaf* pCondLeaf = dynamic_cast<CSCondition_Leaf*>(pCond);
			if (pCondLeaf == NULL) continue;

			if (pCondLeaf->m_nCT == nCondType)
			{
				return pCondLeaf;
			}
		}
	}
	return NULL;
}

XItemConditionsInfoMgr::XItemConditionsInfoMgr() : XConditionsInfoMgr()
{

}

XItemConditionsInfoMgr::~XItemConditionsInfoMgr()
{

}