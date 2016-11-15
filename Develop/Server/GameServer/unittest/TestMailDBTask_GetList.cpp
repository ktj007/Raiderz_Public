#include "stdafx.h"
#include "GDBTaskMailGetList.h"
#include "GTestWrapper.h"
#include "GUTHelper_Player.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GPlayerMailBox.h"



SUITE(MailDBTask_GetList)
{
	const int64 TEST_DB_TOP_MAIL_UID = 200;
	const int64 TEST_DB_BOTTOM_MAIL_UID = 1;

	class TestDBTaskMailGetlist : public GDBTaskMailGetList
	{
	public:
		typedef map<int64, _DATA_MAIL> TestMailDataMap;

	public:
		TestDBTaskMailGetlist(const MUID& uidReqPlayer)
			: GDBTaskMailGetList(uidReqPlayer)
		{
			if (m_bInit == false)
				InitTestDB();
		}

		void InitTestDB()
		{
			const int TEST_MAX_MAIL_COUNT = 100;

			uint64 nMailUID = TEST_DB_BOTTOM_MAIL_UID;
			wstring strSender = L".sender";
			wstring strTitle = L".Title";

			/// MUID 는 2, 4, 6, 8 식으로 들어가게 설정.
			for (int i = 0; i < TEST_MAX_MAIL_COUNT; ++i)
			{
				CString strIndex = L"%d";
				strIndex.Format(strIndex, i);

				_DATA_MAIL tdMail;
				tdMail.nMailUID			= nMailUID * 2;
				tdMail.nType			= MT_PLAYER;
				tdMail.strSender		= strIndex.GetString() + strSender;
				tdMail.strTitle			= strIndex.GetString() + strTitle;
				tdMail.bHasText			= (i % 2 ? true : false);
				tdMail.nAppendedMoney	= 0;
				tdMail.bReadFlag		= (i % 2 ? true : false);
				tdMail.nDefaultItemID	= 0;
				tdMail.nRemainDelTime	= 86400 * (i + 1);
				nMailUID++;

				m_mapTestMailDB.insert(TestMailDataMap::value_type(tdMail.nMailUID, tdMail));
			}

			m_bInit = true;
		}
		void OnExecute(mdb::MDatabase& rfDB) override
		{
			/// 실제 DB 로직을 시뮬레이트 합니다.
			int nCount = 0;
			TestMailDataMap::reverse_iterator ritr = m_mapTestMailDB.rbegin();
			for (; ritr != m_mapTestMailDB.rend(); ++ritr)
			{
				_DATA_MAIL& refMail = ritr->second;
				
				if (m_Data.nReqMailUIDBottom < refMail.nMailUID && refMail.nMailUID <= m_Data.nReqMailUIDTop)
				{
					m_Data.vecDBMailList.push_back(refMail);
					nCount++;
					if (m_Data.nReqMailCount == nCount)
						break;
				}
			}

			SetTaskSuccess();
		}

	private:
		static bool				m_bInit;
		static TestMailDataMap	m_mapTestMailDB;
	};

	// 정적 변수 정의
	bool TestDBTaskMailGetlist::m_bInit = false;
	TestDBTaskMailGetlist::TestMailDataMap TestDBTaskMailGetlist::m_mapTestMailDB;
	
	struct Fixture 
	{
		Fixture()
		{
			m_pPlayer = m_helper.NewPlayer();
			m_pPlayer->GetMailbox().GetDBMailUID().Init(TEST_DB_TOP_MAIL_UID, TEST_DB_BOTTOM_MAIL_UID);
		}
		~Fixture()
		{
		}

		GUTHelper_Player	m_helper;
		GEntityPlayer*		m_pPlayer;
		GTestMgrWrapper<GPlayerObjectManager >	m_wrapperPlayerObjectManager;

	};

	TEST_FIXTURE(Fixture, TestDBTaskMailGetList_Success)
	{
		// arrange
		const int TEST_COUNT = 40;

		TestDBTaskMailGetlist task(m_pPlayer->GetUID());
		task.Input(TEST_DB_TOP_MAIL_UID, TEST_DB_BOTTOM_MAIL_UID, TEST_COUNT);
		task.OnExecute(mdb::MDatabase());

		// act
		task.OnCompleted();

		// assert
		CHECK_EQUAL(TEST_COUNT, m_pPlayer->GetMailbox().GetSize());
	}

	TEST_FIXTURE(Fixture, TestDBTaskMailGetList_Duplicate)
	{
		// arrange
		const int TEST_COUNT = 40;

		TestDBTaskMailGetlist task1(m_pPlayer->GetUID());
		task1.Input(TEST_DB_TOP_MAIL_UID, TEST_DB_BOTTOM_MAIL_UID, TEST_COUNT);
		task1.OnExecute(mdb::MDatabase());

		TestDBTaskMailGetlist task2(m_pPlayer->GetUID());
		task2.Input(TEST_DB_TOP_MAIL_UID, TEST_DB_BOTTOM_MAIL_UID, TEST_COUNT);
		task2.OnExecute(mdb::MDatabase());

		// act
		task1.OnCompleted();
		task2.OnCompleted();

		// assert
		CHECK_EQUAL(TEST_COUNT, m_pPlayer->GetMailbox().GetSize());
	}

	TEST_FIXTURE(Fixture, TestDBTaskMailGetList_UIDRangeSizeIsOne)
	{
		// arrange
		m_pPlayer->GetMailbox().GetDBMailUID().Init(TEST_DB_TOP_MAIL_UID, TEST_DB_TOP_MAIL_UID-1);

		const int TEST_COUNT = 40;

		TestDBTaskMailGetlist task(m_pPlayer->GetUID());
		task.Input(TEST_DB_TOP_MAIL_UID, TEST_DB_TOP_MAIL_UID-1, TEST_COUNT);
		task.OnExecute(mdb::MDatabase());

		// act
		task.OnCompleted();

		// assert
		CHECK_EQUAL(1, m_pPlayer->GetMailbox().GetSize());
	}

	TEST_FIXTURE(Fixture, TestDBTaskMailGetList_ReqCountIsOne)
	{
		// arrange
		const int TEST_COUNT = 1;

		TestDBTaskMailGetlist task(m_pPlayer->GetUID());
		task.Input(TEST_DB_TOP_MAIL_UID, TEST_DB_BOTTOM_MAIL_UID, TEST_COUNT);
		task.OnExecute(mdb::MDatabase());

		// act
		task.OnCompleted();

		// assert
		CHECK_EQUAL(1, m_pPlayer->GetMailbox().GetSize());
	}

	TEST_FIXTURE(Fixture, TestDBTaskMailGetList_ReqCountIsOne_CheckValue)
	{
		// arrange
		const int TEST_COUNT = 1;

		TestDBTaskMailGetlist task(m_pPlayer->GetUID());
		task.Input(TEST_DB_TOP_MAIL_UID, TEST_DB_BOTTOM_MAIL_UID, TEST_COUNT);
		task.OnExecute(mdb::MDatabase());
		task.OnCompleted();

		// act
		vector<TD_MAIL_MAILBOX_MAIL_INFO> vecMailInfo;
		m_pPlayer->GetMailbox().GetMailListTo_TD(vecMailInfo);

		// assert
		ASSERT_EQUAL(1, vecMailInfo.size());
		CHECK_EQUAL(TEST_DB_TOP_MAIL_UID, vecMailInfo[0].uidMail);
	}

}
