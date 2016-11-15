#include "StdAfx.h"
#include "ZSharedFieldMaster.h"
#include "ZSharedField.h"

ZSharedFieldMaster::ZSharedFieldMaster(int nFieldID)
: m_nFieldID(nFieldID)
, m_nEnterableChannelCursor(0)
{
}

ZSharedFieldMaster::~ZSharedFieldMaster()
{
}

void ZSharedFieldMaster::Add(ZSharedField* pSharedField)
{
	RVALID(NULL != pSharedField);

	if (IsExistChannel(pSharedField->GetChannelID()))
		return;

	// 정렬되게 삽입합니다.
	ZChannelVector::iterator itr = m_vecSharedField.begin();
	for (size_t i = 0; i < m_vecSharedField.size(); ++i)
	{
		if (pSharedField->GetChannelID() < m_vecSharedField[i]->GetChannelID())
			break;

		++itr;
	}
	m_vecSharedField.insert(itr, pSharedField);

	if (pSharedField->IsDefaultChannel())
		m_setDefaultChannel.insert(pSharedField->GetChannelID());
}

void ZSharedFieldMaster::Remove(int nChannelID)
{
	ZChannelVector::iterator itr = m_vecSharedField.begin();
	for (; itr != m_vecSharedField.end(); ++itr)
	{
		ZSharedField* pField = (*itr);
		if (nChannelID == pField->GetChannelID())
		{
			m_vecSharedField.erase(itr);
			break;
		}
	}
}

bool ZSharedFieldMaster::IsExistChannel(int nChannelID)
{
	return (NULL != FindChannel(nChannelID));
}

bool ZSharedFieldMaster::IsDefaultChannel(int nChannelID)
{
	set<int>::iterator itr = m_setDefaultChannel.find(nChannelID);
	if (itr == m_setDefaultChannel.end()) return false;
	return true;
}

ZSharedField* ZSharedFieldMaster::FindChannel(int nChannelID)
{
	if (nChannelID < 1)
		return NULL;

	ZChannelVector::iterator itr = m_vecSharedField.begin();
	for (; itr != m_vecSharedField.end(); ++itr)
	{
		ZSharedField* pField = (*itr);
		if (nChannelID == pField->GetChannelID())
			return (*itr);
	}
	return NULL;
}

ZSharedField* ZSharedFieldMaster::GetEnterableChannel()
{
	if (0 == m_vecSharedField.size()) return NULL;

	ZSharedField* pField = NULL;

	/// 채널 포화상태가 QUIET 인 채널이 있다면 반환
	for (int i = 0; i < (int)m_vecSharedField.size(); ++i)
	{
		pField = m_vecSharedField[i];
		if (pField->IsQuiet())
			return pField;
	}

	/// 포화상태가 QUIET 인 채널이 없을 경우 입장 가능한 채널을 분산해서 반환
	for (int i = 0; i < (int)m_vecSharedField.size(); ++i)
	{
		if (m_vecSharedField.size() <= m_nEnterableChannelCursor)
			m_nEnterableChannelCursor = 0;

		pField = m_vecSharedField[m_nEnterableChannelCursor++];
		if (pField->IsEnterable())
			return pField;
	}
	return NULL;
}

int ZSharedFieldMaster::GetAllocableChannelID()
{
	int nAllocableChannelID = INVALID_CHANNELID;

	for (int i = 1; i < MAX_CHANNEL_SIZE; ++i)
	{
		if (!IsExistChannel(i) && !IsDefaultChannel(i))
		{
			nAllocableChannelID = i;
			break;
		}
	}
	return nAllocableChannelID;
}
