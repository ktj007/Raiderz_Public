#include "stdafx.h"
#include "GSharedFieldMaster.h"
#include "CCommandTable.h"
#include "GConst.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GServer.h"
#include "GSharedFieldNavigator.h"


//////////////////////////////////////////////////////////////////////////
// 
// GSharedFieldMaster
//
//////////////////////////////////////////////////////////////////////////

GSharedFieldMaster::GSharedFieldMaster(GFieldInfo* pFieldInfo)
: m_pNavigator(NULL)
, m_nFieldID(pFieldInfo->m_nFieldID)
, m_nDefaultChannelCount(pFieldInfo->m_nDefaultChannelCount)
{
	m_WeatherMgr.Init(pFieldInfo, GConst::CONST_ENV_WEATHER_TICK_SEC);
	m_pNavigator = new GSharedFieldNavigator(this);
}

GSharedFieldMaster::~GSharedFieldMaster()
{
	SAFE_DELETE(m_pNavigator);
}

void GSharedFieldMaster::Clear()
{
	for (SHAREDFIELD_MAP::iterator itor = m_ChannelMap.begin(); 
		itor != m_ChannelMap.end(); 
		++itor)
	{
		GField* pField = (*itor).second;
		pField->Destroy();
		delete pField;
	}
	m_ChannelMap.clear();
}

GSharedField* GSharedFieldMaster::CreateSharedField( MUID uidNewField )
{
	GSharedField* pNewField = new GSharedFieldMLeaf(uidNewField, &m_WeatherMgr);
	if (!pNewField->Create(m_nFieldID))
		return NULL;

	return pNewField;
}

bool GSharedFieldMaster::AddChannel( GSharedField* pField, CHANNELID nNewChannelID )
{
	if (nNewChannelID == INVALID_CHANNELID)
		return false;

	VALID_RET(!GetChannel(nNewChannelID), false);
	pField->SetChannelID(nNewChannelID);

	m_ChannelMap.insert(SHAREDFIELD_MAP::value_type(nNewChannelID, pField));

	return true;
}
GSharedField* GSharedFieldMaster::DeleteChannel(int nChannelID)
{
	if (nChannelID <= GetDefaultChannelCount())		return NULL;	// 최소 채널수
	if (nChannelID == PRIME_CHANNELID)				return NULL;	// 프라임 채널

	GSharedField* pChannel = this->GetChannel(nChannelID);

	if (pChannel == NULL)
		return NULL;

	m_ChannelMap.erase(nChannelID);

	return pChannel;
}

CHANNELID GSharedFieldMaster::GetEnterableChannelID()
{
	return m_pNavigator->GetEnterableChannelID();
}

void GSharedFieldMaster::Update(float fDelta)
{
	PFI_B(84, "GSharedField::Update - GSharedField:Update_Weather");
	if (m_WeatherMgr.Update(fDelta))
	{
		for each (const SHAREDFIELD_MAP::value_type& data in m_ChannelMap)
		{
			GField* pField = data.second;
			if (NULL == pField) continue;

			pField->OnWeatherChanged();
		}
	}
	PFI_E(84);
}


SHAREDFIELD_MAP& GSharedFieldMaster::GetChannels()
{
	return m_ChannelMap;
}

void GSharedFieldMaster::GetChannels(vector<int>& vecChannelID)
{
	SHAREDFIELD_MAP::iterator iter = m_ChannelMap.begin();
	for (; iter != m_ChannelMap.end(); ++iter)
	{
		GSharedField* pSharedFIeld = (*iter).second;
		vecChannelID.push_back(pSharedFIeld->GetChannelID());
	}
}

GSharedField* GSharedFieldMaster::GetChannel(int nChannelID)
{
	SHAREDFIELD_MAP::iterator iter = 
		m_ChannelMap.find(nChannelID);

	if (iter == m_ChannelMap.end())
		return NULL;

	return (*iter).second;
}

GSharedField* GSharedFieldMaster::GetEnterableChannel()
{
	return m_pNavigator->GetEnterableChannel();
}

int GSharedFieldMaster::NewChannelID()
{
	return m_pNavigator->GetNewID();
}
