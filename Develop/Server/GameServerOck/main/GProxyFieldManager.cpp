#include "stdafx.h"
#include "GProxyFieldManager.h"
#include "GProxyField.h"
#include "GFieldInfo.h"
#include "GProxySharedFieldMaster.h"
#include "GProxyDynamicFieldMaster.h"


GProxyFieldManager::GProxyFieldManager()
{

}

GProxyFieldManager::~GProxyFieldManager()
{
	// 순서를 지켜야합니다. -- aibeast
	ClearProxySharedFields();
	ClearProxyFieldGroups();
	ClearProxyFields();
}

void GProxyFieldManager::DeleteField(MUID uidProxyField)
{
	GProxyFieldMap::iterator itor = m_FieldMap.find(uidProxyField);
	if (itor == m_FieldMap.end()) return;

	GProxyField* pField = (*itor).second;
	pField->Drop();

	m_FieldMap.erase(itor);
}

GProxyField* GProxyFieldManager::Find(MUID uidProxyField)
{
	GProxyFieldMap::iterator itor = m_FieldMap.find(uidProxyField);
	if (itor == m_FieldMap.end()) return NULL;

	return (*itor).second;
}

GProxySharedFieldMaster* GProxyFieldManager::FindSharedFieldMaster(int nFieldID)
{
	GProxySharedFieldMasterMap::iterator itr = m_SharedFieldMasterMap.find(nFieldID);
	if (itr == m_SharedFieldMasterMap.end()) return NULL;

	return (*itr).second;
}

GProxyField* GProxyFieldManager::FindSharedField(int nFieldID, int nChannelID)
{
	GProxySharedFieldMaster* pSharedFieldMaster = FindSharedFieldMaster(nFieldID);
	if (NULL == pSharedFieldMaster) return NULL;

	return pSharedFieldMaster->GetSharedField(nChannelID);
}

GProxyField* GProxyFieldManager::AddSharedField(MUID uidField, GFieldInfo* pFieldInfo, int nGameServerID, int nChannelID)
{
	GProxyField* pProxyField = FindSharedField(pFieldInfo->m_nFieldID, nChannelID);
	if (pProxyField) return pProxyField;

	GProxySharedFieldMaster* pFieldMaster = FindSharedFieldMaster(pFieldInfo->m_nFieldID);
	if (NULL == pFieldMaster)
		pFieldMaster = CreateSharedFIeldMaster(pFieldInfo->m_nFieldID);

	pProxyField = new GProxyField(uidField, pFieldInfo, nGameServerID);
	pProxyField->InitSharedField(nChannelID);

	if (false == pFieldMaster->AddSharedField(pProxyField))
	{
		SAFE_DELETE(pProxyField);
		return NULL;
	}

	AddField(pProxyField);
	return pProxyField;
}

void GProxyFieldManager::DeleteSharedField( MUID uidField )
{
	GProxyFieldMap::iterator itor = m_FieldMap.find(uidField);
	if (itor == m_FieldMap.end()) return;

	GProxyField* pField = (*itor).second;
	if (pField->IsDynamic()) return;

	GProxySharedFieldMaster* pFieldMaster = FindSharedFieldMaster(pField->GetFieldInfo()->m_nFieldID);
	if (NULL != pFieldMaster)
		pFieldMaster->DelSharedField(uidField);

	DeleteField(pField->GetUID());
}

GProxyDynamicFieldMaster* GProxyFieldManager::AddFieldGroup(MUID uidFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamicFieldType)
{
	if (NULL == pFieldGroupInfo) return NULL;

	GProxyDynamicFieldMaster* pProxyFieldGroup = FindFieldGroup(uidFieldGroup);
	if (pProxyFieldGroup) return pProxyFieldGroup;

	pProxyFieldGroup = new GProxyDynamicFieldMaster(uidFieldGroup, pFieldGroupInfo, nGameServerID, eDynamicFieldType);
	m_FieldGroupMap.insert(GProxyDynamicFieldMasterMap::value_type(pProxyFieldGroup->GetUID(), pProxyFieldGroup));

	return pProxyFieldGroup;
}

bool GProxyFieldManager::AddDynamicField(MUID uidField, GFieldInfo* pFieldInfo, MUID uidOwnerFieldGroup)
{
	if (NULL == pFieldInfo) return false;

	GProxyField* pProxyField = Find(uidField);
	if (pProxyField) return false;

	GProxyDynamicFieldMaster* pFieldGroup = FindFieldGroup(uidOwnerFieldGroup);
	if (NULL == pFieldGroup) return false;

	pProxyField = new GProxyField(uidField, pFieldInfo, pFieldGroup->GetGameServerID());
	pProxyField->InitDynamicField(pFieldGroup->GetUID());
	AddField(pProxyField);

	pFieldGroup->AddDynamicField(pProxyField);

	return true;
}

