#include "stdafx.h"
#include "GFieldFactory.h"
#include "FBaseNetClient.h"
#include "FBaseMockLink.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "SUnitTestUtil.h"
#include "STransData_M2G.h"
#include "CCommandResultTable.h"
#include "GCommandTable.h"
#include "GProxyFieldManager.h"
#include "GFieldMgr.h"
#include "GConfig.h"
#include "GConst.h"
#include "GUTHelper.h"
#include "GPlayerField.h"
#include "GPlayerFieldShared.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicField.h"

// - 테스트 항목 TODO
//
// - 게임에 처음 접속했는데 모든 채널이 꽉차면 새로운 채널을 생성하고 필드에 들어가야 한다.
// - 새로운 필드로 이동했는데 모든 채널이 꽉차면 새로운 채널을 생성하고 필드에 들어가야 한다.
// - 



SUITE(FieldCreation_OnCommand)
{
	struct FFieldCreation_OnCommand : public FBaseNetClient, FBaseMockLink, FBaseFieldInfo, FBaseFieldMgr
	{
		FFieldCreation_OnCommand()
		{
			m_nFieldID = 1;
			m_pFieldInfo = AddFieldInfo(m_nFieldID);

			m_pFieldGroupInfo = AddFieldGroupInfo(DYNAMIC_FIELD_GROUP_ID, DYNAMIC_FIELD_ID1);
		}
		virtual ~FFieldCreation_OnCommand()
		{

		}

		void OnRecv_COMM_RESPONSE_LOGIN(TDMG_MANAGED_SHARED_FIELD& managedSharedField)
		{
			int nParamResult = CR_SUCCESS;
			MUID uidParamAlloc = SUnitTestUtil::NewUID();

			m_pNetAgent->OnRecv(MMC_COMM_RESPONSE_LOGIN_M2G, 3, NEW_INT(nParamResult), NEW_UID(uidParamAlloc), NEW_BLOB(&managedSharedField, sizeof(TDMG_MANAGED_SHARED_FIELD), 1) );
		}
		void OnRecv_COMM_RESPONSE_LOGIN()
		{
			TDMG_MANAGED_SHARED_FIELD managedSharedField;
			managedSharedField.nFieldID = m_nFieldID;
			managedSharedField.nChannelCount = 1;
			managedSharedField.nChannelID[0] = 1;
			managedSharedField.uidField[0] = SUnitTestUtil::NewUID();

			OnRecv_COMM_RESPONSE_LOGIN(managedSharedField);
		}

		void OnRecv_FIELD_ADD(TDMG_SHARED_FIELD_INFO* pTDFieldInfo, int nFieldCount)
		{
			uint32 nChecksum = 0;
			SFieldChecksum fieldChecksum;
			for (int i = 0; i < nFieldCount; i++)
			{
				fieldChecksum.Add(pTDFieldInfo[i].uidField);
			}
			nChecksum = fieldChecksum.GetValue();

			m_pNetAgent->OnRecv(MMC_FIELD_ADD_SHARED_FIELD, 2, 
				NEW_BLOB(pTDFieldInfo, sizeof(TDMG_SHARED_FIELD_INFO), nFieldCount),
				NEW_UINT(nChecksum));
		}
		void OnRecv_FIELD_ADD(MUID uidField, int nFieldID, int nChannelID)
		{
			TDMG_SHARED_FIELD_INFO tdfieldInfo;
			tdfieldInfo.nGameServerID = GConfig::m_nMyServerID;
			tdfieldInfo.uidField = uidField;
			tdfieldInfo.nFieldID = nFieldID;
			tdfieldInfo.nChannelID = nChannelID;

			OnRecv_FIELD_ADD(&tdfieldInfo, 1);
		}
		void OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(TDMG_DYNAMIC_FIELD_GROUP_INFO tdFieldGroupInfo
													, std::vector<TDMG_DYNAMIC_FIELD_NODE> vecDynamicField
													, uint32 nChecksum)
		{
			m_pNetAgent->OnRecv(MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP
				, 3
				, NEW_SINGLE_BLOB(&tdFieldGroupInfo, sizeof(TDMG_DYNAMIC_FIELD_GROUP_INFO))
				, NEW_BLOB(vecDynamicField)
				, NEW_UINT(nChecksum)
				);
		}


		TDMG_DYNAMIC_FIELD_GROUP_INFO MakeTDMG_DYNAMIC_FIELD_GROUP_INFO(MUID uidFieldGroup, MUID uidCreator, int nFieldGroupID, int nServerID)
		{
			TDMG_DYNAMIC_FIELD_GROUP_INFO data;
			data.uidDynamicFieldGroup = uidFieldGroup;
			data.uidCreator = uidCreator;
			data.nFieldGroupID = nFieldGroupID;
			data.nType = DFT_TRIAL;
			data.nGameServerID = nServerID;
			return data;
		}
		TDMG_DYNAMIC_FIELD_NODE MakeTDMG_DYNAMIC_FIELD_NODE(MUID uidDynamicField, int nFieldID, MUID uidFieldGroup)
		{
			TDMG_DYNAMIC_FIELD_NODE data;
			data.uidDynamicField = uidDynamicField;
			data.nFieldID = nFieldID;
			data.uidFieldGroup = uidFieldGroup;
			return data;
		}

		static const int DYNAMIC_FIELD_GROUP_ID = 100;
		static const int DYNAMIC_FIELD_ID1 = 101;
		static const int DYNAMIC_FIELD_ID2 = 102;

		int					m_nFieldID;
		GFieldInfo*			m_pFieldInfo;
		FIELD_GROUP_INFO*	m_pFieldGroupInfo;
	};

	TEST_FIXTURE(FFieldCreation_OnCommand, TestFieldCreation_AddChannel)
	{
		gmgr.pFieldMgr->Clear();
		const int nChannelID = 1;

		OnRecv_COMM_RESPONSE_LOGIN();

		MUID uidField1 = SUnitTestUtil::NewUID();
		MUID uidField2 = SUnitTestUtil::NewUID();

		OnRecv_FIELD_ADD(uidField1, m_nFieldID, nChannelID);

		CHECK_EQUAL(1, m_pMasterServerFacade->GetProxyFieldManager()->GetFieldCount());
		CHECK_EQUAL(1, gmgr.pFieldMgr->GetFieldQty());

		GSharedField* pField = gmgr.pFieldMgr->GetSharedField(m_nFieldID, nChannelID);
		CHECK(pField != NULL);

		GEntityPlayer* pEntityPlayer1 = GUTHelper::NewEntityPlayer(NULL);
		MockLink* pMockLink1 = NewLink(pEntityPlayer1);

		bool bRetGate = pEntityPlayer1->GetPlayerField().GetFieldShared().GateRequest(m_nFieldID, nChannelID, vec3::ZERO, vec3(0.0f, 1.0f, 0.0f));
		CHECK_EQUAL(true, bRetGate);
	}

	TEST_FIXTURE(FFieldCreation_OnCommand, TestFieldCreation_DynamicField_Success)
	{
		CHECK_EQUAL(0, m_pMasterServerFacade->GetProxyFieldManager()->GetFieldGroupCount());
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		// Arrange
		MUID uidFieldGroup = SUnitTestUtil::NewUID();
		MUID uidCreator = SUnitTestUtil::NewUID();
		MUID uidDynamicField = SUnitTestUtil::NewUID();

		TDMG_DYNAMIC_FIELD_GROUP_INFO tdFieldGroupInfo = MakeTDMG_DYNAMIC_FIELD_GROUP_INFO(uidFieldGroup, uidCreator, DYNAMIC_FIELD_GROUP_ID, GConfig::m_nMyServerID);

		std::vector<TDMG_DYNAMIC_FIELD_NODE> vecDynamicField;
		TDMG_DYNAMIC_FIELD_NODE tdDynamicFieldNode = MakeTDMG_DYNAMIC_FIELD_NODE(uidDynamicField, DYNAMIC_FIELD_ID1, uidFieldGroup);
		vecDynamicField.push_back(tdDynamicFieldNode);

		SFieldChecksum checksum;
		checksum.Add(tdFieldGroupInfo.uidDynamicFieldGroup);

		// Act
		OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(tdFieldGroupInfo, vecDynamicField, checksum.GetValue());

		// Assert
		CHECK_EQUAL(1, m_pMasterServerFacade->GetProxyFieldManager()->GetFieldGroupCount());
		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
		CHECK((GDynamicFieldMaster*)NULL != pFieldGroup);

		GDynamicField* pDynamicField = pFieldGroup->Get(DYNAMIC_FIELD_ID1);
		CHECK((GDynamicField*)NULL != pDynamicField);
		CHECK_EQUAL(uidDynamicField, pDynamicField->GetUID());
	}

	TEST_FIXTURE(FFieldCreation_OnCommand, TestFieldCreation_DynamicField_Success_TwoDynamicField)
	{
		// Arrange
		GFieldInfo* pDynamicField2Info = AddFieldInfo(DYNAMIC_FIELD_ID2);
		pDynamicField2Info->m_bDynamic = true;
		m_pFieldGroupInfo->vecFieldInfo.push_back(pDynamicField2Info);

		MUID uidFieldGroup = SUnitTestUtil::NewUID();
		MUID uidCreator = SUnitTestUtil::NewUID();
		MUID uidDynamicField1 = SUnitTestUtil::NewUID();
		MUID uidDynamicField2 = SUnitTestUtil::NewUID();

		TDMG_DYNAMIC_FIELD_GROUP_INFO tdFieldGroupInfo = MakeTDMG_DYNAMIC_FIELD_GROUP_INFO(uidFieldGroup, uidCreator, DYNAMIC_FIELD_GROUP_ID, GConfig::m_nMyServerID);

		std::vector<TDMG_DYNAMIC_FIELD_NODE> vecDynamicField;
		TDMG_DYNAMIC_FIELD_NODE tdDynamicFieldNode;

		tdDynamicFieldNode = MakeTDMG_DYNAMIC_FIELD_NODE(uidDynamicField1, DYNAMIC_FIELD_ID1, uidFieldGroup);
		vecDynamicField.push_back(tdDynamicFieldNode);

		tdDynamicFieldNode = MakeTDMG_DYNAMIC_FIELD_NODE(uidDynamicField2, DYNAMIC_FIELD_ID2, uidFieldGroup);
		vecDynamicField.push_back(tdDynamicFieldNode);

		SFieldChecksum checksum;
		checksum.Add(tdFieldGroupInfo.uidDynamicFieldGroup);
 
		// Act
		OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(tdFieldGroupInfo, vecDynamicField, checksum.GetValue());

		// Assert
		CHECK_EQUAL(1, m_pMasterServerFacade->GetProxyFieldManager()->GetFieldGroupCount());
		CHECK_EQUAL(2, m_pMasterServerFacade->GetProxyFieldManager()->GetFieldCount());
		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
		CHECK((GDynamicFieldMaster*)NULL != pFieldGroup);

		GDynamicField* pDynamicField;
		pDynamicField = pFieldGroup->Get(DYNAMIC_FIELD_ID1);
		CHECK((GDynamicField*)NULL != pDynamicField);
		CHECK_EQUAL(uidDynamicField1, pDynamicField->GetUID());

		pDynamicField = pFieldGroup->Get(DYNAMIC_FIELD_ID2);
		CHECK((GDynamicField*)NULL != pDynamicField);
		CHECK_EQUAL(uidDynamicField2, pDynamicField->GetUID());
	}

}