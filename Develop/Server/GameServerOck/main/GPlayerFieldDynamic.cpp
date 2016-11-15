#include "stdafx.h"
#include "GGlobal.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerField.h"
#include "GPlayerFieldMasterServer.h"
#include "GDynamicFieldMaster.h"
#include "GField.h"
#include "GDynamicField.h"
#include "GSharedField.h"
#include "GFieldMgr.h"
#include "GFieldInfoMgr.h"
#include "GEntityPlayer.h"
#include "GPartySystem.h"
#include "GPlayerBattleArena.h"
#include "GBattleArenaMgr.h"
#include "GConfig.h"
#include "GServer.h"
#include "GMasterServerFacade.h"
#include "GDynamicFieldFactory.h"
#include "SCmdRouter_Login.h"
#include "GCommandCenter.h"
#include "GPlayerFieldGateRetryer.h"
#include "CCommandResultTable.h"


GPlayerFieldDynamic::GPlayerFieldDynamic(GEntityPlayer* pOwner, GPlayerField* pPlayerField)
: m_pOwner(pOwner)
, m_pPlayerField(pPlayerField)
{

}

void GPlayerFieldDynamic::SetEnterPos(const MARKER_INFO& EnterPos, CHANNELID nEnterChannelID/*=INVALID_CHANNELID*/)
{
	m_EnterPos = EnterPos;
	m_EnterChannelID = nEnterChannelID;
}

void GPlayerFieldDynamic::ClearEnterPos()
{
	m_EnterChannelID = INVALID_CHANNELID;
	m_EnterPos = MARKER_INFO::Invalid();
}

void GPlayerFieldDynamic::ClearCheckpoint()
{
	m_Checkpoint = MARKER_INFO::Invalid();
}

bool GPlayerFieldDynamic::GateToEnterPos(int nDynamicFieldID)
{
	const MARKER_INFO& rDynamicFieldEnterPos = GetEnterPos();
	VALID_RET(rDynamicFieldEnterPos.IsValid(), false);
	
	GFieldInfo* pEnterFieldInfo = gmgr.pFieldInfoMgr->Find(rDynamicFieldEnterPos.nFieldID);
	VALID_RET(pEnterFieldInfo, false);

	return m_pPlayerField->GateToMarker(rDynamicFieldEnterPos);
}

bool GPlayerFieldDynamic::GateToCheckPoint()
{
	const MARKER_INFO& rDynamicFieldCheckpoint = GetCheckpoint();
	VALID_RET(rDynamicFieldCheckpoint.IsValid(), false);
	return m_pPlayerField->GateToMarker(rDynamicFieldCheckpoint);
}

bool GPlayerFieldDynamic::GateRequest(DYNAMIC_FIELD_TYPE eType, int nFieldGroupID, int nFieldID, vec3 vecDestPos, vec3 vecDestDir, bool bSingleEnter/*=false*/)
{
	if (DFT_NONE == eType || 0 == nFieldGroupID || 0 == nFieldID)
		return false;

	/// Gate Retryer 가 작동 중이라면 처리하지 않는다.
	if (m_pPlayerField->GetFieldGateRetryer().IsRunRetryer())
		return false;

	/// 알고 있는 필드 그룹인지 검사.
	if (NULL == gmgr.pFieldInfoMgr->FindFieldGroup(nFieldGroupID))
	{
		mlog("Error! GPlayerFieldDynamic::GateRequest(), Not Exist FieldGroupInfo. (AID: %I64d , CID: %d , UID(%u:%u) , id: %d)\n"
			, m_pOwner->GetAID(), m_pOwner->GetCID(), m_pOwner->GetUID().High, m_pOwner->GetUID().Low, nFieldGroupID);
		return m_pOwner->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_FIELD_ID);
	}

	// 입장 가능한 Field Group 찾기
	MUID uidDestFieldGroup = MUID::ZERO;

	uidDestFieldGroup = GetEnterableDynamicFieldGroup(eType, nFieldGroupID, bSingleEnter);
	if (uidDestFieldGroup.IsInvalid())
	{
		/// 투기장 최초 입장시 항상 로컬에 만들도록 합니다.
		if (DFT_BATTLEARENA == eType)
		{
			GDynamicFieldFactory dynamicFieldFactory;
			GDynamicFieldMaster* pFieldGroup = dynamicFieldFactory.CreateDynamicField(eType, m_pOwner, nFieldGroupID);
			if (NULL == pFieldGroup)
				return false;

			uidDestFieldGroup = pFieldGroup->GetUID();
		}
	}

	// 로컬에 존재하는 필드 그룹이면 입장 처리.
	if (IsExistLocalServer(uidDestFieldGroup))
	{
		if (!IsValidFieldGroupWith(uidDestFieldGroup, eType, nFieldGroupID))
			return false;
		
		if (!GateExecute(uidDestFieldGroup, nFieldID, vecDestPos, vecDestDir))
			return false;
		
		return true;
	}

	// 로컬 서버에 없거나 UID가 유효하지 않을 경우 Master Server 에 묻는다.
	if (!m_pPlayerField->GetFieldMasterServer().AskGateDynamicField(nFieldID, vecDestPos, vecDestDir, nFieldGroupID, uidDestFieldGroup, eType, bSingleEnter))
		return false;

	return true;
}

bool GPlayerFieldDynamic::GateRequest(const GATE_INFO_DYNAMIC* pGateInfo)
{
	return GateRequest(pGateInfo->eType, pGateInfo->nFieldGroupID, pGateInfo->nFieldID, pGateInfo->vecDestPos, pGateInfo->vecDestDir, pGateInfo->bSingleEnter);
}

