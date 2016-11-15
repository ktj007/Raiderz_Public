#include "stdafx.h"
#include "XCmdParam_LoginServer.h"
#include "XNetClient.h"
#include "XGlobalVar_Char.h"
#include "CCommandResultTable.h"
#include "FCmdParam.h"

SUITE(CmdParam_LoginServer)
{
	TEST_FIXTURE(FCmdParam, TestCmdParam__MC_COMM_RESPONSE_LOGIN)
	{
		m_pCommand = XMakeCMD3(MC_COMM_RESPONSE_LOGIN, new MCommandParameterInt(100), new MCommandParameterUID(MUID(5, 10)), new MCommandParameterUChar(SERVER_MODE_NORMAL));

		XCmdParam__MC_COMM_RESPONSE_LOGIN cmdParam;
		CHECK_EQUAL(true, cmdParam.Parse(m_pCommand));

		CHECK_EQUAL(100, cmdParam.m_nResult);
		CHECK_EQUAL(MUID(5, 10), cmdParam.m_uidAlloc);
		CHECK_EQUAL(SERVER_MODE_NORMAL, cmdParam.m_nServerMode);
	}


	TEST_FIXTURE(FCmdParam, TestCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST)
	{
		vector<TD_AccountCharInfo> vec;
		TD_AccountCharInfo td1;
		wcsncpy_s(td1.szName, L"bird1", _TRUNCATE);
		td1.nLevel = 15;
		td1.nFieldID = 9001;
		vec.push_back(td1);

		TD_AccountCharInfo td2;
		wcsncpy_s(td2.szName, L"bird2", _TRUNCATE);
		td2.nLevel = 30;
		td2.nFieldID = 107;
		vec.push_back(td2);

		m_pCommand = XMakeCMD1(MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST, new MCommandParameterBlob(vec));

		XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST cmdParam;
		CHECK_EQUAL(true, cmdParam.Parse(m_pCommand));

		vector<XAccountCharInfo> vecAccountCharInfo;

		cmdParam.AssignTo(vecAccountCharInfo);

		ASSERT_EQUAL(2, vecAccountCharInfo.size());

		CHECK_EQUAL(15, vecAccountCharInfo[0].nLevel);
		CHECK_EQUAL(9001, vecAccountCharInfo[0].nFieldID);
		CHECK(wstring(L"bird1") == vecAccountCharInfo[0].szName);

		CHECK_EQUAL(30, vecAccountCharInfo[1].nLevel);
		CHECK_EQUAL(107, vecAccountCharInfo[1].nFieldID);
		CHECK(wstring(L"bird2") == vecAccountCharInfo[1].szName);

	}

	TEST_FIXTURE(FCmdParam, TestCmdParam__MC_CHAR_RESPONSE_SEL_MYCHAR)
	{
		m_pCommand = XMakeCMD1(MC_CHAR_RESPONSE_SEL_MYCHAR, new MCommandParameterInt(CR_SUCCESS));

		XCmdParam__MC_CHAR_RESPONSE_SEL_MYCHAR cmdParam;
		CHECK_EQUAL(true, cmdParam.Parse(m_pCommand));

		CHECK_EQUAL(CR_SUCCESS, cmdParam.m_nResult);
	}

	TEST_FIXTURE(FCmdParam, TestCmdParam__MC_COMM_MOVE_TO_GAME_SERVER)
	{
		TD_LOGIN_GAME_SERVER_INFO td;
		wstring strHostName = L"TestHostName";
		wcsncpy_s(td.strHostName, strHostName.c_str(), _TRUNCATE);
		td.nPort = 5000;
		td.uidConnectionKey = MUID(5, 5);

		m_pCommand = XMakeCMD1(MC_COMM_MOVE_TO_GAME_SERVER, new MCommandParameterBlob(&td, sizeof(td), 1));

		XCmdParam__MC_COMM_MOVE_TO_GAME_SERVER cmdParam;
		CHECK_EQUAL(true, cmdParam.Parse(m_pCommand));

		CHECK(!wcsncmp(cmdParam.m_pLoginGameServerInfo->strHostName, strHostName.c_str(), MAX_HOSTNAME_LEN));
		CHECK_EQUAL(5000, cmdParam.m_pLoginGameServerInfo->nPort);
		CHECK_EQUAL(MUID(5, 5), cmdParam.m_pLoginGameServerInfo->uidConnectionKey);
	}


	TEST_FIXTURE(FCmdParam, TestCmdParam__MC_CHAR_RESPONSE_INSERT_CHAR)
	{
		m_pCommand = XMakeCMD1(MC_CHAR_RESPONSE_INSERT_CHAR, new MCommandParameterInt(CR_SUCCESS));

		XCmdParam__MC_CHAR_RESPONSE_INSERT_CHAR cmdParam;
		CHECK_EQUAL(true, cmdParam.Parse(m_pCommand));

		CHECK_EQUAL(CR_SUCCESS, cmdParam.m_nResult);
	}

	TEST_FIXTURE(FCmdParam, TestCmdParam__MC_CHAR_RESPONSE_DELETE_CHAR)
	{
		m_pCommand = XMakeCMD2(MC_CHAR_RESPONSE_DELETE_CHAR, new MCommandParameterInt(CR_SUCCESS), new MCommandParameterChar(1));

		XCmdParam__MC_CHAR_RESPONSE_DELETE_CHAR cmdParam;
		CHECK_EQUAL(true, cmdParam.Parse(m_pCommand));

		CHECK_EQUAL(CR_SUCCESS, cmdParam.m_nResult);
		CHECK_EQUAL(1, cmdParam.m_nCharIndex);
	}
}
