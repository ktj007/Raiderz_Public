#include "stdafx.h"
#include "ZFieldManager.h"
#include "ZFixtureHelper.h"
#include "ZSharedField.h"
#include "ZDynamicFieldMaster.h"
#include "SDef.h"

SUITE(BaseFieldManager)
{
	struct FBaseFieldManager : public FBaseFieldInfo
	{
		int m_nFieldID1;
		int m_nFieldID2;
		int m_nFieldGroupID1;
		int m_nFieldGroupID2;

		ZBaseFieldManager m_BaseFieldManager;

		FBaseFieldManager()
		{
			InitTestFieldInfo();
			InitTestFieldGroupInfo();
		}
		~FBaseFieldManager()
		{
			m_BaseFieldManager.Clear();
		}

		void InitTestFieldInfo()
		{
			m_nFieldID1 = 1;
			AddFieldInfo(m_nFieldID1, L"TestField1", L"", false, 1);

			m_nFieldID2 = 2;
			AddFieldInfo(m_nFieldID2, L"TestField2", L"", false, 1);
		}
		void InitTestFieldGroupInfo()
		{
			int nTestDynamicFieldInfoID = 1001;
			AddFieldInfo(nTestDynamicFieldInfoID, L"", L"", true, 0);

			m_nFieldGroupID1 = 1000;
			AddFieldGroupInfo(m_nFieldGroupID1, nTestDynamicFieldInfoID);

			m_nFieldGroupID2 = 2000;
			AddFieldGroupInfo(m_nFieldGroupID2, nTestDynamicFieldInfoID);
		}


		MUID AddTestSharedField(int nFieldID, int nGameServerID)
		{
			ZFieldInfo* pFieldInfo = gmgr.pFieldInfoManager->Find(nFieldID);
			if(NULL == pFieldInfo) return MUID::Invalid();

			int nTestChannelID = ZTestHelper::NewID();
			bool bIsDefaultChannel = true;
			ZSharedField* pSharedField = new ZSharedField(ZTestHelper::NewUID(), pFieldInfo, nTestChannelID, bIsDefaultChannel, nGameServerID);
			m_BaseFieldManager.GetSharedFieldMap().insert(ZBaseFieldManager::ZSharedFieldMap::value_type(pSharedField->GetUID(), pSharedField));
			return pSharedField->GetUID();
		}
		void DelTestSharedField(MUID uidSharedField)
		{
			ZBaseFieldManager::ZSharedFieldMap::iterator itor = m_BaseFieldManager.GetSharedFieldMap().find(uidSharedField);
			if (itor != m_BaseFieldManager.GetSharedFieldMap().end())
			{
				ZSharedField* pSharedField = (*itor).second;
				pSharedField->Drop();
				m_BaseFieldManager.GetSharedFieldMap().erase(itor);
			}
		}

		MUID AddTestDynamicFieldMaster(int nFieldGroupID, int nGameServerID, MUID uidCreator)
		{
			FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoManager->FindGroupInfo(nFieldGroupID);
			if(NULL == pFieldGroupInfo) return MUID::Invalid();

			ZDynamicFieldMaster* pFieldGroup = new ZDynamicFieldMaster(ZTestHelper::NewUID(), pFieldGroupInfo, nGameServerID, DFT_TRIAL, uidCreator);
			m_BaseFieldManager.GetDynamicFieldMasterMap().insert(ZBaseFieldManager::ZDynamicFieldMasterMap::value_type(pFieldGroup->GetUID(), pFieldGroup));
			return pFieldGroup->GetUID();
		}
		void DelTestDynamicFieldMaster(MUID uidDynamicFieldMaster)
		{
			ZBaseFieldManager::ZDynamicFieldMasterMap::iterator itor = m_BaseFieldManager.GetDynamicFieldMasterMap().find(uidDynamicFieldMaster);
			if (itor != m_BaseFieldManager.GetDynamicFieldMasterMap().end())
			{
				ZDynamicFieldMaster* pDynamicFieldMaster = (*itor).second;
				pDynamicFieldMaster->Drop();
				m_BaseFieldManager.GetDynamicFieldMasterMap().erase(itor);
			}
		}
	};

	TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_GetSharedFieldCount)
	{
		int nTestGameServerID = 2;

		CHECK_EQUAL(0, m_BaseFieldManager.GetSharedFieldCount());

		MUID uidSharedFIeld1 = AddTestSharedField(m_nFieldID1, nTestGameServerID);
		CHECK_EQUAL(1, m_BaseFieldManager.GetSharedFieldCount());

		MUID uidSharedFIeld2 = AddTestSharedField(m_nFieldID2, nTestGameServerID);
		CHECK_EQUAL(2, m_BaseFieldManager.GetSharedFieldCount());

		DelTestSharedField(uidSharedFIeld1);
		CHECK_EQUAL(1, m_BaseFieldManager.GetSharedFieldCount());

		DelTestSharedField(uidSharedFIeld2);
		CHECK_EQUAL(0, m_BaseFieldManager.GetSharedFieldCount());
	}

	TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_GetDynamicFieldMasterCount)
	{
		int nTestGameServerID = 2;
		MUID uidCreator = ZTestHelper::NewUID();

		CHECK_EQUAL(0, m_BaseFieldManager.GetDynamicFieldMasterCount());

		MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(m_nFieldGroupID1, nTestGameServerID, uidCreator);
		CHECK_EQUAL(1, m_BaseFieldManager.GetDynamicFieldMasterCount());

		MUID uidTestFieldGroup2 = AddTestDynamicFieldMaster(m_nFieldGroupID2, nTestGameServerID, uidCreator);
		CHECK_EQUAL(2, m_BaseFieldManager.GetDynamicFieldMasterCount());

		DelTestDynamicFieldMaster(uidTestFieldGroup1);
		CHECK_EQUAL(1, m_BaseFieldManager.GetDynamicFieldMasterCount());

		DelTestDynamicFieldMaster(uidTestFieldGroup2);
		CHECK_EQUAL(0, m_BaseFieldManager.GetDynamicFieldMasterCount());
	}

	TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_FindSharedField)
	{
		int nTestGameServerID = 2;
		MUID uidSharedFIeld1 = AddTestSharedField(m_nFieldID1, nTestGameServerID);
		MUID uidSharedFIeld2 = AddTestSharedField(m_nFieldID2, nTestGameServerID);

		ZSharedField* pSharedField1 = m_BaseFieldManager.FindSharedField(uidSharedFIeld1);
		CHECK(NULL != pSharedField1);
		CHECK_EQUAL(uidSharedFIeld1,	pSharedField1->GetUID());
		CHECK_EQUAL(m_nFieldID1,		pSharedField1->GetFieldInfo()->m_nFieldID);
		CHECK_EQUAL(nTestGameServerID,	pSharedField1->GetGameServerID());

		ZSharedField* pSharedField2 = m_BaseFieldManager.FindSharedField(uidSharedFIeld2);
		CHECK(NULL != pSharedField2);
		CHECK_EQUAL(uidSharedFIeld2,	pSharedField2->GetUID());
		CHECK_EQUAL(m_nFieldID2,		pSharedField2->GetFieldInfo()->m_nFieldID);
		CHECK_EQUAL(nTestGameServerID,	pSharedField2->GetGameServerID());
	}

	TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_FindSharedField_Failure_InvalidUID)
	{
		int nTestGameServerID = 2;
		MUID uidSharedFIeld1 = AddTestSharedField(m_nFieldID1, nTestGameServerID);

		MUID uidInvalid = 5000;
		CHECK(uidSharedFIeld1 != uidInvalid);

		ZSharedField* pSharedField1 = m_BaseFieldManager.FindSharedField(uidInvalid);
		CHECK_EQUAL((ZSharedField*)NULL, pSharedField1);

		pSharedField1 = m_BaseFieldManager.FindSharedField(0);
		CHECK_EQUAL((ZSharedField*)NULL, pSharedField1);
	}

	TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_FindDynamicFieldMaster)
	{
		int nTestGameServerID = 2;
		MUID uidCreator1 = ZTestHelper::NewUID();
		MUID uidCreator2 = ZTestHelper::NewUID();

		MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(m_nFieldGroupID1, nTestGameServerID, uidCreator1);
		MUID uidTestFieldGroup2 = AddTestDynamicFieldMaster(m_nFieldGroupID2, nTestGameServerID, uidCreator2);

		ZDynamicFieldMaster* pFieldGroup1 = m_BaseFieldManager.FindDynamicFieldMaster(uidTestFieldGroup1);
		CHECK(NULL != pFieldGroup1);
		CHECK_EQUAL(uidTestFieldGroup1,	pFieldGroup1->GetUID());
		CHECK_EQUAL(m_nFieldGroupID1,	pFieldGroup1->GetInfo()->nID);
		CHECK_EQUAL(uidCreator1,		pFieldGroup1->GetCreatorUID());
		CHECK_EQUAL(nTestGameServerID,	pFieldGroup1->GetGameServerID());

		ZDynamicFieldMaster* pFieldGroup2 = m_BaseFieldManager.FindDynamicFieldMaster(uidTestFieldGroup2);
		CHECK(NULL != pFieldGroup2);
		CHECK_EQUAL(uidTestFieldGroup2,	pFieldGroup2->GetUID());
		CHECK_EQUAL(m_nFieldGroupID2,	pFieldGroup2->GetInfo()->nID);
		CHECK_EQUAL(uidCreator2,		pFieldGroup2->GetCreatorUID());
		CHECK_EQUAL(nTestGameServerID,	pFieldGroup2->GetGameServerID());
	}

	TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_FindDynamicFieldMaster_Failure_InvalidUID)
	{
		int nTestGameServerID = 2;
		MUID uidCreator = ZTestHelper::NewUID();
		MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(m_nFieldGroupID1, nTestGameServerID, uidCreator);

		MUID uidInvalid = 5000;
		CHECK(uidTestFieldGroup1 != uidInvalid);

		ZDynamicFieldMaster* pFieldGroup1 = m_BaseFieldManager.FindDynamicFieldMaster(uidInvalid);
		CHECK_EQUAL((ZDynamicFieldMaster*)NULL, pFieldGroup1);

		pFieldGroup1 = m_BaseFieldManager.FindDynamicFieldMaster(0);
		CHECK_EQUAL((ZDynamicFieldMaster*)NULL, pFieldGroup1);
	}

	TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Clear)
	{
		int nTestGameServerID = 2;
		MUID uidSharedFIeld1 = AddTestSharedField(m_nFieldID1, nTestGameServerID);
		MUID uidSharedFIeld2 = AddTestSharedField(m_nFieldID2, nTestGameServerID);

		MUID uidCreator1 = ZTestHelper::NewUID();
		MUID uidCreator2 = ZTestHelper::NewUID();
		MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(m_nFieldGroupID1, nTestGameServerID, uidCreator1);
		MUID uidTestFieldGroup2 = AddTestDynamicFieldMaster(m_nFieldGroupID2, nTestGameServerID, uidCreator2);

		CHECK_EQUAL(2, m_BaseFieldManager.GetSharedFieldCount());
		CHECK_EQUAL(2, m_BaseFieldManager.GetDynamicFieldMasterCount());

		m_BaseFieldManager.Clear();

		CHECK_EQUAL(0, m_BaseFieldManager.GetSharedFieldCount());
		CHECK_EQUAL(0, m_BaseFieldManager.GetDynamicFieldMasterCount());

		ZSharedField* pSharedField1 = m_BaseFieldManager.FindSharedField(uidSharedFIeld1);
		ZSharedField* pSharedField2 = m_BaseFieldManager.FindSharedField(uidSharedFIeld2);
		ZDynamicFieldMaster* pFieldGroup1 = m_BaseFieldManager.FindDynamicFieldMaster(uidTestFieldGroup1);
		ZDynamicFieldMaster* pFieldGroup2 = m_BaseFieldManager.FindDynamicFieldMaster(uidTestFieldGroup2);

		CHECK(NULL == pSharedField1);
		CHECK(NULL == pSharedField2);
		CHECK(NULL == pFieldGroup1);
		CHECK(NULL == pFieldGroup2);
	}


	namespace fixture_helper_method
	{
		namespace fixture_AddTestSharedField
		{
			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_AddTestSharedField)
			{
				CHECK_EQUAL(0, m_BaseFieldManager.GetSharedFieldMap().size());

				int nTestGameServerID = 2;
				MUID uidSharedFIeld1 = AddTestSharedField(m_nFieldID1, nTestGameServerID);

				CHECK(MUID::Invalid() != uidSharedFIeld1);
				CHECK_EQUAL(1, m_BaseFieldManager.GetSharedFieldMap().size());
			}

			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_AddTestSharedField_Failure)
			{
				CHECK_EQUAL(0, m_BaseFieldManager.GetSharedFieldMap().size());

				MUID uidSharedFIeld1 = AddTestSharedField(0, 0);

				CHECK(MUID::Invalid() == uidSharedFIeld1);
				CHECK_EQUAL(0, m_BaseFieldManager.GetSharedFieldMap().size());
			}
		}

		namespace fixture_DelTestSharedField
		{
			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_DelTestSharedField)
			{
				int nTestGameServerID = 2;
				MUID uidSharedFIeld1 = AddTestSharedField(m_nFieldID1, nTestGameServerID);

				CHECK_EQUAL(1, m_BaseFieldManager.GetSharedFieldMap().size());

				DelTestSharedField(uidSharedFIeld1);

				CHECK_EQUAL(0, m_BaseFieldManager.GetSharedFieldMap().size());
			}

			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_DelTestSharedField_Failure)
			{
				int nTestGameServerID = 2;
				MUID uidSharedFIeld1 = AddTestSharedField(m_nFieldID1, nTestGameServerID);

				CHECK_EQUAL(1, m_BaseFieldManager.GetSharedFieldMap().size());

				DelTestSharedField(MUID::Invalid());

				CHECK_EQUAL(1, m_BaseFieldManager.GetSharedFieldMap().size());
			}
		}


		namespace fixture_AddTestDynamicFieldMaster
		{
			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_AddTestDynamicFieldMaster)
			{
				CHECK_EQUAL(0, m_BaseFieldManager.GetDynamicFieldMasterMap().size());

				int nTestGameServerID = 2;
				MUID uidCreator1 = ZTestHelper::NewUID();
				MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(m_nFieldGroupID1, nTestGameServerID, uidCreator1);

				CHECK(MUID::Invalid() != uidTestFieldGroup1);
				CHECK_EQUAL(1, m_BaseFieldManager.GetDynamicFieldMasterMap().size());
			}

			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_AddTestDynamicFieldMaster_Failure)
			{
				CHECK_EQUAL(0, m_BaseFieldManager.GetDynamicFieldMasterMap().size());

				MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(0, 0, 0);

				CHECK(MUID::Invalid() == uidTestFieldGroup1);
				CHECK_EQUAL(0, m_BaseFieldManager.GetDynamicFieldMasterMap().size());
			}
		}

		namespace fixture_DelTestDynamicFieldMaster
		{
			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_DelTestDynamicFieldMaster)
			{
				int nTestGameServerID = 2;
				MUID uidCreator1 = ZTestHelper::NewUID();
				MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(m_nFieldGroupID1, nTestGameServerID, uidCreator1);

				CHECK_EQUAL(1, m_BaseFieldManager.GetDynamicFieldMasterMap().size());

				DelTestDynamicFieldMaster(uidTestFieldGroup1);

				CHECK_EQUAL(0, m_BaseFieldManager.GetDynamicFieldMasterMap().size());
			}

			TEST_FIXTURE(FBaseFieldManager, FBaseFieldManager_Fixture_DelTestDynamicFieldMaster_Failure)
			{
				int nTestGameServerID = 2;
				MUID uidCreator1 = ZTestHelper::NewUID();
				MUID uidTestFieldGroup1 = AddTestDynamicFieldMaster(m_nFieldGroupID1, nTestGameServerID, uidCreator1);

				CHECK_EQUAL(1, m_BaseFieldManager.GetDynamicFieldMasterMap().size());

				DelTestDynamicFieldMaster(MUID::Invalid());

				CHECK_EQUAL(1, m_BaseFieldManager.GetDynamicFieldMasterMap().size());
			}
		}
	}

}