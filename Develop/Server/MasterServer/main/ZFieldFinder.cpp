#include "StdAfx.h"
#include "ZFieldFinder.h"
#include "STransData_M2G.h"
#include "ZPlayer.h"
#include "ZGameServerInfoManager.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZGateCommandRouter.h"
#include "ZDynamicField.h"
#include "ZDynamicFieldMaster.h"
#include "ZSmartFieldAllocator.h"
#include "ZPartyManager.h"
#include "ZFieldInfoManager.h"
#include "ZSharedFieldMaster.h"
#include "ZSharedField.h"


bool ZFieldFinder::FindEnterableSharedField(ZSharedField*& poutDestField, int nFieldID, int nChannelID)
{
	// 필드 확인
	ZSharedFieldMaster* pSharedFieldMaster = gmgr.pFieldManager->FindSharedFieldMaster(nFieldID);
	if (NULL == pSharedFieldMaster)
	{
		mlog3("Error! ZFieldFinder::FindEnterableSharedField(), Not Exist Shared Field Master(%d)\n", nFieldID);
		return false;
	}

	/// 대상 채널 탐색
	ZSharedField* pSharedField = pSharedFieldMaster->FindChannel(nChannelID);
	if (NULL != pSharedField)
	{
		/// 대상 채널이 입장 가능한 상태인지 체크
		if (pSharedField->IsEnterable())
		{
			poutDestField = pSharedField;
			return true;
		}
	}

	/// 입장 가능한 채널 탐색
	pSharedField = pSharedFieldMaster->GetEnterableChannel();
	if (NULL != pSharedField)
	{
		poutDestField = pSharedField;
		return true;
	}

	/// 입장할 채널을 찾지 못했으면 채널이 생성 될 때까지 대기한다.
	poutDestField = NULL;
	return true;
}

ZGameServerObject* ZFieldFinder::FindFirstGameServer(int nFieldID)
{
	int nGameServerID = gmgr.pFieldManager->GetGameServerIDHasSharedField(nFieldID);
	return gmgr.pGameServerObjectManager->FindGameServer(nGameServerID);
}

ZDynamicFieldMaster* ZFieldFinder::GetKnownFieldGroup(MUID uidPlayer, MUID uidFieldGroup, int nFieldGroup, DYNAMIC_FIELD_TYPE eType)
{
	ZDynamicFieldMaster* pFieldGroup = NULL;
	pFieldGroup = gmgr.pFieldManager->FindDynamicFieldMaster(uidFieldGroup);
	if (NULL == pFieldGroup)
	{
		pFieldGroup = _GetEnterableFieldGroup(uidPlayer, nFieldGroup, eType);
	}
	return pFieldGroup;
}

ZDynamicFieldMaster* ZFieldFinder::GetUnknownFieldGroup(MUID uidPlayer, int nFieldGroup, DYNAMIC_FIELD_TYPE eType, bool isSingleType)
{
	ZDynamicFieldMaster* pFieldGroup = NULL;

	if (!isSingleType)
	{
		pFieldGroup = _GetEnterableFieldGroup(uidPlayer, nFieldGroup, eType);
	}

	if (NULL == pFieldGroup)
	{
		MUID uidNewFieldGroup = 0;
		ZSmartFieldAllocator fieldAllocator(gmgr.pFieldManager);
		fieldAllocator.AllocNewDynamicFieldGroup(uidPlayer, nFieldGroup, eType, &uidNewFieldGroup);

		pFieldGroup = gmgr.pFieldManager->FindDynamicFieldMaster(uidNewFieldGroup);
		if (NULL == pFieldGroup) return NULL;
	}

	return pFieldGroup;
}


ZDynamicFieldMaster* ZFieldFinder::_GetEnterableFieldGroup(MUID uidPlayer, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType)
{
	ZDynamicFieldMaster* pEnterableFieldGroup = NULL;

	switch(eType)
	{
	case DFT_TRIAL:
		pEnterableFieldGroup = _FindExistTrialFieldGroup(uidPlayer, nFieldGroupID, eType);
		break;
	case DFT_INNROOM:
		pEnterableFieldGroup = _FindExistInnRoom();
		break;
	case DFT_BATTLEARENA:
		break;
	}

	return pEnterableFieldGroup;
}


ZDynamicFieldMaster* ZFieldFinder::_FindExistTrialFieldGroup(MUID uidPlayer, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (NULL == pPlayer) return NULL;

	ZDynamicFieldMaster* pExistFieldGroup = NULL;

	MUID uidParty = pPlayer->GetPartyUID();
	if (gmgr.pPartyManager->IsPartyMember(uidParty, pPlayer->GetUID()))
	{
		MUID uidPartyFieldGroup = gmgr.pPartyManager->GetFieldGroupUID(uidParty, nFieldGroupID);
		pExistFieldGroup = gmgr.pFieldManager->FindDynamicFieldMaster(uidPartyFieldGroup);
	}

	return pExistFieldGroup;
}

ZDynamicFieldMaster* ZFieldFinder::_FindExistInnRoom()
{
	ZDynamicFieldMaster* pExistFieldGroup = NULL;

	return pExistFieldGroup;
}


ZGameServerObject* ZFieldFinder::FindGameServer_HadReservedChannel(int nFieldID, int nChannelID)
{
	ZGameServerObject* pGameServer = NULL;

	int nGameServerID = 0;

	ZGameServerInfoMap& refGameServerInfoMap = gmgr.pGameServerInfoManager->GetServerInfoMap();
	ZGameServerInfoMap::iterator itr = refGameServerInfoMap.begin();
	for (; itr != refGameServerInfoMap.end(); ++itr)
	{
		ZGameServerInfo* pGameServerInfo = (*itr).second;

		for (size_t i = 0; i < pGameServerInfo->m_vecManagedSharedField.size(); ++i)
		{
			if (pGameServerInfo->m_vecManagedSharedField[i].nFieldID != nFieldID)
				continue;

			for (size_t j = 0; j < pGameServerInfo->m_vecManagedSharedField[i].vecReservedChannelID.size(); ++j)
			{
				if (pGameServerInfo->m_vecManagedSharedField[i].vecReservedChannelID[j] != nChannelID)
					continue;
				
				nGameServerID = pGameServerInfo->m_nID;
				break;
			}

			if (nGameServerID != 0)
				break;
		}

		if (nGameServerID != 0)
			break;
	}

	pGameServer = gmgr.pGameServerObjectManager->FindGameServer(nGameServerID);
	return pGameServer;
}