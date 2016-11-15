#include "stdafx.h"
#include "ZFieldManager.h"
#include "ZSharedField.h"
#include "ZSharedFieldMaster.h"
#include "ZDynamicField.h"
#include "ZDynamicFieldMaster.h"
#include "ZFieldInfoManager.h"
#include "ZFieldCommandRouter.h"
#include "ZServer.h"


ZFieldManager::ZFieldManager() : ZBaseFieldManager()
{

}

ZFieldManager::~ZFieldManager()
{
	
}

ZSharedField* ZFieldManager::CreateSharedField(int nFieldID, int nChannelID, int nGameServerID, bool bIsDefaultChannel, bool bRoute)
{
	if (0 == nFieldID || 0 == nChannelID || 0 == nGameServerID) return NULL;

	MUID uidNew = m_UIDGenerator.Generate();

	ZFieldInfo* pFieldInfo = gmgr.pFieldInfoManager->Find(nFieldID);
	if (NULL == pFieldInfo) return NULL;

	ZSharedFieldMaster* pFieldMaster = FindSharedFieldMaster(nFieldID);
	if (NULL != pFieldMaster)
	{
		/// 이미 존재하는 채널 검사
		ZSharedField* pExistChannel = pFieldMaster->FindChannel(nChannelID);
		if (pExistChannel != NULL)
		{
			mlog3("Error! ZFieldManager::CreateSharedField(), Already Exist Channel! (FieldID=%d, ChannelID=%d, GameServerID=%d)\n"
				, nFieldID, nChannelID, pExistChannel->GetGameServerID());
			return NULL;
		}
	}

	ZSharedField* pNewField = new ZSharedField(uidNew, pFieldInfo, nChannelID, bIsDefaultChannel, nGameServerID);
	AddSharedField(pNewField, bRoute);

	return pNewField;
}
void ZFieldManager::DestroySharedField(MUID uidSharedField, bool bRoute)
{
	ZSharedField* pSharedField = FindSharedField(uidSharedField);
	if (NULL == pSharedField) return;

	DelSharedField(pSharedField, bRoute);
}

ZDynamicFieldMaster* ZFieldManager::CreateDynamicFieldMaster(int nFieldGroupID, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamicFieldType, MUID uidOwnerPlayer, bool bRoute)
{
	if (0 == nFieldGroupID || 0 == nGameServerID || uidOwnerPlayer.IsInvalid()) return NULL;

	MUID uidNewFieldGroup = gsys.pServer->NewUID();

	FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoManager->FindGroupInfo(nFieldGroupID);
	if (NULL == pFieldGroupInfo) return NULL;

	ZDynamicFieldMaster* pNewDynamicFieldGroup = new ZDynamicFieldMaster(uidNewFieldGroup, pFieldGroupInfo, nGameServerID, eDynamicFieldType, uidOwnerPlayer);
	AddDynamicFieldMaster(pNewDynamicFieldGroup, bRoute);

	return pNewDynamicFieldGroup;
}
void ZFieldManager::DestroyDynamicFieldMaster(MUID uidDynamicFieldMaster, bool bRoute)
{
	ZDynamicFieldMaster* pDynamicFieldGroup = FindDynamicFieldMaster(uidDynamicFieldMaster);
	if (NULL == pDynamicFieldGroup) return;

	for (int i = 0; i < (int)pDynamicFieldGroup->GetFieldVector().size(); ++i)
	{
		ZDynamicField* pDynamicField = pDynamicFieldGroup->GetFieldVector()[i];
		DelField(pDynamicField->GetUID());
	}

	DelDynamicFieldMaster(pDynamicFieldGroup, bRoute);
}

void ZFieldManager::AddSharedField( ZSharedField* pSharedField, bool bRoute )
{
	ZBaseFieldManager::AddSharedField(pSharedField);
	CalcFieldsChecksum();

	if (bRoute)
	{
		ZFieldCommandRouter::RouteAddSharedField(pSharedField);
	}
}
void ZFieldManager::DelSharedField( ZSharedField* pSharedField, bool bRoute )
{
	MUID uidSharedField = pSharedField->GetUID();
	ZBaseFieldManager::DelSharedField(uidSharedField);
	CalcFieldsChecksum();

	if (bRoute)
	{
		ZFieldCommandRouter::RouteDelSharedField(uidSharedField);
	}
}

