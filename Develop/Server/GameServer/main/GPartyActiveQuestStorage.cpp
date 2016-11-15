#include "stdafx.h"
#include "GPartyActiveQuestStorage.h"

GPartyActiveQuestStorage::GPartyActiveQuestStorage()
{
	// do nothing
}

GPartyActiveQuestStorage::~GPartyActiveQuestStorage()
{
	// do nothing
}

bool GPartyActiveQuestStorage::HasQuest( int nQuestID ) const
{
	MAP_ACTIVE_QUEST::const_iterator it = m_mapActiveQuests.find(nQuestID);
	return it != m_mapActiveQuests.end();
}

void GPartyActiveQuestStorage::AddQuest( int nQuestID )
{
	MAP_ACTIVE_QUEST::iterator iter = m_mapActiveQuests.find(nQuestID);
	if (iter == m_mapActiveQuests.end())
	{
		m_mapActiveQuests.insert(MAP_ACTIVE_QUEST::value_type(nQuestID, 1));
	}
	else
	{
		iter->second += 1;
	}
}

void GPartyActiveQuestStorage::AddQuest(int nQuestID, int nCount)
{
	_ASSERT(0 <= nCount);

	MAP_ACTIVE_QUEST::iterator iter = m_mapActiveQuests.find(nQuestID);
	if (iter == m_mapActiveQuests.end())
	{
		m_mapActiveQuests.insert(MAP_ACTIVE_QUEST::value_type(nQuestID, nCount));
	}
	else
	{
		iter->second += nCount;
	}
}

void GPartyActiveQuestStorage::RemoveQuest( int nQuestID )
{
	MAP_ACTIVE_QUEST::iterator iter = m_mapActiveQuests.find(nQuestID);
	if (iter != m_mapActiveQuests.end())
	{
		int& nCount = iter->second;
		nCount--;
		if (nCount <= 0)
		{
			m_mapActiveQuests.erase(iter);
		}
	}
}

void GPartyActiveQuestStorage::ClearQuest(void)
{
	m_mapActiveQuests.clear();
}
