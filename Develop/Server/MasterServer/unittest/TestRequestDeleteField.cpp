#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZFieldManager.h"
#include "ZSharedField.h"
#include "ZDynamicFieldMaster.h"
#include "ZGameServerObject.h"
#include "ZGameServerFieldMap.h"
#include "CCommandTable_Master.h"

SUITE(RequestDeleteField)
{
	struct Fixture : public FBaseServerField
	{
		int m_nTestFieldID;
		
		MUID m_uidGameServer;
		MockLink* m_pLinkGameServer;

		Fixture()
		{
			m_nTestFieldID = 100;

			AddFieldInfo(m_nTestFieldID, L"FieldOfGameServer", L"", false, 2);

			m_uidGameServer = AddGameServerObject(1, m_nTestFieldID)->GetUID();
			m_pLinkGameServer = ZTestHelper::NewLink(m_uidGameServer);
		}
		~Fixture()
		{
			ZTestHelper::ClearLinks();
		}

		MUID AddFieldGroup(ZGameServerObject* pGameServer, int nFieldGroupID, int nFirstFieldID)
		{
			AddFieldInfo(nFirstFieldID, L"TestDynamicField", L"", true, 0);
			AddFieldGroupInfo(nFieldGroupID, nFirstFieldID);

			MUID uidOwner = ZTestHelper::NewUID();
			ZDynamicFieldMaster* pFieldGroup = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, pGameServer->GetID(), DFT_TRIAL, uidOwner);
			pGameServer->GetFieldMap().AddDynamicFieldMaster(pFieldGroup);

			return pFieldGroup->GetUID();
		}
		MCommandResult RecvMMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP(MUID uidFieldGroup)
		{
			return m_pLinkGameServer->OnRecv(MMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP,
											1, 
											NEW_UID(uidFieldGroup));
		}

	};

	namespace fixture
	{
		TEST_FIXTURE(Fixture, TestDeleteField_Fixture_ExistFieldInGameServer)
		{
			ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
			CHECK(NULL != pGameServer);
			CHECK_EQUAL(1, pGameServer->GetFieldMap().GetSharedFieldCount());
			CHECK_EQUAL(1, gmgr.pFieldManager->GetSharedFieldCount());
		}

		TEST_FIXTURE(Fixture, TestDeleteField_Fixture_AddFieldGroup)
		{
			ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
			CHECK_EQUAL(0, pGameServer->GetFieldMap().GetDynamicFieldMasterCount());
			CHECK_EQUAL(0, gmgr.pFieldManager->GetDynamicFieldMasterCount());

			int nFieldGroupID = 1000;
			int nFieldID = 1001;

			MUID uidNewFieldGroup = AddFieldGroup(pGameServer, nFieldGroupID, nFieldID);

			CHECK(0 != uidNewFieldGroup);
			CHECK_EQUAL(1, pGameServer->GetFieldMap().GetDynamicFieldMasterCount());
			CHECK_EQUAL(1, gmgr.pFieldManager->GetDynamicFieldMasterCount());
		}
	}

	
	TEST_FIXTURE(Fixture, TestDeleteField_DeleteDynamicFieldGroup)
	{
		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);
		
		int nFieldGroupID = 1000;
		int nFieldID = 1001;

		MUID uidNewFieldGroup = AddFieldGroup(pGameServer, nFieldGroupID, nFieldID);

		CHECK_EQUAL(1, pGameServer->GetFieldMap().GetDynamicFieldMasterCount());
		CHECK_EQUAL(1, gmgr.pFieldManager->GetDynamicFieldMasterCount());

		MCommandResult nResult = RecvMMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP(uidNewFieldGroup);
		CHECK_EQUAL(nResult, CR_TRUE);

		CHECK_EQUAL(0, pGameServer->GetFieldMap().GetDynamicFieldMasterCount());
		CHECK_EQUAL(0, gmgr.pFieldManager->GetDynamicFieldMasterCount());
	}

	TEST_FIXTURE(Fixture, TestDeleteField_DeleteDynamicFieldGroup_Failure_InvalidFieldGroupUID)
	{
		ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_uidGameServer);

		int nFieldGroupID = 1000;
		int nFieldID = 1001;

		MUID uidNewFieldGroup = AddFieldGroup(pGameServer, nFieldGroupID, nFieldID);

		CHECK_EQUAL(1, pGameServer->GetFieldMap().GetDynamicFieldMasterCount());
		CHECK_EQUAL(1, gmgr.pFieldManager->GetDynamicFieldMasterCount());

		MUID uidInvalid = 300;

		MCommandResult nResult = RecvMMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP(uidInvalid);

		CHECK_EQUAL(1, pGameServer->GetFieldMap().GetDynamicFieldMasterCount());
		CHECK_EQUAL(1, gmgr.pFieldManager->GetDynamicFieldMasterCount());
	}


}