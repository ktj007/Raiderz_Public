#include "stdafx.h"
#include "GFieldSystem.h"
#include "GServer.h"
#include "GEntityPlayer.h"
#include "GFieldMgr.h"
#include "GSharedField.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GCommandCenter.h"
#include "GFieldInfoMgr.h"
#include "GSystem.h"
#include "GPlayerField.h"
#include "GPlayerFieldMasterServer.h"
#include "GPlayerFieldShared.h"
#include "GProxyFieldManager.h"
#include "GProxyField.h"
#include "GMasterServerFacade.h"
#include "CCommandResultTable.h"
#include "GConfig.h"
#include "GServerGateRouter.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GPlayerDoing.h"
#include "GClientFieldRouter.h"


bool GFieldSystem::PrepareChangeChannel( GEntityPlayer* pPlayer, CHANNELID nChannelID )
{
	// 채널 이동 예약 체크
	if (false == Check_PrepareChangeChannel(pPlayer, nChannelID)) return false;

	// MasterServer에 채널 이동 예약 요청
	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	if (NULL == pServerRouter) return false;
	pServerRouter->PrepareChangeChannelReq(pPlayer->GetUID(), nChannelID);

	return true;
}
bool GFieldSystem::Check_PrepareChangeChannel( GEntityPlayer* pPlayer, CHANNELID nChannelID )
{
	VALID_RET(pPlayer, false);
	GField* pField = pPlayer->GetField();
	VALID_RET(pField, false);
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(pPlayer->GetUID());
	VALID_RET(pPlayerObject, false);

	if (!pPlayerObject->IsInWorld())											return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_INVALID_STATE);		
	if (pPlayer->GetDoing().IsBusy())											return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_BUSY);					
	if (pPlayer->IsNowCombat())													return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_DONT_DURING_COMBAT);	
	if (!pField->IsSharedField())												return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_UNABLE_PLACE);
	if (nChannelID == ToSharedField(pField)->GetChannelID())					return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_SAME_CHANNEL);
	if (pPlayer->GetPlayerField().GetFieldShared().IsPreparedChangeChannel())	return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_PROCEED);

	// 채널 검사
	if (GConfig::m_bStandAlone)
	{
		GSharedField* pDestChannel = gmgr.pFieldMgr->GetSharedField(pPlayer->GetFieldID(), nChannelID);
		if (NULL == pDestChannel)
		{
			// 존재 하지 않는 채널
			return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_IS_INVALID);
		}
		if (!pDestChannel->IsEnterable())
		{
			// 존재 하지 않는 채널
			return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_IS_FULL);
		}
	}
	else
	{
		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
		VALID_RET(pProxyFieldManager != NULL, false);

		GProxyField* pProxyField = pProxyFieldManager->FindSharedField(pPlayer->GetFieldID(), nChannelID);
		if (NULL == pProxyField)
		{
			// 존재 하지 않는 채널
			return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_IS_INVALID);
		}
		if (!pProxyField->IsEnterable())
		{
			// 존재 하지 않는 채널
			return FailAndRoutePrepareChangeChannelRes(pPlayer, CR_FAIL_CHANGE_CHANNEL_IS_FULL);
		}
	}

	return true;
}

bool GFieldSystem::CancelChangeChannel( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	if (!pPlayer->GetPlayerField().GetFieldShared().IsPreparedChangeChannel())
		return false;

	// MasterServer에 알림
	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	if (NULL == pServerRouter) return false;
	pServerRouter->CancelChangeChannel(pPlayer->GetUID());

	// 채널 이동 예약 정보 제거
	pPlayer->GetPlayerField().GetFieldShared().ClearPrepareChannelChange();

	return true;
}

bool GFieldSystem::ChangeChannel( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	if (false == Check_ChangeChannel(pPlayer)) return false;
	Apply_ChangeChannel(pPlayer);

	return true;
}

