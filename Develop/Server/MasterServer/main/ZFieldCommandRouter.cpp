#include "stdafx.h"
#include "ZFieldCommandRouter.h"
#include "ZCommandCenter.h"
#include "ZGlobal.h"
#include "ZFieldManager.h"
#include "ZCommandTable.h"
#include "ZFieldInfo.h"
#include "ZSharedField.h"
#include "ZDynamicField.h"
#include "ZDynamicFieldMaster.h"
#include "ZGameServerObjectManager.h"

ZFieldManager* ZFieldCommandRouter::GetFieldManager()
{
	return gmgr.pFieldManager;
}

void ZFieldCommandRouter::RouteAddSharedField(ZSharedField* pField)
{
	if (0 == gmgr.pGameServerObjectManager->GetClientsCount()) return;

	minet::MCommand* pNewCommand = MakeCommand_AddSharedField(pField);
	gsys.pCommandCenter->RouteToGameServer(pNewCommand);
}

void ZFieldCommandRouter::RouteDelSharedField(MUID uidField)
{
	if (0 == gmgr.pGameServerObjectManager->GetClientsCount()) return;

	minet::MCommand* pNewCommand = MakeCommand_DelSharedField(uidField);
	gsys.pCommandCenter->RouteToGameServer(pNewCommand);
}

void ZFieldCommandRouter::RouteAddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster)
{
	if (0 == gmgr.pGameServerObjectManager->GetClientsCount()) return;

	minet::MCommand* pNewCommand = MakeCommand_AddDynamicFieldGroup(pDynamicFieldMaster);
	gsys.pCommandCenter->RouteToGameServer(pNewCommand);
}

void ZFieldCommandRouter::RouteDelDynamicFieldMaster(MUID uidDynamicFieldMaster)
{
	if (0 == gmgr.pGameServerObjectManager->GetClientsCount()) return;

	minet::MCommand* pNewCommand = MakeCommand_DelDynamicFieldGroup(uidDynamicFieldMaster);
	gsys.pCommandCenter->RouteToGameServer(pNewCommand);
}

void ZFieldCommandRouter::SendAllFieldInfo(MUID uidTarget)
{
	minet::MCommand* pNewCommand = MakeCommand_AllSharedFieldInfo();
	gsys.pCommandCenter->SendCommand(uidTarget, pNewCommand);
}

void ZFieldCommandRouter::RouteAllFieldInfo()
{
	if (0 == gmgr.pGameServerObjectManager->GetClientsCount()) return;

	minet::MCommand* pNewCommand = MakeCommand_AllSharedFieldInfo();
	gsys.pCommandCenter->RouteToGameServer(pNewCommand);
}

void ZFieldCommandRouter::SendAllFieldGroupInfo(MUID uidTarget)
{
	minet::MCommand* pNewCommand = MakeCommand_AllFieldGroupInfo();
	gsys.pCommandCenter->SendCommand(uidTarget, pNewCommand);
}

void ZFieldCommandRouter::RouteUpdateFieldInfo(ZField* pField)
{
	if (0 == gmgr.pGameServerObjectManager->GetClientsCount()) return;

	minet::MCommand* pNewCommand = MakeCommand_UpdateProxyFieldInfo(pField);
	gsys.pCommandCenter->RouteToGameServer(pNewCommand);
}

void ZFieldCommandRouter::AssignSharedFieldInfoTransData(ZSharedField* pSrcField, TDMG_SHARED_FIELD_INFO* pTar)
{
	pTar->nGameServerID = pSrcField->GetGameServerID();
	pTar->uidField = pSrcField->GetUID();
	pTar->nFieldID = pSrcField->GetFieldInfo()->m_nFieldID;
	pTar->nChannelID = pSrcField->GetChannelID();
}

void ZFieldCommandRouter::AssignDynamicFieldGroupInfoTransData(ZDynamicFieldMaster* pSrcFieldMaster, TDMG_DYNAMIC_FIELD_GROUP_INFO* pTar)
{
	pTar->uidDynamicFieldGroup = pSrcFieldMaster->GetUID();
	pTar->uidCreator = pSrcFieldMaster->GetCreatorUID();
	pTar->nGameServerID = pSrcFieldMaster->GetGameServerID();
	pTar->nFieldGroupID = pSrcFieldMaster->GetInfo()->nID;
	pTar->nType = (int8)pSrcFieldMaster->GetType();
}

void ZFieldCommandRouter::AssignDynamicFieldInfosTrasData(ZDynamicFieldMaster* pSrcFieldGroup, std::vector<TDMG_DYNAMIC_FIELD_NODE >* pTar)
{
	for each (ZDynamicField* pDynamicField in pSrcFieldGroup->GetFieldVector())
	{
		TDMG_DYNAMIC_FIELD_NODE tdDynamicField;

		tdDynamicField.uidFieldGroup = pDynamicField->GetFieldGroupUID();
		tdDynamicField.uidDynamicField = pDynamicField->GetUID();
		tdDynamicField.nFieldID = pDynamicField->GetFieldInfo()->m_nFieldID;

		pTar->push_back(tdDynamicField);
	}
}

