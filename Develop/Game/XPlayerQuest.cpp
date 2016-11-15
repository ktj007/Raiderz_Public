#include "stdafx.h"
#include "XPlayerQuest.h"
#include "XQuestInfo.h"
#include "XSystem.h"

XPlayerQObjective::XPlayerQObjective(XQObjectiveInfo* pQObjectiveInfo)
: m_pQObjectiveInfo(pQObjectiveInfo)
, nProgress(0)
, bComplete(false)
{

}

XPlayerQuest::XPlayerQuest(XQuestInfo* pQuestInfo)
: m_pQuestInfo(pQuestInfo)
, m_nState(QST_EXIST)
, m_nAcceptTime(0)
, m_nLimitTime(NO_LIMIT_TIME)
, bChallengerQuest(false)
{	
	vector<XQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetAllQObjectiveInfo();
	for each (XQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)	
	{
		m_mapPlayerQObjective.insert(MAP_PLAYERQOBJECTIVE::value_type(pQObjectiveInfo->nID, new XPlayerQObjective(pQObjectiveInfo)));
	}
}

XPlayerQuest::~XPlayerQuest()
{
	Clear();
}

void XPlayerQuest::Clear()
{
	for_each(m_mapPlayerQObjective.begin(), m_mapPlayerQObjective.end(), DPSO());
	m_mapPlayerQObjective.clear();
}

XPlayerQObjective* XPlayerQuest::Get( int nObjectiveID )
{
	MAP_PLAYERQOBJECTIVE::iterator itor = m_mapPlayerQObjective.find(nObjectiveID);

	if (m_mapPlayerQObjective.end() == itor) return NULL;

	return (*itor).second;	
}

bool XPlayerQuest::UpdateObjective(int nObjectiveID, int nProgress, bool bComplete)
{
	MAP_PLAYERQOBJECTIVE::iterator itor = m_mapPlayerQObjective.find(nObjectiveID);
	if (m_mapPlayerQObjective.end() == itor) return false;

	XPlayerQObjective* pPlayerQObjective = (*itor).second;
	if (NULL == pPlayerQObjective) return false;

	bool bBeforeComplete = pPlayerQObjective->bComplete;

	pPlayerQObjective->nProgress = nProgress;
	pPlayerQObjective->bComplete = bComplete;

	if (false == bBeforeComplete && true == pPlayerQObjective->bComplete)
	{
		bool bAllQObjectiveComplete = true;
		for each (pair<int, XPlayerQObjective*> pairPlayerQObjective in m_mapPlayerQObjective)
		{
			XPlayerQObjective* pOtherPlayerQObjective = pairPlayerQObjective.second;
			if (NULL == pOtherPlayerQObjective) continue;

			if (false == pOtherPlayerQObjective->bComplete)
			{
				bAllQObjectiveComplete = false;
				break;
			}
		}

		if (true == bAllQObjectiveComplete)
		{
			m_nState = QST_COMPLETE;
		}
	}

	return true;
}

uint32 XPlayerQuest::GetRemainTime()
{
	if (m_nLimitTime >= global.system->GetTimer().GetNowGlobalTime())
	{
		return m_nLimitTime-global.system->GetTimer().GetNowGlobalTime();
	}

	return 0;
}

XPlayerQuests::~XPlayerQuests()
{
	Clear();
}

void XPlayerQuests::Clear()
{
	for_each(m_mapPlayerQuest.begin(), m_mapPlayerQuest.end(), DPSO());
	m_mapPlayerQuest.clear();
}

XPlayerQuest* XPlayerQuests::Insert(int nQuestID)
{
	XPlayerQuest* pPlayerQuest = Get(nQuestID);
	_ASSERT(NULL == pPlayerQuest);
	if (NULL != pPlayerQuest)
	{
		mlog("퀘스트 중복 수여\n");
		return NULL;
	}

	XQuestInfo* pQuestInfo =  info.quest->Get(nQuestID);
	if (NULL ==pQuestInfo)
	{
		mlog("퀘스트가 없음\n");
		return NULL;
	}

	XPlayerQuest* pNewPlayerQuest = new XPlayerQuest(pQuestInfo);		
	m_mapPlayerQuest.insert(MAP_PLAYERQUEST::value_type(pQuestInfo->nID, pNewPlayerQuest));

	return pNewPlayerQuest;
}

void XPlayerQuests::Cancel( int nQuestID )
{
	Delete(nQuestID);
}

void XPlayerQuests::Delete(int nQuestID)
{
	MAP_PLAYERQUEST::iterator itor = m_mapPlayerQuest.find(nQuestID);
	if (itor == m_mapPlayerQuest.end()) return;

	XPlayerQuest* pPlayerQuest = (*itor).second;
	if (NULL == pPlayerQuest) return;

	SAFE_DELETE(pPlayerQuest);
	m_mapPlayerQuest.erase(itor);
}

void XPlayerQuests::Reward(int nQuestID)
{
	XPlayerQuest* pPlayerQuest = Get(nQuestID);
	if (NULL == pPlayerQuest) return;

	Delete(nQuestID);
}

XPlayerQuest* XPlayerQuests::Get(int nQuestID)
{
	MAP_PLAYERQUEST::iterator itor = m_mapPlayerQuest.find(nQuestID);
	if (itor == m_mapPlayerQuest.end()) return NULL;

	XPlayerQuest* pPlayerQuest = (*itor).second;
	if (NULL == pPlayerQuest) return NULL;

	return pPlayerQuest;
}

bool XPlayerQuests::GetIndex( size_t nIndex, XPlayerQuest** ppOutQuest)
{
	MAP_PLAYERQUEST::iterator itr = m_mapPlayerQuest.begin();
	for ( size_t i = 0;  i < nIndex;  i++, itr++)
	{
		if ( itr == m_mapPlayerQuest.end())		return false;
	}
	*ppOutQuest = (*itr).second;
	return true;
}

bool XPlayerQuests::IsPlayerQuest(int nQuestID)
{
	if (NULL == Get(nQuestID)) return false;

	return true;
}

const wchar_t* XPlayerQuests::GetQuestIDList(wstring& szText)
{
	szText = L"";
	wchar_t buff[512] = {0,};

	MAP_PLAYERQUEST::iterator itorb = m_mapPlayerQuest.begin();
	MAP_PLAYERQUEST::iterator itore = m_mapPlayerQuest.end();
	for (; itorb != itore; ++itorb)
	{
		XPlayerQuest* pPlayerQuest = (*itorb).second;
		if (NULL == pPlayerQuest) continue;

		_itow_s(pPlayerQuest->m_pQuestInfo->nID, buff, 512, 10);

		if (itorb != m_mapPlayerQuest.begin())
		{
			szText += L",";
		}

		szText += buff;
	}

	return szText.c_str();
}
