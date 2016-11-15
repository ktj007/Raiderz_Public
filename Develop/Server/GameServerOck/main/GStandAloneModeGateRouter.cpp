#include "StdAfx.h"
#include "GStandAloneModeGateRouter.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GSharedField.h"
#include "GEntityPlayer.h"
#include "GFieldMgr.h"
#include "GConfig.h"
#include "GPlayerFieldShared.h"
#include "GFieldSystem.h"

void GStandAloneModeGateRouter::PrepareChangeChannelReq(MUID uidRequestPlayer, int nChannelID)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (NULL == pPlayer) return;

	GSharedField* pDestChannel = gmgr.pFieldMgr->GetSharedField(pPlayer->GetFieldID(), nChannelID);
	if (NULL == pDestChannel) return;
	
	// 예약 시작 시간 기록
	pPlayer->GetPlayerField().GetFieldShared().PrepareChangeChannel(nChannelID);

	// 이동 예약
	pDestChannel->ReserveChangeChannel_ForStandAlone(pPlayer->GetUID());
}

void GStandAloneModeGateRouter::CancelChangeChannel(MUID uidRequestPlayer)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (NULL == pPlayer) return;

	int nPreparedFieldID = pPlayer->GetFieldID();
	int nPreparedChannelID = pPlayer->GetPlayerField().GetFieldShared().GetPrepareChannelID();

	// 채널 이동 예약 취소
	GSharedField* pDestChannel = gmgr.pFieldMgr->GetSharedField(nPreparedFieldID, nPreparedChannelID);
	if (NULL != pDestChannel)
		pDestChannel->CancelChangeChannel_ForStandAlone(pPlayer->GetUID());
}

void GStandAloneModeGateRouter::AskGateChangeChannel(MUID uidRequestPlayer, const GGateInfo* pGateInfo)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (NULL == pPlayer) return;

	GSharedField* pSharedField = gmgr.pFieldMgr->GetSharedField(pGateInfo->nFieldID, pGateInfo->nChannelID);
	if (NULL == pSharedField) return;

	pPlayer->GetPlayerField().GetFieldShared().GateExecute(pSharedField, pGateInfo->vPosition, pGateInfo->vDirection);
}
