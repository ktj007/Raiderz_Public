#include "StdAfx.h"
#include "SCmdRouter_Login.h"
#include "SCommandCenter.h"
#include "MCommand.h"
#include "CCommandTable.h"
#include "CCommandTable_Login.h"
#include "SDef.h"
#include "CTransData.h"


SCmdRouter_Login::SCmdRouter_Login(SCommandCenter* pCommandCenter)
: m_pCommandCenter(pCommandCenter)
{
	VALID(NULL != m_pCommandCenter);
}

void SCmdRouter_Login::ResponseLogin(const MUID& uidTargetPlayer, const int nResult, int nServerMode/*=SERVER_MODE_NORMAL*/)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_COMM_RESPONSE_LOGIN,
		uidTargetPlayer,
		3,
		NEW_INT(nResult),
		NEW_UID(uidTargetPlayer),
		NEW_UCHAR(nServerMode));

	m_pCommandCenter->PostCommand(pNewCommand);
}

void SCmdRouter_Login::ResponsePmangLogin(const MUID& uidTargetPlayer, const int nResult, int nServerMode/*=SERVER_MODE_NORMAL*/)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_COMM_RESPONSE_LOGIN_ON_PMANG, 
		uidTargetPlayer,
		3,
		NEW_INT(nResult),
		NEW_UID(uidTargetPlayer),
		NEW_UCHAR(nServerMode));

	m_pCommandCenter->PostCommand(pNewCommand);
}

void SCmdRouter_Login::ResponseAccountCharList(const MUID& uidTargetPlayer, const vector<TD_AccountCharInfo>& vecAccountCharInfo)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST
		, uidTargetPlayer
		, 1
		, NEW_BLOB(vecAccountCharInfo));
	
	m_pCommandCenter->PostCommand(pNewCommand);
}

void SCmdRouter_Login::ResponseInsertChar(const MUID& uidTargetPlayer, const int nResult)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_CHAR_RESPONSE_INSERT_CHAR
		, uidTargetPlayer
		, 1
		, NEW_INT(nResult)
		);

	m_pCommandCenter->PostCommand(pNewCommand);
}

void SCmdRouter_Login::ResponseDeleteChar(const MUID& uidTargetPlayer, const int nResult, int nCharIndex)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_CHAR_RESPONSE_DELETE_CHAR
		, uidTargetPlayer
		, 2
		, NEW_INT(nResult)
		, NEW_CHAR(nCharIndex)
		);

	m_pCommandCenter->PostCommand(pNewCommand);
}

void SCmdRouter_Login::ResponseSelMyChar(const MUID& uidTargetPlayer, const int nResult)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_CHAR_RESPONSE_SEL_MYCHAR
		, uidTargetPlayer
		, 1
		, NEW_INT(nResult));

	m_pCommandCenter->PostCommand(pNewCommand);
}

void SCmdRouter_Login::MoveToGameServer(const MUID& uidTargetPlayer, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_COMM_MOVE_TO_GAME_SERVER
		, uidTargetPlayer
		, 1
		, NEW_BLOB(pLoginGameServerInfo, sizeof(TD_LOGIN_GAME_SERVER_INFO), 1));

	m_pCommandCenter->PostCommand(pNewCommand);
}

void SCmdRouter_Login::DuplicatedPlayerLogin(const MUID& uidTargetPlayer)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_COMM_DUPLICATED_PLAYER_LOGIN
		, uidTargetPlayer
		, 0
		, NULL);

	m_pCommandCenter->PostCommand(pNewCommand);
}


void SCmdRouter_Login::PreparingToLoad(const MUID& uidTargetPlayer, int nReqFieldID)
{
	MCommand* pNewCommand = m_pCommandCenter->MakeNewCommand(MC_FIELD_PREPARING_TO_LOAD
		, uidTargetPlayer
		, 1
		, NEW_INT(nReqFieldID));

	m_pCommandCenter->PostCommand(pNewCommand);
}
