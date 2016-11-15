#include "StdAfx.h"
#include "ZDynamicFieldMaster.h"
#include "ZDynamicField.h"

ZDynamicFieldMaster::ZDynamicFieldMaster(MUID uidNew, FIELD_GROUP_INFO* pInfo, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamiicFieldType, MUID uidCreator)
: m_UID(uidNew)
, m_pInfo(pInfo)
, m_nGameServerID(nGameServerID)
, m_eDynamiicFieldType(eDynamiicFieldType)
, m_uidCreator(uidCreator)
{
	_ASSERT(pInfo);
}

ZDynamicFieldMaster::~ZDynamicFieldMaster()
{
	Clear();
}

void ZDynamicFieldMaster::Insert(ZDynamicField* pField)
{
	if (!pField) return;

	m_vecDynamicField.push_back(pField);
}

void ZDynamicFieldMaster::Clear()
{
	for each (ZDynamicField* pDynamicField in m_vecDynamicField)
	{
		pDynamicField->Drop();
	}
	m_vecDynamicField.clear();
}

int ZDynamicFieldMaster::GetPlayerCount()
{
	int nPlayerCount = 0;
	for each (ZDynamicField* pDynamicField in m_vecDynamicField)
	{
		nPlayerCount += pDynamicField->GetPlayerCount();
	}
	return nPlayerCount;
}

ZDynamicField* ZDynamicFieldMaster::FindField(int nDynamicFieldID)
{
	for (int i = 0; i < (int)m_vecDynamicField.size(); ++i)
	{
		if (m_vecDynamicField[i]->GetID() == nDynamicFieldID)
			return m_vecDynamicField[i];
	}
	return NULL;
}
