#include "StdAfx.h"
#include "XCmdHandler_GM.h"
#include "XPost_Interaction.h"
#include "XMyPlayer.h"

XCmdHandler_GM::XCmdHandler_GM(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_GM_QUEST_RESET,		OnQuestResetDone);
	SetCmdHandler(MC_GM_QUEST_VAR,			OnQuestVar);
	SetCmdHandler(MC_GM_SHOW_ENEMY_UID,		OnShowEnemyUID);
	SetCmdHandler(MC_GM_LOG_CRT_LIST,		OnLogCRTList);	
	SetCmdHandler(MC_GM_SERVER_DUMP_RESP,	OnResponseServerDump);
	SetCmdHandler(MC_GM_CHANGE_SERVER_MODE,	OnGMChangeServerMode);
}

MCommandResult XCmdHandler_GM::OnQuestResetDone(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.ui->OnSystemMsg(L"수행한 퀘스트가 초기화 되었습니다.");

	return CR_TRUE;
}

MCommandResult XCmdHandler_GM::OnQuestVar(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nQuestVar;
	if (pCommand->GetParameter(&nQuestVar,	0, MPT_INT)==false) return CR_ERROR;

	global.ui->OnSystemMsg(L"QuestVAr : {0}\n", FSParam(nQuestVar));

	return CR_TRUE;
}

MCommandResult XCmdHandler_GM::OnShowEnemyUID(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<MUID> vecEnemyUID;
	if (pCommand->GetBlob(vecEnemyUID,	0)==false) return CR_ERROR;

	for each (MUID uidEnemy in vecEnemyUID)
	{		
		global.ui->OnSystemMsg(L"EnemyUID : {0}", FSParam(uidEnemy));
	}	

	return CR_TRUE;
}


MCommandResult XCmdHandler_GM::OnLogCRTList(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<int> vecLogCRT;
	if (pCommand->GetBlob(vecLogCRT,	0)==false) return CR_ERROR;

	global.ui->OnSystemMsg(L"===LogCRTList===\n");
	for each (int nLogCRT in vecLogCRT)
	{
		global.ui->OnSystemMsg(L"{0}\n", FSParam(nLogCRT));
	}	

	return CR_TRUE;
}

MCommandResult XCmdHandler_GM::OnResponseServerDump(MCommand* pCommand, MCommandHandler* pHandler)
{
#ifndef _PUBLISH
	global.ui->OutputToConsole(L"GameServer에 덤프를 남겼습니다.\n");
#endif

	return CR_TRUE;
}

MCommandResult XCmdHandler_GM::OnGMChangeServerMode(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nServerMode;
	if (pCommand->GetParameter(&nServerMode,	0, MPT_INT)==false) return CR_ERROR;

	if (gvar.Network.nServerMode != nServerMode)
		gvar.Network.nServerMode = (CSServerMode)nServerMode;

	return CR_TRUE;
}
