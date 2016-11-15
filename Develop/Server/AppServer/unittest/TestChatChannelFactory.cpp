#include "stdafx.h"
#include "PTestWrapper.h"
#include "PTestHelper.h"
#include "PChatChannel.h"
#include "PChatChannelManager.h"
#include "PChatChannelFactory.h"
#include "SUnitTestUtil.h"

SUITE(ChatChannelFactory)
{
	struct Fixture
	{
		Fixture()	{}
		~Fixture()	{}

		PChatChannelFactory	m_TestChatChannelFactory;
	};

	TEST_FIXTURE(Fixture, TestChatChannelFactory_NewChatChannel_Private)
	{
		int nOwnerCID = SUnitTestUtil::NewID();
		std::wstring strChannelName = L"TestChannelName";
		PChatChannel* pChatChannel = m_TestChatChannelFactory.NewChatChannel_Private(nOwnerCID, strChannelName);
		CHECK(NULL != pChatChannel);

		CHECK_EQUAL(nOwnerCID,		pChatChannel->GetOwnerCID());
		CHECK(strChannelName ==	pChatChannel->GetName());

		SAFE_DELETE(pChatChannel);
	}
	TEST_FIXTURE(Fixture, TestChatChannelFactory_NewChatChannel_Party)
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		PChatChannel* pChatChannel = m_TestChatChannelFactory.NewChatChannel_Party(uidParty);
		CHECK(NULL != pChatChannel);

		CHECK_EQUAL(uidParty,		pChatChannel->GetPartyUID());

		SAFE_DELETE(pChatChannel);
	}
}