bool GPlayerFieldDynamic::GateExecute(MUID uidFieldGroup, int nFieldID, vec3 vecDestPos, vec3 vecDestDir)
{
	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
	VALID_RET(pFieldGroup, false);
	FIELD_ENTER_FAIL_TYPE nEnterFailType = 
		pFieldGroup->IsEnterable();

	switch(nEnterFailType)
	{
	case ENTERFAIL_PROCEED_COMBAT:
		m_pOwner->RouteAdviceMsg(CR_FAIL_ENTER_FIELD_PROCEED_COMBAT);
		return false;
	case ENTERFAIL_PROCEED_INTERMISSION:
		m_pOwner->RouteAdviceMsg(CR_FAIL_ENTER_FIELD_PROCEED_INTERMISION);
		return false;
	}

	GField* pField = pFieldGroup->Get(nFieldID);
	if (!pField) return false;

	m_pPlayerField->SetDynamicFieldGroupUID(pFieldGroup->GetUID());

	if (GetEnterPos().IsInvalid())
	{
		MARKER_INFO markerExit;
		markerExit.nFieldID = m_pOwner->GetFieldID();
		markerExit.vPoint = m_pOwner->GetPos();
				
		
		// 퇴장마커가 워프센서랑 겹치면 정책위반. 필드 첫 마커로 강제 설정한다.
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(markerExit.nFieldID);
		if (pFieldInfo != NULL &&  pFieldInfo->IsInWarpSensor(markerExit.vPoint))
		{
			const MARKER_INFO* pFirstMarker = pFieldInfo->GetFirstMarker();			
			if (pFirstMarker != NULL)
			{
				markerExit.vPoint = pFirstMarker->vPoint;
			}
		}


		CHANNELID nChannelID = GetChannelID(m_pOwner->GetField());
		SetEnterPos(markerExit, nChannelID);
	}

	if (m_pLisener)
	{
		m_pLisener->OnGateExecute(pField, vecDestPos, vecDestDir);
	}

	_EndGateExecute(pFieldGroup);

	return true;
}

MUID GPlayerFieldDynamic::GetEnterableDynamicFieldGroup(DYNAMIC_FIELD_TYPE eType, int nFieldGroupID, bool bIsSingleEnter)
{
	MUID uidFieldGroup = MUID::ZERO;

	uidFieldGroup = GetFieldGroupWhenReconnect();
	if (uidFieldGroup.IsValid())
	{
		return uidFieldGroup;
	}

	switch (eType)
	{
	case DFT_INNROOM:		break;	// 여관은 항상 새로 생성합니다.
	case DFT_BATTLEARENA:	uidFieldGroup = FindEnterableBattleArena(nFieldGroupID);				break;
	case DFT_TRIAL:			uidFieldGroup = FindEnterableTrialField(nFieldGroupID, bIsSingleEnter);	break;
	}

	return uidFieldGroup;
}

MUID GPlayerFieldDynamic::GetFieldGroupWhenReconnect()
{
	GDynamicFieldMaster* pExistFieldMaster = NULL;
	MUID uidFieldGroup = m_pPlayerField->GetDynamicFieldGroupUID();
	if (uidFieldGroup.IsInvalid())
		return MUID::ZERO;

	return uidFieldGroup;
}

MUID GPlayerFieldDynamic::FindEnterableBattleArena(int nFieldGroupID)
{
	MUID uidFieldGroup = MUID::ZERO;

	MUID uidBattleArena = m_pOwner->GetPlayerBattleArena().GetBattleArenaUID();
	GBattleArena* pBattleArena = gmgr.pBattleArenaMgr->FindBattleArena(uidBattleArena);
	if (NULL == pBattleArena)
		return MUID::ZERO;

	if (GBattleArena::INVALID == pBattleArena->GetStatus() ||
		GBattleArena::SCOREBOARD == pBattleArena->GetStatus() ||
		GBattleArena::FINISH == pBattleArena->GetStatus())
		return MUID::ZERO;

	uidFieldGroup = pBattleArena->GetFieldGroupUID();
	if (uidFieldGroup.IsInvalid())
		return MUID::ZERO;

	return uidFieldGroup;
}

MUID GPlayerFieldDynamic::FindEnterableTrialField(int nFieldGroupID, bool bIsSingleEnter)
{
	MUID uidFieldGroup = MUID::ZERO;

	if (bIsSingleEnter)
		return MUID::ZERO;

	if (!gsys.pPartySystem->IsPartyMember(m_pOwner))
		return MUID::ZERO;

	MUID uidParty = m_pOwner->GetPartyUID();
	uidFieldGroup = gsys.pPartySystem->FindField(uidParty, nFieldGroupID);
	if (uidFieldGroup.IsInvalid())
		return MUID::ZERO;

	return uidFieldGroup;
}


bool GPlayerFieldDynamic::IsExistLocalServer(MUID uidFieldGroup)
{
	return (NULL != gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup));
}

bool GPlayerFieldDynamic::IsValidFieldGroupWith(MUID uidDestFieldGroup, DYNAMIC_FIELD_TYPE eType, int nFieldGroupID)
{
	GDynamicFieldMaster* pTarFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(uidDestFieldGroup);
	if (pTarFieldGroup != NULL &&
		pTarFieldGroup->GetType() == eType &&
		pTarFieldGroup->GetInfo()->nID == nFieldGroupID)
		return true;

	return false;
}