bool GFieldSystem::Check_ChangeChannel( GEntityPlayer* pPlayer )
{
	CHANNELID nChannelID	= pPlayer->GetPlayerField().GetFieldShared().GetPrepareChannelID();
	uint32 nPrepareTime		= pPlayer->GetPlayerField().GetFieldShared().GetPrepareChannelTime();

	int32 nElapsedTime = gsys.pSystem->GetNowTime() - nPrepareTime;
	nElapsedTime /= 1000;	// sec 단위변경

	if (nElapsedTime == 0 ||								// 준비단계 무시했음
		nElapsedTime < CHANNEL_CHANGE_PREPARE_TIME_MIN ||	// 너무 빨리 이동요청
		nElapsedTime > CHANNEL_CHANGE_PREPARE_TIME_MAX)		// 너무 늦게 이동요청
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_CHANGE_CHANNEL_INVALID_PREPARE_TIME);
		return false;	// 채널이동을 하기에 유효하지 않은 요청시각입니다.
	}

	GField* pField = pPlayer->GetField();
	VALID_RET(pField, false);
	if (!pField->IsSharedField())	
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_CHANGE_CHANNEL_UNABLE_PLACE);
		return false;	// 정적 필드가 아니면 채널변경 불가능
	}

	if (nChannelID == ToSharedField(pField)->GetChannelID())
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_CHANGE_CHANNEL_SAME_CHANNEL);
		return false;	// 같은 채널은 변경 불가능
	}

	if (GConfig::m_bStandAlone)
	{
		GSharedField* pDestChannel = gmgr.pFieldMgr->GetSharedField(pPlayer->GetFieldID(), nChannelID);	
		if (NULL == pDestChannel)
		{
			pPlayer->FailAndRouteSystemMsg(CR_FAIL_CHANGE_CHANNEL_IS_INVALID);
			return false;	// 존재 하지 않는 채널
		}
		if (!pDestChannel->HasReservation_ForStandAlone(pPlayer->GetUID()))
		{
			pPlayer->FailAndRouteSystemMsg(CR_FAIL_CHANGE_CHANNEL_INVALID_PREPARE_TIME);
			return false;	// 예약되지 않은 플레이어
		}
	}
	else
	{
		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
		VALID_RET(pProxyFieldManager != NULL, false);

		GProxyField* pProxyField = pProxyFieldManager->FindSharedField(pPlayer->GetFieldID(), nChannelID);
		if (NULL == pProxyField)
		{
			pPlayer->FailAndRouteSystemMsg(CR_FAIL_CHANGE_CHANNEL_IS_INVALID);
			return false;	// 존재 하지 않는 채널
		}
	}
	return true;
}
void GFieldSystem::Apply_ChangeChannel( GEntityPlayer* pPlayer )
{
	CHANNELID nChannelID = pPlayer->GetPlayerField().GetFieldShared().GetPrepareChannelID();

	/// MasterServer 에 채널 이동 문의
	pPlayer->GetPlayerField().GetFieldMasterServer().AskGateChangeChannel(pPlayer->GetFieldID(), pPlayer->GetPos(), pPlayer->GetDir(), nChannelID);

	pPlayer->GetPlayerField().GetFieldShared().ClearPrepareChannelChange();
}


void GFieldSystem::Warp( GEntityActor* pActor, vec3 vecDestPos, vec3 vecDestDir, bool bRoute )
{
	pActor->ClearEnteredSensorID();

	int nOldSectorX = pActor->GetSectorX();
	int nOldSectorY = pActor->GetSectorY();

	pActor->SetPos(vecDestPos);
	pActor->SetDir(vecDestDir);
	pActor->SetFacingDir(vecDestDir);

	// 안티치트
	if (pActor->IsPlayer())
		ToEntityPlayer(pActor)->CheatCheck_Warp();

	if (bRoute)
	{
		MCommand* pNewCommand = MakeNewCommand(MC_ENTITY_WARP,
									3, 
									NEW_UID(pActor->GetUID()),
									NEW_VEC(vecDestPos),
									NEW_VEC(vecDestDir)
									);

		// 워프가 이웃섹터의 이웃섹터내에서 이루어졌다면 주변 통보
		int nDifferenceX = abs(nOldSectorX - pActor->GetSectorX());
		int nDifferenceY = abs(nOldSectorY - pActor->GetSectorY());		
		
		if (nDifferenceX <= 2 && nDifferenceY <= 2)
		{
			pActor->RouteToThisCell(pNewCommand->Clone(), pActor->GetUID());
		}

		pActor->RouteToMe(pNewCommand);
	}
}

bool GFieldSystem::FailAndRoutePrepareChangeChannelRes( GEntityPlayer* pPlayer, CCommandResultTable error_code )
{
	GClientFieldRouter router;
	router.PrepareChangeChannel(pPlayer, error_code);
	return false;
}