minet::MCommand* ZFieldCommandRouter::MakeCommand_AllSharedFieldInfo()
{
	uint32 nFieldChecksum = GetFieldManager()->GetChecksumFields();

	void* pBlobPtr = NULL;

	size_t nBlobCount = GetFieldManager()->GetSharedFieldCount();
	int nBlobIndex = 0;

	TDMG_SHARED_FIELD_INFO* pNewBlobParam = NULL;

	if (nBlobCount > 0)
	{
		pNewBlobParam = new TDMG_SHARED_FIELD_INFO[nBlobCount];

		for (ZBaseFieldManager::ZSharedFieldMap::iterator itor = GetFieldManager()->GetSharedFieldMap().begin();
			itor != GetFieldManager()->GetSharedFieldMap().end(); ++itor)
		{
			ZSharedField* pSharedField = (*itor).second;
			if (pSharedField->GetFieldInfo() == NULL) continue;

			AssignSharedFieldInfoTransData(pSharedField, &pNewBlobParam[nBlobIndex]);

			nBlobIndex++;
		}

		if (nBlobIndex > 0)
		{
			pBlobPtr = &pNewBlobParam[0];
		}
	}

	nBlobCount = nBlobIndex;

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_FIELD_SHARED_FIELD_INFO_ALL
		, 2
		, NEW_BLOB(pBlobPtr, sizeof(TDMG_SHARED_FIELD_INFO), nBlobIndex)
		, NEW_UINT(nFieldChecksum));

	SAFE_DELETE_ARRAY(pNewBlobParam);

	return pNewCommand;
}

minet::MCommand* ZFieldCommandRouter::MakeCommand_AddSharedField(ZSharedField* pField)
{
	uint32 nFieldChecksum = GetFieldManager()->GetChecksumFields();

	TDMG_SHARED_FIELD_INFO tdFieldInfo;
	AssignSharedFieldInfoTransData(pField, &tdFieldInfo);

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_FIELD_ADD_SHARED_FIELD
		, 2
		, NEW_BLOB(&tdFieldInfo, sizeof(TDMG_SHARED_FIELD_INFO), 1)
		, NEW_UINT(nFieldChecksum));


	return pNewCommand;
}

minet::MCommand* ZFieldCommandRouter::MakeCommand_DelSharedField( MUID uidField )
{
	uint32 nFieldChecksum = GetFieldManager()->GetChecksumFields();

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_FIELD_DEL_SHARED_FIELD
		, 2
		, NEW_UID(uidField)
		, NEW_UINT(nFieldChecksum));

	return pNewCommand;
}

minet::MCommand* ZFieldCommandRouter::MakeCommand_AddDynamicFieldGroup(ZDynamicFieldMaster* pDynamicFieldMaster)
{
	uint32 nChecksum = GetFieldManager()->GetChecksumDynamicFieldMasters();

	TDMG_DYNAMIC_FIELD_GROUP_INFO tdFieldGroupInfo;
	std::vector<TDMG_DYNAMIC_FIELD_NODE> vecDynamicField;

	AssignDynamicFieldGroupInfoTransData(pDynamicFieldMaster, &tdFieldGroupInfo);
	AssignDynamicFieldInfosTrasData(pDynamicFieldMaster, &vecDynamicField);

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP
		, 3
		, NEW_SINGLE_BLOB(&tdFieldGroupInfo, sizeof(TDMG_DYNAMIC_FIELD_GROUP_INFO))
		, NEW_BLOB(vecDynamicField)
		, NEW_UINT(nChecksum));

	return pNewCommand;
}

minet::MCommand* ZFieldCommandRouter::MakeCommand_DelDynamicFieldGroup(MUID uidDynamicFieldMaster)
{
	uint32 nChecksum = GetFieldManager()->GetChecksumDynamicFieldMasters();

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP
		, 2
		, NEW_UID(uidDynamicFieldMaster)
		, NEW_UINT(nChecksum));

	return pNewCommand;
}

minet::MCommand* ZFieldCommandRouter::MakeCommand_AllFieldGroupInfo()
{
	std::vector<TDMG_DYNAMIC_FIELD_GROUP_INFO> vecFieldGroupInfo;
	std::vector<TDMG_DYNAMIC_FIELD_NODE> vecDynamicFieldNode;
	uint32 nChecksum = GetFieldManager()->GetChecksumDynamicFieldMasters();

	for (ZBaseFieldManager::ZDynamicFieldMasterMap::iterator itr = GetFieldManager()->GetDynamicFieldMasterMap().begin();
		itr != GetFieldManager()->GetDynamicFieldMasterMap().end(); ++itr)
	{
		ZDynamicFieldMaster* pFieldGroup = (*itr).second;
		
		TDMG_DYNAMIC_FIELD_GROUP_INFO tdDynamicFieldGroupInfo;
		
		AssignDynamicFieldGroupInfoTransData(pFieldGroup, &tdDynamicFieldGroupInfo);
		AssignDynamicFieldInfosTrasData(pFieldGroup, &vecDynamicFieldNode);
		
		vecFieldGroupInfo.push_back(tdDynamicFieldGroupInfo);
	}
	
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL
		, 3
		, NEW_BLOB(vecFieldGroupInfo)
		, NEW_BLOB(vecDynamicFieldNode)
		, NEW_UINT(nChecksum));
	
	return pNewCommand;
}

minet::MCommand* ZFieldCommandRouter::MakeCommand_UpdateProxyFieldInfo(ZField* pField)
{
	TDMG_PROXY_FIELD_UPDATE_INFO tdProxyFieldUpdateInfo;
	tdProxyFieldUpdateInfo.nPlayerCount = pField->GetPlayerCount();

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_FIELD_UPDATE_PROXY_FIELD_INFO
		, 2
		, NEW_UID(pField->GetUID())
		, NEW_SINGLE_BLOB(&tdProxyFieldUpdateInfo, sizeof(TDMG_PROXY_FIELD_UPDATE_INFO)));

	return pNewCommand;
}
