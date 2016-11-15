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
						, 100
						, 0
						, nExpiRemainTimeSec						
						);
			return pMail;
		}

		GUTHelper_Player	m_helperPlayer;
		GEntityPlayer*		m_pPlayer;

		GTestMgrWrapper<GPlayerObjectManager>	m_wrapperPlayerObjectManager;
	};

	TEST_FIXTURE(Fixture, TestPlayerMailBox_SetMailUIDRange_Success)
	{
		// arrange
		int64 nMailUIDTop = 10;
		int64 nMailUIDBottom = 1;

		// act
		bool bRet = m_pPlayer->GetMailbox().GetDBMailUID().Init(nMailUIDTop, nMailUIDBottom);

		// assert
		CHECK_EQUAL(nMailUIDTop, m_pPlayer->GetMailbox().GetDBMailUID().GetTop());
		CHECK_EQUAL(nMailUIDBottom - 1, m_pPlayer->GetMailbox().GetDBMailUID().GetBottom());
		CHECK_EQUAL(nMailUIDTop, m_pPlayer->GetMailbox().GetDBMailUID().GetCursor());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBox_SetMailUIDRange_Failure_TopMailUID_INVALID)
	{
		// arrange
		int64 nMailUIDTop = 0;
		int64 nMailUIDBottom = 1;

		// act
		bool bRet = m_pPlayer->GetMailbox().GetDBMailUID().Init(nMailUIDTop, nMailUIDBottom);

		// assert
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, m_pPlayer->GetMailbox().GetDBMailUID().GetTop());
		CHECK_EQUAL(0, m_pPlayer->GetMailbox().GetDBMailUID().GetBottom());
		CHECK_EQUAL(0, m_pPlayer->GetMailbox().GetDBMailUID().GetCursor());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBox_SetMailUIDRange_Success_BottomMailUID_ZERO)
	{
		// arrange
		int64 nMailUIDTop = 10;
		int64 nMailUIDBottom = 0;

		// act
		bool bRet = m_pPlayer->GetMailbox().GetDBMailUID().Init(nMailUIDTop, nMailUIDBottom);

		// assert
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(nMailUIDTop, m_pPlayer->GetMailbox().GetDBMailUID().GetTop());
		CHECK_EQUAL(nMailUIDBottom, m_pPlayer->GetMailbox().GetDBMailUID().GetBottom());
		CHECK_EQUAL(nMailUIDTop, m_pPlayer->GetMailbox().GetDBMailUID().GetCursor());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBox_AddMail_Success)
	{
		// arrange
		int64 nMailUID = 999;
		GMail* pMail = new GMail(nMailUID);

		// act
		bool bRet = m_pPlayer->GetMailbox().AddMail(pMail);

		// check
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(1, m_pPlayer->GetMailbox().GetSize());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBox_AddMail_Failure_MailUID_OutOfRange)
	{
		// arrange
		int64 nMailUIDTop = 100;
		int64 nMailUIDBottom = 10;
		
		m_pPlayer->GetMailbox().GetDBMailUID().Init(nMailUIDTop, nMailUIDBottom);

		int64 nMailUID = nMailUIDBottom - 2;
		GMail* pMail = new GMail(nMailUID);

		// act
		bool bRet = m_pPlayer->GetMailbox().AddMail(pMail);

		// check
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, m_pPlayer->GetMailbox().GetSize());

		SAFE_DELETE(pMail);
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBox_DelMail_Success)
	{
		// arrange
		int64 nMailUID = 999;
		GMail* pMail = new GMail(nMailUID);
		m_pPlayer->GetMailbox().AddMail(pMail);

		// act
		m_pPlayer->GetMailbox().DelMail(nMailUID);

		// check
		CHECK_EQUAL(0, m_pPlayer->GetMailbox().GetSize());
	}

	TEST_FIXTURE(Fixture, TestPlayerMailBox_DelMail_Failure_InvalidMailUID)
	{
		// arrange
		int64 nMailUID = 999;
		GMail* pMail = new GMail(nMailUID);
		m_pPlayer->GetMailbox().AddMail(pMail);

		int64 nInvalidMailUID = 0;

		// act
		m_pPlayer->GetMailbox().DelMail(nInvalidMailUID);

		// check
		CHECK_EQUAL(1, m_pPlayer->GetMailbox().GetSize());
	}

	struct Fixture_InitDBMailUID_WarpSystem : public Fixture
	{
		Fixture_InitDBMailUID_WarpSystem()
		{
			m_pPlayer->GetMailbox().GetDBMailUID().Init(10000, 1);
			m_nMailUID = 999;
		}

		int64	m_nMailUID;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem> m_testSystem; ///< 시간 조작을 위한 랩퍼.
	};

	TEST_FIXTURE(Fixture_InitDBMailUID_WarpSystem, TestPlayerMailBox_GetMailListTo_TD_MAIL_MAILBOX_MAIL_INFO)
	{
		// arrange
		GMail* pMail = new GMail(m_nMailUID);

		CID nCID = SUnitTestUtil::NewCID();
		MAIL_TYPE eType = MT_PLAYER;
		wstring strSender = L"aibeast";
		wstring strTitle = L"aibeast님 날 가져요";
		pMail->Init(nCID, eType, true, strSender, strTitle, false, 0, 0, 1000);

		m_pPlayer->GetMailbox().AddMail(pMail);

		vector<TD_MAIL_MAILBOX_MAIL_INFO> vecMailboxList;

		// act
		m_pPlayer->GetMailbox().GetMailListTo_TD(vecMailboxList);

		// check
		ASSERT_EQUAL(1,			vecMailboxList.size());
		CHECK_EQUAL(m_nMailUID,	vecMailboxList[0].uidMail);
		CHECK_EQUAL(true,		vecMailboxList[0].bIsRead);
		CHECK_EQUAL(eType,		(MAIL_TYPE)vecMailboxList[0].nType);
		CHECK(strSender == wstring(vecMailboxList[0].strSenderName));
		CHECK(strTitle == wstring(vecMailboxList[0].strMailTitle));
	}

	TEST_FIXTURE(Fixture_InitDBMailUID_WarpSystem, TestPlayerMailbox_GetExpiredMailList_Success)
	{
		// arrange
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();
		
		int nExpiRemainTimeSec = 1;	///< 유효기간을 1초로 설정.
		GMail* pMail = NewTestMail(m_nMailUID, nExpiRemainTimeSec);
		pMail->InitSystemTimeMSForExpiCheck(0);

		refMailbox.AddMail(pMail);

		vector<int64> vecExpiredMailUID;

		// act
		gsys.pSystem->Sleep(1.0f);	///< 1초 경과.
		refMailbox.GetExpiredMailList(vecExpiredMailUID);

		// check
		CHECK_EQUAL(1, vecExpiredMailUID.size());
	}

	TEST_FIXTURE(Fixture_InitDBMailUID_WarpSystem, TestPlayerMailbox_GetExpiredMailList_Failure_LeakTime)
	{
		// arrange
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();

		int nExpiRemainTimeSec = 10;	///< 유효기간을 10초로 설정.
		GMail* pMail = NewTestMail(m_nMailUID, nExpiRemainTimeSec);
		pMail->InitSystemTimeMSForExpiCheck(0);

		refMailbox.AddMail(pMail);

		vector<int64> vecExpiredMailUID;

		// act
		gsys.pSystem->Sleep(1.0f);	///< 1초 경과.
		refMailbox.GetExpiredMailList(vecExpiredMailUID);

		// check
		CHECK_EQUAL(0, vecExpiredMailUID.size());
	}

	TEST_FIXTURE(Fixture_InitDBMailUID_WarpSystem, TestPlayerMailbox_GetMailTo_TD_Check_ExpiRemainTimeSec)
	{
		// arrange
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();

		int nExpiDBRemainTimeSec = 10;	///< DB에서 받아온 유효기간을 10초로 설정.
		GMail* pMail = NewTestMail(m_nMailUID, nExpiDBRemainTimeSec);
		pMail->InitSystemTimeMSForExpiCheck(0);
		refMailbox.AddMail(pMail);

		float fElapsedTIme = 5.0f;
		gsys.pSystem->Sleep(fElapsedTIme);	///< 5초 경과.

		// act
		TD_MAIL_MAILBOX_MAIL_INFO info;
		refMailbox.GetMailTo_TD(info, m_nMailUID);

		// check
		CHECK_EQUAL(nExpiDBRemainTimeSec - fElapsedTIme, info.nExpiringTimeSec);
	}

	TEST_FIXTURE(Fixture_InitDBMailUID_WarpSystem, TestPlayerMailbox_GetMailTo_TD_Check_ExpiRemainTimeSec_TimeOver)
	{
		// arrange
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();

		int nExpiDBRemainTimeSec = 10;	///< DB에서 받아온 유효기간을 10초로 설정.
		GMail* pMail = NewTestMail(m_nMailUID, nExpiDBRemainTimeSec);
		pMail->InitSystemTimeMSForExpiCheck(0);
		refMailbox.AddMail(pMail);

		float fElapsedTIme = 15.0f;
		gsys.pSystem->Sleep(fElapsedTIme);	///< 15초 경과.

		// act
		TD_MAIL_MAILBOX_MAIL_INFO info;
		refMailbox.GetMailTo_TD(info, m_nMailUID);

		// check
		CHECK_EQUAL(0, info.nExpiringTimeSec);
	}

}
