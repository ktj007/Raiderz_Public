#include "stdafx.h"
#include "PCmdHandler_Log.h"
#include "MCommand.h"
#include "../PCommandTable.h"
#include "CCommandTable_Log.h"
#include "../PCommandCenter.h"
#include "CTransData_Log.h"
#include "../PStrings.h"
#include "../PGlobal.h"
#include "../PGameServerObject.h"
#include "../PGameServerObjectManager.h"

PCmdHandler_Log::PCmdHandler_Log(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MLC_COMM_LOGIN_REQ,	OnLoginReq);
	SetCmdHandler(MLC_CHAR_LOG_REQ,		OnCharLogReq);	
}

/// TODO : aibeast - 오래된 코드라 서버간 로그인시 필요한 인자값이 충분치 않음. 차후 수정합니다.
MCommandResult PCmdHandler_Log::OnLoginReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	int nCommandVersion;
	if (pCmd->GetParameter(&nCommandVersion,0, MPT_INT)==false) return CR_FALSE;

	MUID uidClient = pCmd->GetSenderUID();
	if (uidClient.IsInvalid()) return CR_FALSE;

	// 커맨드 버전 확인
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
	{
		MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MLC_COMM_LOGIN, uidClient, 2, NEW_INT(CR_FAIL_LOGIN_COMMAND_INVALID_VERSION), NEW_UID(uidClient));
		gsys.pCommandCenter->PostCommand(pNewCommand);
		return CR_TRUE;
	}

	// 게임 서버 생성
	PGameServerObject* pNewGameServer = static_cast<PGameServerObject*>(gmgr.pGameServerObjectManager->NewObject(uidClient, 0));
	gmgr.pGameServerObjectManager->AddObject(pNewGameServer);

	// 로그인 응답
	MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MLC_COMM_LOGIN, uidClient, 2, NEW_INT(CR_SUCCESS), NEW_UID(uidClient));
	gsys.pCommandCenter->PostCommand(pNewCommand);

	return CR_TRUE;
}

MCommandResult PCmdHandler_Log::OnCharLogReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(pCmd->GetSenderUID());
	if (NULL == pGameServer) return CR_FALSE;

	vector<TDL_DATE> vecTDDate;
	int nCID;
	LOG_CHAR_TYPE nType;
	wstring strIP;
	
	if (pCmd->GetBlob(vecTDDate,0)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nCID,1, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nType,2, MPT_INT)==false) return CR_FALSE;	
	if (pCmd->GetParameter(strIP,3, MPT_WSTR)==false) return CR_FALSE;	

	if (1 != vecTDDate.size()) return CR_FALSE;
	wstring strDate = GStrings::tmToString(vecTDDate[0].date);

	pGameServer->InsertLog_Char(strDate, nCID, nType, strIP);	

	return CR_TRUE;
}

MCommandResult PCmdHandler_Log::OnItemAcquireLogReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(pCmd->GetSenderUID());
	if (NULL == pGameServer) return CR_FALSE;

	vector<TDL_DATE> vecTDDate;
	int nCID;
	LOG_ITEM_ACQUIRE_TYPE nType;
	vector<TDL_LOG_ITEM> vecTDItem;
	int nGold;

	if (pCmd->GetBlob(vecTDDate,0)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nCID,1,MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nType,2,MPT_INT)==false) return CR_FALSE;	
	if (pCmd->GetBlob(vecTDItem,3)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nGold,4,MPT_INT)==false) return CR_FALSE;	

	if (1 != vecTDDate.size()) return CR_FALSE;
	wstring strDate = GStrings::tmToString(vecTDDate[0].date);

	for (vector<TDL_LOG_ITEM>::iterator itor = vecTDItem.begin(); itor != vecTDItem.end(); ++itor)
	{
		pGameServer->InsertLog_ItemAcquire(strDate, nCID, nType, (*itor).nIUID, (*itor).nQuantity, nGold);
	}	

	return CR_TRUE;
}

MCommandResult PCmdHandler_Log::OnItemLostLogReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(pCmd->GetSenderUID());
	if (NULL == pGameServer) return CR_FALSE;

	vector<TDL_DATE> vecTDDate;
	int nCID;
	LOG_ITEM_LOST_TYPE nType;
	vector<TDL_LOG_ITEM> vecTDItem;
	int nGold;

	if (pCmd->GetBlob(vecTDDate,0)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nCID,1,MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nType,2,MPT_INT)==false) return CR_FALSE;	
	if (pCmd->GetBlob(vecTDItem,3)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nGold,4,MPT_INT)==false) return CR_FALSE;	

	if (1 != vecTDDate.size()) return CR_FALSE;
	wstring strDate = GStrings::tmToString(vecTDDate[0].date);

	for (vector<TDL_LOG_ITEM>::iterator itor = vecTDItem.begin(); itor != vecTDItem.end(); ++itor)
	{
		pGameServer->InsertLog_ItemLost(strDate, nCID, nType, (*itor).nIUID, (*itor).nQuantity, nGold);
	}	

	return CR_TRUE;
}

MCommandResult PCmdHandler_Log::OnItemTradeLogReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(pCmd->GetSenderUID());
	if (NULL == pGameServer) return CR_FALSE;

	vector<TDL_DATE> vecTDDate;
	int nCID1, nCID2;
	vector<TDL_LOG_ITEM> vecTDItem1, vecTDItem2;
	int nGold1, nGold2;

	if (pCmd->GetBlob(vecTDDate,	0)==false) return CR_FALSE;

	if (pCmd->GetParameter(&nCID1,	1,MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetBlob(vecTDItem1,	2)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nGold1,	3,MPT_INT)==false) return CR_FALSE;	

	if (pCmd->GetParameter(&nCID2,	4,MPT_INT)==false) return CR_FALSE;	
	if (pCmd->GetBlob(vecTDItem2,	5)==false) return CR_FALSE;	
	if (pCmd->GetParameter(&nGold2,	6,MPT_INT)==false) return CR_FALSE;	

	if (1 != vecTDDate.size()) return CR_FALSE;
	wstring strDate = GStrings::tmToString(vecTDDate[0].date);

	pGameServer->InsertLog_ItemTrade(strDate,	nCID1, vecTDItem1, nGold1, 
												nCID2, vecTDItem2, nGold2);

	return CR_TRUE;
}

MCommandResult PCmdHandler_Log::OnQuestLogReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(pCmd->GetSenderUID());
	if (NULL == pGameServer) return CR_FALSE;

	vector<TDL_DATE> vecTDDate;
	int nCID;
	LOG_QUEST_TYPE nType;
	int nQID;

	if (pCmd->GetBlob(vecTDDate,0)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nCID,1,MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nType,2,MPT_INT)==false) return CR_FALSE;	
	if (pCmd->GetParameter(&nQID,3,MPT_INT)==false) return CR_FALSE;	

	if (1 != vecTDDate.size()) return CR_FALSE;
	wstring strDate = GStrings::tmToString(vecTDDate[0].date);

	pGameServer->InsertLog_Quest(strDate, nCID, nType, nQID);

	return CR_TRUE;
}