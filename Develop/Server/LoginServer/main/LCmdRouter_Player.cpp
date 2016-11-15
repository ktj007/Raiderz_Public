#include "StdAfx.h"
#include "LCmdRouter_Player.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LMasterClient.h"
#include "CCommandTable_Master.h"
#include "CCommandTable_Login.h"
#include "LCommandCenter.h"
#include "PmDef.h"

void LCmdRouter_Player::PostAddPlayerReq(const MUID& uidPlayer, AID nAID, const wstring& strUserID)
{
	minet::MCommand* pNewCommand = gsys.pServer->GetMasterClient()->MakeNewSendCommand(MLC_PLAYER_ADD_PLAYER_REQ
		, 3
		, NEW_UID(uidPlayer)
		, NEW_INT(nAID)
		, NEW_WSTR(strUserID.c_str()));

	gsys.pServer->GetMasterClient()->GetNetClient()->Post(pNewCommand);
}

void LCmdRouter_Player::PostDeletePlayer(AID nAID, const MUID& uidPlayer)
{
	minet::MCommand* pNewCommand = gsys.pServer->GetMasterClient()->MakeNewSendCommand(MLC_PLAYER_DELETE_PLAYER
		, 2
		, NEW_INT(nAID)
		, NEW_UID(uidPlayer));

	gsys.pServer->GetMasterClient()->GetNetClient()->Post(pNewCommand);
}

void LCmdRouter_Player::PostPmangAddPlayerReq(const MUID& uidPlayer, AID nAID, wstring strUserID, const PmUserData* pUserData)
{
	TD_PMANG_USER_DATA tdUserData;
	pUserData->Export(&tdUserData);

	MCommand* pNewCommand = gsys.pServer->GetMasterClient()->MakeNewSendCommand(MLC_PLAYER_PMANG_ADD_PLAYER_REQ
																				, 4
																				, NEW_UID(uidPlayer)
																				, NEW_INT(nAID)
																				, NEW_WSTR(strUserID.c_str())
																				, NEW_SINGLE_BLOB(&tdUserData, sizeof(TD_PMANG_USER_DATA))
																				);

	gsys.pServer->GetMasterClient()->GetNetClient()->Post(pNewCommand);
}

void LCmdRouter_Player::PostPlayerState_SELECT_CHAR(AID nAID, const MUID& uidPlayer)
{
	minet::MCommand* pNewCommand = gsys.pServer->GetMasterClient()->MakeNewSendCommand(MLC_PLAYER_STATE_SELECT_CHAR
		, 2
		, NEW_INT(nAID)
		, NEW_UID(uidPlayer)
		);

	gsys.pServer->GetMasterClient()->GetNetClient()->Post(pNewCommand);
}