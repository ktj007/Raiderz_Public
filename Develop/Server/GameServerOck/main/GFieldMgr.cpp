#include "stdafx.h"
#include "GSharedFieldMaster.h"
#include "GDynamicFieldMaster.h"
#include "GTrialField.h"
#include "GInnRoomField.h"
#include "GDynamicFieldPolicy_TrialField.h"
#include "GDynamicFieldPolicy_InnRoom.h"
#include "GDynamicFieldPolicy_BattleArena.h"
#include "GFieldMgr.h"
#include "GServer.h"
#include "GSTLUtil.h"
#include "GFieldInfoMgr.h"
#include "GManagedFieldInfoMgr.h"
#include "GTestSystem.h"
#include "GEntityPlayer.h"
#include "GFieldFactory.h"
#include "GConfig.h"
#include "GMasterServerFacade.h"
#include "GServerFieldRouter.h"
#include "GFieldEnteringMgr.h"


GFieldMgr::GFieldMgr()
{
	m_pFieldFactory = new GFieldFactory(this);
	m_pFieldEnteringQueue = new GFieldEnteringMgr();
}

GFieldMgr::~GFieldMgr()
{
	Clear();
	SAFE_DELETE(m_pFieldFactory);
	SAFE_DELETE(m_pFieldEnteringQueue);
}

MUID GFieldMgr::NewUID()
{
	return gsys.pServer->NewUID();
}

void GFieldMgr::Clear()
{
	// 순서를 지켜야합니다. -- mizelan
	SAFE_DELETE_MAP_OP(m_mapSharedFieldMasters,		DeletePairSecondObject);
	SAFE_DELETE_MAP_OP(m_mapFields,					DestroyAndDeletePairSecondObject);
	SAFE_DELETE_MAP_OP(m_mapDynamicFieldMasters,	DestroyAndDeletePairSecondObject);
}

void GFieldMgr::AddField(GField* pField)
{
	m_mapFields.insert(FIELD_BY_UID_MAP::value_type(pField->GetUID().Value, pField));
	pField->m_bRegisteredManager = true;

	pField->OnCreated();
}

void GFieldMgr::EraseField(GField* pField)
{
	VALID(pField);
	m_mapFields.erase(pField->GetUID().Value);
}

void GFieldMgr::DeleteField(const MUID& uidField)
{
	FIELD_BY_UID_MAP::iterator itor = m_mapFields.find(uidField.Value);
	if (itor != m_mapFields.end())
	{
		GField* pField = (*itor).second;
		pField->Destroy();
		delete pField;

		m_mapFields.erase(itor);
	}
}

GField* GFieldMgr::GetField(const MUID& uidField)
{
	FIELD_BY_UID_MAP::iterator itor = m_mapFields.find(uidField.Value);
	if (itor != m_mapFields.end())
	{
		return (*itor).second;
	}
	return NULL;
}

GSharedField* GFieldMgr::CreateSharedField( MUID uidNewField, int nFieldID, CHANNELID nNewChannelID )
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	VALID_RET(pFieldInfo, NULL);

	GSharedFieldMaster* pSharedFieldMaster = NULL;
	SHARED_FIELD_MASTER_MAP::iterator it = m_mapSharedFieldMasters.find(nFieldID);
	if (it == m_mapSharedFieldMasters.end())
	{
		return NULL;
	}

	pSharedFieldMaster = it->second;
	VALID_RET(pSharedFieldMaster, NULL);

	GSharedField* pNewField = pSharedFieldMaster->CreateSharedField(uidNewField);
	VALID_RET(pNewField, NULL);
	
	AddField(pNewField);
	pSharedFieldMaster->AddChannel(pNewField, nNewChannelID);

	return pNewField;
}

GSharedFieldMaster* GFieldMgr::GetSharedFieldMaster(int nFieldID)
{
	SHARED_FIELD_MASTER_MAP::iterator iter = 
		m_mapSharedFieldMasters.find(nFieldID);

	if (iter == m_mapSharedFieldMasters.end())
		return NULL;

	return (*iter).second;
}

GSharedField*	GFieldMgr::GetSharedField(int nFieldID, CHANNELID nChannelID)
{
	GSharedFieldMaster* pSharedFieldMaster = GetSharedFieldMaster(nFieldID);
	if (pSharedFieldMaster == NULL)	return NULL;

	GSharedField* pSharedField = pSharedFieldMaster->GetChannel(nChannelID);
	return pSharedField;
}

