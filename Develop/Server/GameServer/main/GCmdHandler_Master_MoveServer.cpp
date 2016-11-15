#include "StdAfx.h"
#include "GCmdHandler_Master_MoveServer.h"
#include "CCommandTable_Master.h"
#include "GGlobal.h"
#include "GClientMoveServerRouter.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "CCommandResultTable.h"


GCmdHandler_Master_MoveServer::GCmdHandler_Master_MoveServer(MCommandCommunicator* pCC)
: MCommandHandler(pCC)
{
	SetCmdHandler(MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES,	OnReserveMoveLoginServerRes);

}

MCommandResult GCmdHandler_Master_MoveServer::OnReserveMoveLoginServerRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRequesterPlayer;
	int nResult;
	MUID uidConnectionKey;

	if (!pCommand->GetParameter(&uidRequesterPlayer,	0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&nResult,				1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidConnectionKey,		2, MPT_UID))	return CR_ERROR;

	/// 요청 플레이어 확인.
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidRequesterPlayer);
	if (pPlayerObject == NULL) return CR_TRUE;

	if (nResult != CR_SUCCESS)
	{
		dlog("Failed! OnReserveMoveLoginServerRes (CommandResult = %d)\n", nResult);
		pPlayerObject->CancelMoveLoginServer(nResult);	// 서버이동 취소

		return CR_TRUE;
	}
	
	if (pPlayerObject->IsReadyMoveLoginServer() == false)
	{
		dlog("Failed! PlayerObject::IsReadyMoveLoginServer() is false! (POS = %d)\n", pPlayerObject->GetState());
		pPlayerObject->CancelMoveLoginServer(nResult);	// 서버이동 취소

		return CR_TRUE;
	}

	pPlayerObject->SetMoveLoginServerConnectionKey(uidConnectionKey);

	return CR_TRUE;
}