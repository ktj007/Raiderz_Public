#include "StdAfx.h"
#include "XDummyHandler_DeleteAllCharacter.h"
#include "XBirdDummyAgent.h"
#include "XCmdParam_LoginServer.h"
#include "CCommandResultTable.h"


minet::MCommandResult XDummyHandler_DeleteAllCharacter::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	if (XBirdDummyAgent::DAS_COMPLETE == pAgent->GetStatus()) return CR_FALSE;

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
	case MC_CHAR_RESPONSE_DELETE_CHAR:
		{
			return OnResponseDeleteChar(pAgent, pClient, pCmd);
		}
		break;
	}

	return CR_FALSE;
}

minet::MCommandResult XDummyHandler_DeleteAllCharacter::OnResponseLogin( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	int nResult;
	pCmd->GetParameter(&nResult, 0, MPT_INT);

	if (nResult != CR_SUCCESS) return CR_FALSE;

	XBIRDPOSTCMD0(pClient, MC_COMM_REQUEST_ACCOUNT_CHAR_LIST);

	return CR_TRUE;
}

minet::MCommandResult XDummyHandler_DeleteAllCharacter::OnResponseAccountCharList( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	USES_CONVERSION_EX;

	XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST cmdParam;
	if (cmdParam.Parse(pCmd) == false) return CR_ERROR;

	// 캐릭터 선택. 없으면 생성
	if (0 == cmdParam.m_nBlobCount)
	{
		pAgent->SetStatusComplete();
	}
	else
	{
		// 삭제 메세지 보내기
		XBIRDPOSTCMD1(pClient, MC_CHAR_REQUEST_DELETE_CHAR, MCommandParameterChar(0));
		return CR_TRUE;
	}

	// 없으면 새로 만든다.
	TD_INSERT_CHARACTER_INFO tdInsertCharacterInfo;
	wcscpy_s(tdInsertCharacterInfo.szName, PLAYER_NAME_LEN, pAgent->GetLoginID());
	tdInsertCharacterInfo.nSex = SEX_MALE;
	tdInsertCharacterInfo.nHair = 0;
	tdInsertCharacterInfo.nFace = 0;
	tdInsertCharacterInfo.nHairColor = 0;
	tdInsertCharacterInfo.nSkinColor = 0;

	XBIRDPOSTCMD1(pClient, MC_CHAR_REQUEST_INSERT_CHAR, MCommandParameterSingleBlob(tdInsertCharacterInfo));

	return CR_TRUE;
}

minet::MCommandResult XDummyHandler_DeleteAllCharacter::OnResponseDeleteChar(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	XBIRDPOSTCMD0(pClient, MC_COMM_REQUEST_ACCOUNT_CHAR_LIST);
	return CR_TRUE;
}