#include "StdAfx.h"
#include "GUTHelper_Condition.h"
#include "GConditionsInfo.h"
#include "SUnitTestUtil.h"
#include "GConditionsInfoMgr.h"
#include "GItemConditionsInfoMgr.h"
#include "GConditionInfo.h"
#include "GEntityPlayer.h"

GConditionsInfo* GUTHelper_Condition::NewCondI(GCondition_Leaf* pCondL, bool bItemCondition)
{
	GConditionsInfo* pNewCondI = new GConditionsInfo();
	pNewCondI->Create();
	pNewCondI->m_nID = SUnitTestUtil::NewID();

	if (NULL != pCondL)
	{
		pNewCondI->m_pConditionComposite->m_vecConditionComponent.push_back(pCondL);
	}

	if (true == bItemCondition)
	{
		gmgr.pItemConditionsInfoMgr->Insert(pNewCondI);
	}
	else
	{
		gmgr.pCondtionsInfoMgr->Insert(pNewCondI);
	}	

	return pNewCondI;
}

GCondition_Leaf* GUTHelper_Condition::NewCondLForFail()
{
	return NewCondLForLevel(INT_MAX);
}

GCondition_Leaf* GUTHelper_Condition::NewCondL()
{
	GCondition_Leaf* pNewCondL = new GCondition_Leaf();

	return pNewCondL;
}

GCondition_Leaf* GUTHelper_Condition::NewCondLForLevel(int nMinLevel, int nMaxLevel )
{	
	GCondition_Leaf* pNewCondL = NewCondL();
	pNewCondL->m_nCT = CT_LEVEL;
	pNewCondL->m_vecParam1.push_back(nMinLevel);
	pNewCondL->m_vecParam1.push_back(nMaxLevel);

	return pNewCondL;
}