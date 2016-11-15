#include "stdafx.h"
#include "GPlayerQuest.h"
#include "GQuestInfo.h"
#include "GPlayerQObjective.h"
#include "GGlueQuest.h"
#include "GSTLUtil.h"
#include "GDef.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GQuestSystem.h"
#include "GQObjectiveInfo.h"
#include "GQuestFailer.h"

GPlayerQuest::GPlayerQuest()
{
	_ASSERT(0 && L"Glue에 바인딩 하기 위한 생성자. 사용하지 마세요.");
}

GPlayerQuest::GPlayerQuest(GQuestInfo* pQuestInfo, GEntityPlayer* pOwner, const uint8 nSlotID, uint32 nAcceptTime, const wstring& strAcceptDt, const wstring& strExpiDt)
: m_pQuestInfo(pQuestInfo)
, m_nVar(0)
, m_nState(QST_EXIST)
, m_nSlotID(nSlotID)
, m_nAcceptTime(nAcceptTime)
, m_strAcceptDt(strAcceptDt)
, m_strExpiDt(strExpiDt)
, m_nLimitTime(NO_LIMIT_TIME)
, m_pOwner(pOwner)
{
	if (NULL == pQuestInfo) return;
	if (NULL == pOwner) return;

	if (pQuestInfo->nTimeLimit != NO_LIMIT_TIME)
	{
		m_nLimitTime = m_nAcceptTime + (pQuestInfo->nTimeLimit*1000);
	}

	vector<GQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetAllQObjectiveInfo();

	for each (GQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)	
	{
		m_mapPlayerQObjective.insert(MAP_PLAYERQOBJECTIVE::value_type(pQObjectiveInfo->nID, new GPlayerQObjective(pQObjectiveInfo)));
	}

	CreateDelegator();
}

GPlayerQuest::~GPlayerQuest()
{
	SAFE_DELETE(m_pDelegator);
	Clear();
}

void GPlayerQuest::Clear()
{
	SAFE_DELETE_MAP(m_mapPlayerQObjective);
}

GPlayerQObjective* GPlayerQuest::Get( int nObjectiveID )
{
	MAP_PLAYERQOBJECTIVE::iterator itor = m_mapPlayerQObjective.find(nObjectiveID);

	if (m_mapPlayerQObjective.end() == itor) return NULL;

	return (*itor).second;	
}

void GPlayerQuest::UpdateQuestVar(int nVar)
{
	m_nVar = nVar;
}

void GPlayerQuest::UpdateQObjective(int nObjectiveID, int nProgress, bool& bChangeToComplete, bool& bChageToIncomplete)
{
	MAP_PLAYERQOBJECTIVE::iterator itor = m_mapPlayerQObjective.find(nObjectiveID);
	if (m_mapPlayerQObjective.end() == itor) return;

	GPlayerQObjective* pPlayerQObjective = (*itor).second;
	if (NULL == pPlayerQObjective) return;

	bool bBeforeComplete = pPlayerQObjective->IsComplete();
	pPlayerQObjective->UpdateProgress(nProgress);	
	bool bAfterComplete = pPlayerQObjective->IsComplete();

	bChangeToComplete  = false == bBeforeComplete && true == bAfterComplete;
	bChageToIncomplete = true == bBeforeComplete && false == bAfterComplete;
}

bool GPlayerQuest::IsQObjectiveUpdatableVar(int nObjectiveID, int nVar, int& nUpdateProgress)
{
	MAP_PLAYERQOBJECTIVE::iterator itor = m_mapPlayerQObjective.find(nObjectiveID);
	if (m_mapPlayerQObjective.end() == itor) return false;

	GPlayerQObjective* pPlayerQObjective = (*itor).second;
	if (NULL == pPlayerQObjective) return false;

	GQObjectiveInfo* pQObjectiveInfo = pPlayerQObjective->GetInfo();
	if (NULL == pQObjectiveInfo) return false;

	int nProgress = pPlayerQObjective->GetProgress();
	nUpdateProgress = pPlayerQObjective->GetProgress();

	switch (pQObjectiveInfo->nType)
	{
	case QOT_INTERACT:
	case QOT_DESTROY:
		{
			VALID_RET(0 <= nVar, false);
			if (true == pPlayerQObjective->IsComplete()) return false;

			if (pQObjectiveInfo->nParam2 - nVar < pPlayerQObjective->GetProgress())
			{
				nUpdateProgress = pQObjectiveInfo->nParam2;
			}
			else
			{
				nUpdateProgress = pPlayerQObjective->GetProgress() + nVar;
			}
		}
		break;
	case QOT_COLLECT:
		{
			if (0 <  nVar && INT_MAX - nVar < pPlayerQObjective->GetProgress())
			{
				nUpdateProgress = INT_MAX;
			}
			else if (0 >  nVar && - nVar > pPlayerQObjective->GetProgress())
			{
				nUpdateProgress = 0;
			}
			else
			{
				nUpdateProgress = pPlayerQObjective->GetProgress() + nVar;
			}			
		}
		break;
	case QOT_ACT:
		{
			if (true == pPlayerQObjective->IsComplete()) return false;
			if (pQObjectiveInfo->vecParam1[0] != nVar) return false;

			nUpdateProgress = 1;
		}
		break;
	case QOT_SCOUT:
	case QOT_ESCORT:
		{
			if (true == pPlayerQObjective->IsComplete()) return false;
			if (pQObjectiveInfo->nParam2 != nVar) return false;

			nUpdateProgress = 1;
		}
		break;	
	default:
		{
			_ASSERT(0);
			return false;
		}
	}

	if (nProgress == nUpdateProgress) return false;

	return true;
}

