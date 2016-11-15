#include "stdafx.h"
#include "CSMsgCommandHelper.h"
#include "MTest.h"
#include "CCommandTable.h"
#include "CCommandTable_App.h"
#include "MSimpleCommandBinder.h"
#include "SUnitTestUtil.h"
#include "CCommandResultTable.h"

// CSMsgCommandHelper는 CSCommon에 있지만, Chatting을 담당하는 AppServer에서 테스트합니다.

SUITE(MsgCommandHelper)
{
	struct Fixture
	{
		CSMsgCommandHelper m_helper;

		template<typename T>
		T GetParameter(auto_ptr<MCommand>& cmd, int idx)			
		{
			return cmd_binder::param<T>(cmd.get(), idx);
		}

		template<>
		wstring GetParameter(auto_ptr<MCommand>& cmd, int idx)
		{
			return cmd_binder::param<const wchar_t*>(cmd.get(), idx);
		}
	};

	TEST_FIXTURE(Fixture, MakeNewClientCommandSayReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		
		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandSayReq(strMsg));		

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_SAY);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandSayRes)
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandSayRes(strMsg, strSenderName, PLAYER_GRADE_GM));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_SAY);
		CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), PLAYER_GRADE_GM);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandNarrationReq)
	{
		// arrange
		wstring strMsg = L"Test Message";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandNarrationReq(strMsg));
		
		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);
		
		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_NARRATION);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandNarrationRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandNarrationRes(strMsg));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_NARRATION);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandShoutReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandShoutReq(strMsg));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_SHOUT);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandShoutRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandShoutRes(strMsg, strSenderName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_SHOUT);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandAuctionReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandAuctionReq(strMsg));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_AUCTION);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandAuctionRes) 
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandAuctionRes(strMsg, strSenderName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_AUCTION);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandAnnounceReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		
		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandAnnounceReq(strMsg));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_ANNOUNCE);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}
	
	TEST_FIXTURE(Fixture, MakeNewClientCommandAnnounceRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandAnnounceRes(strMsg, strSenderName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_ANNOUNCE);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);		
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandAnnounceReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		CID nSenderCID = 1;
		
		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandAnnounceReq(strMsg, nSenderCID));
		
		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);
		
		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_ANNOUNCE);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_EQUAL(m_helper.GetSenderID(pCmd.get()), nSenderCID);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandAnnounceRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";
		
		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandAnnounceRes(strMsg, strSenderName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_ANNOUNCE);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandWhisperReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strReceiverName = L"TestReceiverName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandWhisperReq(strMsg, strReceiverName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_WHISPER);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.GetReceiverName(pCmd.get()) == strReceiverName);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandWhisperRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandWhisperRes(strMsg, strSenderName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_WHISPER);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandWhisperReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		CID nSenderCID = 1;
		wstring strReceiverName = L"TestReceiverName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandWhisperReq(strMsg, nSenderCID, strReceiverName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_WHISPER);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_EQUAL(m_helper.GetSenderID(pCmd.get()), nSenderCID);
		ASSERT_CHECK(m_helper.GetReceiverName(pCmd.get()) == strReceiverName);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandWhisperRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";
		CID nReceiverCID = 1;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandWhisperRes(strMsg, strSenderName, nReceiverCID));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_WHISPER);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_EQUAL(m_helper.GetReceiverID(pCmd.get()), nReceiverCID);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandPartyReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandPartyReq(strMsg));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);
		
		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_PARTY);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandPartyRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";
		
		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandPartyRes(strMsg, strSenderName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_PARTY);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandPartyReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		CID nSenderCID = 1;
		MUID uidParty = SUnitTestUtil::NewUID();

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandPartyReq(strMsg, nSenderCID, uidParty));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_PARTY);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_EQUAL(m_helper.GetSenderID(pCmd.get()), nSenderCID);
		ASSERT_EQUAL(m_helper.GetReceiverUID(pCmd.get()), uidParty);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandPartyRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";
		MUID uidParty = SUnitTestUtil::NewUID();

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandPartyRes(strMsg, strSenderName, uidParty));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_PARTY);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_EQUAL(m_helper.GetReceiverUID(pCmd.get()), uidParty);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}	

	TEST_FIXTURE(Fixture, MakeNewClientCommandGuildReq)
	{
		// arrange
		wstring strMsg = L"Test Message";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandGuildReq(strMsg));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_GUILD);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4),0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandGuildRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandGuildRes(strMsg, strSenderName));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_GUILD);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandGuildReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		CID nSenderCID = 1;
		int nGuildID = 2;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandGuildReq(strMsg, nSenderCID, nGuildID));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_GUILD);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_EQUAL(m_helper.GetSenderID(pCmd.get()), nSenderCID);
		ASSERT_EQUAL(m_helper.GetReceiverID(pCmd.get()), nGuildID);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandGuildRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";
		int nGuildID = 1;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandGuildRes(strMsg, strSenderName, nGuildID));
		
		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_GUILD);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_EQUAL(m_helper.GetReceiverID(pCmd.get()), nGuildID);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandChannelReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		int nChannelID = 1;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandChannelReq(strMsg, nChannelID));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_CHANNEL);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_EQUAL(m_helper.GetReceiverID(pCmd.get()), nChannelID);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandChannelRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";
		int nChannelID = 1;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandChannelRes(strMsg, strSenderName, nChannelID));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_CHANNEL);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_EQUAL(m_helper.GetReceiverID(pCmd.get()), nChannelID);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandChannelReq)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		CID nSenderCID = 1;
		int nChannelID = 2;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandChannelReq(strMsg, nSenderCID, nChannelID));
		
		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_REQ);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_CHANNEL);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_EQUAL(m_helper.GetSenderID(pCmd.get()), nSenderCID);
		ASSERT_EQUAL(m_helper.GetReceiverID(pCmd.get()), nChannelID);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewServerCommandChannelRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";
		wstring strSenderName = L"TestSenderName";
		int nChannelID = 1;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewServerCommandChannelRes(strMsg, strSenderName, nChannelID));
		
		// assert
		ASSERT_EQUAL(pCmd->GetID(), MPC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_CHANNEL);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.GetSenderName(pCmd.get()) == strSenderName);
		ASSERT_EQUAL(m_helper.GetReceiverID(pCmd.get()), nChannelID);
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandSystemRes)		
	{
		// arrange
		CCommandResultTable nResult = CR_FAIL;

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandSystemRes(nResult));

		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_SYSTEM);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == L"");
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), nResult);
	}

	TEST_FIXTURE(Fixture, MakeNewClientCommandNoticeRes)	
	{
		// arrange
		wstring strMsg = L"Test Message";

		// act
		auto_ptr<MCommand> pCmd(m_helper.MakeNewClientCommandNoticeRes(strMsg));
		
		// assert
		ASSERT_EQUAL(pCmd->GetID(), MC_MSG_RES);
		ASSERT_EQUAL(pCmd->GetParameterCount(), 5);

		ASSERT_EQUAL(GetParameter<int>(pCmd, 0), MT_NOTICE);
		ASSERT_CHECK(GetParameter<wstring>(pCmd, 1) == strMsg);
		ASSERT_CHECK(m_helper.IsSenderEmpty(pCmd.get()));
		ASSERT_CHECK(m_helper.IsReceiverEmpty(pCmd.get()));
		ASSERT_EQUAL(GetParameter<int>(pCmd, 4), 0);
	}
}
