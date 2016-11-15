#include "stdafx.h"
#include "ZBaseFieldManager.h"
#include "ZSharedField.h"
#include "ZSharedFieldMaster.h"
#include "ZDynamicField.h"
#include "ZDynamicFieldMaster.h"
#include "ZFieldInfoManager.h"
#include "ZServer.h"


ZBaseFieldManager::~ZBaseFieldManager()
{
	Clear();
}

void ZBaseFieldManager::AddField(ZField* pField)
{
	m_FieldMap.insert(ZFieldMap::value_type(pField->GetUID(), pField));
}
void ZBaseFieldManager::DelField(MUID uidField)
{
	ZFieldMap::iterator itor = m_FieldMap.find(uidField);
	if (itor != m_FieldMap.end())
	{
		ZField* pField = (*itor).second;
		m_FieldMap.erase(itor);
	}
}

void ZBaseFieldManager::AddSharedField(ZSharedField* pSharedField)
{
	m_SharedFieldMap.insert(ZSharedFieldMap::value_type(pSharedField->GetUID(), pSharedField));
	AddField(pSharedField);

	ZSharedFieldMaster* pSharedFieldMaster = FindSharedFieldMaster(pSharedField->GetFieldInfo()->m_nFieldID);
	if (NULL == pSharedFieldMaster)
	{
		pSharedFieldMaster = new ZSharedFieldMaster(pSharedField->GetFieldInfo()->m_nFieldID);
		AddSharedFieldMaster(pSharedFieldMaster);
	}
	pSharedFieldMaster->Add(pSharedField);
}
void ZBaseFieldManager::DelSharedField(MUID uidSharedField)
{
	ZSharedFieldMap::iterator itor = m_SharedFieldMap.find(uidSharedField);
	if (itor == m_SharedFieldMap.end()) return;

	ZSharedField* pSharedField = (*itor).second;
	int nFieldID = pSharedField->GetFieldInfo()->m_nFieldID;
	int nChannelID = pSharedField->GetChannelID();

	ZSharedFieldMaster* pSharedFieldMaster = FindSharedFieldMaster(nFieldID);
	if (NULL != pSharedFieldMaster)
		pSharedFieldMaster->Remove(nChannelID);

	pSharedField->Drop();
	m_SharedFieldMap.erase(itor);
	DelField(uidSharedField);
}

void ZBaseFieldManager::AddSharedFieldMaster(ZSharedFieldMaster* pSharedFieldMaster)
{
	m_SharedFieldMasterMap.insert(ZSharedFieldMasterMap::value_type(pSharedFieldMaster->GetFieldID(), pSharedFieldMaster));
}

void ZBaseFieldManager::AddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster)
{
	m_DynamicFieldMasterMap.insert(ZDynamicFieldMasterMap::value_type(pDynamicFieldMaster->GetUID(), pDynamicFieldMaster));

	for each (ZFieldInfo* pFieldInfo in pDynamicFieldMaster->GetInfo()->vecFieldInfo)
	{
		MUID uidNewDynamicField = gsys.pServer->NewUID();
		ZDynamicField* pDynamicField = new ZDynamicField(
			uidNewDynamicField
			, pFieldInfo
			, pDynamicFieldMaster->GetGameServerID()
			, pDynamicFieldMaster->GetUID());

		pDynamicFieldMaster->Insert(pDynamicField);
		AddField(pDynamicField);
	}
}
void ZBaseFieldManager::DelDynamicFieldMaster(MUID uidDynamicFieldMaster)
{
	ZDynamicFieldMasterMap::iterator itor = m_DynamicFieldMasterMap.find(uidDynamicFieldMaster);
	if (itor != m_DynamicFieldMasterMap.end())
	{
		ZDynamicFieldMaster* pDynamicFieldMaster = (*itor).second;
		pDynamicFieldMaster->Drop();
		m_DynamicFieldMasterMap.erase(itor);
	}
}

void ZBaseFieldManager::Clear()
{
	m_FieldMap.clear();

	for (ZSharedFieldMap::iterator itor = m_SharedFieldMap.begin(); itor != m_SharedFieldMap.end(); ++itor)
	{
		ZSharedField* pSharedField = (*itor).second;
		pSharedField->Drop();
	}
	m_SharedFieldMap.clear();

	for (ZSharedFieldMasterMap::iterator itor = m_SharedFieldMasterMap.begin(); itor != m_SharedFieldMasterMap.end(); ++itor)
	{
		ZSharedFieldMaster* pSharedFieldMaster = (*itor).second;
		SAFE_DELETE(pSharedFieldMaster);
	}
	m_SharedFieldMasterMap.clear();

	for (ZDynamicFieldMasterMap::iterator itor = m_DynamicFieldMasterMap.begin();
		itor != m_DynamicFieldMasterMap.end(); ++itor)
	{
		ZDynamicFieldMaster* pFieldGroup = (*itor).second;
		pFieldGroup->Drop();
	}
	m_DynamicFieldMasterMap.clear();
}

ZField* ZBaseFieldManager::FindField(MUID uidField)
{
	ZFieldMap::iterator itor = m_FieldMap.find(uidField);
	if (itor == m_FieldMap.end()) return NULL;

	return (*itor).second;
}
ZSharedField* ZBaseFieldManager::FindSharedField(MUID uidSharedField)
{
	ZSharedFieldMap::iterator itor = m_SharedFieldMap.find(uidSharedField);
	if (itor == m_SharedFieldMap.end()) return NULL;

	return (*itor).second;
}
ZSharedField* ZBaseFieldManager::FindSharedField(int nFieldID, int nChannelID)
{
	ZSharedFieldMaster* pSharedFieldMaster = FindSharedFieldMaster(nFieldID);
	if (NULL == pSharedFieldMaster) return NULL;

	ZSharedField* pField = pSharedFieldMaster->FindChannel(nChannelID);
	return pField;
}
ZSharedFieldMaster* ZBaseFieldManager::FindSharedFieldMaster(int nFieldID)
{
	ZSharedFieldMasterMap::iterator itor = m_SharedFieldMasterMap.find(nFieldID);
	if (itor == m_SharedFieldMasterMap.end()) return NULL;

	return (*itor).second;
}
ZDynamicFieldMaster* ZBaseFieldManager::FindDynamicFieldMaster(MUID uidDynamicFieldMaster)
{
	ZDynamicFieldMasterMap::iterator itor = m_DynamicFieldMasterMap.find(uidDynamicFieldMaster);
	if (itor == m_DynamicFieldMasterMap.end()) return NULL;

	return (*itor).second;
}