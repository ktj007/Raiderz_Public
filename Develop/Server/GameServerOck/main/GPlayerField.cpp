#include "StdAfx.h"
#include "GPlayerField.h"
#include "GPlayerFieldMasterServer.h"
#include "GPlayerFieldShared.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerFieldDynamic_StandAlone.h"
#include "GPlayerFieldLazyGateway.h"
#include "GPlayerFieldGateRetryer.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GFieldSystem.h"
#include "GFieldInfoMgr.h"
#include "GFieldMgr.h"
#include "GEntityPlayer.h"
#include "GSharedField.h"
#include "GDynamicFieldMaster.h"
#include "GInnRoomInfo.h"
#include "GInnRoomInfoMgr.h"
#include "GEnvManager.h"
#include "GCommandCenter.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GRebirthSystem.h"
#include "GPlayerBattleArena.h"
#include "GServer.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GSharedFieldMaster.h"
#include "GASyncTask_MoveGameServer.h"
#include "GConfig.h"
#include "GPlayerSystem.h"
#include "GConst.h"
#include "SProxyPlayer.h"
#include "SProxyPlayerManager.h"
#include "GProxyDynamicFieldMaster.h"
#include "GProxyFieldManager.h"
#include "GMasterServerFacade.h"
#include "CCommandTable_Master.h"



GPlayerField::GPlayerField(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_pLisener(NULL)
{
	m_pFieldMasterServer = new GPlayerFieldMasterServer(m_pOwner, this);
	m_pFieldShared = new GPlayerFieldShared(m_pOwner, this);

	if (GConfig::IsStandAloneMode())
	{
		m_pFieldDynamic = new GPlayerFieldDynamic_StandAlone(m_pOwner, this);
	}
	else
	{
		m_pFieldDynamic = new GPlayerFieldDynamic(m_pOwner, this);
	}

	m_pFieldLazyGateway = new GPlayerFieldLazyGateway(m_pOwner, this);
	m_pFieldGateRetryer = new GPlayerFieldGateRetryer(m_pOwner, this);
}

GPlayerField::~GPlayerField(void)
{
	SAFE_DELETE(m_pFieldGateRetryer);
	SAFE_DELETE(m_pFieldLazyGateway);
	SAFE_DELETE(m_pFieldDynamic);
	SAFE_DELETE(m_pFieldShared);
	SAFE_DELETE(m_pFieldMasterServer);
}

void GPlayerField::SetLisener( GPlayerFieldListener* pLisener )
{
	m_pLisener = pLisener;
	m_FieldEntry.SetLisener(pLisener);
	m_pFieldShared->SetListener(pLisener);
	m_pFieldDynamic->SetListener(pLisener);
}

void GPlayerField::Update(float fDelta)
{
	m_pFieldLazyGateway->ProcessScheduledGate();

	if (m_pFieldGateRetryer->IsRunRetryer())
		m_pFieldGateRetryer->Update(fDelta);
}

bool GPlayerField::GateToMarker(int nFieldID, int nMarkerID)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (!pFieldInfo)
		return false;

	const MARKER_INFO* pMarker = pFieldInfo->FindMarker(nMarkerID);
	if (!pMarker)
		return false;

	return GateToMarker(*pMarker, true) != NULL;
}

bool GPlayerField::GateToMarker(const MARKER_INFO& refMarker, bool bRouteToMe)
{
	return Gate(refMarker.nFieldID, refMarker.vPoint, refMarker.vDir, bRouteToMe);
}

bool GPlayerField::GateToActor(GEntityActor* pActor)
{
	return Gate(pActor->GetFieldID(), pActor->GetPos(), pActor->GetDir(), true) != NULL;
}

