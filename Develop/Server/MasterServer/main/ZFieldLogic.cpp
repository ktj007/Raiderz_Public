#include "stdafx.h"
#include "ZFieldLogic.h"
#include "ZFieldManager.h"
#include "ZSharedField.h"
#include "ZFieldInfoManager.h"
#include "ZGlobal.h"
#include "ZFieldCommandRouter.h"
#include "ZGateCommandRouter.h"
#include "ZSmartFieldAllocator.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZSharedField.h"
#include "ZDynamicField.h"
#include "ZDynamicFieldMaster.h"
#include "ZFieldFinder.h"
#include "ZParty.h"
#include "ZPartyManager.h"
#include "ZPartyLogic.h"
#include "ZSharedFieldMaster.h"
#include "ZPlayerLogic.h"


void ZFieldLogic::AllFieldDeleteInGameServer(int nGameServerID)
{
	ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(nGameServerID);
	if (pGameServerObject != NULL)
	{
		pGameServerObject->GetFieldMap().Clear();
	}

	gmgr.pFieldManager->ClearOnGameServer(nGameServerID);
}

bool ZFieldLogic::OnRequestDelDynamicFieldGroup(MUID uidRequestServer, MUID uidFieldGroup)
{
	ZGameServerObject* pRequestServerObject = gmgr.pGameServerObjectManager->FindGameServer(uidRequestServer);
	if (pRequestServerObject == NULL)	return false;

	ZDynamicFieldMaster* pFieldGroup = pRequestServerObject->GetFieldMap().FindDynamicFieldMaster(uidFieldGroup);
	if (NULL == pFieldGroup) return false;

	pRequestServerObject->GetFieldMap().DelDynamicFieldMaster(uidFieldGroup);
	gmgr.pFieldManager->DestroyDynamicFieldMaster(uidFieldGroup, true);

	return true;
}

void ZFieldLogic::OnGateAskSharedField(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, TD_GATE_INFO* pGateInfo)
{
	// 이미 예약된 필드가 있는지 확인 - 로그인 서버에서 게임서버로 이동할 때 이미 이동처리가 됐을 경우
	MUID uidReservedField = pRequestPlayer->GetReservedFieldUID();
	if (uidReservedField.IsValid())
	{
		ZSharedField* pReservedFIeld = gmgr.pFieldManager->FindSharedField(uidReservedField);
		if (NULL != pReservedFIeld)
		{
			if (pReservedFIeld->GetID() == pGateInfo->nFieldID &&
				pRequestServer->GetID() == pReservedFIeld->GetGameServerID())
			{
				pGateInfo->nChannelID = pReservedFIeld->GetChannelID();
				ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_EXECUTE, pGateInfo);
				return;
			}
		}
	}

	// 입장할 필드 찾기 - 입장 가능한 공유 필드를 반드시 찾는다.
	ZFieldFinder fieldFinder;
	ZSharedField* pDestField = NULL;
	if (false == fieldFinder.FindEnterableSharedField(pDestField, pGateInfo->nFieldID, pGateInfo->nChannelID))
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_FAIL, NULL);
		mlog3("Error! ZFieldLogic::OnGateAskSharedField(), Failed Find Enterable SharedField(id=%d, channel=%d)\n", pGateInfo->nFieldID, pGateInfo->nChannelID);
		return;
	}

	// 아직 채널이 만들어지지 않음. 재시도 응답.
	if (NULL == pDestField)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_RETRY, pGateInfo);
		return;
	}

	// 필드가 있는 게임서버 찾기
	int nTargetGameServerID = pDestField->GetGameServerID();
	ZGameServerObject* pTargetServerObject = gmgr.pGameServerObjectManager->FindGameServer(nTargetGameServerID);
	if (NULL == pTargetServerObject)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_FAIL, NULL);
		mlog3("Error! ZFieldLogic::OnGateAskSharedField(), DestField(%d, %d)'s GameServer(%d) is INVALID\n", pGateInfo->nFieldID, pGateInfo->nChannelID, nTargetGameServerID);
		return;
	}

	// Gate 시작 처리
	ZPlayerLogic playerLogic;
	playerLogic.PrepareGateField(pRequestPlayer, pDestField);

	// 결과 전송
	pGateInfo->nChannelID = pDestField->GetChannelID();

	if (pRequestServer->GetUID() == pTargetServerObject->GetUID())
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_EXECUTE, pGateInfo);
	}
	else
	{
		ZGateCommandRouter::RouteGateMoveServer(pRequestServer->GetUID(), pRequestPlayer->GetUID(), nTargetGameServerID, pGateInfo);
	}
}

