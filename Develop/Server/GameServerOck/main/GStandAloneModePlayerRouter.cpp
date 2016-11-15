#include "stdafx.h"
#include "GStandAloneModePlayerRouter.h"
#include "GEntityPlayer.h"
#include "GPlayerField.h"
#include "GPlayerFieldEntry.h"
#include "GGameStarter.h"
#include "GGlobal.h"
#include "GMsgCommandFacade.h"
#include "CCommandResultTable.h"
#include "STransData.h"
#include "GConfig.h"
#include "SProxyPlayerManager.h"
#include "GMasterServerFacade.h"


GStandAloneModePlayerRouter::GStandAloneModePlayerRouter()
{
	// do nothing
}

GStandAloneModePlayerRouter::~GStandAloneModePlayerRouter()
{
	// do nothing
}

void GStandAloneModePlayerRouter::RouteDeletePlayer(int nAID, const MUID& uidPlayer)
{
	// do nothing
}

void GStandAloneModePlayerRouter::RoutePlayerState_InWorldReq(const GEntityPlayer* pEntityPlayer)
{
	/// GCmdHandler_Master_Global::OnPlayerStateInWorldRes() 핸들러와 동일한 로직을 처리합니다.
	int nFieldID = const_cast<GEntityPlayer*>(pEntityPlayer)->GetPlayerField().GetFieldEntry().GetReservedWarpFieldID();
	vec3 vReqPos = const_cast<GEntityPlayer*>(pEntityPlayer)->GetPlayerField().GetFieldEntry().GetReservedWarpPos();
	const_cast<GEntityPlayer*>(pEntityPlayer)->GetPlayerField().GetFieldEntry().ClearReserveWarp();


	GGameStarter gameStarter;
	gameStarter.Start(pEntityPlayer->GetUID(), nFieldID, vReqPos);

	// 실패 시, 메시지 전달
	switch(gameStarter.GetResult())
	{
	case GGameStarter::RESULT_FAILED_TO_FIND_PLAYER:
		gsys.pMsgCommandFacade->RouteSystemMsg(pEntityPlayer->GetUID(), CR_FAIL_SYSTEM_INVALID_PC_UID);
		break;
	case GGameStarter::RESULT_FAILED_TO_GATE:
		gsys.pMsgCommandFacade->RouteSystemMsg(pEntityPlayer->GetUID(), CR_FAIL_ENTER_FIELD);
		break;	
	}


	// 프록시 플레이어 정보 추가
	TD_PROXY_PLAYER_INFO info;

	info.nAID = (AID)(pEntityPlayer->GetAID());
	info.nCID = pEntityPlayer->GetCID();
	info.UID = pEntityPlayer->GetUID();
	wcsncpy_s(info.szName, pEntityPlayer->GetName(), _TRUNCATE);
	info.nGameServerID = GConfig::m_nMyServerID;
	info.uidField = pEntityPlayer->GetFieldUID();
	info.uidParty = pEntityPlayer->GetPartyUID();
	info.nGuildID = pEntityPlayer->GetPlayerInfo()->nGID;
	info.nFieldID = pEntityPlayer->GetFieldID();

	SProxyPlayerManager* pPlayerManager = gsys.pMasterServerFacade->GetProxyPlayerManager();
	if (pPlayerManager)
	{
		pPlayerManager->AddPlayer(info);
	}
}

void GStandAloneModePlayerRouter::RoutePlayerInFieldNotify(const GEntityPlayer* pEntityPlayer, MUID uidField, int nFieldID)
{
	// do nothing
}