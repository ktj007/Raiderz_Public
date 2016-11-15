#include "stdafx.h"
#include "ZSmartFieldAllocator.h"
#include "ZFieldManager.h"
#include "ZFieldInfoManager.h"
#include "STransData_M2G.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerObject.h"
#include "ZDynamicFieldMaster.h"
#include "ZConst.h"
#include "ZFitnessCalculator.h"
#include "ZSharedFieldMaster.h"
#include "ZSharedField.h"
#include "CSDef.h"
#include "ZFieldFinder.h"


ZSharedField* ZSmartFieldAllocator::AllocNewChannel(ZSharedFieldMaster* pFieldMaster)
{
	if (NULL == pFieldMaster) return NULL;
	
	// 필드 확인
	int nFieldID = pFieldMaster->GetFieldID();

	ZFieldInfo* pFieldInfo = gmgr.pFieldInfoManager->Find(nFieldID);
	if (NULL == pFieldInfo)	return NULL;
	
	int nChannelID = pFieldMaster->GetAllocableChannelID();
	
	// 채널이 예약된 게임서버 탐색.
	ZFieldFinder finder;
	ZGameServerObject* pTargetGameServer = NULL;
	pTargetGameServer = finder.FindGameServer_HadReservedChannel(nFieldID, nChannelID);
	if (NULL == pTargetGameServer)
	{
		// 예약된 채널이 아니면 점수 계산해서 적합한 서버 선택.
		pTargetGameServer = GetRecommendedGameServer_ToCreateSharedField(pFieldMaster);
		if (NULL == pTargetGameServer) return NULL;
	}
	
	bool bIsDefault = false;
	ZSharedField* pNewField = gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID, pTargetGameServer->GetID(), bIsDefault, true);
	if (NULL == pNewField) return NULL;

	// GameServerObject의 FieldMap에도 추가한다.
	pTargetGameServer->GetFieldMap().AddSharedField(pNewField);

	return pNewField;
}

ZGameServerObject* ZSmartFieldAllocator::AllocNewDynamicFieldGroup(MUID uidRequestPlayer, int nFieldGroupID, DYNAMIC_FIELD_TYPE eType, MUID* outuidCreatedFieldGroup)
{
	if (NULL == gmgr.pFieldInfoManager->FindGroupInfo(nFieldGroupID))
		return NULL;

	ZGameServerObject* pRecommendedGameServer = NULL;
	
	if (ZConst::ALLOC_DYNAMIC_SERVER_ANOTHER_SERVER)
	{
		ZPlayer* pRequestPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
		if (NULL == pRequestPlayer) return NULL;

		pRecommendedGameServer = gmgr.pGameServerObjectManager->FindAnotherGameServer(pRequestPlayer->GetGameServerID());
		if (NULL == pRecommendedGameServer) return NULL;
	}
	else
	{
		pRecommendedGameServer = GetRecommendedGameServer_ToCreateFieldGroup(uidRequestPlayer);
		if (NULL == pRecommendedGameServer) return NULL;
	}

	int nOwnerServerID = pRecommendedGameServer->GetID();
	ZDynamicFieldMaster* pNewFieldGroup = m_pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, nOwnerServerID, eType, uidRequestPlayer, true);
	if (NULL == pNewFieldGroup) return NULL;

	pRecommendedGameServer->GetFieldMap().AddDynamicFieldMaster(pNewFieldGroup);

	*outuidCreatedFieldGroup = pNewFieldGroup->GetUID();

	return pRecommendedGameServer;
}

ZGameServerObject* ZSmartFieldAllocator::GetRecommendedGameServer_ToCreateSharedField(ZSharedFieldMaster* pFieldMaster)
{
	if (NULL == pFieldMaster) return NULL;

	ZGameServerObject* pRequestGameServer = NULL;
	ZGameServerObject* pRecommendedGameServer = NULL;
	int nRecommandedGameServerValue = 0;
	int nItorGameServerValue = 0;

	ZGameServerObjectManager::ZServerObjectIDMap::iterator itor = gmgr.pGameServerObjectManager->GetGameServerObjectIDMap().begin();
	for (; itor != gmgr.pGameServerObjectManager->GetGameServerObjectIDMap().end(); ++itor)
	{
		ZGameServerObject* pItorGameServer = (*itor).second;

		nItorGameServerValue = ZFitnessCalculator::CalcToCreateChannel(pItorGameServer);

		if (nRecommandedGameServerValue < nItorGameServerValue)
		{
			nRecommandedGameServerValue = nItorGameServerValue;
			pRecommendedGameServer = pItorGameServer;
		}
	}

	if (nRecommandedGameServerValue == 0)
		return NULL;

	return pRecommendedGameServer;
}

ZGameServerObject* ZSmartFieldAllocator::GetRecommendedGameServer_ToCreateFieldGroup(MUID uidRequestPlayer)
{
	ZPlayer* pRequestPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (NULL == pRequestPlayer) return NULL;

	ZGameServerObject* pRequestGameServer = gmgr.pGameServerObjectManager->FindGameServer(pRequestPlayer->GetGameServerID());
	if (NULL == pRequestGameServer) return NULL;
	
	ZGameServerObject* pRecommendedGameServer = pRequestGameServer;
	int nRecommandedGameServerValue = ZFitnessCalculator::CalcToCreateFieldGroup(pRequestGameServer, ZConst::FITNESS_ADDITIONAL_VALUE_TO_ALLOC_FIELD);
	int nItorGameServerValue;

	ZGameServerObjectManager::ZServerObjectIDMap::iterator itor = gmgr.pGameServerObjectManager->GetGameServerObjectIDMap().begin();
	for (; itor != gmgr.pGameServerObjectManager->GetGameServerObjectIDMap().end(); ++itor)
	{
		ZGameServerObject* pItorGameServer = (*itor).second;

		if (pRecommendedGameServer->GetUID() == pItorGameServer->GetUID())
			continue;

		nItorGameServerValue = ZFitnessCalculator::CalcToCreateFieldGroup(pItorGameServer);

		if (nRecommandedGameServerValue < nItorGameServerValue)
		{
			nRecommandedGameServerValue = nItorGameServerValue;
			pRecommendedGameServer = pItorGameServer;
		}
	}

	if (nRecommandedGameServerValue == 0)
		return NULL;

	return pRecommendedGameServer;
}