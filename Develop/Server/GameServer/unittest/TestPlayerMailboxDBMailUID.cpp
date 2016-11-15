#include "stdafx.h"
#include "GPlayerMailBox.h"
#include "GUTHelper_Player.h"
#include "GPlayerObjectManager.h"
#include "GTestWrapper.h"
#include "GEntityPlayer.h"
#include "GPlayerMailBox.h"
#include "GMail.h"
#include "SUnitTestUtil.h"
#include "GUnitTestUtil.h"
#include "GTestWrapper.h"


SUITE(PlayerMailBox)
{
	struct Fixture
	{
		Fixture()
		{
			m_pPlayer = m_helperPlayer.NewPlayer();
		}
		~Fixture()
		{

		}

		GMail* NewTestMail(int64 nMailUID, int nExpiRemainTimeSec)
		{
			GMail* pMail = new GMail(nMailUID);
			pMail->Init(SUnitTestUtil::NewCID()
				,MT_PLAYER
				, false
				, L"Tester"
				, L"TestMail"
				, false
				, 0
				, 0
				, nExpiRemainTimeSec						
				);
			return pMail;
		}

		GUTHelper_Player	m_helperPlayer;
		GEntityPlayer*		m_pPlayer;

		GTestMgrWrapper<GPlayerObjectManager>	m_wrapperPlayerObjectManager;
	};

	TEST_FIXTURE(Fixture, TestPlayerMailBoxDBMailUID_GetCursor)
	{
		// arrange
		const uint64 nUIDTop = 3;
		const uint64 nUIDBottom = 1;
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();

		refMailbox.GetDBMailUID().Init(nUIDTop, nUIDBottom);

		CHECK_EQUAL(nUIDTop, refMailbox.GetDBMailUID().GetCursor());

		int64 nMailUID = nUIDTop;
		GMail* pMail = new GMail(nMailUID);

		// act
		refMailbox.AddMail(pMail);

		// check
		CHECK_EQUAL(nUIDTop - 1, refMailbox.GetDBMailUID().GetCursor());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBoxDBMailUID_GetCursor_Boundary)
	{
		// arrange
		const uint64 nUIDTop = 3;
		const uint64 nUIDBottom = 1;
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();

		refMailbox.GetDBMailUID().Init(nUIDTop, nUIDBottom);

		CHECK_EQUAL(nUIDTop, refMailbox.GetDBMailUID().GetCursor());

		int64 nMailUID = nUIDBottom;
		GMail* pMail = new GMail(nMailUID);

		// act
		refMailbox.AddMail(pMail);

		// check
		CHECK_EQUAL(0, refMailbox.GetDBMailUID().GetCursor());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBoxDBMailUID_Refresh_Top)
	{
		// arrange
		const uint64 nUIDTop = 3;
		const uint64 nUIDBottom = 1;
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();

		refMailbox.GetDBMailUID().Init(nUIDTop, nUIDBottom);

		CHECK_EQUAL(nUIDTop, refMailbox.GetDBMailUID().GetCursor());

		int64 nMailUID = 5;
		GMail* pMail = new GMail(nMailUID);

		// act
		refMailbox.AddMail(pMail);

		// check
		CHECK_EQUAL(nUIDTop, refMailbox.GetDBMailUID().GetCursor());
		CHECK_EQUAL(5, refMailbox.GetDBMailUID().GetTop());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBoxDBMailUID_IsRemainMailAtDB_Failure)
	{
		// arrange
		m_pPlayer->GetMailbox().GetDBMailUID().Init(5, 1);

		int64 nMailUID = 1;
		GMail* pMail = new GMail(nMailUID);

		// act
		m_pPlayer->GetMailbox().AddMail(pMail);

		// check
		CHECK_EQUAL(false, m_pPlayer->GetMailbox().IsRemainMailAtDB());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBoxDBMailUID_IsRemainMailAtDB_Success)
	{
		// arrange
		m_pPlayer->GetMailbox().GetDBMailUID().Init(5, 1);

		int64 nMailUID = 3;
		GMail* pMail = new GMail(nMailUID);

		// act
		m_pPlayer->GetMailbox().AddMail(pMail);

		// check
		CHECK_EQUAL(true, m_pPlayer->GetMailbox().IsRemainMailAtDB());
	}

}
