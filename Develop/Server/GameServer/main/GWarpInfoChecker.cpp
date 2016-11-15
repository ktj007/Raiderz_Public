#include "stdafx.h"
#include "GWarpInfoChecker.h"
#include "CSTriggerInfo.h"
#include "GGlobal.h"
#include "GEntityPlayer.h"
#include "GConditionsSystem.h"
#include "GQuestInfoMgr.h"
#include "GQuestInfo.h"
#include "GPartySystem.h"
#include "GParty.h"
#include "GMasterServerFacade.h"
#include "GProxyField.h"
#include "GProxyDynamicFieldMaster.h"
#include "GProxyFieldManager.h"
#include "GConfig.h"
#include "GFieldMgr.h"
#include "GDynamicFieldMaster.h"

GWarpInfoChecker::GWarpInfoChecker(const SENSOR_WARP_INFO& warpInfo)
: m_warpInfo(warpInfo)
{
}

GWarpInfoChecker::~GWarpInfoChecker()
{
	// do nothing
}

bool GWarpInfoChecker::CheckCondition(GEntityPlayer* pPlayer) const
{
	VALID_RET(pPlayer != NULL, false);

	int nConditionID = m_warpInfo.nConditionID;

	// 싱글 퀘스트가 아닌, 자동진입 트라이얼 필드는 파티원중 한명이라도 조건에 만족하면 입장가능.
	if (!IsSingleQuestWarp() && IsAutoTrialWarp())
	{
		return gsys.pConditionsSystem->CheckForPartyDisjunction(pPlayer, nConditionID);
	}

	// 그외 자신만 검사
	return gsys.pConditionsSystem->Check(pPlayer, nConditionID);
}

GWarpInfoChecker::Result GWarpInfoChecker::CheckQuestField(GEntityPlayer* pPlayer) const
{
	VALID_RET(pPlayer != NULL, FAILURE);

	// 워프타입 확인
	if (IsSingleQuestWarp())	return SUCCESS;
	if (!IsTrialWarp())			return SUCCESS;
	
	
	// 파티 확인
	if (!pPlayer->HasParty())	return SUCCESS;

	MUID uidParty = pPlayer->GetPartyUID();
	GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
	VALID_RET(pParty != NULL, SUCCESS);

	
	// 필드 확인
	int nFieldGroupID = m_warpInfo.nFieldGroupID;
	MUID uidFieldGroup = pParty->FindField(nFieldGroupID);

	if (uidFieldGroup.IsInvalid())
	{
		// 새 필드 입장
		int nMemberCount = pParty->GetMemberCount();
		int nLimitCount = GetQuestLimitPersonnel();

		return nMemberCount <= nLimitCount ? SUCCESS : FAILURE;
	}
	else
	{
		// 기존 필드 입장
		int nEnteredCount = 0;
		if (GConfig::IsStandAloneMode())
		{
			GDynamicFieldMaster* pFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
			VALID_RET(pFieldMaster != NULL, FAILURE);

			nEnteredCount = pFieldMaster->GetPlayerQty();
		}
		else
		{
			GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
			VALID_RET(pProxyFieldManager != NULL, FAILURE);
			GProxyDynamicFieldMaster* pProxyFieldGroup = pProxyFieldManager->FindFieldGroup(uidFieldGroup);
			VALID_RET(pProxyFieldGroup != NULL, FAILURE);

			nEnteredCount = pProxyFieldGroup->GetPlayerCount();
		}
		
		int nLimitCount = GetQuestLimitPersonnel();

		return nEnteredCount < nLimitCount ? SUCCESS : FULLHOUSE;
	}
}

bool GWarpInfoChecker::IsTrialWarp(void) const
{
	return m_warpInfo.nFieldGroupID != INVALID_ID;
}

bool GWarpInfoChecker::IsSharedWarp(void) const
{
	return m_warpInfo.nFieldID != INVALID_ID;
}

bool GWarpInfoChecker::IsAutoWarp(void) const
{
	return m_warpInfo.eGateType == TGT_AUTO;
}

bool GWarpInfoChecker::IsManualWarp(void) const
{
	return m_warpInfo.eGateType == TGT_MANUAL;
}

