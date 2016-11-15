#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "CCommandResultTable.h"
#include "MockLink.h"
#include "GCommandCenter.h"
#include "GCommandTable.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"

#include "CSmsgCommandFacade.h"
#include "CSMsgHandler.h"

SUITE(Chat)
{
	struct MockMsgHandler: public CSMsgHandler
	{
		CSMsgType m_fakeType;
		bool m_isCallRequest;
		bool m_isCallResponse;
		const minet::MCommand* m_pCmd;
		using CSMsgHandler::m_strMsg;
		using CSMsgHandler::m_nAdditional;

		MockMsgHandler(): m_isCallRequest(false), m_isCallResponse(false)	{}

		virtual bool OnRequest(const minet::MCommand* pCmd) override
		{ 
			m_isCallRequest = true;
			m_pCmd = pCmd;
			return true;
		}
		
		virtual bool OnResponse(const minet::MCommand* pCmd) override	
		{ 
			m_isCallResponse = true;
			m_pCmd = pCmd;
			return true;	
		}
		
		virtual CSMsgType GetHandlerType(void) override	{ return m_fakeType;	}

		void SetFakeType(CSMsgType type)		{ m_fakeType = type;	}
		wstring GetSenderName(void)				{ return m_msgHelper.GetSenderName(m_pCmd);		}
		wstring GetReceiverName(void)			{ return m_msgHelper.GetReceiverName(m_pCmd);	}
		int GetSenderID(void)					{ return m_msgHelper.GetSenderID(m_pCmd);		}
		int GetReceiverID(void)					{ return m_msgHelper.GetReceiverID(m_pCmd);		}
		MUID GetSenderUID(void)					{ return m_msgHelper.GetSenderUID(m_pCmd);		}
		MUID GetReceiverUID(void)				{ return m_msgHelper.GetReceiverUID(m_pCmd);	}
	};

	struct Fixture
	{
		CSMsgCommandFacade	m_msgFacade;
		CSMsgCommandHelper	m_msgHelper;
		MockMsgHandler*		m_pMsgHandler;

		wstring m_strMsg;
		wstring m_strSenderName;
		wstring m_strReceiverName;
		int m_nChannelID;
		int m_nSenderCID;
		int m_nReceiverCID;
		MUID m_uidParty;
		int m_nGuildID;


		Fixture()
		: m_pMsgHandler(NULL)		
		, m_strMsg(L"TestMessage")
		, m_strSenderName(L"TestSenderName")
		, m_strReceiverName(L"TestReceiverName")
		, m_nChannelID(1)		
		, m_nSenderCID(2)
		, m_nReceiverCID(3)
		, m_uidParty(1)
		, m_nGuildID(4)
		{}
		~Fixture()	{}

		void SetMsgHandler(CSMsgType type)
		{
			m_pMsgHandler = new MockMsgHandler();
			m_pMsgHandler->SetFakeType(type);
			m_msgFacade.SetHandler(type, m_pMsgHandler);
		}
	};

	TEST_FIXTURE(Fixture, TestSetMsgHandler_Request)
	{
		// arrange
		SetMsgHandler(MT_SAY);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandSayReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_isCallRequest);
		CHECK(!m_pMsgHandler->m_isCallResponse);		
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestSetMsgHandler_Response)
	{
		// arrange
		SetMsgHandler(MT_SAY);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandSayReq(m_strMsg));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(!m_pMsgHandler->m_isCallRequest);
		CHECK(m_pMsgHandler->m_isCallResponse);
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestSetMsgHandler_NotMatchHandler)
	{
		// arrange
		SetMsgHandler(MT_SHOUT);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandSayReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(!m_pMsgHandler->m_isCallRequest);
		CHECK(!m_pMsgHandler->m_isCallResponse);
		CHECK(m_pMsgHandler->m_strMsg.empty());
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandSayReq)
	{
		// arrange
		SetMsgHandler(MT_SAY);				

		// act		
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandSayReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}
	
	TEST_FIXTURE(Fixture, TestMakeNewClientCommandSayRes)
	{
		// arrange
		SetMsgHandler(MT_SAY);

		// act		
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandSayRes(m_strMsg, m_strSenderName, PLAYER_GRADE_GM));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
		CHECK_EQUAL(m_pMsgHandler->m_nAdditional, PLAYER_GRADE_GM);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandNarrationReq)
	{
		// arrange
		SetMsgHandler(MT_NARRATION);

		// act		
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandNarrationReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandNarrationRes)
	{
		// arrange
		SetMsgHandler(MT_NARRATION);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandNarrationRes(m_strMsg));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandShoutReq)
	{
		// arrange
		SetMsgHandler(MT_SHOUT);

		// arrange
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandShoutReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandShoutRes)
	{
		// arrange
		SetMsgHandler(MT_SHOUT);

		// arrange
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandShoutRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());
		
		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandShoutFakeRes)
	{
		// arrange
		SetMsgHandler(MT_SHOUT_FAKE);

		// arrange
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandShoutFakeRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());
		
		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandAuctionReq)
	{
		// arrange
		SetMsgHandler(MT_AUCTION);

		// arrange
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandAuctionReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandAuctionRes)
	{
		// arrange
		SetMsgHandler(MT_AUCTION);

		// arrange
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandAuctionRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandAnnounceReq)
	{
		// arrange
		SetMsgHandler(MT_ANNOUNCE);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandAnnounceReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandAnnounceRes)
	{
		// arrange
		SetMsgHandler(MT_ANNOUNCE);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandAnnounceRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandWhisperReq)
	{
		// arrange
		SetMsgHandler(MT_WHISPER);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandWhisperReq(m_strMsg, m_strReceiverName));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetReceiverName() == m_strReceiverName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandWhisperRes)
	{
		// arrange
		SetMsgHandler(MT_WHISPER);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandWhisperRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandPartyReq)
	{
		// arrange
		SetMsgHandler(MT_PARTY);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandPartyReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandPartyRes)
	{
		// arrange
		SetMsgHandler(MT_PARTY);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandPartyRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandGuildReq)
	{
		// arrange
		SetMsgHandler(MT_GUILD);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandGuildReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandGuildRes)
	{
		// arrange
		SetMsgHandler(MT_GUILD);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandGuildRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandChannelReq)
	{
		// arrange
		SetMsgHandler(MT_CHANNEL);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandChannelReq(m_strMsg, m_nChannelID));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverID(), m_nChannelID);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandChannelRes)
	{
		// arrange
		SetMsgHandler(MT_CHANNEL);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandChannelRes(m_strMsg, m_strSenderName, m_nChannelID));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverID(), m_nChannelID);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandGlobalReq)
	{
		// arrange
		SetMsgHandler(MT_GLOBAL);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandGlobalReq(m_strMsg));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommnadGlobalRes)
	{
		// arrange
		SetMsgHandler(MT_GLOBAL);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandGlobalRes(m_strMsg, m_strSenderName));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandSystemRes)
	{
		// arrange
		SetMsgHandler(MT_SYSTEM);
		CCommandResultTable nResult = CR_FAIL;

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandSystemRes(nResult));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK_EQUAL(m_pMsgHandler->m_nAdditional, nResult);		
	}

	TEST_FIXTURE(Fixture, TestMakeNewClientCommandNoticeRes)
	{
		// arrange
		SetMsgHandler(MT_NOTICE);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewClientCommandNoticeRes(m_strMsg));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandAnnounceReq)
	{
		// arrange
		SetMsgHandler(MT_ANNOUNCE);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandAnnounceReq(m_strMsg, m_nSenderCID));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK_EQUAL(m_pMsgHandler->GetSenderID(), m_nSenderCID);
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandAnnounceRes)
	{
		// arrange
		SetMsgHandler(MT_ANNOUNCE);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandAnnounceRes(m_strMsg, m_strSenderName));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);		
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandWhisperReq)
	{
		// arrange
		SetMsgHandler(MT_WHISPER);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandWhisperReq(m_strMsg, m_nSenderCID, m_strReceiverName));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK_EQUAL(m_pMsgHandler->GetSenderID(), m_nSenderCID); 
		CHECK(m_pMsgHandler->GetReceiverName() == m_strReceiverName);
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandWhisperRes)
	{
		// arrange
		SetMsgHandler(MT_WHISPER);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandWhisperRes(m_strMsg, m_strSenderName, m_nReceiverCID));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverID(), m_nReceiverCID);		
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandPartyReq)
	{
		// arrange
		SetMsgHandler(MT_PARTY);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandPartyReq(m_strMsg, m_nSenderCID, m_uidParty));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK_EQUAL(m_pMsgHandler->GetSenderID(), m_nSenderCID);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverUID(), m_uidParty);		
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandPartyRes)
	{
		// arrange
		SetMsgHandler(MT_PARTY);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandPartyRes(m_strMsg, m_strSenderName, m_uidParty));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverUID(), m_uidParty);		
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandGuildReq)
	{
		// arrange
		SetMsgHandler(MT_GUILD);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandGuildReq(m_strMsg, m_nSenderCID, m_nGuildID));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK_EQUAL(m_pMsgHandler->GetSenderID(), m_nSenderCID);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverID(), m_nGuildID);		
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandGuildRes)
	{
		// arrange
		SetMsgHandler(MT_GUILD);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandGuildRes(m_strMsg, m_strSenderName, m_nGuildID));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverID(), m_nGuildID);		
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandChannelReq)
	{
		// arrange
		SetMsgHandler(MT_CHANNEL);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandChannelReq(m_strMsg, m_nSenderCID, m_nChannelID));
		m_msgFacade.RequestMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK_EQUAL(m_pMsgHandler->GetSenderID(), m_nSenderCID);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverID(), m_nChannelID);		
	}

	TEST_FIXTURE(Fixture, TestMakeNewServerCommandChannelRes)
	{
		// arrange
		SetMsgHandler(MT_CHANNEL);

		// act
		auto_ptr<MCommand> pCmd(m_msgHelper.MakeNewServerCommandChannelRes(m_strMsg, m_strSenderName, m_nChannelID));
		m_msgFacade.ResponseMessage(pCmd.get());

		// assert
		CHECK(m_pMsgHandler->m_strMsg == m_strMsg);
		CHECK(m_pMsgHandler->GetSenderName() == m_strSenderName);
		CHECK_EQUAL(m_pMsgHandler->GetReceiverID(), m_nChannelID);
	}
}
