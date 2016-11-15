#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "FBasePlayer.h"
#include "MockLink.h"
#include "LTestHelper.h"
#include "LPlayerLogic.h"
#include "SAccountInfo.h"
#include "LPlayerObject.h"
#include "CCommandTable.h"
#include "CCommandResultTable.h"
#include "LSystem.h"
#include "LServer.h"
#include "LPlayerLoginGameServerProcManager.h"


SUITE(SelectCharacter)
{
	struct Fixture : public FBasePlayer
	{
		Fixture()
		{
			InitLoginedPlayer();

			m_pPlayerLoginGameServerProcManager = gsys.pServer->GetPlayerLoginGameServerProcManager();
		}
		virtual ~Fixture()
		{
			m_pPlayerLoginGameServerProcManager->Clear();
		}

		void InitLoginedPlayer()
		{
			MUID uidTestPlayer = SUnitTestUtil::NewUID();
			m_pTestPlayer = NewTestPlayerObject(uidTestPlayer);

			ACCOUNT_INFO info;
			info.nAID = 1;
			_tcscpy_s(info.szUserID, ACCOUNT_NAME_LEN + 1, _T("TEST_ID"));
			m_pTestPlayer->InitAccountInfo(info);

			SAccountCharFieldInfo accountCharFieldInfo;
			accountCharFieldInfo.nCID = SUnitTestUtil::NewCID();
			m_pTestPlayer->AddCharFieldInfo(accountCharFieldInfo);

			m_pPlayerLink = LTestHelper::NewLink(uidTestPlayer);
		}

		LPlayerObject*						m_pTestPlayer;
		MockLink*							m_pPlayerLink;
		LPlayerLoginGameServerProcManager*	m_pPlayerLoginGameServerProcManager;

		LPlayerLogic m_PlayerLogic;
	};

	TEST_FIXTURE(Fixture, TestSelectCharacter_Fixture)
	{
		CHECK_EQUAL(POS_SELECT_CHAR, m_pTestPlayer->GetState());
		CHECK_EQUAL(0, m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestSelectCharacter_Success_State_SELECT_CHAR)
	{
		m_pTestPlayer->OnSelectChar();

		int8 nCharIndex = 0;
		bool bRet = m_PlayerLogic.SelMyChar(m_pTestPlayer->GetUID(), nCharIndex);

		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(1, m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestSelectCharacter_Success_State_LOGIN_GAME_SERVER)
	{
		int8 nCharIndex = 0;
		m_pTestPlayer->OnLoginGameServer(nCharIndex);

		bool bRet = m_PlayerLogic.SelMyChar(m_pTestPlayer->GetUID(), nCharIndex);

		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(1, m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestSelectCharacter_Failure_State_MOVE_SERVER)
	{
		m_pTestPlayer->OnMoveServer();
		CHECK_EQUAL(POS_MOVE_SERVER, m_pTestPlayer->GetState());

		int8 nCharIndex = 0;
		bool bRet = m_PlayerLogic.SelMyChar(m_pTestPlayer->GetUID(), nCharIndex);

		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, m_pPlayerLoginGameServerProcManager->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestSelectCharacter_Failure_InvalidCharIndex_1)
	{
		int8 nInvalidCharIndex = 1;
		bool bRet = m_PlayerLogic.SelMyChar(m_pTestPlayer->GetUID(), nInvalidCharIndex);

		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, m_pPlayerLoginGameServerProcManager->GetQueueSize());

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_RESPONSE_SEL_MYCHAR, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_LOGIN_INVALID_CHAR_INFO, m_pPlayerLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestSelectCharacter_Failure_InvalidCharIndex_Minus1)
	{
		int8 nInvalidCharIndex = -1;
		bool bRet = m_PlayerLogic.SelMyChar(m_pTestPlayer->GetUID(), nInvalidCharIndex);

		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, m_pPlayerLoginGameServerProcManager->GetQueueSize());

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_RESPONSE_SEL_MYCHAR, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_LOGIN_INVALID_CHAR_INFO, m_pPlayerLink->GetParam<int>(0, 0));
	}


}