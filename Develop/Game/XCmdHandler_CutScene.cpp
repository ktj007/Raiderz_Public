#include "StdAfx.h"
#include "XCmdHandler_CutScene.h"
#include "XGame.h"
#include "XMyPlayer.h"
#include "XSensorManager.h"
#include "XNonPlayer.h"

XCmdHandler_CutScene::XCmdHandler_CutScene(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_CUTSCENE_BEGIN_ME,					OnCutSceneBeginMe);
	SetCmdHandler(MC_CUTSCENE_BEGIN_OTHER,				OnCutSceneBeginOther);
	SetCmdHandler(MC_CUTSCENE_END_ME,					OnCutSceneEndMe);
	SetCmdHandler(MC_CUTSCENE_END_OTHER,				OnCutSceneEndOther);
}

MCommandResult XCmdHandler_CutScene::OnCutSceneBeginMe(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCutSceneID = 0;
	if (pCommand->GetParameter(&nCutSceneID,	0, MPT_INT)==false) return CR_ERROR;

	gg.game->OnChangeStart(GPS_CUTSCENE, nCutSceneID);
	gg.game->OnChangeEnd();
	return CR_TRUE;
}

MCommandResult XCmdHandler_CutScene::OnCutSceneBeginOther(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer(uidPlayer);
	if(pPlayer)
	{
		pPlayer->SetCutScene(true);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_CutScene::OnCutSceneEndMe(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCutSceneID = 0;
	if (pCommand->GetParameter(&nCutSceneID,	0, MPT_INT)==false) return CR_ERROR;

	gg.game->OnChangeStart(GPS_GAMEPLAY);
	gg.game->OnChangeEnd();

	return CR_TRUE;
}

MCommandResult XCmdHandler_CutScene::OnCutSceneEndOther(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer(uidPlayer);
	if(pPlayer)
	{
		pPlayer->SetCutScene(false);
	}

	return CR_TRUE;
}