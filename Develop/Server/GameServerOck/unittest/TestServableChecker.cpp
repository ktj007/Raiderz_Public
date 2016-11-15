#include "StdAfx.h"
#include "GServableChecker.h"
#include "GServerInfo.h"
#include "GServer.h"
#include "GMasterServerFacade.h"
#include "GAppServerFacade.h"
#include "GManagedFieldInfoMgr.h"

#include "FBaseMasterServerFacade.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"


SUITE(ServableChecker)
{
	struct FServableChecker : public FBaseMasterServerFacade, FBaseFieldInfo , FBaseFieldMgr
	{
		GServableChecker m_ServableChecker;

		bool IsCheckSuccess()
		{
			if (true == m_ServableChecker.IsServable() &&
				SERVER_STAT_ACTIVE == gsys.pServer->GetServerInfo().nStat)
				return true;

			return false;
		}
		void CreateSharedFields()
		{
			GFieldInfo* pFieldInfo0 = AddFieldInfo();
			GFieldInfo* pFieldInfo1 = AddFieldInfo();

			const int TEST_FIELD_COUNT = 2;
			TDMG_MANAGED_SHARED_FIELD tdManagedSharedFields[TEST_FIELD_COUNT];
			tdManagedSharedFields[0].nFieldID = pFieldInfo0->m_nFieldID;
			tdManagedSharedFields[0].nChannelCount = pFieldInfo0->m_nDefaultChannelCount;
			tdManagedSharedFields[0].nChannelID[0] = 1;
			tdManagedSharedFields[0].uidField[0] = SUnitTestUtil::NewUID();
			tdManagedSharedFields[0].nChannelID[1] = 2;
			tdManagedSharedFields[0].uidField[1] = SUnitTestUtil::NewUID();

			tdManagedSharedFields[1].nFieldID = pFieldInfo1->m_nFieldID;
			tdManagedSharedFields[1].nChannelCount = pFieldInfo1->m_nDefaultChannelCount;
			tdManagedSharedFields[1].nChannelID[0] = 1;
			tdManagedSharedFields[1].uidField[0] = SUnitTestUtil::NewUID();
			tdManagedSharedFields[1].nChannelID[1] = 2;
			tdManagedSharedFields[1].uidField[1] = SUnitTestUtil::NewUID();

			gsys.pMasterServerFacade->CreateManagedSharedFields(tdManagedSharedFields, TEST_FIELD_COUNT);
		}
	};
	
	TEST_FIXTURE(FServableChecker, TestServableChecker)
	{
		const float TIME_CHECK_REGULATOR = SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC;

		CHECK_EQUAL(false, IsCheckSuccess());

		CHECK_EQUAL(false, gsys.pMasterServerFacade->GetNetClient()->IsConnected());
		CHECK_EQUAL(0, gsys.pMasterServerFacade->GetManagedFieldInfoMgr()->size());

		m_ServableChecker.Update(0.0f);
		CHECK_EQUAL(false, IsCheckSuccess());

		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CHECK_EQUAL(false, IsCheckSuccess());

		/// MasterServer 와 연결 설정
		gsys.pMasterServerFacade->GetNetClient()->Connect(NULL, 0);
		CHECK_EQUAL(true, gsys.pMasterServerFacade->GetNetClient()->IsConnected());

		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CHECK_EQUAL(false, IsCheckSuccess());

		/// AppServer 와 연결 설정
		gsys.pAppServerFacade->GetNetClient()->Connect(NULL, 0);
		CHECK_EQUAL(true, gsys.pAppServerFacade->GetNetClient()->IsConnected());

		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CHECK_EQUAL(false, IsCheckSuccess());

		/// 마스터서버에서 보낸 필드 정보와, 로컬에 생성한 필드 정보 일치하는지 체크
		CreateSharedFields();
		CHECK_EQUAL(2, gsys.pMasterServerFacade->GetManagedFieldInfoMgr()->size());

		/// Servable Check 성공
		m_ServableChecker.Update(TIME_CHECK_REGULATOR);
		CHECK_EQUAL(true, IsCheckSuccess());
	}
}
