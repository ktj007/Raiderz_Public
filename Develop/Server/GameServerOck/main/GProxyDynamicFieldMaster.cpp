#include "StdAfx.h"
#include "GProxyDynamicFieldMaster.h"
#include "GProxyField.h"

GProxyDynamicFieldMaster::GProxyDynamicFieldMaster(MUID uidFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamicFieldType)
: m_UID(uidFieldGroup)
, m_pFieldGroupInfo(pFieldGroupInfo)
, m_nGameServerID(nGameServerID)
, m_eDynamicFieldType(eDynamicFieldType)
{

}

GProxyDynamicFieldMaster::~GProxyDynamicFieldMaster()
{
	Clear();
}

void GProxyDynamicFieldMaster::Clear()
{
	m_FieldVector.clear();
}

int GProxyDynamicFieldMaster::GetPlayerCount()
{
	int nPlayerCount = 0;

	for (size_t i = 0; i < m_FieldVector.size(); ++i)
	{
		nPlayerCount += m_FieldVector[i]->GetPlayerCount();
	}
	return nPlayerCount;
}

void GProxyDynamicFieldMaster::AddDynamicField(GProxyField* pProxyDynamicField)
{
	m_FieldVector.push_back(pProxyDynamicField);
}