#include "stdafx.h"
#include "XDummyHandler_FieldLogin.h"
#include "XBirdDummyAgent.h"
#include "XDummyPattern.h"
#include "MiNetHelper.h"
#include "CCommandResultTable.h"
#include "XCmdParam_LoginServer.h"
#include "XPost_Char.h"

minet::MCommandResult XDummyHandler_FieldLogin::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
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
			return OnResponseInsertChar(pAgent, pClient, pCmd);
		}
		break;
	case MC_CHAR_RESPONSE_SEL_MYCHAR:
		{
			return OnResponseSelMyChar(pAgent, pClient, pCmd);
		}
		break;
	case MC_COMM_MOVE_TO_GAME_SERVER:
		{
			return OnMoveToGameServer(pAgent, pClient, pCmd);
		}
		break;
	case MC_FIELD_CHANGE_FIELD:
		{
			return OnChangeField(pAgent, pClient, pCmd);
		}
		break;
	}

	return CR_FALSE;
}

minet::MCommandResult XDummyHandler_FieldLogin::OnResponseLogin( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	int nResult;
	pCmd->GetParameter(&nResult, 0, MPT_INT);

	if (nResult != CR_SUCCESS) return CR_FALSE;

	XBIRDPOSTCMD0(pClient, MC_COMM_REQUEST_ACCOUNT_CHAR_LIST);

	return CR_TRUE;
}

minet::MCommandResult XDummyHandler_FieldLogin::OnResponseAccountCharList( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	USES_CONVERSION_EX;

	XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST cmdParam;
	if (cmdParam.Parse(pCmd) == false) return CR_ERROR;

	// 캐릭터 선택. 없으면 생성
	if(cmdParam.m_nBlobCount > 0)
	{
		// 선택 메세지 보내기
		XBIRDPOSTCMD1(pClient, MC_CHAR_REQUEST_SEL_MYCHAR, MCommandParameterInt(0));
		return CR_TRUE;
	}

	// 없으면 새로 만든다.
	TD_INSERT_CHARACTER_INFO tdInsertCharacterInfo;
	wcscpy_s(tdInsertCharacterInfo.szName, PLAYER_NAME_LEN, pAgent->GetLoginID());
	tdInsertCharacterInfo.nSex = SEX_FEMALE;
	tdInsertCharacterInfo.nHair = 0;
	tdInsertCharacterInfo.nFace = 0;
	tdInsertCharacterInfo.nHairColor = 1;
	tdInsertCharacterInfo.nSkinColor = 1;
	tdInsertCharacterInfo.nEquipmentColorIndex = 1;
	tdInsertCharacterInfo.nEquipmentIndex = 1;
	tdInsertCharacterInfo.nTalentStyle = TS_DEFENDER;

	XBIRDPOSTCMD1(pClient, MC_CHAR_REQUEST_INSERT_CHAR, MCommandParameterSingleBlob(tdInsertCharacterInfo));
	
	return CR_TRUE;
}

minet::MCommandResult XDummyHandler_FieldLogin::OnResponseInsertChar( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	XCmdParam__MC_CHAR_RESPONSE_INSERT_CHAR cmdParam;
	if (!cmdParam.Parse(pCmd)) return CR_ERROR;

	if (cmdParam.m_nResult == CR_SUCCESS)
	{
		XBIRDPOSTCMD1(pClient, MC_CHAR_REQUEST_SEL_MYCHAR, MCommandParameterInt(0));
		return CR_TRUE;
	}

	mlog("Failed! XDummyHandler_FieldLogin::OnResponseInsertChar() : errcode - %d\n", cmdParam.m_nResult);
	return CR_FALSE;
}

minet::MCommandResult XDummyHandler_FieldLogin::OnResponseSelMyChar( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	XCmdParam__MC_CHAR_RESPONSE_SEL_MYCHAR cmdParam;
	if (cmdParam.Parse(pCmd) == false) return CR_ERROR;

	if (cmdParam.m_nResult != CR_SUCCESS) 
	{
		pClient->Disconnect();
		return CR_TRUE;
	}

	return CR_FALSE;
}

minet::MCommandResult XDummyHandler_FieldLogin::OnMoveToGameServer( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	USES_CONVERSION_EX;

	XCmdParam__MC_COMM_MOVE_TO_GAME_SERVER cmdParam;
	if (cmdParam.Parse(pCmd) == false) return CR_ERROR;

	TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo = cmdParam.m_pLoginGameServerInfo;

	if (pLoginGameServerInfo)
	{
		wstring strIP = pLoginGameServerInfo->strHostName;
		int nPort = (int)pLoginGameServerInfo->nPort;

		pAgent->SetConnectionState(XBirdDummyAgent::MOVING_FROM_SERVER_TO_SERVER);
		pAgent->GetConnectionManager()->MoveFromLoginServerToGameServer(strIP, nPort, pLoginGameServerInfo->uidConnectionKey);
	}

	return CR_FALSE;
}

minet::MCommandResult XDummyHandler_FieldLogin::OnChangeField( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	int nFieldID;
	int nChannelID;
	vec3 vPos;
	if (!pCmd->GetParameter(&nFieldID,		0, MPT_INT))	return CR_ERROR;
	if (!pCmd->GetParameter(&nChannelID,	1, MPT_INT))	return CR_ERROR;
	if (!pCmd->GetParameter(&vPos,			2, MPT_VEC))	return CR_ERROR;

	pAgent->SetLoginField(nFieldID, nChannelID, vPos);
	XBIRDPOSTCMD0(pClient, MC_FIELD_LOADING_COMPLETE);

	return CR_FALSE;
}

//////////////////////////////////////////////////////////////////////////
minet::MCommandResult XDummyHandler_FieldLoginDisconnect::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_FIELD_START_GAME:
		{
			pClient->Disconnect();
			return CR_TRUE;
		}
	case MC_LOCAL_NET_ONDISCONNECT:
		{
			if (pAgent->GetConnectionManager()->GetStatus() != CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER)
			{
				pAgent->PostConnectToLoginServer();
				pAgent->SetDisconnectForTest();
			}
		}
	}

	return CR_FALSE;
}