void ZFieldLogic::OnGateAskDynamicField(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, const TD_GATE_FIELD_GROUP_INFO* pGateFieldGroupInfo)
{
	MUID uidFieldGroup	= pGateFieldGroupInfo->GateInfo.uidDynamicFieldGroup;
	int nFieldGroupID	= pGateFieldGroupInfo->nFieldGroupID;
	bool isSingleType	= pGateFieldGroupInfo->isSingleType;
	DYNAMIC_FIELD_TYPE eType= (DYNAMIC_FIELD_TYPE)pGateFieldGroupInfo->nType;

	// 입장할 필드 그룹 찾기
	ZFieldFinder fieldFinder;
	ZDynamicFieldMaster* pFieldGroup = NULL;
	if (uidFieldGroup.IsInvalid())
		pFieldGroup = fieldFinder.GetUnknownFieldGroup(pRequestPlayer->GetUID(), nFieldGroupID, eType, isSingleType);
	else
		pFieldGroup = fieldFinder.GetKnownFieldGroup(pRequestPlayer->GetUID(), uidFieldGroup, nFieldGroupID, eType);

	if (NULL == pFieldGroup)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_REMAIN, NULL);
		mlog3("Error! ZFieldLogic::OnGateAskDynamicField(), Failed Find Enterable FieldGroup(id=%d)\n", nFieldGroupID);
		return;
	}

	// 결과 전송
	int nTargetGameServerID = pFieldGroup->GetGameServerID();
	ZGameServerObject* pTargetGameServer = gmgr.pGameServerObjectManager->FindGameServer(nTargetGameServerID);
	if (NULL == pTargetGameServer)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_FAIL, NULL);
		mlog3("Error! ZFieldLogic::OnGateAskDynamicField(), DestFieldGroup(%d)'s GameServer(%d) is INVALID\n", nFieldGroupID, nTargetGameServerID);
		return;
	}

	// Gate 시작 처리
	ZField* pFirstField = pFieldGroup->FindField(pGateFieldGroupInfo->GateInfo.nFieldID);
	if (NULL == pFirstField)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_FAIL, NULL);
		mlog3("Error! ZFieldLogic::OnGateAskDynamicField(), FirstField is NULL(FieldGroup ID=d%, FirstFIeld ID=%d)\n", nFieldGroupID, pGateFieldGroupInfo->GateInfo.nFieldID);
		return;
	}

	ZPlayerLogic playerLogic;
	playerLogic.PrepareGateField(pRequestPlayer, pFirstField);

	TD_GATE_INFO newGateInfo = pGateFieldGroupInfo->GateInfo;
	newGateInfo.uidDynamicFieldGroup = pFieldGroup->GetUID();

	if (pRequestServer->GetUID() == pTargetGameServer->GetUID())
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_EXECUTE, &newGateInfo);
	}
	else
	{
		ZGateCommandRouter::RouteGateMoveServer(pRequestServer->GetUID(), pRequestPlayer->GetUID(), nTargetGameServerID, &newGateInfo);
	}

	// 후처리
	_GateFieldGroup_End(pRequestPlayer, pFieldGroup);
}

void ZFieldLogic::_GateFieldGroup_End(ZPlayer* pRequestPlayer, ZDynamicFieldMaster* pFieldGroup)
{
	if (NULL == pFieldGroup) return;

	if (DFT_TRIAL == pFieldGroup->GetType() && pFieldGroup->GetPlayerCount() == 1)
	{
		MUID uidParty = pRequestPlayer->GetPartyUID();
		ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
		if (NULL == pParty) return;

		if (MUID::Invalid() != pParty->GetFieldGroupUID(pFieldGroup->GetInfo()->nID))
			return;

		gsys.pPartyLogic->AddField(uidParty, pRequestPlayer->GetUID(), pFieldGroup->GetUID());
	}
}

void ZFieldLogic::OnGatePrepareChangeChannelReq(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, int nChannelID)
{
	VALID(NULL != pRequestServer);
	VALID(NULL != pRequestPlayer);
	VALID(0 < nChannelID);

	if (pRequestPlayer->GetReservedFieldUID().IsValid())
	{
		ZGateCommandRouter::RouteGatePrepareChangeChannelRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), CR_FAIL_CHANGE_CHANNEL_PROCEED, nChannelID);
		return;
	}

	ZSharedField* pDestChannel = gmgr.pFieldManager->FindSharedField(pRequestPlayer->GetFieldID(), nChannelID);
	if (NULL == pDestChannel)
	{
		ZGateCommandRouter::RouteGatePrepareChangeChannelRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), CR_FAIL_CHANGE_CHANNEL_IS_INVALID, nChannelID);
		return;
	}

	if (!pDestChannel->IsEnterable())
	{
		ZGateCommandRouter::RouteGatePrepareChangeChannelRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), CR_FAIL_CHANGE_CHANNEL_IS_FULL, nChannelID);
		return;
	}

	ZPlayerLogic playerLogic;
	playerLogic.PrepareGateField(pRequestPlayer, pDestChannel);

	ZGateCommandRouter::RouteGatePrepareChangeChannelRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), CR_SUCCESS, nChannelID);
}

void ZFieldLogic::OnGateCancelChangeChannel(ZPlayer* pRequestPlayer)
{
	VALID(NULL != pRequestPlayer);

	if (pRequestPlayer->GetReservedFieldUID().IsInvalid())
		return;

	ZPlayerLogic playerLogic;
	playerLogic.CancelPrepareGateField(pRequestPlayer);
}

void ZFieldLogic::OnGateAskChangeChannel(ZGameServerObject* pRequestServer, ZPlayer* pRequestPlayer, const TD_GATE_INFO* pGateInfo)
{
	VALID(NULL != pRequestServer);
	VALID(NULL != pRequestPlayer);
	VALID(NULL != pGateInfo);

	MUID uidReservedField = pRequestPlayer->GetReservedFieldUID();

	ZSharedField* pReservedFIeld = gmgr.pFieldManager->FindSharedField(uidReservedField);
	if (NULL == pReservedFIeld)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_FAIL, NULL);
		return;
	}

	if (pReservedFIeld->GetChannelID() != pGateInfo->nChannelID)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_FAIL, NULL);
		return;
	}

	ZGameServerObject* pTargetServer = gmgr.pGameServerObjectManager->FindGameServer(pReservedFIeld->GetGameServerID());
	if (NULL == pTargetServer)
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_FAIL, NULL);
		return;
	}

	if (pRequestServer->GetUID() == pTargetServer->GetUID())
	{
		ZGateCommandRouter::RouteGateRes(pRequestServer->GetUID(), pRequestPlayer->GetUID(), RGT_EXECUTE, pGateInfo);
	}
	else
	{
		ZGateCommandRouter::RouteGateMoveServer(pRequestServer->GetUID(), pRequestPlayer->GetUID(), pReservedFIeld->GetGameServerID(), pGateInfo);
	}
}