bool GPlayerQuest::IsAllQObjectiveComplete()
{
	for each (MAP_PLAYERQOBJECTIVE::value_type pair in m_mapPlayerQObjective)
	{
		GPlayerQObjective* pPlayerQObjective = pair.second;
		if (NULL == pPlayerQObjective) continue;

		if (false == pPlayerQObjective->IsComplete())
		{
			return false;
		}
	}

	return true;	
}

void GPlayerQuest::AllQObjectiveComplete()
{
	for each (map<int, GPlayerQObjective*>::value_type val in m_mapPlayerQObjective)
	{
		GPlayerQObjective* pPlayerQObjective = val.second;

		switch (pPlayerQObjective->GetInfo()->nType)
		{
		case QOT_INTERACT:
		case QOT_DESTROY:
		case QOT_COLLECT:
			{
				pPlayerQObjective->UpdateProgress(pPlayerQObjective->GetInfo()->nParam2);
			}
			break;
		case QOT_ACT:
		case QOT_SCOUT:
		case QOT_ESCORT:
			{
				pPlayerQObjective->UpdateProgress(1);
			}
			break;
		}		
	}
}

void GPlayerQuest::Complete()
{
	m_nState = QST_COMPLETE;	 
}

void GPlayerQuest::Incomplete()
{
	m_nState = QST_EXIST;	 
}

void GPlayerQuest::Fail()
{
	m_nState = QST_FAIL;
}

void GPlayerQuest::Update(float fDelta)
{
	if (true == IsFail())	return;
	if (true == IsComplete())	return;
	if (m_nLimitTime == NO_LIMIT_TIME || 
		m_nLimitTime > gsys.pSystem->GetNowTime())	return;

	// 타임오버
	gsys.pQuestSystem->GetFailer().Fail(m_pOwner, m_pQuestInfo->nID);
}

bool GPlayerQuest::IsComplete()
{
	return (QST_COMPLETE == m_nState);
}

bool GPlayerQuest::IsFail()
{
	return (QST_FAIL == m_nState);
}

bool GPlayerQuest::IsActive()
{
	return (QST_FAIL != m_nState);
}

int GPlayerQuest::GetQuestNeedItemQuantity(int nItemID)
{
	int nNeedItemQuantity = 0;

	if (true == IsComplete()) return nNeedItemQuantity;

	for each (const MAP_PLAYERQOBJECTIVE::value_type& data in m_mapPlayerQObjective)
	{
		GPlayerQObjective* pPlayerQObjective = data.second;
		if (NULL == pPlayerQObjective) continue;
		GQObjectiveInfo* pQObjectiveInfo = pPlayerQObjective->GetInfo();
		if (NULL == pQObjectiveInfo) continue;

		if (true == pPlayerQObjective->IsComplete()) continue;
		if (QOT_COLLECT != pQObjectiveInfo->nType) continue;
		if (nItemID != pQObjectiveInfo->vecParam1[0]) continue;

		nNeedItemQuantity += pQObjectiveInfo->nParam2 - pPlayerQObjective->GetProgress();
	}

	return nNeedItemQuantity;
}

vector<pair<int, int>> GPlayerQuest::GetQuestNeedItem()
{
	vector<pair<int,INT>> vecQuestNeedItem;

	for each (const MAP_PLAYERQOBJECTIVE::value_type data in m_mapPlayerQObjective)
	{
		GPlayerQObjective* pPlayerQObjective = data.second;
		if (NULL == pPlayerQObjective) continue;
		GQObjectiveInfo* pQObjectiveInfo = pPlayerQObjective->GetInfo();
		if (NULL == pQObjectiveInfo) continue;

		if (true == pPlayerQObjective->IsComplete()) continue;
		if (QOT_COLLECT != pQObjectiveInfo->nType) continue;

		int nNeedItemID = pQObjectiveInfo->vecParam1[0];
		int nNeedItemQuantity = pQObjectiveInfo->nParam2 - pPlayerQObjective->GetProgress();

		vecQuestNeedItem.push_back(vector<pair<int,INT>>::value_type(nNeedItemID, nNeedItemQuantity));		
	}

	return vecQuestNeedItem;
}

void GPlayerQuest::CreateDelegator()
{
	m_pDelegator = new GGlueQuest(this);
}

GGlueQuest* GPlayerQuest::GetDelegator(void)
{
	VALID_RET(m_pDelegator, NULL);
	return m_pDelegator;
}

QUEST_STATE_TYPE GPlayerQuest::GetState()
{
	return m_nState;
}

wstring& GPlayerQuest::GetAcceptDt()
{
	return m_strAcceptDt;
}

wstring& GPlayerQuest::GetExpiDt()
{
	return m_strExpiDt;
}

uint8 GPlayerQuest::GetSlotID()
{
	return m_nSlotID;
}