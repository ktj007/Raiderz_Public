#include "StdAfx.h"
#include "XCmdHandler_BattleArena.h"
#include "XModuleActorControl.h"
#include "XMyPlayer.h"
#include "XController.h"
#include "XGame.h"
#include "XBattleArenaInfo.h"
#include "XBattleArena.h"
#include "XMMCode.h"
#include "XStrings.h"

XCmdHandler_BattleArena::XCmdHandler_BattleArena(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_BATTLEARENA_REGISTER,				OnRegister);
	SetCmdHandler(MC_BATTLEARENA_DEREGISTER,			OnDeregister);
	SetCmdHandler(MC_BATTLEARENA_MATCHED,				OnMatched);
	SetCmdHandler(MC_BATTLEARENA_COUNTING,				OnCounting);
	SetCmdHandler(MC_BATTLEARENA_GAMESTART,				OnGameStart);
	SetCmdHandler(MC_BATTLEARENA_READY_DEATHMATCH,		OnReadyDeathmatch);
	SetCmdHandler(MC_BATTLEARENA_SCOREBOARD_DEATHMATCH,	OnScoreboardDeathmatch);
	SetCmdHandler(MC_BATTLEARENA_DEATHMATCH_UPDATE,		OnUpdateDeathmatch);
	SetCmdHandler(MC_BATTLEARENA_REJOIN_DEATHMATCH,		OnRejoinDeathmatch);
	SetCmdHandler(MC_BATTLEARENA_FINISHED,				OnFinish);
	SetCmdHandler(MC_BATTLEARENA_DIE,					OnDie);	
	SetCmdHandler(MC_BATTLEARENA_KILL_INFO,				OnKillInfo);
}

