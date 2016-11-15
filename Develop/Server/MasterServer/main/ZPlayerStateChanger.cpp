#include "StdAfx.h"
#include "ZPlayerStateChanger.h"
#include "ZPlayerManager.h"
#include "ZPlayer.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "ZPlayerLogic.h"

#include "ZPlayerCommandRouter.h"
#include "ZGateCommandRouter.h"
#include "ZLoginServerFacade.h"
#include "ZMoveServerCommandRouter.h"


ZPlayerStateChanger::ZPlayerStateChanger()
{
}

ZPlayerStateChanger::~ZPlayerStateChanger()
{
}


bool ZPlayerStateChanger::ChangeToSELECT_CHAR(AID nAID)
{
	if (!ChangeToSELECT_CHAR_Check(nAID)) return false;

	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer) return false;

	pPlayer->SetState(PS_SELECT_CHAR);

	return true;
}
bool ZPlayerStateChanger::ChangeToSELECT_CHAR_Check(AID nAID)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer) return false;

	if (pPlayer->GetState() != PS_MOVE_LOGINSERVER &&		///< 게임서버에서 로그인서버로 이동 시
		pPlayer->GetState() != PS_READY_LOGIN_GAMESERVER &&	///< 게임서버로 첫 진입 중
		pPlayer->GetState() != PS_MOVE_GAMESERVER)			///< 게임서버 이동 중
	{
		mlog("Error! ZPlayerStateChanger::ChangeToSELECT_CHAR( AID= %d ), Player State is INVALID! (State: %d)\n", nAID, pPlayer->GetState());
		return false;
	}

	return true;
}

bool ZPlayerStateChanger::ChangeToREADY_LOGIN_GAMESERVER(AID nAID)
{
	RVALID_RET(nAID > 0, false);


	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (pPlayer == NULL)
	{
		mlog("Failed! ZPlayerStateChanger::ChangeToREADY_LOGIN_GAMESERVER(), Not Exist Player! ( AID= %d )\n", nAID);
		return false;
	}

	// 상태 체크
	if (pPlayer->GetState() != PS_SELECT_CHAR)
	{
		mlog("Failed! ZPlayerStateChanger::ChangeToREADY_LOGIN_GAMESERVER(), Player State is Not PS_SELECT_CHAR. ( AID= %d )\n", nAID);
		return false;
	}

	pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);

	return true;
}

bool ZPlayerStateChanger::Cancel_READY_LOGIN_GAMESERVER(AID nAID)
{
	RVALID_RET(nAID > 0, false);


	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (pPlayer == NULL)
	{
		mlog("Failed! ZPlayerStateChanger::Cancel_READY_LOGIN_GAMESERVER(), Not Exist Player! ( AID= %d )\n", nAID);
		return false;
	}

	// 상태 체크
	if (pPlayer->GetState() != PS_READY_LOGIN_GAMESERVER)
	{
		mlog("Failed! ZPlayerStateChanger::Cancel_READY_LOGIN_GAMESERVER(), Player State is Not PS_READY_LOGIN_GAMESERVER. ( AID= %d , State= %d )\n", nAID, pPlayer->GetState());
		return false;
	}

	pPlayer->SetState(PS_SELECT_CHAR);

	return true;
}

bool ZPlayerStateChanger::ChangeToIN_WORLD(AID nAID)
{
	RVALID_RET(nAID > 0, false);

	if (!ChangeToIN_WORLD_Check(nAID)) return false;

	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer) return false;

	pPlayer->SetState(PS_IN_WORLD);

	return true;
}