GSharedField* GFieldMgr::GetEnterableSharedField(int nFieldID)
{
	GSharedFieldMaster* pSharedFieldMaster = GetSharedFieldMaster(nFieldID);

	if (pSharedFieldMaster == NULL)
		return NULL;

	GSharedField* pSharedField = pSharedFieldMaster->GetEnterableChannel();
	if (!pSharedField)
	{
		// 마스터서버 없이 작동할때는, 없는 필드는 직접 만든다.
		if (GConfig::IsStandAloneMode())
		{
			MUID uidNewField = gsys.pServer->NewUID();
			pSharedField = CreateSharedField(uidNewField, pSharedFieldMaster->GetFieldID(), pSharedFieldMaster->NewChannelID());
		}
	}

	return pSharedField;
}

void GFieldMgr::AddSharedField(MUID uidField, int nFieldID, int nChannelID)
{
	// FieldMaster가 없으면 만든다
	if (NULL == GetSharedFieldMaster(nFieldID))
	{
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
		if (pFieldInfo == NULL)	return;

		CreateSharedFieldMaster(pFieldInfo);
	}
	
	// Channel이 없으면 만든다.
	GSharedField* pSharedField = GetSharedField(nFieldID, nChannelID);
	if (NULL == pSharedField)
	{
		CreateSharedField(uidField, nFieldID, nChannelID);
	}
	else
	{
		// 이미 존재하는 필드인데 DeleteMe 플래그가 켜져있으면 취소.
		if (pSharedField->m_bDeleteMe)
			pSharedField->m_bDeleteMe = false;
	}
}

void GFieldMgr::Update(float fDelta)
{
	m_pFieldEnteringQueue->Update(fDelta);

	Update_SharedField(fDelta);
	Update_DynamicField(fDelta);
	Update_CommonField(fDelta);
}

void GFieldMgr::Update_SharedField(float fDelta)
{
	PFC_THISFUNC;
	for each (const SHARED_FIELD_MASTER_MAP::value_type& data in m_mapSharedFieldMasters)
	{
		data.second->Update(fDelta);
	}
}

void GFieldMgr::Update_DynamicField(float fDelta)
{
	PFC_THISFUNC;
	for (DYNAMIC_FIELD_GROUP_MAP::iterator itor = m_mapDynamicFieldMasters.begin(); itor != m_mapDynamicFieldMasters.end(); )
	{
		GDynamicFieldMaster* pDymFieldGroup = itor->second;

		pDymFieldGroup->Update(fDelta);

		if (pDymFieldGroup->m_bDeleteMe)
		{
			OnDeleteDynamicFieldMaster(pDymFieldGroup);

			itor = m_mapDynamicFieldMasters.erase(itor);

			pDymFieldGroup->Destroy();
			delete pDymFieldGroup;
		}
		else
		{
			++itor;
		}
	}
}

void GFieldMgr::Update_CommonField(float fDelta)
{
	PFC_THISFUNC;
	for (FIELD_BY_UID_MAP::iterator itor = m_mapFields.begin(); itor != m_mapFields.end(); )
	{
		GField* pField = itor->second;

		{
			PFC_BLOCK("GFieldMgr::Update_CommonField - pField->Update");
			pField->Update(fDelta);
		}
		
		if (pField->m_bDeleteMe)
		{
			PFC_BLOCK("GFieldMgr::Update_CommonField - delete field");
			if (pField->IsSharedField())
			{
				OnDeleteSharedField(ToSharedField(pField));
			}

			itor = m_mapFields.erase(itor);

			pField->Destroy();
			delete pField;
		}
		else
		{
			++itor;
		}
	}
}

void GFieldMgr::OnDeleteDynamicFieldMaster(GDynamicFieldMaster* pDynamicFieldGroup)
{	
	GServerFieldRouter* pServerRouter = gsys.pMasterServerFacade->GetFieldRouter();
	pServerRouter->RequestDelDynamicFieldGroup(pDynamicFieldGroup->GetUID());
}

