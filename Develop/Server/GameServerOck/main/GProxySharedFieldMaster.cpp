#include "stdafx.h"
#include "GProxySharedFieldMaster.h"
#include "GProxyField.h"


bool GProxySharedFieldMaster::AddSharedField(GProxyField* pSharedField)
{
	if (NULL == pSharedField) return false;
	if (NULL != GetSharedField(pSharedField->GetChannelID())) return false;

	m_mapSharedField.insert(GProxySharedFieldMap::value_type(pSharedField->GetChannelID(), pSharedField));
	return true;
}

void GProxySharedFieldMaster::DelSharedField(MUID uidField)
{
	for each (GProxySharedFieldMap::value_type pairField in m_mapSharedField)
	{
		if (uidField == pairField.second->GetUID())
		{
			m_mapSharedField.erase(pairField.second->GetChannelID());
			break;
		}
	}
}

GProxyField* GProxySharedFieldMaster::GetSharedField(int nChannelID)
{
	GProxySharedFieldMap::iterator itr = m_mapSharedField.find(nChannelID);
	if (itr == m_mapSharedField.end()) return NULL;

	return (*itr).second;
}

void GProxySharedFieldMaster::GetChannelIDList(vector<int>& vecChannelID)
{
	for each(GProxySharedFieldMap::value_type pairField in m_mapSharedField)
	{
		int channelID = pairField.second->GetChannelID();
		vecChannelID.push_back(channelID);
	}
}