bool GPlayerField::GateToTrial(int nDynamicFieldGroupID, bool bSingleEnter)
{
	FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(nDynamicFieldGroupID);
	if (!pFieldGroupInfo)
		return false;

	if (NULL == pFieldGroupInfo->pStartFieldInfo ||
		NULL == pFieldGroupInfo->pStartMarkerInfo) return false;

	GATE_INFO_DYNAMIC sGateInfo(DFT_TRIAL, nDynamicFieldGroupID
		, pFieldGroupInfo->pStartMarkerInfo->nFieldID, pFieldGroupInfo->pStartMarkerInfo->vPoint, pFieldGroupInfo->pStartMarkerInfo->vDir
		, bSingleEnter);

	return GetFieldDynamic().GateRequest(&sGateInfo);
}

bool GPlayerField::GateToInnRoom(void)
{
	GInnRoomInfo* pInnRoomInfo = gmgr.pInnRoomInfoMgr->Get(m_pOwner->GetPlayerInfo()->nInnRoomID);
	if (!pInnRoomInfo)
		return false;

	FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(pInnRoomInfo->m_nFieldGroupID);
	if (!pFieldGroupInfo)
		return false;

	if (NULL == pFieldGroupInfo->pStartFieldInfo ||
		NULL == pFieldGroupInfo->pStartMarkerInfo) return false;

	GATE_INFO_DYNAMIC sGateInfo(DFT_INNROOM, pInnRoomInfo->m_nFieldGroupID
		, pFieldGroupInfo->pStartMarkerInfo->nFieldID, pFieldGroupInfo->pStartMarkerInfo->vPoint, pFieldGroupInfo->pStartMarkerInfo->vDir);

	return GetFieldDynamic().GateRequest(&sGateInfo);
}

void GPlayerField::Warp(vec3 vecDestPos, vec3 vecDestDir, bool bRoute)
{
	gsys.pFieldSystem->Warp(m_pOwner, vecDestPos, vecDestDir, bRoute);
	m_pLisener->OnWarp(vecDestPos, vecDestDir);
}

bool GPlayerField::Gate(int nDestFieldID, vec3 vecDestPos, vec3 vecDestDir, bool bRoute, CHANNELID nChannelID)
{
	// 같은 필드내에 이동
	if (IsSameField(nDestFieldID, nChannelID))
	{
		Warp(vecDestPos, vecDestDir, bRoute);
		return true;
	}

	// 여기서부터는 다른 필드로 이동 ----

	// 투기장 참가중에는 게이트 금지
	if (m_pOwner->GetPlayerBattleArena().IsEntered() &&
		m_pOwner->GetPlayerBattleArena().IsNowPlaying())
		return false;

	// dynamic field
	if (IsDynamicField(nDestFieldID))
	{
		GATE_INFO_DYNAMIC sGateInfo(GetDynamicFieldGroupType(GetDynamicFieldGroupUID()), GetDynamicFieldGroupID(GetDynamicFieldGroupUID()), nDestFieldID, vecDestPos, vecDestDir, false);
		return m_pFieldDynamic->GateRequest(&sGateInfo);
	}

	// shared field
	return GateToSharedFIeld(nDestFieldID, vecDestPos, vecDestDir, nChannelID);
}

bool GPlayerField::IsSameField(int nDestFieldID, CHANNELID nChannelID)
{
	GField* pCurField = m_pOwner->GetField();
	if (NULL == pCurField) return false;

	if (pCurField->GetID() != nDestFieldID)
	{
		return false;
	}

	if (pCurField->IsSharedField())
	{
		if (nChannelID != INVALID_CHANNELID && 
			nChannelID != ToSharedField(pCurField)->GetChannelID())
		{
			return false;
		}
	}
	else if (pCurField->IsDynamicField())
	{
		if (GetDynamicFieldGroupUID() != ToDynamicField(pCurField)->GetGroupUID())
		{
			return false;
		}
	}

	return true;
}