bool ZPlayerStateChanger::ChangeToIN_WORLD_Check(AID nAID)
{
	RVALID_RET(nAID > 0, false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->Find(nAID);
	if (NULL == pPlayer)
	{
		mlog("Failed! ZPlayerStateChanger::ChangeToIN_WORLD_Check(), Not Exist Player( AID= %d )\n", nAID);
		return false;
	}

	if (pPlayer->GetState() != PS_MOVE_GAMESERVER)
	{
		mlog3("Failed! ZPlayerStateChanger::ChangeToIN_WORLD_Check(), PlayerState=%d\n", pPlayer->GetState());
		return false;
	}

	return true;
}

bool ZPlayerStateChanger::ChangeToREADY_MOVE_GAMESERVER(MUID uidPlayer)
{
	RVALID_RET(uidPlayer > 0, false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
	{
		mlog("Failed! ZPlayerStateChanger::ChangeToREADY_MOVE_SERVER(), Not Exist Player ( UID= %u:%u )\n", uidPlayer.High, uidPlayer.Low);
		return false;
	}

	if (pPlayer->GetState() != PS_IN_WORLD)
	{
		mlog("Error! ZPlayerStateChanger::ChangeToMOVE_SERVER(), Player State is not PS_IN_WORLD\n");
		return false;
	}

	pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

	return true;
}

bool ZPlayerStateChanger::Cancel_READY_MOVE_GAMESERVER(MUID uidPlayer)
{
	RVALID_RET(uidPlayer > 0, false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
	{
		mlog("Failed! ZPlayerStateChanger::Cancel_READY_MOVE_SERVER(), Not Exist Player ( UID= %u:%u )\n", uidPlayer.High, uidPlayer.Low);
		return false;
	}

	if (pPlayer->GetState() != PS_READY_MOVE_GAMESERVER &&
		pPlayer->GetState() != PS_MOVE_GAMESERVER)
	{
		mlog("Failed! ZPlayerStateChanger::Cancel_READY_MOVE_SERVER(), Invalid Player State ( State= %d )\n", pPlayer->GetState());
		return false;
	}

	pPlayer->SetState(PS_IN_WORLD);

	return true;
}

bool ZPlayerStateChanger::ChangeToMOVE_GAMESERVER(MUID uidPlayer)
{
	RVALID_RET(uidPlayer > 0, false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
	{
		mlog("Error! ZPlayerStateChanger::ChangeToMOVE_SERVER(), Not Exist Player ( UID= %u:%u )\n", uidPlayer.High, uidPlayer.Low);
		return false;
	}

	// 상태 검사.
	if (pPlayer->GetState() != PS_READY_LOGIN_GAMESERVER &&
		pPlayer->GetState() != PS_READY_MOVE_GAMESERVER)
	{
		mlog("Error! ZPlayerStateChanger::ChangeToMOVE_SERVER(), Player State is not PS_READY_MOVE_SERVER\n");
		return false;
	}

	pPlayer->SetState(PS_MOVE_GAMESERVER);

	return true;
}

bool ZPlayerStateChanger::ChangeToREADY_MOVE_LOGINSERVER(const MUID& uidPlayer)
{
	RVALID_RET(uidPlayer > 0, false);
	
	if (!ChangeToREADY_MOVE_LOGINSERVER_Check(uidPlayer)) return false;

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL) return false;

	pPlayer->SetState(PS_READY_MOVE_LOGINSERVER);

	return true;
}
bool ZPlayerStateChanger::ChangeToREADY_MOVE_LOGINSERVER_Check(const MUID& uidPlayer)
{
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
	{
		mlog("Failed! ZPlayerStateChanger::ChangeToREADY_MOVE_LOGINSERVER_Check(), Not Exist Player ( UID= %u:%u )\n", uidPlayer.High, uidPlayer.Low);
		return false;
	}

	if (pPlayer->GetState() != PS_IN_WORLD) return false;
	return true;
}

bool ZPlayerStateChanger::Cancel_READY_MOVE_LOGINSERVER(MUID uidPlayer)
{
	RVALID_RET(uidPlayer > 0, false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
	{
		mlog("Failed! ZPlayerStateChanger::Cancel_READY_MOVE_LOGINSERVER(), Not Exist Player ( UID= %u:%u )\n", uidPlayer.High, uidPlayer.Low);
		return false;
	}

	// 상태 체크
	if (pPlayer->GetState() != PS_READY_MOVE_LOGINSERVER&&
		pPlayer->GetState() != PS_MOVE_LOGINSERVER)	return false;

	if (pPlayer->GetState() == PS_IN_WORLD)	return true;

	pPlayer->SetState(PS_IN_WORLD);

	return true;
}

bool ZPlayerStateChanger::ChangeToMOVE_LOGINSERVER(const MUID& uidPlayer)
{
	RVALID_RET(uidPlayer > 0, false);

	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);
	if (pPlayer == NULL)
	{
		mlog("Error! ZPlayerStateChanger::ChangeToMOVE_LOGINSERVER(), Not Exist Player ( UID= %u:%u )\n", uidPlayer.High, uidPlayer.Low);
		return false;
	}

	// 상태 검사.
	if (pPlayer->GetState() != PS_READY_MOVE_LOGINSERVER)
	{
		mlog("Error! ZPlayerStateChanger::ChangeToMOVE_LOGINSERVER(), Player State is not PS_READY_MOVE_LOGINSERVER! (State: %d)\n", pPlayer->GetState());
		return false;
	}

	pPlayer->SetState(PS_MOVE_LOGINSERVER);

	return true;
}