MCommandResult XCmdHandler_BattleArena::OnRegister(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nGameType = 0;
	if (pCommand->GetParameter(&nGameType,		0, MPT_INT)==false) return CR_ERROR;

	global.ui->OnSystemMsg( XGetStr( L"BATTLEARENA_REGISTERED"));

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnDeregister(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.ui->OnSystemMsg( XGetStr( L"BATTLEARENA_REGISTERCANCELLED"));

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnMatched(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.ui->OnSystemMsg( XGetStr( L"BATTLEARENA_OPPONENTFOUND"));

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnCounting(MCommand* pCommand, MCommandHandler* pHandler)
{
	float fCount=0.0f;
	if (pCommand->GetParameter(&fCount,		0, MPT_FLOAT)==false) return CR_ERROR;

	// 서버에서 날려준 시간에 맞추어서 계속 보여줘야 한다. 1:1 과 같이 매초날아오지 않는다.
	wstring strMsg = CSFormatString::Format( XGetStr( L"BATTLEARENA_ENTERINGAREA"), FSParam((int)fCount));
	global.ui->OnPresentationUpper(strMsg);

	gvar.World.nDynamicFieldExpiredTime = fCount;

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnGameStart(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return CR_ERROR;
	MUID* pEntryUID = (MUID*)pParam->GetPointer();
	_ASSERT(pParam->GetBlobCount() >= 2);

	if (gg.game == NULL) return CR_ERROR;

	gvar.World.nDynamicFieldExpiredTime = 0;

// 	if(global.script)
// 		global.script->GetGlueGameEvent().OnDynamicTimeHide();

	gg.currentgamestate->GetBattleArena()->Start(pEntryUID, pParam->GetBlobCount());

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnReadyDeathmatch(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nMaxLife = 0;
	if (pCommand->GetParameter(&nMaxLife,		0, MPT_INT)==false) return CR_ERROR;

	wstring strMsg = CSFormatString::Format( XGetStr( L"BATTLEARENA_AREAWINRULE"), FSParam(nMaxLife));
	global.ui->OnPresentationUpper(strMsg);

	if(gg.currentgamestate && gg.currentgamestate->GetBattleArena())
	{
		gvar.MyInfo.BattleArenaInfo.SetOutBattle();
		gg.currentgamestate->GetBattleArena()->SetMaxLife(nMaxLife);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnScoreboardDeathmatch(MCommand* pCommand, MCommandHandler* pHandler)
{
	float fWaitSecond = 0.0f;
	MUID uidWinner;
	int nWinnerScore = 0;
	int nLoserScore = 0;
	float fGameElaspedTime = 0.0f;
	if (pCommand->GetParameter(&fWaitSecond,		0, MPT_FLOAT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidWinner,			1, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nWinnerScore,		2, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nLoserScore,		3, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fGameElaspedTime,	4, MPT_FLOAT)==false) return CR_ERROR;

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnRebirthHide();
		global.script->GetGlueGameEvent().OnShowBattleScore(fWaitSecond, uidWinner, nWinnerScore, nLoserScore, fGameElaspedTime);
	}

	// 키 입력 금지
	if ( gg.controller) 
		gg.controller->SetEnabled( false);

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnUpdateDeathmatch(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return CR_ERROR;
	int* pScores = (int*)pParam->GetPointer();
	_ASSERT(pParam->GetBlobCount() >= 2);

	if(gg.currentgamestate && gg.currentgamestate->GetBattleArena())
	{
		gg.currentgamestate->GetBattleArena()->SetCurrentScore(pScores[0], pScores[1]);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnRejoinDeathmatch(MCommand* pCommand, MCommandHandler* pHandler)
{
	float fGameElaspedTime;
	int nMaxLife;
	MUID uidTeam[2];
	int nScore[2];
	if (pCommand->GetParameter(&fGameElaspedTime,	0, MPT_FLOAT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nMaxLife,			1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidTeam[0],			2, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidTeam[1],			3, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nScore[0],			4, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nScore[1],			5, MPT_INT)==false) return CR_ERROR;

	// 이시점에서 gg.game 이 생성되어있지 않아 GetArena()를 사용할수 없어서
	// BattleArenaInfo를 MyInfo로 옮김
	gvar.MyInfo.BattleArenaInfo.SetInBattle();
	gvar.MyInfo.BattleArenaInfo.SetInfo(fGameElaspedTime, nMaxLife, uidTeam, nScore);

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnFinish(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_ERROR;

	gg.currentgamestate->GetBattleArena()->Finish();

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnHideBattleScore();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnDie(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidEntity;
	float fRespawnTime = 0.0f;
	if (pCommand->GetParameter(&uidEntity,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fRespawnTime,	1, MPT_FLOAT)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->Find(uidEntity);
	if (pObject == NULL) return CR_TRUE;

	gg.currentgamestate->GetBattleArena()->SetRespawnTime(fRespawnTime);

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnDie();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_BattleArena::OnKillInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 정보 받기
	MUID uidVictim;
	MUID uidKiller;
	int nKillingTalentID;

	if (!pCommand->GetParameter(&uidVictim, 0, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&uidKiller, 1, MPT_UID))		return CR_ERROR;
	if (!pCommand->GetParameter(&nKillingTalentID, 2, MPT_INT))	return CR_ERROR;

	
	// 이름 찾기
	wstring strVictimName = L"UnknownPlayer";
	XPlayer* pVictim = gg.omgr->FindPlayer(uidVictim);
	if (pVictim != NULL)
	{
		strVictimName = pVictim->GetName();
	}
	
	wstring strKillerName = L"UnknownKiller";
	XPlayer* pKiller = gg.omgr->FindPlayer(uidKiller);
	if (pKiller != NULL)
	{
		strKillerName = pKiller->GetName();
	}

	wstring strTalentName;
	XTalentInfo* pTalentInfo = info.talent->Get(nKillingTalentID);
	if (pTalentInfo != NULL)
	{
		strTalentName = pTalentInfo->GetName();
	}


	// 킬 메시지 조합
/*
	wstringstream killMessageStream;
	
	killMessageStream << strKillerName << L"이(가) " << strVictimName << L"에게 ";
	
	if (!strTalentName.empty())
	{
		killMessageStream << strTalentName << L"에 의해 ";
	}	
	
	killMessageStream << L"제거되었습니다.";

	// NPC 나레이션 출력
	global.ui->OnPresentationUpper(killMessageStream.str());
*/
	wstring strMessage;
	strMessage = FormatString( XGetStr( L"BATTLEARENA_BEGIN"), FSParam( strVictimName, strKillerName));



	return CR_TRUE;
}
