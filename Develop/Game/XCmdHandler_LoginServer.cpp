#include "StdAfx.h"
#include "XCmdHandler_LoginServer.h"
#include "XNetwork.h"
#include "XConnectionManager.h"
#include "XStrings.h"
#include "XUISystem.h"
#include "XGameFrameID.h"
#include "XGameFrameManager.h"
#include "XPost_Local.h"
#include "MiNetHelper.h"
#include "CCommandResultTable.h"
#include "XBaseApplication.h"
#include "XCharFrame.h"
#include "XPost_Comm.h"
#include "XPost_Char.h"
#include "XCutSceneFieldIntroMgr.h"


XCmdHandler_LoginServer::XCmdHandler_LoginServer(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_COMM_RESPONSE_LOGIN,				OnResponseLogin);
	SetCmdHandler(MC_COMM_RESPONSE_LOGIN_ON_PMANG,		OnResponseLoginOnPmang);
	SetCmdHandler(MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST,	OnResponseAccountCharList);
	SetCmdHandler(MC_COMM_MOVE_TO_GAME_SERVER,			OnMoveToGameServer);
	
	SetCmdHandler(MC_COMM_ENTER_WORLD_MSG,				OnEnterWorldMsg);
	SetCmdHandler(MC_COMM_ENTER_WORLD_LOCAL_MSG,		OnEnterWorldMsgLocal);

	SetCmdHandler(MC_CHAR_RESPONSE_SEL_MYCHAR,			OnResponseSelMyChar);
	SetCmdHandler(MC_CHAR_RESPONSE_INSERT_CHAR,			OnResponseInsertChar);
	SetCmdHandler(MC_CHAR_RESPONSE_DELETE_CHAR,			OnResponseDeleteChar);

	SetCmdHandler(MC_COMM_DUPLICATED_PLAYER_LOGIN,		OnDuplicatedPlayerLogin);
}

MCommandResult XCmdHandler_LoginServer::_OnResponseLogin( XCmdParam__MC_COMM_RESPONSE_LOGIN& cmdParam )
{
	if (cmdParam.m_nResult == CR_SUCCESS)
	{
		if ( global.script)		global.script->GetGlueGameEvent().OnLoginEvent( "LOGIN", "CONNECTED");

		XPostChar_RequestAccountCharList();


		global.net->GetConnectionManager()->OnConnectedToLoginServer();

		gvar.MyInfo.MyUID = cmdParam.m_uidAlloc;
		gvar.Network.nServerMode = (CSServerMode)cmdParam.m_nServerMode;

// #ifndef _PUBLISH
// 		if ( gvar.Network.bDirectLogin == true)	XPostChar_RequestAccountCharList();
// #endif

		return CR_TRUE;
	}
	else
	{
		if ( global.script)		
		{
			global.script->GetGlueGameEvent().OnLoginEvent( "LOGIN", "FAILED");
			global.script->GetGlueGameEvent().OnLoginEvent( "LOGIN_ERROR", MLocale::ConvUTF16ToAnsi(XCmdResultStr(cmdParam.m_nResult)));
		}
	}

	return CR_TRUE;

}


MCommandResult XCmdHandler_LoginServer::OnResponseLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	XCmdParam__MC_COMM_RESPONSE_LOGIN cmdParam;
	if (cmdParam.Parse(pCommand) == false) return CR_ERROR;


	return _OnResponseLogin(cmdParam);
}

MCommandResult XCmdHandler_LoginServer::OnResponseLoginOnPmang(MCommand* pCommand, MCommandHandler* pHandler)
{
	XCmdParam__MC_COMM_RESPONSE_LOGIN cmdParam;
	if (cmdParam.Parse(pCommand) == false) return CR_ERROR;

	return _OnResponseLogin(cmdParam);
}