bool GPlayerField::IsDynamicField(int nDestFieldID)
{
	GFieldInfo * pFieldInfo = gmgr.pFieldInfoMgr->Find(nDestFieldID);
	if (NULL == pFieldInfo)
		return false;

	if (pFieldInfo->m_bDynamic)
		return true;

	return false;
}

bool GPlayerField::GateToSharedFIeld(int nFieldID, vec3 vecDestPos, vec3 vecDestDir, CHANNELID nChannelID/*=INVALID_CHANNELID*/)
{
	if (INVALID_CHANNELID == nChannelID)
	{
		GField* pCurField = m_pOwner->GetField();
		if (NULL != pCurField)
		{
			if (pCurField->IsDynamicField())
			{
				nChannelID = m_pFieldDynamic->GetEnterPosChannelID();
			}
			else
			{
				nChannelID = GetChannelID(pCurField);
			}
		}
	}

	return m_pFieldShared->GateRequest(nFieldID, nChannelID, vecDestPos, vecDestDir);
}

bool GPlayerField::GateToSoulBidning()
{
	const MARKER_INFO* pMarker = gsys.pPlayerSystem->GetSoulBindingMarker(m_pOwner->GetSoulBindingID());
	if (NULL == pMarker) return false;

	return m_pOwner->GetPlayerField().GateToMarker(*pMarker);
}

bool GPlayerField::GateToFirstField(void)
{	
	if (IsRunForTest() || GConfig::IsStandAloneMode())
	{
		return GateAnywhere();
	}

	return false;

}

bool GPlayerField::GateToRequestField(int nFieldId, vec3 vecDestPos, vec3 vecDestDir)
{
	if (IsRunForTest() || GConfig::IsStandAloneMode())
	{
		if (Gate(nFieldId, vecDestPos, vecDestDir))
		{
			return true;
		}

		return GateToFirstField();
	}

	return false;
}

bool GPlayerField::GateAnywhere(void)
{
	if (IsRunForTest() || GConfig::IsStandAloneMode())
	{
		// 입장 가능한 아무 필드에나 입장 시킨다.
		GFieldMgr::SHARED_FIELD_MASTER_MAP& mapSharedFieldMaster = gmgr.pFieldMgr->GetSharedFieldMap();	
		if (mapSharedFieldMaster.empty())	return false;

		GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator it = mapSharedFieldMaster.begin();
		
		int nFieldID = it->first;
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
		if (pFieldInfo == NULL)	return false;
		
		vec3 vecDest = vec3::ZERO;
		const MARKER_INFO* pMarkerCheckpoint = pFieldInfo->GetFirstMarker();
		if (pMarkerCheckpoint != NULL)
		{
			vecDest = pMarkerCheckpoint->vPoint;
		}
		
		return Gate(nFieldID, vecDest, vec3(0, 1, 0));
	}

	return false;
}

bool GPlayerField::GateToPlayer( wstring strTarPlayerName )
{
	int nFieldID = 0;
	CHANNELID nChannelID = INVALID_CHANNELID;
	vec3 vPos = vec3::ZERO;
	vec3 vDir = vec3::ZERO;
	MUID uidFieldGroup = MUID::ZERO;

	GEntityPlayer* pTarPlayer = gmgr.pPlayerObjectManager->GetEntityFromPlayerID(strTarPlayerName);
	if (NULL != pTarPlayer)
	{
		GField* pField = pTarPlayer->GetField();
		if (pField==NULL) return false;

		nFieldID = pField->GetInfo()->m_nFieldID;
		vPos = pTarPlayer->GetPos();
		vDir = pTarPlayer->GetDir();

		if (pTarPlayer->IsStayDynamicField())
		{
			uidFieldGroup = pTarPlayer->GetPlayerField().GetDynamicFieldGroupUID();
		}
		else
		{
			nChannelID = ToSharedField(pField)->GetChannelID();
		}
	}
	else
	{
		SProxyPlayerManager* pPlayerMananger = gsys.pMasterServerFacade->GetProxyPlayerManager();
		if (NULL == pPlayerMananger) return false;
		SProxyPlayer* pTarPlayer = pPlayerMananger->FindPlayer(strTarPlayerName);
		if (NULL == pTarPlayer) return false;
		GProxyFieldManager* pFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
		if (NULL == pFieldManager) return false;

		nFieldID = pTarPlayer->GetFieldID();	

		if (MUID::ZERO != pTarPlayer->GetFieldGroupUID())
		{
			GProxyDynamicFieldMaster* pFieldGroup = pFieldManager->FindFieldGroup(uidFieldGroup);
			if (NULL == pFieldGroup) return false;

			uidFieldGroup = pTarPlayer->GetFieldGroupUID();
		}
		else
		{
			nChannelID = pTarPlayer->GetChannelID();
		}		
	}

	SetDynamicFieldGroupUID(uidFieldGroup);
	Gate(nFieldID, vPos, vDir, true, nChannelID);	

	return true;
}