void GFieldMgr::OnDeleteSharedField(GSharedField* pSharedField)
{
	VALID(pSharedField);

	int			nFieldID	= pSharedField->GetID();
	CHANNELID	nChannelID	= pSharedField->GetChannelID();
	VALID(nChannelID != PRIME_CHANNELID);
	VALID(nChannelID != INVALID_CHANNELID);

	GSharedFieldMaster* pSharedFieldMaster = GetSharedFieldMaster(nFieldID);
	VALID(pSharedFieldMaster);
	pSharedFieldMaster->DeleteChannel(nChannelID);
}

GDynamicFieldMaster* GFieldMgr::CreateDynamicFieldMaster(int nFieldGroupInfoID, DYNAMIC_FIELD_TYPE nType, const MUID& uidCreator)
{
	const MUID uidDynamicFieldGroup = NewUID();

	return CreateDynamicFieldMaster(uidDynamicFieldGroup, nFieldGroupInfoID, nType, uidCreator);
}

GDynamicFieldMaster* GFieldMgr::CreateDynamicFieldMaster(MUID uidNew, int nFieldGroupInfoID, DYNAMIC_FIELD_TYPE nType, const MUID& uidCreator, vector<pair<int, MUID>>& vecDynamicFieldUID/*=vector<pair<int, MUID>>()*/)
{
	FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(nFieldGroupInfoID);
	VALID_RET(pFieldGroupInfo, NULL);
	if (!pFieldGroupInfo)
		return NULL;

	GDynamicFieldMaster* pNewDynamicFieldGroup = m_pFieldFactory->NewDynamicFieldMaster(uidCreator, nType);

	switch (nType)
	{
	case DFT_INNROOM:
		{
			CreateInnRoom(pNewDynamicFieldGroup, pFieldGroupInfo, uidNew, vecDynamicFieldUID);
		}break;

	case DFT_TRIAL:
		{
			CreateTrialField(pNewDynamicFieldGroup, pFieldGroupInfo, uidNew, vecDynamicFieldUID);
		}break;
	case DFT_BATTLEARENA:
		{
			CreateBattleArena(pNewDynamicFieldGroup, pFieldGroupInfo, uidNew, vecDynamicFieldUID);
		}break;
	default:
		_ASSERT(0);
		return NULL;
	}

	pNewDynamicFieldGroup->Create(uidNew, pFieldGroupInfo);
	m_mapDynamicFieldMasters.insert(DYNAMIC_FIELD_GROUP_MAP::value_type(pNewDynamicFieldGroup->GetUID(), pNewDynamicFieldGroup));

	return pNewDynamicFieldGroup;
}

GDynamicFieldMaster* GFieldMgr::GetDynamicFieldMaster(const MUID& uidDynamicFieldGroup)
{
	DYNAMIC_FIELD_GROUP_MAP::iterator itor = m_mapDynamicFieldMasters.find(uidDynamicFieldGroup);
	if (m_mapDynamicFieldMasters.end() == itor) return NULL;

	return (*itor).second;
}

GDynamicFieldMaster* GFieldMgr::GetDynamicFieldMaster( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, NULL);
	return GetDynamicFieldMaster(pPlayer->GetPlayerField().GetDynamicFieldGroupUID());
}

GField* GFieldMgr::GetDynamicField(const MUID& uidDynamicFieldGroup, int nFieldId)
{
	GDynamicFieldMaster* pFieldGroup = GetDynamicFieldMaster(uidDynamicFieldGroup);

	if (pFieldGroup == NULL)
	{
		return NULL;
	}

	return pFieldGroup->Get(nFieldId);
}

vector<GField*> GFieldMgr::GetFieldList(int nFieldID)
{
	vector<GField*> vecFields;
	for (FIELD_BY_UID_MAP::iterator itor = m_mapFields.begin(); itor != m_mapFields.end(); ++itor)
	{
		GField* pField = (GField*)((*itor).second);

		if (pField->GetID() == nFieldID)
		{
			vecFields.push_back(pField);
		}
	}
	return vecFields;
}

GSharedFieldMaster* GFieldMgr::CreateSharedFieldMaster( GFieldInfo* pFieldInfo )
{
	PFI_BLOCK_THISFUNC(6510);
	VALID_RET(pFieldInfo, NULL);

	int nFieldID = pFieldInfo->m_nFieldID;
	GSharedFieldMaster* pSharedFieldMaster = m_pFieldFactory->NewSharedFieldMaster(pFieldInfo);
	m_mapSharedFieldMasters.insert(SHARED_FIELD_MASTER_MAP::value_type(nFieldID, pSharedFieldMaster));	

	return pSharedFieldMaster;
}