MCommandResult XCmdHandler_LoginServer::OnResponseAccountCharList(MCommand* pCommand, MCommandHandler* pHandler)
{
	XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST cmdParam;
	if (cmdParam.Parse(pCommand) == false) return CR_ERROR;

	global.script->GetGlueGameEvent().OnLoginEvent( "LOGIN_ERROR", "END");

	cmdParam.AssignTo(gvar.Char.m_CharInfos);


	// 만약 바로 접속인자가 있으면 바로 접속
#ifndef _PUBLISH

	if (gvar.Char.m_CharInfos.empty())
	{
		// 캐릭터가 없다면, 캐릭터 자동 선택 입장을 취소한다.
		gvar.Network.bDirectSelChar = false;
	}

	if ( gvar.Network.bDirectLogin == true  &&  gvar.Network.bDirectSelChar == true)
	{
		int nSelIndex = 0;

		if ( gvar.Network.strSelChar.empty() == false)
		{
			for ( size_t i = 0;  i < gvar.Char.m_CharInfos.size();  ++i)
			{
				XAccountCharInfo& char_info = gvar.Char.m_CharInfos[ i];
				if ( !wcsicmp( char_info.szName, gvar.Network.strSelChar.c_str()))
				{
					nSelIndex = i;
					break;
				}
			}
		}

		XPostChar_RequestSelMyChar( nSelIndex);
		return CR_TRUE;
	}
#endif


	if (global.gfmgr->GetCurrFrameID() != CHAR_FRAME_ID)
	{
		// 캐릭터 선택화면으로  프레임 변환
		XPostLocal_ChangeFrameFrame(CHAR_FRAME_ID);
	}
	else
	{
		// Call event
		global.script->GetGlueGameEvent().OnCharacterEvent( "CHARACTER", "REFRESH");
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_LoginServer::OnResponseSelMyChar(MCommand* pCommand, MCommandHandler* pHandler)
{
	XCmdParam__MC_CHAR_RESPONSE_SEL_MYCHAR cmdParam;
	if (cmdParam.Parse(pCommand) == false) return CR_ERROR;

	if (cmdParam.m_nResult != CR_SUCCESS) 
	{
		global.script->GetGlueGameEvent().OnCharacterEvent( "WORLD_ENTER", "HIDE");

		int nFrame = global.app->GetCurrGameFrame();
		if (nFrame == CHAR_FRAME_ID)
		{
			global.script->GetGlueGameEvent().OnCharacterEvent( "CHARACTER_ERROR", MLocale::ConvUTF16ToAnsi(XCmdResultStr(cmdParam.m_nResult)));
		}

		return CR_TRUE;
	}

	// 성공한 경우
	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().Reset();

	return CR_TRUE;
}

MCommandResult XCmdHandler_LoginServer::OnMoveToGameServer(MCommand* pCommand, MCommandHandler* pHandler)
{
	XCmdParam__MC_COMM_MOVE_TO_GAME_SERVER cmdParam;
	if (cmdParam.Parse(pCommand) == false) return CR_ERROR;

	if (cmdParam.m_pLoginGameServerInfo)
	{
		wstring strIP = cmdParam.m_pLoginGameServerInfo->strHostName;
		int nPort = (int)cmdParam.m_pLoginGameServerInfo->nPort;

		global.net->GetConnectionManager()->MoveFromLoginServerToGameServer(strIP, nPort, cmdParam.m_pLoginGameServerInfo->uidConnectionKey);
	}

	global.script->GetGlueGameEvent().OnCharacterEvent( "WORLD_ENTER", "PROGRESS_MSG", CR_ENTER_WORLD_START_MOVING_TO_GAMESERVER );
	mlog("EnterWorld! (%d)START_MOVING_TO_GAMESERVER\n", CR_ENTER_WORLD_START_MOVING_TO_GAMESERVER);

	return CR_TRUE;
}

MCommandResult XCmdHandler_LoginServer::OnEnterWorldMsg(MCommand* pCommand, MCommandHandler* pHandler)
{
	CommEnterWorldMsg(pCommand);
	return CR_TRUE;
}

MCommandResult XCmdHandler_LoginServer::OnEnterWorldMsgLocal(MCommand* pCommand, MCommandHandler* pHandler)
{
	CommEnterWorldMsg(pCommand);
	return CR_TRUE;
}

void XCmdHandler_LoginServer::CommEnterWorldMsg(MCommand* pCommand)
{
	XCmdParam_IntResult cmdParam;
	if (cmdParam.Parse(pCommand) == false) return;

	global.script->GetGlueGameEvent().OnCharacterEvent( "WORLD_ENTER", "PROGRESS_MSG", cmdParam.m_nResult );

	string strResultStr = MLocale::ConvUTF16ToAnsi(XCmdResultStr(cmdParam.m_nResult));
	mlog("EnterWorld! (%d)%s\n", cmdParam.m_nResult, strResultStr.c_str());
}

MCommandResult XCmdHandler_LoginServer::OnResponseInsertChar(MCommand* pCommand, MCommandHandler* pHandler)
{
	XCmdParam__MC_CHAR_RESPONSE_INSERT_CHAR cmdParam;
	if (!cmdParam.Parse(pCommand)) return CR_ERROR;

	if (cmdParam.m_nResult == CR_SUCCESS)
	{
		if ( global.script)
		{
			global.script->GetGlueGameEvent().OnCharacterEvent( "CHARACTER", "CREATE", 1, 0);
		}

		return CR_TRUE;
	}

	if (global.gfmgr && global.gfmgr->GetCurrFrameID() == CHAR_FRAME_ID)
	{
		if ( global.script)		global.script->GetGlueGameEvent().OnCharacterEvent( "CHARACTER", "CREATE", 0, cmdParam.m_nResult);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_LoginServer::OnResponseDeleteChar(MCommand* pCommand, MCommandHandler* pHandler)
{
	XCmdParam__MC_CHAR_RESPONSE_DELETE_CHAR cmdParam;
	if (!cmdParam.Parse(pCommand)) return CR_ERROR;

	if (global.gfmgr && global.gfmgr->GetCurrFrameID() == CHAR_FRAME_ID)
	{
		if (cmdParam.m_nResult == CR_SUCCESS)
		{
			if (0 <= cmdParam.m_nCharIndex && cmdParam.m_nCharIndex < gvar.Char.m_CharInfos.size())
			{
				XCharFrame* pCharFrame = static_cast<XCharFrame*>(global.gfmgr->GetCurrFrame());
				pCharFrame->DeleteCurrChar();

				gvar.Char.m_CharInfos.erase(gvar.Char.m_CharInfos.begin() + cmdParam.m_nCharIndex);
				global.script->GetGlueGameEvent().OnCharacterEvent( "CHARACTER", "REFRESH");
			}
		}
		else
		{
			global.script->GetGlueGameEvent().OnCharacterEvent("CHARACTER_ERROR", MLocale::ConvUTF16ToAnsi(XCmdResultStr(cmdParam.m_nResult)));
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_LoginServer::OnDuplicatedPlayerLogin(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strErrorMsg = XCmdResultStr(CR_FAIL_DUPLICATED_PLAYER_LOGIN);

	if ( global.app->GetCurrGameFrame() != LOGIN_FRAME_ID)
	{
		/// 연결이 끊기면 바로 로긴프레임으로 이동하니까
		/// 로긴프레임으로 이동후에 에러 메세지 표시하는 걸로
		/// enum으로 안한건 include 줄일려고.
		gvar.Game.bDisconnectByDuplicatedLogin = true;
		gvar.Game.strLastErrorMsg = strErrorMsg;
	}
	else
	{
		/// 로그인프레임일 때 커맨드가 올 수도 있다.		
		global.script->GetGlueGameEvent().OnLoginEvent( "LOGIN_ERROR", MLocale::ConvUTF16ToAnsi(strErrorMsg.c_str()));
	}

	return CR_TRUE;
}