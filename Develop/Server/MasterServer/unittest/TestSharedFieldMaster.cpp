#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZSharedFieldMaster.h"
#include "ZSharedField.h"


SUITE(SharedFieldMaster)
{
	struct Fixture : public FBaseFieldInfo, FBaseField
	{
		Fixture()
		{
			m_pFieldInfo = AddFieldInfo();
			m_pSharedField = AddSharedField(m_pFieldInfo);
			m_pSharedFieldMaster = gmgr.pFieldManager->FindSharedFieldMaster(m_pFieldInfo->m_nFieldID);
		}
		~Fixture()
		{

		}

		ZFieldInfo* m_pFieldInfo;
		ZSharedField* m_pSharedField;
		ZSharedFieldMaster* m_pSharedFieldMaster;
	};

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_Remove)
	{
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetChannelCount());

		/// SUT 실행
		m_pSharedFieldMaster->Remove(m_pSharedField->GetChannelID());

		/// 검증
		CHECK_EQUAL(0, m_pSharedFieldMaster->GetChannelCount());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetAllocableChannelID1)
	{
		/// 선행조건 확인
		int nFirstChannelID = m_pSharedField->GetChannelID();
		CHECK_EQUAL(1, nFirstChannelID);
		CHECK_EQUAL(true, m_pSharedFieldMaster->IsExistChannel(nFirstChannelID));
		
		/// SUT 실행
		int nAllocableChannelID = m_pSharedFieldMaster->GetAllocableChannelID();

		/// 검증
		CHECK_EQUAL(2, nAllocableChannelID);
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetAllocableChannelID2)
	{
		/// Fixture 설치
		int nChannelID = 2;
		AddSharedField(m_pFieldInfo, nChannelID);

		/// SUT 실행
		int nAllocableChannelID = m_pSharedFieldMaster->GetAllocableChannelID();

		/// 검증
		CHECK_EQUAL(3, nAllocableChannelID);
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetAllocableChannelID3)
	{
		/// Fixture 설치
		int nChannelID[3] = {2, 3, 4};
		AddSharedField(m_pFieldInfo, nChannelID[0]);
		AddSharedField(m_pFieldInfo, nChannelID[1]);
		AddSharedField(m_pFieldInfo, nChannelID[2]);

		/// SUT 실행
		int nAllocableChannelID = m_pSharedFieldMaster->GetAllocableChannelID();

		/// 검증
		CHECK_EQUAL(5, nAllocableChannelID);
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel)
	{
		/// Fixture 설치
		int nChannelID2 = 2;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID2);

		CHECK_EQUAL(true, m_pSharedField->IsEnterable());	///< 1번 채널
		CHECK_EQUAL(true, m_pSharedField->IsQuiet());
		CHECK_EQUAL(true, pSharedField2->IsEnterable());	///< 2번 채널
		CHECK_EQUAL(true, pSharedField2->IsQuiet());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_Sorting)
	{
		/// Fixture 설치
		int nChannelID1 = 1;
		m_pSharedFieldMaster->Remove(nChannelID1); ///< 1번 채널을 제거.

		int nChannelID3 = 3;
		AddSharedField(m_pFieldInfo, nChannelID3); ///< 3번 채널을 먼저 추가.

		int nChannelID2 = 2;
		AddSharedField(m_pFieldInfo, nChannelID2); ///< 2번 채널을 나중에 추가.

		/// SUT 실행, 검증 - Should Return Channel2 By Sorting
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_AddChannel_Sort)
	{
		/// SUT 실행
		int nChannelID4 = 4;
		AddSharedField(m_pFieldInfo, nChannelID4); ///< 4번 채널 추가.

		int nChannelID2 = 2;
		AddSharedField(m_pFieldInfo, nChannelID2); ///< 2번 채널 추가.

		int nChannelID3 = 3;
		AddSharedField(m_pFieldInfo, nChannelID3); ///< 3번 채널 추가.

		/// 검증
		ZSharedFieldMaster::ZChannelVector vecChannel = m_pSharedFieldMaster->GetChannelVec();
		CHECK_EQUAL(1, vecChannel[0]->GetChannelID());
		CHECK_EQUAL(2, vecChannel[1]->GetChannelID());
		CHECK_EQUAL(3, vecChannel[2]->GetChannelID());
		CHECK_EQUAL(4, vecChannel[3]->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_FirstChannel_IsNotQuiet_Should_Return_SecondChannel)
	{
		/// Fixture 설치
		int nChannelID2 = 2;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID2);
		while (m_pSharedField->IsQuiet())
			m_pSharedField->IncreasePlayerCount();

		CHECK_EQUAL(true, m_pSharedField->IsEnterable());
		CHECK_EQUAL(false, m_pSharedField->IsQuiet());
		CHECK_EQUAL(true, pSharedField2->IsEnterable());
		CHECK_EQUAL(true, pSharedField2->IsQuiet());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_FirstChannelIsEnterable)
	{
		/// Fixture 설치
		int nChannelID = 2;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID);
		CHECK_EQUAL(true, m_pSharedField->IsEnterable());
		CHECK_EQUAL(true, pSharedField2->IsEnterable());

		/// SUT 실행
		ZSharedField* pEnterableField = m_pSharedFieldMaster->GetEnterableChannel();

		/// 검증
		CHECK_EQUAL(1, pEnterableField->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_NotEnterableFirstChannel)
	{
		/// Fixture 설치
		int nChannelID = 2;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID);
		for (int i = 0; i < m_pSharedField->GetPlayerLimit(); ++i) m_pSharedField->IncreasePlayerCount();
		
		CHECK_EQUAL(false, m_pSharedField->IsEnterable());
		CHECK_EQUAL(true, pSharedField2->IsEnterable());

		/// SUT 실행
		ZSharedField* pEnterableField = m_pSharedFieldMaster->GetEnterableChannel();

		/// 검증
		CHECK_EQUAL(2, pEnterableField->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_AllChannelIsNotEnterable)
	{
		/// Fixture 설치
		int nChannelID = 2;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID);
		for (int i = 0; i < m_pSharedField->GetPlayerLimit(); ++i)
		{
			m_pSharedField->IncreasePlayerCount();
			pSharedField2->IncreasePlayerCount();
		}

		CHECK_EQUAL(false, m_pSharedField->IsEnterable());
		CHECK_EQUAL(false, pSharedField2->IsEnterable());

		/// SUT 실행
		ZSharedField* pEnterableField = m_pSharedFieldMaster->GetEnterableChannel();

		/// 검증
		CHECK_EQUAL((ZSharedField*)NULL, pEnterableField);
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_NotExistQuietChannel_Sequencial)
	{
		/// Fixture 설치
		int nChannelID2 = 2;
		int nChannelID3 = 3;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID2);
		ZSharedField* pSharedField3 = AddSharedField(m_pFieldInfo, nChannelID3);
		while (m_pSharedField->IsQuiet())
		{
			m_pSharedField->IncreasePlayerCount();
			pSharedField2->IncreasePlayerCount();
			pSharedField3->IncreasePlayerCount();
		}
		CHECK_EQUAL(true, m_pSharedField->IsEnterable());	///< 1번 채널
		CHECK_EQUAL(false, m_pSharedField->IsQuiet());
		CHECK_EQUAL(true, pSharedField2->IsEnterable());	///< 2번 채널
		CHECK_EQUAL(false, pSharedField2->IsQuiet());
		CHECK_EQUAL(true, pSharedField3->IsEnterable());	///< 3번 채널
		CHECK_EQUAL(false, pSharedField3->IsQuiet());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel2
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel3
		CHECK_EQUAL(3, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_NotExistQuietChannel_DeleteMiddleChannel)
	{
		/// Fixture 설치
		int nChannelID2 = 2;
		int nChannelID3 = 3;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID2);
		ZSharedField* pSharedField3 = AddSharedField(m_pFieldInfo, nChannelID3);
		while (m_pSharedField->IsQuiet())
		{
			m_pSharedField->IncreasePlayerCount();
			pSharedField2->IncreasePlayerCount();
			pSharedField3->IncreasePlayerCount();
		}

		m_pSharedFieldMaster->GetEnterableChannel(); ///< Return Channel 1
		m_pSharedFieldMaster->GetEnterableChannel(); ///< Return Channel 2

		m_pSharedFieldMaster->Remove(2); ///< 2번 채널 제거

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel3
		CHECK_EQUAL(3, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());
	}

	TEST_FIXTURE(Fixture, TestSharedFieldMaster_GetEnterableChannel_NotExistQuietChannel_DeleteLastChannel)
	{
		/// Fixture 설치
		int nChannelID2 = 2;
		int nChannelID3 = 3;
		ZSharedField* pSharedField2 = AddSharedField(m_pFieldInfo, nChannelID2);
		ZSharedField* pSharedField3 = AddSharedField(m_pFieldInfo, nChannelID3);
		while (m_pSharedField->IsQuiet())
		{
			m_pSharedField->IncreasePlayerCount();
			pSharedField2->IncreasePlayerCount();
			pSharedField3->IncreasePlayerCount();
		}

		m_pSharedFieldMaster->GetEnterableChannel(); ///< Return Channel 1
		m_pSharedFieldMaster->GetEnterableChannel(); ///< Return Channel 2

		m_pSharedFieldMaster->Remove(3); ///< 3번 채널 제거

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel2
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());

		/// SUT 실행, 검증 - Should Return Channel1
		CHECK_EQUAL(1, m_pSharedFieldMaster->GetEnterableChannel()->GetChannelID());
	}

}