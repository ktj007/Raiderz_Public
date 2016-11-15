#include "StdAfx.h"
#include "GSharedFieldNavigator.h"
#include "GDef.h"
#include "GSharedFieldMaster.h"

GSharedFieldNavigator::GSharedFieldNavigator(GSharedFieldMaster* pMaster)
: m_pMaster(pMaster)
, m_nNextChannelID(PRIME_CHANNELID)
{
}

GSharedFieldNavigator::~GSharedFieldNavigator(void)
{
}

CHANNELID GSharedFieldNavigator::GetNewID()
{
	VALID_RET(m_pMaster, INVALID_CHANNELID);
	int nCurrentID = m_nNextChannelID;

	while(nCurrentID<MAX_CHANNEL_SIZE)
	{
		if (!m_pMaster->GetChannel(nCurrentID))
		{
			// 비어있는 채널
			this->m_nNextChannelID = nCurrentID+1;
			return nCurrentID;
		}
		else
		{
			nCurrentID++;
		}
	}

	VALID_RET(false, INVALID_CHANNELID);
}


CHANNELID GSharedFieldNavigator::GetEnterableChannelID()
{
	SHAREDFIELD_MAP& mapChannels = m_pMaster->GetChannels();
	for (SHAREDFIELD_MAP::iterator iter = mapChannels.begin(); 
		iter != mapChannels.end(); 
		++iter)
	{
		CHANNELID nChannelID = (*iter).first;
		GField* pField = (*iter).second;

		if (!pField)
			continue;

		if (pField->IsEnterable())
			return nChannelID;
	}

	return INVALID_CHANNELID; 
}

GSharedField*	GSharedFieldNavigator::GetEnterableChannel()
{
	CHANNELID nJoinableChannel = GetEnterableChannelID();

	if (nJoinableChannel == INVALID_CHANNELID)
	{
		return NULL;
	}

	return m_pMaster->GetChannel(nJoinableChannel);
}
