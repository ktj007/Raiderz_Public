#include "stdafx.h"
#include "XUnitTestUtil.h"
#include "XChatChannelManager.h"
#include "XChatChannel.h"

SUITE(ChatChannel)
{
	struct Fixture
	{
		Fixture()
		{
			m_uidChannel = XUnitTestUtil::NewMUID();
			m_strChannelName = L"TestChannelName";
			m_isOwner = true;
		}

		~Fixture()	{}

		XChatChannelManager m_manager;
		MUID m_uidChannel;
		wstring m_strChannelName;
		bool m_isOwner;
	};

	TEST_FIXTURE(Fixture, JoinChatChannel)
	{
		// act		
		CHECK(m_manager.Join(m_uidChannel, m_strChannelName, m_isOwner));

		// assert		
		XChatChannel* pChannel = m_manager.Find(m_uidChannel);
		ASSERT_CHECK(pChannel != NULL);

		CHECK_EQUAL(pChannel->GetUID(), m_uidChannel);
		CHECK(pChannel->GetName() == m_strChannelName);
		CHECK_EQUAL(pChannel->IsOwner(), m_isOwner);
	}

	TEST_FIXTURE(Fixture, LeaveChatChannel)
	{
		// arrange
		m_manager.Join(m_uidChannel, m_strChannelName, m_isOwner);
		CHECK(!m_manager.IsEmpty());
		
		// act		
		m_manager.Leave(m_uidChannel);

		// assert
		CHECK(m_manager.IsEmpty());
	}

	TEST_FIXTURE(Fixture, FindChannelByName)
	{
		// arrange
		m_manager.Join(m_uidChannel, m_strChannelName, m_isOwner);
		CHECK(!m_manager.IsEmpty());

		// act
		XChatChannel* pChannel = m_manager.Find(m_strChannelName);
		
		// assert
		ASSERT_CHECK(pChannel != NULL);
		CHECK_EQUAL(pChannel->GetUID(), m_uidChannel);
		CHECK(pChannel->GetName() == m_strChannelName);
		CHECK_EQUAL(pChannel->IsOwner(), m_isOwner);
	}
}