void ZFieldManager::AddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster, bool bRoute)
{
	ZBaseFieldManager::AddDynamicFieldMaster(pDynamicFieldMaster);
	CalcDynamicFieldMastersChecksum();

	if (bRoute)
	{
		ZFieldCommandRouter::RouteAddDynamicFieldMaster(pDynamicFieldMaster);
	}
}
void ZFieldManager::DelDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster, bool bRoute)
{
	MUID uidDynamicFieldMaster = pDynamicFieldMaster->GetUID();
	ZBaseFieldManager::DelDynamicFieldMaster(uidDynamicFieldMaster);
	CalcDynamicFieldMastersChecksum();

	if (bRoute)
	{
		ZFieldCommandRouter::RouteDelDynamicFieldMaster(uidDynamicFieldMaster);
	}
}

void ZFieldManager::CalcFieldsChecksum()
{
	m_ChecksumFields.Reset();

	for (ZSharedFieldMap::iterator itor = m_SharedFieldMap.begin(); itor != m_SharedFieldMap.end(); ++itor)
	{
		ZSharedField* pSharedField = (*itor).second;
		m_ChecksumFields.Add(pSharedField->GetUID());
	}
}
void ZFieldManager::CalcDynamicFieldMastersChecksum()
{
	m_ChecksumDynamicFieldMasters.Reset();

	for (ZDynamicFieldMasterMap::iterator itor = m_DynamicFieldMasterMap.begin();
		itor != m_DynamicFieldMasterMap.end(); ++itor)
	{
		ZDynamicFieldMaster* pDynamicFieldMaster = (*itor).second;
		m_ChecksumDynamicFieldMasters.Add(pDynamicFieldMaster->GetUID());
	}
}

void ZFieldManager::Clear()
{
	ZBaseFieldManager::Clear();
	CalcFieldsChecksum();
}
void ZFieldManager::ClearOnGameServer(int nGameServerID)
{
	vector<MUID> vecSharedFieldUID;
	for (ZSharedFieldMap::iterator itor = m_SharedFieldMap.begin(); itor != m_SharedFieldMap.end(); ++itor)
	{
		ZSharedField* pSharedField = (*itor).second;
		if (nGameServerID != pSharedField->GetGameServerID())
			continue;
		vecSharedFieldUID.push_back(pSharedField->GetUID());
	}

	vector<MUID> vecFieldGroupUID;
	for (ZDynamicFieldMasterMap::iterator itor = m_DynamicFieldMasterMap.begin();
		itor != m_DynamicFieldMasterMap.end(); ++itor)
	{
		ZDynamicFieldMaster* pFieldGroup = (*itor).second;
		if (nGameServerID != pFieldGroup->GetGameServerID())
			continue;
		vecFieldGroupUID.push_back(pFieldGroup->GetUID());
	}

	for (int i = 0; i < (int)vecSharedFieldUID.size(); ++i)
	{
		ZBaseFieldManager::DelSharedField(vecSharedFieldUID[i]);
	}

	for (int i = 0; i < (int)vecFieldGroupUID.size(); ++i)
	{
		ZBaseFieldManager::DelDynamicFieldMaster(vecFieldGroupUID[i]);
	}

	CalcFieldsChecksum();
	RouteAllFieldInfo();
}

void ZFieldManager::RouteAllFieldInfo()
{
	ZFieldCommandRouter::RouteAllFieldInfo();
}

int ZFieldManager::GetGameServerIDHasSharedField(int nFieldID, int nChannelID)
{
	for each(pair<MUID, ZSharedField*> pairField in m_SharedFieldMap)
	{
		ZSharedField* pSharedField = pairField.second;
		ZFieldInfo* pInfo = pSharedField->GetFieldInfo();
		_ASSERT(pInfo != NULL);

		if (pInfo->m_nFieldID == nFieldID)
		{
			// 요청 채널ID가 있을때만, 채널ID 검사
			if (nChannelID == INVALID_ID)
			{
				return pSharedField->GetGameServerID();
			}
			else
			{
				if (pSharedField->GetType() != FT_SHARED) continue;
				
				if (nChannelID == static_cast<ZSharedField*>(pSharedField)->GetChannelID())
					return pSharedField->GetGameServerID();
			}
		}
	}

	return 0;
}

ZSharedField* ToSharedField(ZField* pField)
{
	if (NULL == pField) return NULL;
	if (pField->GetType() != FT_SHARED) return NULL;
	return static_cast<ZSharedField*>(pField);
}
ZDynamicField* ToDynamicField(ZField* pField)
{
	if (NULL == pField) return NULL;
	if (pField->GetType() != FT_DYNAMIC) return NULL;
	return static_cast<ZDynamicField*>(pField);
}