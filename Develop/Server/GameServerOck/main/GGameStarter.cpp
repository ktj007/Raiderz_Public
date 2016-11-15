#include "stdafx.h"
#include "GGameStarter.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GPlayerGameStart_State.h"
#include "GPlayerGameStart_AutoRebirther.h"
#include "GPlayerGameStart_FieldGater.h"
#include "GWorld.h"
#include "GMasterServerFacade.h"
#include "GFieldMgr.h"
#include "GGlobal.h"
#include "GServerPlayerRouter.h"
#include "GPlayerBattleArena.h"
#include "GMsgCommandFacade.h"
#include "GConfig.h"
#include "GServer.h"


void GGameStarter::Start( MUID uidPlayer, int nReqFieldID, vec3 vReqPos )
{
	// 플레이어 찾기
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayerObject == NULL)
	{
		SetResult(RESULT_FAILED_TO_FIND_PLAYER);
		return;
	}

	GEntityPlayer* pPlayer = pPlayerObject->GetEntity();
	if (pPlayer == NULL) 
	{
		SetResult(RESULT_FAILED_TO_FIND_PLAYER);
		return;
	}


	// 게임시작 상태
	pPlayerObject->OnStartGame();


	GPlayerGameStart_State playerGameStartState = GPlayerGameStart_State(pPlayer);

	// 부활 처리
	GPlayerGameStart_AutoRebirther autoRebirther(pPlayer);
	autoRebirther.Rebirth(playerGameStartState);

	// 내 캐릭터 정보 전송
	gsys.pWorld->SendMyInfo(uidPlayer);	


	// TODO: 공지사항 보여줌

	// 필드 입장
	if (GateToField(pPlayer, playerGameStartState, nReqFieldID, vReqPos) == false)
	{
		SetResult(RESULT_FAILED_TO_GATE);
		return;
	}

	// 게임 시작
	pPlayer->GameStart();

	SetResult(RESULT_SUCCESS);
}

bool GGameStarter::GateToField( GEntityPlayer* pPlayer, GPlayerGameStart_State& playerGameStartState, int nReqFieldID, vec3 vReqPos )
{
	if (nReqFieldID != -1)
	{
		// 요청 필드 입장
		if (!pPlayer->GetPlayerField().GateToRequestField(nReqFieldID, vReqPos))
		{
			return false;
		}
	}
	else
	{
		GPlayerGameStart_FieldGater fieldGater(pPlayer);

		if (!fieldGater.Gate(playerGameStartState))
		{
			// 동적필드 관련 정보 초기화
			pPlayer->GetPlayerField().SetDynamicFieldGroupUID(MUID::Invalid());
			pPlayer->GetPlayerBattleArena().SetBattleArenaUID(MUID::Invalid());

			// 서버에 불려져 있는 첫 필드 입장
			if (!pPlayer->GetPlayerField().GateToFirstField())
			{
				return false;
			}			
		}
	}

	return true;
}