bool GPlayerField::Summon( wstring strTarPlayerName )
{
	GField* pField = m_pOwner->GetField();
	if (NULL == pField) return false;

	MUID uidFieldGroup = MUID::ZERO;
	CHANNELID nChannelID = INVALID_CHANNELID;
	if (pField->IsDynamicField())
	{
		uidFieldGroup = m_pOwner->GetPlayerField().GetDynamicFieldGroupUID();
	}
	else
	{
		nChannelID = ToSharedField(pField)->GetChannelID();
	}	

	GEntityPlayer* pTarPlayer = gmgr.pPlayerObjectManager->GetEntityFromPlayerID(strTarPlayerName);
	if (NULL != pTarPlayer)
	{
		if (true == pTarPlayer->GetPlayerField().GetFieldEntry().IsReserved())
		{
			return false;
		}

		SetDynamicFieldGroupUID(uidFieldGroup);
		pTarPlayer->GetPlayerField().Gate(pField->GetID(), m_pOwner->GetPos(), m_pOwner->GetDir(), true, nChannelID);
	}
	else
	{
		MCommand* pNewCmdToMaster = gsys.pMasterServerFacade->MakeNewCommand(MMC_GM_SUMMON_REQ, 
			5,
			NEW_WSTR(strTarPlayerName.c_str()),
			NEW_UID(uidFieldGroup),
			NEW_INT(pField->GetID()),
			NEW_INT(nChannelID),
			NEW_VEC(m_pOwner->GetPos()));
		gsys.pMasterServerFacade->Route(pNewCmdToMaster);
	}

	return true;
}

DYNAMIC_FIELD_TYPE GPlayerField::GetDynamicFieldGroupType(MUID uidFieldGroup)
{
	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
	if (NULL != pFieldGroup)
	{
		return pFieldGroup->GetType();
	}
	else
	{
		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
		GProxyDynamicFieldMaster* pProxyFieldGroup = pProxyFieldManager->FindFieldGroup(uidFieldGroup);
		if (NULL != pProxyFieldGroup)
		{
			return pProxyFieldGroup->GetDynamicFieldType();
		}
	}

	return DFT_NONE;
}

int GPlayerField::GetDynamicFieldGroupID(MUID uidFieldGroup)
{
	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
	if (NULL != pFieldGroup)
	{
		return pFieldGroup->GetInfo()->nID;
	}
	else
	{
		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
		GProxyDynamicFieldMaster* pProxyFieldGroup = pProxyFieldManager->FindFieldGroup(uidFieldGroup);
		if (NULL != pProxyFieldGroup)
		{				
			return pProxyFieldGroup->GetInfo()->nID;
		}
	}

	return 0;
}

void GPlayerField::SetDynamicFieldGroupUID( const MUID& uidDynamicFieldGroup )
{
	m_GateInfo.uidDynamicFieldGroup = uidDynamicFieldGroup;
}

//////////////////////////////////////////////////////////////////////////

