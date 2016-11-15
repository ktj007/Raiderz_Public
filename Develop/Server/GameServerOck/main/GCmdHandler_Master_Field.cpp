#include "stdafx.h"
#include "GCmdHandler_Master_Field.h"
#include "CCommandTable_Master.h"
#include "STransData_M2G.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"
#include "GConfig.h"
#include "GDynamicFieldMaster.h"
#include "GFieldMgr.h"

GCmdHandler_Master_Field::GCmdHandler_Master_Field(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{	
	SetCmdHandler(MMC_FIELD_ADD_SHARED_FIELD,				OnAddSharedField);
	SetCmdHandler(MMC_FIELD_DEL_SHARED_FIELD,				OnDelSharedField);
	SetCmdHandler(MMC_FIELD_SHARED_FIELD_INFO_ALL,			OnFieldInfoAll);

	SetCmdHandler(MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP,		OnAddDynamicFieldGroup);
	SetCmdHandler(MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP,		OnDelDynamicFieldGroup);
	SetCmdHandler(MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL,	OnDynamicFieldGroupInfoAll);

	SetCmdHandler(MMC_FIELD_UPDATE_PROXY_FIELD_INFO,		OnUpdateProxyFieldInfo);	
}

MCommandResult GCmdHandler_Master_Field::OnAddSharedField(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParamFieldInfo = pCommand->GetParameter(0);
	if (pParamFieldInfo->GetType() != MPT_BLOB) return CR_ERROR;

	uint32 nChecksum = 0;
	if( !pCommand->GetParameter(&nChecksum,	1, MPT_UINT) )	return CR_ERROR;

	TDMG_SHARED_FIELD_INFO* pTDFieldInfo = static_cast<TDMG_SHARED_FIELD_INFO*>(pParamFieldInfo->GetPointer());

	gsys.pMasterServerFacade->AddProxySharedFields(pTDFieldInfo, pParamFieldInfo->GetBlobCount(), nChecksum);
	gsys.pMasterServerFacade->AddManagedSharedFields(pTDFieldInfo, pParamFieldInfo->GetBlobCount());

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Field::OnDelSharedField(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidField;
	if( !pCommand->GetParameter(&uidField,	0, MPT_UID) )	return CR_ERROR;

	uint32 nChecksum = 0;
	if( !pCommand->GetParameter(&nChecksum,	1, MPT_UINT) )	return CR_ERROR;

	gsys.pMasterServerFacade->DelManagedSharedField(uidField);
	gsys.pMasterServerFacade->DelProxySharedField(uidField, nChecksum);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Field::OnFieldInfoAll(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParamFieldInfo = pCommand->GetParameter(0);
	if (pParamFieldInfo->GetType() != MPT_BLOB) return CR_ERROR;

	uint32 nChecksum = 0;
	if( !pCommand->GetParameter(&nChecksum,	1, MPT_UINT) )	return CR_ERROR;

	TDMG_SHARED_FIELD_INFO* pTDFieldInfo = static_cast<TDMG_SHARED_FIELD_INFO*>(pParamFieldInfo->GetPointer());

	gsys.pMasterServerFacade->ClearProxySharedFields();
	gsys.pMasterServerFacade->AddProxySharedFields(pTDFieldInfo, pParamFieldInfo->GetBlobCount(), nChecksum);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Field::OnAddDynamicFieldGroup(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam_FieldGroupInfo = pCommand->GetParameter(0);
	if (pParam_FieldGroupInfo->GetType() != MPT_SINGLE_BLOB) return CR_ERROR;

	MCommandParameter* pParam_DynamicFieldList = pCommand->GetParameter(1);
	if (pParam_DynamicFieldList->GetType() != MPT_BLOB) return CR_ERROR;

	uint32 nChecksum = 0;
	if( !pCommand->GetParameter(&nChecksum,	2, MPT_UINT) )	return CR_ERROR;

	TDMG_DYNAMIC_FIELD_GROUP_INFO* pTDFieldGroupInfo = static_cast<TDMG_DYNAMIC_FIELD_GROUP_INFO*>(pParam_FieldGroupInfo->GetPointer());
	if (NULL == pTDFieldGroupInfo)	return CR_ERROR;

	int nDynamicFieldCount = pParam_DynamicFieldList->GetBlobCount();
	TDMG_DYNAMIC_FIELD_NODE* pTDDynamicFieldArray = static_cast<TDMG_DYNAMIC_FIELD_NODE*>(pParam_DynamicFieldList->GetPointer());
	if (NULL == pTDDynamicFieldArray)	return CR_ERROR;

	gsys.pMasterServerFacade->AddProxyFieldGroups(1, pTDFieldGroupInfo, nDynamicFieldCount, pTDDynamicFieldArray, nChecksum);

	// 동적필드의 소속 게임서버 체크.
	if (GConfig::m_nMyServerID != pTDFieldGroupInfo->nGameServerID)
		return CR_TRUE;

	// 동적필드 소속이 본 서버라면 실제 인스턴스 생성.
	MUID uidFieldGroup = pTDFieldGroupInfo->uidDynamicFieldGroup;
	int nFieldGroupID = pTDFieldGroupInfo->nFieldGroupID;
	DYNAMIC_FIELD_TYPE eType = (DYNAMIC_FIELD_TYPE)pTDFieldGroupInfo->nType;
	MUID uidCreator = pTDFieldGroupInfo->uidCreator;

	vector<pair<int, MUID>> vecDynamicFieldUID;
	for (int i = 0; i < nDynamicFieldCount; ++i)
	{
		vecDynamicFieldUID.push_back(pair<int, MUID>(pTDDynamicFieldArray[i].nFieldID, pTDDynamicFieldArray[i].uidDynamicField));
	}

	GDynamicFieldMaster* pDynamicFieldGroup = gmgr.pFieldMgr->CreateDynamicFieldMaster(uidFieldGroup, nFieldGroupID, eType, uidCreator, vecDynamicFieldUID);
	if (NULL == pDynamicFieldGroup) return CR_ERROR;

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Field::OnDelDynamicFieldGroup(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidFieldGroup;
	if( !pCommand->GetParameter(&uidFieldGroup,	0, MPT_UID) )	return CR_ERROR;

	uint32 nChecksum = 0;
	if( !pCommand->GetParameter(&nChecksum,	1, MPT_UINT) )	return CR_ERROR;

	gsys.pMasterServerFacade->DelProxyFieldGroup(uidFieldGroup, nChecksum);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Field::OnDynamicFieldGroupInfoAll(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam_FieldGroupList = pCommand->GetParameter(0);
	if (pParam_FieldGroupList->GetType() != MPT_BLOB) return CR_ERROR;

	MCommandParameter* pParam_DynamicFieldList = pCommand->GetParameter(1);
	if (pParam_DynamicFieldList->GetType() != MPT_BLOB) return CR_ERROR;

	uint32 nChecksum = 0;
	if( !pCommand->GetParameter(&nChecksum,	2, MPT_UINT) )	return CR_ERROR;


	int nFieldGroupCount = pParam_FieldGroupList->GetBlobCount();
	TDMG_DYNAMIC_FIELD_GROUP_INFO* pTDFieldGroupList = static_cast<TDMG_DYNAMIC_FIELD_GROUP_INFO*>(pParam_FieldGroupList->GetPointer());

	int nDynamicFieldCount = pParam_DynamicFieldList->GetBlobCount();
	TDMG_DYNAMIC_FIELD_NODE* pTDDynamicFieldList = static_cast<TDMG_DYNAMIC_FIELD_NODE*>(pParam_DynamicFieldList->GetPointer());


	gsys.pMasterServerFacade->AddProxyFieldGroups(nFieldGroupCount, pTDFieldGroupList, nDynamicFieldCount, pTDDynamicFieldList, nChecksum);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Field::OnUpdateProxyFieldInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidField;
	if( !pCommand->GetParameter(&uidField,	0, MPT_UID) )	return CR_ERROR;

	MCommandParameter* pParam_ProxyFieldInfo = pCommand->GetParameter(1);
	if (pParam_ProxyFieldInfo->GetType() != MPT_SINGLE_BLOB) return CR_ERROR;

	TDMG_PROXY_FIELD_UPDATE_INFO* pTDProxyFieldUpdateInfo = static_cast<TDMG_PROXY_FIELD_UPDATE_INFO*>(pParam_ProxyFieldInfo->GetPointer());
	if (NULL == pTDProxyFieldUpdateInfo)	return CR_ERROR;


	gsys.pMasterServerFacade->UpdateProxyFieldInfo(uidField, pTDProxyFieldUpdateInfo);

	return CR_TRUE;
}