void GFieldMgr::CreateInnRoom( GDynamicFieldMaster* pNewDynamicFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, const MUID uidDynamicFieldGroup, vector<pair<int, MUID>>& vecDynamicFieldUID )
{
	PFI_BLOCK_THISFUNC(6511);
	GDynamicFieldPolicy_InnRoom* pNewPolicy = new GDynamicFieldPolicy_InnRoom(pNewDynamicFieldGroup);
	pNewDynamicFieldGroup->SetPolicy(pNewPolicy);		

	for each (const GFieldInfo* pFieldInfo in pFieldGroupInfo->vecFieldInfo)
	{
		MUID uidNewDynamicField = _PickDynamicFieldUID(pFieldInfo, vecDynamicFieldUID);

		GInnRoomField* pInRoomField = m_pFieldFactory->NewInnRoomField(uidNewDynamicField, uidDynamicFieldGroup);
		pInRoomField->Create(pFieldInfo);

		pNewDynamicFieldGroup->Insert(pInRoomField);
		AddField(pInRoomField);
	}
}

void GFieldMgr::CreateTrialField( GDynamicFieldMaster* pNewDynamicFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, const MUID uidDynamicFieldGroup, vector<pair<int, MUID>>& vecDynamicFieldUID )
{
	PFI_BLOCK_THISFUNC(6512);
	GDynamicFieldPolicy_TrialField* pNewPolicy = new GDynamicFieldPolicy_TrialField(pNewDynamicFieldGroup);
	pNewDynamicFieldGroup->SetPolicy(pNewPolicy);

	for each (const GFieldInfo* pFieldInfo in pFieldGroupInfo->vecFieldInfo)
	{
		MUID uidNewDynamicField = _PickDynamicFieldUID(pFieldInfo, vecDynamicFieldUID);

		GTrialField* pTrialField = m_pFieldFactory->NewTrialField(uidNewDynamicField, uidDynamicFieldGroup);
		pTrialField->Create(pFieldInfo);

		pNewDynamicFieldGroup->Insert(pTrialField);
		AddField(pTrialField);
	}
}

void GFieldMgr::CreateBattleArena( GDynamicFieldMaster* pNewDynamicFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, const MUID uidDynamicFieldGroup, vector<pair<int, MUID>>& vecDynamicFieldUID )
{
	PFI_BLOCK_THISFUNC(6513);
	GDynamicFieldPolicy_BattleArena* pNewPolicy = new GDynamicFieldPolicy_BattleArena(pNewDynamicFieldGroup);
	pNewDynamicFieldGroup->SetPolicy(pNewPolicy);

	for each (const GFieldInfo* pFieldInfo in pFieldGroupInfo->vecFieldInfo)
	{
		MUID uidNewDynamicField = _PickDynamicFieldUID(pFieldInfo, vecDynamicFieldUID);

		GTrialField* pTrialField = m_pFieldFactory->NewTrialField(uidNewDynamicField, uidDynamicFieldGroup);
		pTrialField->Create(pFieldInfo);

		pNewDynamicFieldGroup->Insert(pTrialField);
		AddField(pTrialField);
	}
}

MUID GFieldMgr::_PickDynamicFieldUID( const GFieldInfo* pFieldInfo, vector<pair<int, MUID>>& vecDynamicFieldUID )
{
	for (size_t i = 0; i < vecDynamicFieldUID.size(); ++i)
	{
		if (pFieldInfo->m_nFieldID == vecDynamicFieldUID[i].first)
		{
			return vecDynamicFieldUID[i].second;
		}
	}
	return NewUID();
}

void GFieldMgr::OnTimeChangedDetail( int nTime )
{
	for each (const FIELD_BY_UID_MAP::value_type& data in m_mapFields)
	{
		GField* pField = (GField*)(data.second);
		pField->OnTimeChangedDetail(nTime);
	}
}


void GFieldMgr::OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime )
{
	for each (const FIELD_BY_UID_MAP::value_type& data in m_mapFields)
	{
		GField* pField = (GField*)(data.second);
		pField->OnTimeChanged(nOldTime, nNewTime);
	}
}