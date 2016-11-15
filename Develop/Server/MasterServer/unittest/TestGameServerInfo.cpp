#include "stdafx.h"
#include "ZGameServerInfo.h"
#include "ZGameServerInfoLoader.h"
#include "ZGameServerInfoManager.h"
#include "ZTestWrapper.h"
#include "ZFieldInfoManager.h"
#include "ZFixtureHelper.h"

SUITE(GameServerInfo)
{
	struct FGameServerInfo : public FBaseFieldInfo
	{
		FGameServerInfo()
		{

		}
		~FGameServerInfo()
		{

		}
		void SetupFieldInfo(int nFieldID, int nDefaultChannelCount, bool bDynamic)
		{
			AddFieldInfo(nFieldID, L"", L"", bDynamic, nDefaultChannelCount);
		}
	};

	TEST(TestGameServerInfo_Constructor)
	{
		ZGameServerInfo info;

		CHECK_EQUAL(0, info.m_nID);
		CHECK_EQUAL(0, info.m_vecManagedSharedField.size());

		ZManagedSharedField managedSharedField;
		CHECK_EQUAL(0, managedSharedField.nFieldID);
	}

	TEST_FIXTURE(FGameServerInfo, TestGameServerInfoLoader_Sample1)
	{
		SetupFieldInfo(3, 4, false);
		SetupFieldInfo(107, 4, false);

		// 초기 설정 
		//		1 - 3(1,3), 107(1,2,3)
		//		2 - 107(4)
		// 결과
		//		1 - 3(1,2,3,4), 107(1,2,3)
		//		2 - 107(4)
		//		3 -

		string strXmlSample1 = "<Maiet><GameServer id='1'>				\
							  <SharedField id='3' channel='1,3' />		\
							  <SharedField id='107' channel='1,2' />	\
							  <SharedField id='107' channel='3' />		\
						  </GameServer>									\
						  <GameServer id='2'>							\
							  <SharedField id='107' channel='4' />		\
						  </GameServer>									\
						  <GameServer id='3'>							\
						  </GameServer></Maiet>";

		ZGameServerInfoManager gameServerInfoManager;
		ZGameServerInfoLoader loader;

		bool bSuccess = loader.LoadStream(&gameServerInfoManager, strXmlSample1.c_str(), gmgr.pFieldInfoManager);
		CHECK_EQUAL(true, bSuccess);

		CHECK_EQUAL(3, gameServerInfoManager.GetSize());

		// 1번 서버
		ZGameServerInfo* pGameServerInfo = gameServerInfoManager.GetInfo(1);

		CHECK(pGameServerInfo != NULL);

		CHECK_EQUAL(1, pGameServerInfo->m_nID);
		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField.size());

		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[0].nFieldID);
		CHECK_EQUAL(4, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID.size());
		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[0]);
		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[1]);
		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[2]);
		CHECK_EQUAL(4, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[3]);

		CHECK_EQUAL(107, pGameServerInfo->m_vecManagedSharedField[1].nFieldID);
		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID.size());
		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[0]);
		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[1]);
		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[2]);


		// 2번 서버
		ZGameServerInfo* pGameServerInfo2 = gameServerInfoManager.GetInfo(2);
		CHECK_EQUAL(1, pGameServerInfo2->m_vecManagedSharedField.size());

		CHECK_EQUAL(107, pGameServerInfo2->m_vecManagedSharedField[0].nFieldID);
		CHECK_EQUAL(1, pGameServerInfo2->m_vecManagedSharedField[0].vecChannelID.size());
		CHECK_EQUAL(4, pGameServerInfo2->m_vecManagedSharedField[0].vecChannelID[0]);
	}

	TEST_FIXTURE(FGameServerInfo, TestGameServerInfoLoader_Sample2)
	{
		const int nDefaultChannelCount = 6;	// Shared Field의 기본 채널은 6개로 설정

		SetupFieldInfo(3, nDefaultChannelCount, false);
		SetupFieldInfo(107, nDefaultChannelCount, false);
		SetupFieldInfo(108, nDefaultChannelCount, false);


		// 초기 설정 
		//		1 - 107(1,2)
		//		2 - 107(1,3), 108(1,2,3,4,5,6,7)
		// 결과
		//		1 - 3(1,2,3,4,5,6) , 107(1,2,4,5,6)
		//		2 - 107(3), 108(1,2,3,4,5,6)
		string strXmlSample2 = "<Maiet><GameServer id='1'>					\
						  <SharedField id='107' channel='1,2' />			\
						  </GameServer>										\
						  <GameServer id='2'>								\
								<SharedField id='107' channel='1,3' />		\
								<SharedField id='108' channel='1,2,3,4,5,6,7' />	\
						  </GameServer></Maiet>";

		ZGameServerInfoManager gameServerInfoManager;
		ZGameServerInfoLoader loader;

		bool bSuccess = loader.LoadStream(&gameServerInfoManager, strXmlSample2.c_str(), gmgr.pFieldInfoManager);
		CHECK_EQUAL(true, bSuccess);

		// 1번 서버
		ZGameServerInfo* pGameServerInfo = gameServerInfoManager.GetInfo(1);

		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField.size());

		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[0].nFieldID);

		CHECK_EQUAL(6, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID.size());
		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[0]);
		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[1]);
		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[2]);
		CHECK_EQUAL(4, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[3]);
		CHECK_EQUAL(5, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[4]);
		CHECK_EQUAL(6, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[5]);


		CHECK_EQUAL(107, pGameServerInfo->m_vecManagedSharedField[1].nFieldID);
		CHECK_EQUAL(5, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID.size());
		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[0]);
		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[1]);
		CHECK_EQUAL(4, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[2]);
		CHECK_EQUAL(5, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[3]);
		CHECK_EQUAL(6, pGameServerInfo->m_vecManagedSharedField[1].vecChannelID[4]);


		// 2번 서버
		ZGameServerInfo* pGameServerInfo2 = gameServerInfoManager.GetInfo(2);

		CHECK_EQUAL(2, pGameServerInfo2->m_vecManagedSharedField.size());

		CHECK_EQUAL(107, pGameServerInfo2->m_vecManagedSharedField[0].nFieldID);
		CHECK_EQUAL(1, pGameServerInfo2->m_vecManagedSharedField[0].vecChannelID.size());
		CHECK_EQUAL(3, pGameServerInfo2->m_vecManagedSharedField[0].vecChannelID[0]);

		CHECK_EQUAL(108, pGameServerInfo2->m_vecManagedSharedField[1].nFieldID);
		CHECK_EQUAL(6, pGameServerInfo2->m_vecManagedSharedField[1].vecChannelID.size());
		CHECK_EQUAL(1, pGameServerInfo2->m_vecManagedSharedField[1].vecChannelID[0]);
		CHECK_EQUAL(2, pGameServerInfo2->m_vecManagedSharedField[1].vecChannelID[1]);
		CHECK_EQUAL(3, pGameServerInfo2->m_vecManagedSharedField[1].vecChannelID[2]);
		CHECK_EQUAL(4, pGameServerInfo2->m_vecManagedSharedField[1].vecChannelID[3]);
		CHECK_EQUAL(5, pGameServerInfo2->m_vecManagedSharedField[1].vecChannelID[4]);
		CHECK_EQUAL(6, pGameServerInfo2->m_vecManagedSharedField[1].vecChannelID[5]);

	}

	TEST_FIXTURE(FGameServerInfo, TestGameServerInfoLoader_Sample3)
	{
		const int nDefaultChannelCount = 4;

		SetupFieldInfo(9001, nDefaultChannelCount, false);


		// 초기 설정 
		//		1 - 
		//		2 - 9001(2)
		// 결과
		//		1 - 9001(1,3,4)
		//		2 - 9001(2)
		string strXmlSample3 = "<Maiet><GameServer id='1'>					\
							   </GameServer>										\
							   <GameServer id='2'>								\
							   <SharedField id='9001' channel='2' />		\
							   </GameServer></Maiet>";

		ZGameServerInfoManager gameServerInfoManager;
		ZGameServerInfoLoader loader;

		bool bSuccess = loader.LoadStream(&gameServerInfoManager, strXmlSample3.c_str(), gmgr.pFieldInfoManager);
		CHECK_EQUAL(true, bSuccess);

		// 1번 서버
		ZGameServerInfo* pGameServerInfo = gameServerInfoManager.GetInfo(1);

		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField.size());

		CHECK_EQUAL(9001, pGameServerInfo->m_vecManagedSharedField[0].nFieldID);

		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID.size());
		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[0]);
		CHECK_EQUAL(3, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[1]);
		CHECK_EQUAL(4, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[2]);

		// 2번 서버
		ZGameServerInfo* pGameServerInfo2 = gameServerInfoManager.GetInfo(2);

		CHECK_EQUAL(1, pGameServerInfo2->m_vecManagedSharedField.size());

		CHECK_EQUAL(9001, pGameServerInfo2->m_vecManagedSharedField[0].nFieldID);
		CHECK_EQUAL(1, pGameServerInfo2->m_vecManagedSharedField[0].vecChannelID.size());
		CHECK_EQUAL(2, pGameServerInfo2->m_vecManagedSharedField[0].vecChannelID[0]);


	}


	TEST_FIXTURE(FGameServerInfo, TestGameServerInfoLoader_Sample_DynamicField)
	{
		const int nDefaultChannelCount = 2;

		SetupFieldInfo(9001, nDefaultChannelCount, false);
		SetupFieldInfo(9002, nDefaultChannelCount, true);		// 9002는 동적 필드라 제외되어야 한다.
		SetupFieldInfo(9003, nDefaultChannelCount, true);		// 9003은 동적 필드라 제외되어야 한다.


		// 결과
		//		1 - 9001(1,2)
		string strXmlSample3 = "<Maiet><GameServer id='1'><SharedField id='9003' channel='1' /></GameServer></Maiet>";

		ZGameServerInfoManager gameServerInfoManager;
		ZGameServerInfoLoader loader;

		bool bSuccess = loader.LoadStream(&gameServerInfoManager, strXmlSample3.c_str(), gmgr.pFieldInfoManager);
		CHECK_EQUAL(true, bSuccess);

		// 1번 서버
		ZGameServerInfo* pGameServerInfo = gameServerInfoManager.GetInfo(1);

		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField.size());

		CHECK_EQUAL(9001, pGameServerInfo->m_vecManagedSharedField[0].nFieldID);

		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID.size());
		CHECK_EQUAL(1, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[0]);
		CHECK_EQUAL(2, pGameServerInfo->m_vecManagedSharedField[0].vecChannelID[1]);
	}

}