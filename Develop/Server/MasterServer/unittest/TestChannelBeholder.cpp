#include "stdafx.h"
#include "ZChannelBeholder.h"
#include "ZFixtureHelper.h"
#include "ZSharedField.h"
#include "ZSharedFieldMaster.h"
#include "ZGameServerInfo.h"
#include "ZGameServerInfoManager.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "CCommandTable_Master.h"
#include "ZTestWrapper.h"
#include "FakeSystem.h"
#include "ZConst.h"


SUITE(ChannelBeholder)
{
	struct FChannelBeholder : public FBaseServerField
	{
		FChannelBeholder()
		{
			InitGameServer();
			InitSharedField();
		}
		~FChannelBeholder()
		{

		}

		void InitGameServer()
		{
			AddFieldInfo(TEST_FIELD_ID);

			ZGameServerInfo* pGameServerInfo = AddGameServerInfo(TEST_GAMESERVER_ID, TEST_FIELD_ID, TEST_DEFAULT_CHANNEL_COUNT, 1, 2);
			ZGameServerObject* pGameServerObj = AddGameServerObject(pGameServerInfo);
		}
		void InitSharedField()
		{
			//ZFieldInfo* pFieldInfo = AddFieldInfo(m_nFieldID);
			//int nChannelID[TEST_DEFAULT_CHANNEL_COUNT] = {1, 2};
			m_pSharedField1 = gmgr.pFieldManager->FindSharedField(TEST_FIELD_ID, 1);	//AddSharedField(pFieldInfo, nChannelID[0], m_nGameServerID);
			m_pSharedField2 = gmgr.pFieldManager->FindSharedField(TEST_FIELD_ID, 2);	//AddSharedField(pFieldInfo, nChannelID[1], m_nGameServerID);

			m_pSharedFieldMaster = gmgr.pFieldManager->FindSharedFieldMaster(TEST_FIELD_ID);
		}

		void Util_MakeFieldToCrowd(ZSharedField* pSharedField)
		{
			for (int i = 0; i < pSharedField->GetPlayerLimit(); ++i)
				pSharedField->IncreasePlayerCount();
		}

		static const int TEST_GAMESERVER_ID = 1;
		static const int TEST_FIELD_ID = 100;
		static const int TEST_DEFAULT_CHANNEL_COUNT = 2;

		ZSharedField* m_pSharedField1;
		ZSharedField* m_pSharedField2;
		ZSharedFieldMaster* m_pSharedFieldMaster;
		ZChannelBeholder m_channelBeholder;

		ZTestSysWrapper2<ZSystem, FakeSystem>	m_FakeSystemWrapper;
	};

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_NotCrowd)
	{
		/// Fixture 설치
		CHECK_EQUAL(0, m_pSharedField1->GetPlayerCount());

		/// SUT 실행
		bool bRet = m_channelBeholder.IsCrowd(m_pSharedField1);

		/// 검증
		CHECK_EQUAL(false, bRet);
	}
	
	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_SharedFIeld)
	{
		/// Fixture 설치
		Util_MakeFieldToCrowd(m_pSharedField1);
		CHECK_EQUAL(m_pSharedField1->GetPlayerLimit(), m_pSharedField1->GetPlayerCount());

		/// SUT 실행
		bool bRet = m_channelBeholder.IsCrowd(m_pSharedField1);

		/// 검증
		CHECK_EQUAL(true, bRet);
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_SharedFIeld_Boundary1)
	{
		/// Fixture 설치 : IsCrowd() 함수가 true 가 되는 경계값에서 1이 부족하도록 설정
		int nBoundary = static_cast<int>(m_pSharedField1->GetPlayerLimit() * (ZConst::RATIO_CHANNEL_CROWD_FOR_NEW_CHANNEL / 100.f));
		for (int i = 0; i < nBoundary - 1; ++i)
			m_pSharedField1->IncreasePlayerCount();

		/// SUT 실행, 검증
		CHECK_EQUAL(false, m_channelBeholder.IsCrowd(m_pSharedField1));
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_SharedFIeld_Boundary2)
	{
		/// Fixture 설치 : IsCrowd() 함수가 true 가 되는 경계값으로 설정
		int nBoundary = static_cast<int>(m_pSharedField1->GetPlayerLimit() * (ZConst::RATIO_CHANNEL_CROWD_FOR_NEW_CHANNEL / 100.f));
		for (int i = 0; i < nBoundary; ++i)
			m_pSharedField1->IncreasePlayerCount();

		/// SUT 실행, 검증
		CHECK_EQUAL(true, m_channelBeholder.IsCrowd(m_pSharedField1));
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_SharedFIeldMaster)
	{
		/// Fixture 설치
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT, m_pSharedFieldMaster->GetChannelCount());
		CHECK_EQUAL(false, m_channelBeholder.IsCrowd(m_pSharedFieldMaster));

		Util_MakeFieldToCrowd(m_pSharedField1);
		Util_MakeFieldToCrowd(m_pSharedField2);
		
		/// SUT 실행, 검증
		CHECK_EQUAL(true, m_channelBeholder.IsCrowd(m_pSharedFieldMaster));
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_SharedFIeldMaster_NotCrowdChannel2)
	{
		/// Fixture 설치
		Util_MakeFieldToCrowd(m_pSharedField1);
		CHECK_EQUAL(0, m_pSharedField2->GetPlayerCount());

		/// SUT 실행, 검증
		CHECK_EQUAL(false, m_channelBeholder.IsCrowd(m_pSharedFieldMaster));
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_SharedFIeldMaster_Boundary1)
	{
		/// Fixture 설치 : IsCrowd() 함수가 true 가 되는 경계값에서 1번 Channel 만 1이 부족하도록 설정
		int nBoundary = static_cast<int>(m_pSharedField1->GetPlayerLimit() * (ZConst::RATIO_CHANNEL_CROWD_FOR_NEW_CHANNEL / 100.f));
		for (int i = 0; i < nBoundary - 1; ++i)
			m_pSharedField1->IncreasePlayerCount();
		for (int i = 0; i < nBoundary; ++i)
			m_pSharedField2->IncreasePlayerCount();

		/// SUT 실행, 검증
		CHECK_EQUAL(false, m_channelBeholder.IsCrowd(m_pSharedFieldMaster));
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_IsCrowd_SharedFIeldMaster_Boundary2)
	{
		/// Fixture 설치 : IsCrowd() 함수가 true 가 되는 경계값으로 설정
		int nBoundary = static_cast<int>(m_pSharedField1->GetPlayerLimit() * (ZConst::RATIO_CHANNEL_CROWD_FOR_NEW_CHANNEL / 100.f));
		for (int i = 0; i < nBoundary; ++i)
			m_pSharedField1->IncreasePlayerCount();
		for (int i = 0; i < nBoundary; ++i)
			m_pSharedField2->IncreasePlayerCount();

		/// SUT 실행, 검증
		CHECK_EQUAL(true, m_channelBeholder.IsCrowd(m_pSharedFieldMaster));
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_Update_AllocNewChannel)
	{
		/// Fixture 설치
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT, m_pSharedFieldMaster->GetChannelCount());

		Util_MakeFieldToCrowd(m_pSharedField1);
		Util_MakeFieldToCrowd(m_pSharedField2);

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT+1, m_pSharedFieldMaster->GetChannelCount());
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_Update2_AllocNewChannel)
	{
		/// Fixture 설치
		Util_MakeFieldToCrowd(m_pSharedField1);
		Util_MakeFieldToCrowd(m_pSharedField2);

		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT+1, m_pSharedFieldMaster->GetChannelCount());

		ZSharedField* pSharedField3 = m_pSharedFieldMaster->FindChannel(3);
		ASSERT_CHECK(NULL != pSharedField3);
		Util_MakeFieldToCrowd(pSharedField3);

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT+2, m_pSharedFieldMaster->GetChannelCount());
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_Update_AllocNewChannel_NotDefaultChannel)
	{
		/// Fixture 설치
		CHECK_EQUAL(true, m_pSharedFieldMaster->IsDefaultChannel(1));
		CHECK_EQUAL(true, m_pSharedFieldMaster->IsDefaultChannel(2));
		CHECK_EQUAL(false, m_pSharedFieldMaster->IsDefaultChannel(3));

		Util_MakeFieldToCrowd(m_pSharedField1);

		gmgr.pFieldManager->DestroySharedField(m_pSharedField2->GetUID());

		CHECK_EQUAL(1, m_pSharedFieldMaster->GetChannelCount());
		CHECK(NULL == m_pSharedFieldMaster->FindChannel(2));
		CHECK(NULL == m_pSharedFieldMaster->FindChannel(3));

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(2, m_pSharedFieldMaster->GetChannelCount());
		CHECK(NULL == m_pSharedFieldMaster->FindChannel(2));
		CHECK(NULL != m_pSharedFieldMaster->FindChannel(3));
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_Update_AllocNewReservedChannel_Failure)
	{
		/// Fixture 설치
		ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(TEST_GAMESERVER_ID);

		const int TEST_SEC_GAMESERVER = 2;
		const int TEST_SEC_FIELD = 200;
		ZGameServerObject* pGameServerObject2 = AddGameServerObject(TEST_SEC_GAMESERVER, TEST_SEC_FIELD);

		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT,	pGameServerObject->GetFieldMap().GetSharedFieldCount());
		CHECK_EQUAL(0,							pGameServerObject2->GetFieldMap().GetSharedFieldCount());

		Util_MakeFieldToCrowd(m_pSharedField1);
		Util_MakeFieldToCrowd(m_pSharedField2);

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT, pGameServerObject->GetFieldMap().GetSharedFieldCount());
		CHECK_EQUAL(1,						    pGameServerObject2->GetFieldMap().GetSharedFieldCount());
	}

	TEST_FIXTURE(FChannelBeholder, TestChannelBeholder_Update_AllocNewReservedChannel_Success)
	{
		/// Fixture 설치
		ZGameServerInfo* pGameServerInfo = gmgr.pGameServerInfoManager->GetInfo(TEST_GAMESERVER_ID);
		pGameServerInfo->m_vecManagedSharedField[0].vecReservedChannelID.push_back(3);	///< 채널 예약. (3)

		ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(TEST_GAMESERVER_ID);

		const int TEST_SEC_GAMESERVER = 2;
		const int TEST_SEC_FIELD = 200;
		ZGameServerObject* pGameServerObject2 = AddGameServerObject(TEST_SEC_GAMESERVER, TEST_SEC_FIELD);

		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT,	pGameServerObject->GetFieldMap().GetSharedFieldCount());
		CHECK_EQUAL(0,							pGameServerObject2->GetFieldMap().GetSharedFieldCount());

		Util_MakeFieldToCrowd(m_pSharedField1);
		Util_MakeFieldToCrowd(m_pSharedField2);

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT+1, pGameServerObject->GetFieldMap().GetSharedFieldCount());
		CHECK_EQUAL(0,							pGameServerObject2->GetFieldMap().GetSharedFieldCount());
	}

	struct FChannelBeholder_TestCollectSilentChannel : FChannelBeholder
	{
		FChannelBeholder_TestCollectSilentChannel()
		{
			InitAdditionalSharedField();
		}

		void InitAdditionalSharedField()
		{
			int nChannelID[2] = {3, 4};
			m_pSharedField3 = AddSharedField(TEST_FIELD_ID, nChannelID[0], TEST_GAMESERVER_ID, false);
			m_pSharedField4 = AddSharedField(TEST_FIELD_ID, nChannelID[1], TEST_GAMESERVER_ID, false);

			m_uidSharedField3 = m_pSharedField3->GetUID();
			m_uidSharedField4 = m_pSharedField4->GetUID();
		}

		static const int TEST_ALL_CHANNEL_COUNT = 4;

		MUID			m_uidSharedField3;
		MUID			m_uidSharedField4;
		ZSharedField*	m_pSharedField3;
		ZSharedField*	m_pSharedField4;
	};

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_IsSilent_true)
	{
		CHECK_EQUAL(0, m_pSharedField3->GetPlayerCount());

		/// SUT 실행 + 검증
		CHECK_EQUAL(true, m_channelBeholder.IsSilent(m_pSharedField3));
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_IsSilent_false)
	{
		/// Fixture 설치
		m_pSharedField3->IncreasePlayerCount();
		CHECK_EQUAL(1, m_pSharedField3->GetPlayerCount());

		/// SUT 실행 + 검증
		CHECK_EQUAL(false, m_channelBeholder.IsSilent(m_pSharedField3));
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_Positive)
	{
		/// Fixture 설치
		CHECK_EQUAL(true, m_channelBeholder.IsSilent(m_pSharedField3));
		CHECK_EQUAL(true, m_channelBeholder.IsSilent(m_pSharedField4));

		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);
		gsys.pSystem->Sleep(ZConst::TICK_EXPIRE_SILENT_CHANNEL * 0.001f);	///< 10초 경과
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(2,						gmgr.pFieldManager->GetSharedFieldCount());
		CHECK_EQUAL((ZSharedField*)NULL,	gmgr.pFieldManager->FindSharedField(m_uidSharedField3));
		CHECK_EQUAL((ZSharedField*)NULL,	gmgr.pFieldManager->FindSharedField(m_uidSharedField4));
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_Positive2)
	{
		/// Fixture 설치
		CHECK_EQUAL(true, m_channelBeholder.IsSilent(m_pSharedField3));
		m_pSharedField4->IncreasePlayerCount();
		CHECK_EQUAL(false, m_channelBeholder.IsSilent(m_pSharedField4));

		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);
		gsys.pSystem->Sleep(ZConst::TICK_EXPIRE_SILENT_CHANNEL * 0.001f);	///< 10초 경과
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT-1,	gmgr.pFieldManager->GetSharedFieldCount());
		CHECK_EQUAL((ZSharedField*)NULL,		gmgr.pFieldManager->FindSharedField(m_uidSharedField3));
		CHECK((ZSharedField*)NULL			 != gmgr.pFieldManager->FindSharedField(m_uidSharedField4));
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_GetSilentChannelCount)
	{
		CHECK_EQUAL(0, m_channelBeholder.GetSilentChannelCount());

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(2, m_channelBeholder.GetSilentChannelCount());
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_GetSilentChannelCount2)
	{
		/// Fixture 설치
		CHECK_EQUAL(0, m_channelBeholder.GetSilentChannelCount());

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(2, m_channelBeholder.GetSilentChannelCount());
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_GetSilentChannelCount3)
	{
		/// Fixture 설치
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());
		CHECK_EQUAL(0, m_channelBeholder.GetSilentChannelCount());

		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);
		CHECK_EQUAL(2, m_channelBeholder.GetSilentChannelCount());

		/// SUT 실행
		float fSkipTime = ZConst::TICK_EXPIRE_SILENT_CHANNEL * 0.001f;
		gsys.pSystem->Sleep(fSkipTime);
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_DEFAULT_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());
		CHECK_EQUAL(0, m_channelBeholder.GetSilentChannelCount());
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_Negative_LackDeltaTIme)
	{
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// SUT 실행
		float fSkipTime = max((ZConst::TICK_EXPIRE_SILENT_CHANNEL * 0.001f) - 1.0f, 0);
		gsys.pSystem->Sleep(fSkipTime);
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_Negative_IsNotSilent)
	{
		/// Fixture 설치
		m_pSharedField3->IncreasePlayerCount();
		m_pSharedField4->IncreasePlayerCount();
		CHECK_EQUAL(false, m_channelBeholder.IsSilent(m_pSharedField3));
		CHECK_EQUAL(false, m_channelBeholder.IsSilent(m_pSharedField4));

		/// SUT 실행
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);
		gsys.pSystem->Sleep(ZConst::TICK_EXPIRE_SILENT_CHANNEL * 0.001f);	///< 10초 경과
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());
	}

	TEST_FIXTURE(FChannelBeholder_TestCollectSilentChannel, TestChannelBeholder_Clear_Expired_SilentChannel_Negative_ChangeState_NotSilent)
	{
		/// Fixture 설치
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		CHECK_EQUAL(2, m_channelBeholder.GetSilentChannelCount());
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());

		m_pSharedField3->IncreasePlayerCount();
		m_pSharedField4->IncreasePlayerCount();
		CHECK_EQUAL(false, m_channelBeholder.IsSilent(m_pSharedField3));
		CHECK_EQUAL(false, m_channelBeholder.IsSilent(m_pSharedField4));

		/// SUT 실행
		gsys.pSystem->Sleep(ZConst::TICK_EXPIRE_SILENT_CHANNEL * 0.001f);	///< 10초 경과
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(0, m_channelBeholder.GetSilentChannelCount());
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());
	}

	struct FChannelBeholder_TestCommand : FChannelBeholder_TestCollectSilentChannel, FBaseMockLink
	{
		FChannelBeholder_TestCommand()
		{
			ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(TEST_GAMESERVER_ID);
			m_pLinkGameServer = NewLink(pGameServerObject->GetUID());
		}

		MockLink* m_pLinkGameServer;
	};

	TEST_FIXTURE(FChannelBeholder_TestCommand, TestChannelBeholder_Clear_Expired_SilentChannel_Command)
	{
		m_pSharedField3->IncreasePlayerCount();
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT, gmgr.pFieldManager->GetSharedFieldCount());

		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// SUT 실행
		gsys.pSystem->Sleep(ZConst::TICK_EXPIRE_SILENT_CHANNEL * 0.001f);	///< 10초 경과
		m_channelBeholder.Update(ZConst::TIME_UPDATE_CHANNEL_BEHOLDER_SEC);

		/// 검증
		CHECK_EQUAL(TEST_ALL_CHANNEL_COUNT - 1, gmgr.pFieldManager->GetSharedFieldCount());

		ASSERT_EQUAL(1,							m_pLinkGameServer->GetCommandCount());
		CHECK_EQUAL(MMC_FIELD_DEL_SHARED_FIELD,	m_pLinkGameServer->GetCommand(0).GetID());
		CHECK_EQUAL(m_uidSharedField4,			m_pLinkGameServer->GetParam<MUID>(0, 0));
	}

}