bool GWarpInfoChecker::IsAutoSharedWarp(void) const
{
	return IsSharedWarp() && IsAutoWarp();
}

bool GWarpInfoChecker::IsManualSharedWarp(void) const
{
	return IsSharedWarp() && IsManualWarp();
}

bool GWarpInfoChecker::IsAutoTrialWarp(void) const
{
	return IsTrialWarp() && IsAutoWarp();
}

bool GWarpInfoChecker::IsManualTrialWarp(void) const
{
	return IsTrialWarp() && IsManualWarp();
}

bool GWarpInfoChecker::IsSingleQuestWarp(void) const
{
	if (!IsTrialWarp())	return false;
	
	int nQuestID = m_warpInfo.nQuestID;
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	VALID_RET(pQuestInfo != NULL, false);

	return pQuestInfo->nTrialType == QTT_SINGLE;
}

int GWarpInfoChecker::GetQuestLimitPersonnel(void) const
{
	if (!IsTrialWarp())	return 0;

	int nQuestID = m_warpInfo.nQuestID;
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	VALID_RET(pQuestInfo != NULL, 0);
	
	return pQuestInfo->nLimitPersonnel;
}

int GWarpInfoChecker::GetFieldID(void) const
{
	return m_warpInfo.nFieldID;
}

int GWarpInfoChecker::GetFieldGroupID(void) const
{
	return m_warpInfo.nFieldGroupID;
}

int GWarpInfoChecker::GetMarkerID(void) const
{
	return m_warpInfo.nMarkerID;
}

const SENSOR_WARP_INFO* GWarpInfoChecker::GetSensorWarpInfo(void) const
{
	return &m_warpInfo;
}

bool GWarpInfoChecker::SameFieldWarp(const GWarpInfoChecker* pChecker) const
{
	// 공유 필드의 경우
	if (IsSharedWarp())
	{
		if (pChecker->IsSharedWarp())
		{
			// 같은 필드, 같은 마커로 이동하면 같은 워프
			bool isSameField = GetFieldID() == pChecker->GetFieldID();
			bool isSameMarker = GetMarkerID() == pChecker->GetMarkerID();

			return isSameField && isSameMarker;
		}
	}
	
	// 트라이얼 필드의 경우
	if (IsTrialWarp())
	{
		if (pChecker->IsTrialWarp())
		{
			// nFieldGroupID 비교
			return GetFieldGroupID() == pChecker->GetFieldGroupID();
		}
	}

	return false;
}

int GWarpInfoChecker::GetAllowEnterPlayerCount(GEntityPlayer* pPlayer) const
{
	VALID_RET(pPlayer != NULL, 0);

	// 워프타입 확인
	if (IsSingleQuestWarp())	return 0;
	if (!IsTrialWarp())			return 0;
	
	
	// 파티 확인
	if (!pPlayer->HasParty())	return 0;

	MUID uidParty = pPlayer->GetPartyUID();
	GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
	VALID_RET(pParty != NULL, 0);


	// 필드 확인
	int nFieldGroupID = m_warpInfo.nFieldGroupID;
	MUID uidFieldGroup = pParty->FindField(nFieldGroupID);

	if (uidFieldGroup.IsInvalid())
	{
		return GetQuestLimitPersonnel();
	}
	else
	{
		// 기존 필드 입장
		int nEnteredCount = 0;
		if (GConfig::IsStandAloneMode())
		{
			GDynamicFieldMaster* pFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
			VALID_RET(pFieldMaster != NULL, 0);

			nEnteredCount = pFieldMaster->GetPlayerQty();
		}
		else
		{
			GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
			VALID_RET(pProxyFieldManager != NULL, 0);
			GProxyDynamicFieldMaster* pProxyFieldGroup = pProxyFieldManager->FindFieldGroup(uidFieldGroup);
			VALID_RET(pProxyFieldGroup != NULL, 0);

			nEnteredCount = pProxyFieldGroup->GetPlayerCount();
		}
		
		int nLimitCount = GetQuestLimitPersonnel();
		return nLimitCount - nEnteredCount;
	}
}