void GProxyFieldManager::DeleteFieldGroup(MUID uidFieldGroup)
{
	GProxyDynamicFieldMasterMap::iterator itor = m_FieldGroupMap.find(uidFieldGroup);
	if (itor == m_FieldGroupMap.end()) return;

	GProxyDynamicFieldMaster* pFieldGroup = (*itor).second;

	for each (GProxyField* pProxyDynamicField in  pFieldGroup->GetDynamicFieldVector())
	{
		DeleteField(pProxyDynamicField->GetUID());
	}


	m_FieldGroupMap.erase(itor);
}

GProxyDynamicFieldMaster* GProxyFieldManager::FindFieldGroup(MUID uidFieldGroup)
{
	GProxyDynamicFieldMasterMap::iterator itor = m_FieldGroupMap.find(uidFieldGroup);
	if (itor == m_FieldGroupMap.end()) return NULL;

	return (*itor).second;
}

void GProxyFieldManager::ClearProxySharedFields()
{
	for (GProxySharedFieldMasterMap::iterator itor = m_SharedFieldMasterMap.begin(); itor != m_SharedFieldMasterMap.end(); ++itor)
	{
		GProxySharedFieldMaster* pFieldMaster = (*itor).second;
		for each (GProxySharedFieldMaster::GProxySharedFieldMap::value_type pairField in pFieldMaster->GetContainer())
		{
			DeleteField(pairField.second->GetUID());
		}
		pFieldMaster->GetContainer().clear();
		SAFE_DELETE(pFieldMaster);
	}
	m_SharedFieldMasterMap.clear();
}

void GProxyFieldManager::ClearProxyFieldGroups()
{
	for (GProxyDynamicFieldMasterMap::iterator itor = m_FieldGroupMap.begin(); itor != m_FieldGroupMap.end(); ++itor)
	{
		GProxyDynamicFieldMaster* pFieldGroup = (*itor).second;
		for each (GProxyField* pProxyDynamicField in  pFieldGroup->GetDynamicFieldVector())
		{
			DeleteField(pProxyDynamicField->GetUID());
		}
	}
	m_FieldGroupMap.clear();
}

void GProxyFieldManager::ClearProxyFields()
{
	for (GProxyFieldMap::iterator itor = m_FieldMap.begin(); itor != m_FieldMap.end(); ++itor)
	{
		GProxyField* pField = (*itor).second;
		pField->Drop();
	}
	m_FieldMap.clear();
}

uint32 GProxyFieldManager::CalcChecksumSharedFields()
{
	m_ChecksumSharedFields.Reset();

	for (GProxySharedFieldMasterMap::iterator itor = m_SharedFieldMasterMap.begin(); itor != m_SharedFieldMasterMap.end(); ++itor)
	{
		GProxySharedFieldMaster* pFieldMaster = (*itor).second;
		for each (GProxySharedFieldMaster::GProxySharedFieldMap::value_type pairField in pFieldMaster->GetContainer())
		{
			m_ChecksumSharedFields.Add(pairField.second->GetUID());
		}
	}

	return m_ChecksumSharedFields.GetValue();
}

uint32 GProxyFieldManager::CalcChecksumFieldGroups()
{
	m_ChecksumFieldGroups.Reset();

	for (GProxyDynamicFieldMasterMap::iterator itor = m_FieldGroupMap.begin(); itor != m_FieldGroupMap.end(); ++itor)
	{
		GProxyDynamicFieldMaster* pFieldGroup = (*itor).second;
		m_ChecksumFieldGroups.Add(pFieldGroup->GetUID());
	}

	return m_ChecksumFieldGroups.GetValue();
}

bool GProxyFieldManager::IsExistChannel(int nFieldID, CHANNELID nChannelID)
{
	if (NULL == FindSharedField(nFieldID, nChannelID)) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
/// Private
void GProxyFieldManager::AddField(GProxyField* pProxyField)
{
	m_FieldMap.insert(GProxyFieldMap::value_type(pProxyField->GetUID(), pProxyField));
}

GProxySharedFieldMaster* GProxyFieldManager::CreateSharedFIeldMaster(int nFieldID)
{
	GProxySharedFieldMaster* pFieldMaster = NULL;
	pFieldMaster = FindSharedFieldMaster(nFieldID);
	if (NULL != pFieldMaster) return pFieldMaster;

	pFieldMaster = new GProxySharedFieldMaster(nFieldID);
	m_SharedFieldMasterMap.insert(GProxySharedFieldMasterMap::value_type(nFieldID, pFieldMaster));

	return pFieldMaster;
}