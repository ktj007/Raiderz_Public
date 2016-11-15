#include "stdafx.h"
#include "XDummyHandler_FieldInsertChar.h"
#include "XBirdDummyAgent.h"
#include "XDummyPattern.h"
#include "MiNetHelper.h"
#include "CCommandResultTable.h"
#include "XCmdParam_LoginServer.h"
#include "XPost_Char.h"

minet::MCommandResult XDummyHandler_InsertChar::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_COMM_RESPONSE_LOGIN:
		{
			return OnResponseLogin(pAgent, pClient, pCmd);
		}
		break;
	case MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST:
		{
			return OnResponseAccountCharList(pAgent, pClient, pCmd);
		}
		break;
	case MC_CHAR_RESPONSE_INSERT_CHAR:
		{
			return OnResponseAccountCharList(pAgent, pClient, pCmd);
		}
		break;
	}
	

	return CR_FALSE;
}

minet::MCommandResult XDummyHandler_InsertChar::OnResponseLogin( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	int nResult;
	pCmd->GetParameter(&nResult, 0, MPT_INT);

	if (nResult != CR_SUCCESS) return CR_FALSE;

	XBIRDPOSTCMD0(pClient, MC_COMM_REQUEST_ACCOUNT_CHAR_LIST);

	return CR_TRUE;
}

minet::MCommandResult XDummyHandler_InsertChar::OnResponseAccountCharList( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	TD_INSERT_CHARACTER_INFO tdInsertCharacterInfo;
	wcscpy_s(tdInsertCharacterInfo.szName, PLAYER_NAME_LEN, pAgent->GetLoginID());
	tdInsertCharacterInfo.nSex = SEX_MALE;
	tdInsertCharacterInfo.nHair = 0;
	tdInsertCharacterInfo.nFace = 0;
	tdInsertCharacterInfo.nHairColor = 1;
	tdInsertCharacterInfo.nSkinColor = 1;

	XBIRDPOSTCMD1(pClient, MC_CHAR_REQUEST_DELETE_CHAR, MCommandParameterChar(0));
	XBIRDPOSTCMD1(pClient, MC_CHAR_REQUEST_INSERT_CHAR, MCommandParameterSingleBlob(tdInsertCharacterInfo));

	return CR_TRUE;
}
