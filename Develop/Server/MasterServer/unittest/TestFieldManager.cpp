#include "stdafx.h"
#include "ZFieldManager.h"
#include "ZSharedField.h"
#include "ZDynamicField.h"
#include "ZDynamicFieldMaster.h"
#include "ZFieldInfoManager.h"
#include "ZGameServerFieldMap.h"
#include "SChecksum.h"
#include "ZFixtureHelper.h"
#include "ZCommandTable.h"
#include "ZPlayerLogic.h"
#include "STransData_M2G.h"
#include "ZGameServerObject.h"
#include "ZSharedFieldMaster.h"


SUITE(FieldManager)
{
	struct FFieldManager : public FBaseFieldInfo
	{
		FFieldManager()
		{
			AddFieldInfo();
			AddFiledGroupInfo();
		}
		~FFieldManager()
		{

		}
		void AddFieldInfo()
		{
			for (int i = 1; i <= 3; ++i)
			{
				ZFieldInfo* pFieldInfo = new ZFieldInfo();
				pFieldInfo->m_nFieldID = i;

				gmgr.pFieldInfoManager->Insert(pFieldInfo);
			}
		}
		void AddFiledGroupInfo()
		{
			m_pFieldGroupInfo = new FIELD_GROUP_INFO();
			m_pFieldGroupInfo->nID = 1;
			m_pFieldGroupInfo->pStartFieldInfo = gmgr.pFieldInfoManager->Find(1);
			m_pFieldGroupInfo->vecFieldInfo.push_back(gmgr.pFieldInfoManager->Find(1));

			gmgr.pFieldInfoManager->InsertFieldGroup(m_pFieldGroupInfo);
		}

		FIELD_GROUP_INFO*					m_pFieldGroupInfo;
		ZTestMgrWrapper<ZFieldManager>		m_fmgr;
	};

	TEST_FIXTURE(FFieldManager, TestFieldManager_CreateSharedField)
	{
		/// Fixture 설치
		int nFieldID = 1;
		int nChannelID = 1;
		int nGameServerID = 1;

		/// SUT 실행
		ZSharedField* pField = gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID, nGameServerID);

		/// 검증
		CHECK_EQUAL(1, gmgr.pFieldManager->GetSharedFieldCount());
		CHECK(MUID(0,1) == pField->GetUID());

		// Checksum 검사
		const MUID uidField = pField->GetUID();
		CHECK_EQUAL(uidField.High + uidField.Low + uidField.Low, gmgr.pFieldManager->GetChecksumFields());
	}

	TEST_FIXTURE(FFieldManager, TestFieldManager_DestroySharedField)
	{
		/// Fixture 설치
		int nFieldID = 1;
		int nChannelID = 1;
		int nGameServerID = 1;
		ZSharedField* pField = gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID, nGameServerID);

		/// SUT 실행
		gmgr.pFieldManager->DestroySharedField(pField->GetUID());

		/// 검증
		CHECK_EQUAL(0, gmgr.pFieldManager->GetSharedFieldCount());
		CHECK_EQUAL(0, gmgr.pFieldManager->GetChecksumFields());
	}

	TEST_FIXTURE(FFieldManager, TestGameServerFieldMap_AddSharedField)
	{
		/// Fixture 설치
		int nFieldID = 1;
		int nChannelID = 1;
		int nGameServerID = 1;
		ZSharedField* pField = gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID, nGameServerID);

		ZGameServerFieldMap fieldMap;

		CHECK_EQUAL(1, pField->GetReferenceCount());

		/// SUT 실행
		fieldMap.AddSharedField(pField);

		/// 검증
		CHECK_EQUAL(2, pField->GetReferenceCount());///< fieldMap은 레퍼런스 카운터만 증가시킨다.
		CHECK_EQUAL(1, fieldMap.GetSharedFieldCount());
	}

	TEST_FIXTURE(FFieldManager, TestGameServerFieldMap_DelSharedField)
	{
		/// Fixture 설치
		int nFieldID = 1;
		int nChannelID = 1;
		int nGameServerID = 1;
		ZSharedField* pField = gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID, nGameServerID);

		ZGameServerFieldMap fieldMap;
		fieldMap.AddSharedField(pField);

		CHECK_EQUAL(2, pField->GetReferenceCount());
		CHECK_EQUAL(1, fieldMap.GetSharedFieldCount());

		/// SUT 실행
		fieldMap.DelSharedField(pField->GetUID());

		/// 검증
		CHECK_EQUAL(1, pField->GetReferenceCount());
		CHECK_EQUAL(0, fieldMap.GetSharedFieldCount());
	}

	TEST_FIXTURE(FFieldManager, TestGameServerFieldMap_GetSharedFieldMasterCount)
	{
		/// Fixture 설치
		int nFieldID = 1;
		int nChannelID = 1;
		int nGameServerID = 1;

		/// SUT 실행
		gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID, nGameServerID);
		/// 검증
		CHECK_EQUAL(1, gmgr.pFieldManager->GetSharedFieldMasterCount());


		/// SUT 실행
		int nChannelID2 = 2;
		gmgr.pFieldManager->CreateSharedField(nFieldID, nChannelID2, nGameServerID);
		/// 검증
		CHECK_EQUAL(1, gmgr.pFieldManager->GetSharedFieldMasterCount());


		/// SUT 실행
		int nFieldID2 = 2;
		gmgr.pFieldManager->CreateSharedField(nFieldID2, nChannelID, nGameServerID);
		/// 검증
		CHECK_EQUAL(2, gmgr.pFieldManager->GetSharedFieldMasterCount());

		/// ...
		gmgr.pFieldManager->CreateSharedField(nFieldID2, nChannelID2, nGameServerID);
		CHECK_EQUAL(2, gmgr.pFieldManager->GetSharedFieldMasterCount());

		///
		int nFieldID3 = 3;
		ZSharedField* pSharedField3 = gmgr.pFieldManager->CreateSharedField(nFieldID3, nChannelID, nGameServerID);
		CHECK_EQUAL(3, gmgr.pFieldManager->GetSharedFieldMasterCount());

		gmgr.pFieldManager->DestroySharedField(pSharedField3->GetUID());
		CHECK_EQUAL(3, gmgr.pFieldManager->GetSharedFieldMasterCount());

		ZSharedFieldMaster* pFieldMaster3 = gmgr.pFieldManager->FindSharedFieldMaster(nFieldID3);
		CHECK_EQUAL(0, pFieldMaster3->GetChannelCount());
	}

	TEST_FIXTURE(FFieldManager, TestFieldManager_CreateDynamicFieldMaster)
	{
		/// Fixture 설치
		int nFieldGroupID = 1;
		int nGameServerID = 1;
		DYNAMIC_FIELD_TYPE eTestDynamicFieldType = DFT_TRIAL;
		MUID uidFieldGroupCreator = 100;

		/// SUT 실행
		ZDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, nGameServerID, eTestDynamicFieldType, uidFieldGroupCreator);

		/// 검증
		CHECK_EQUAL(1, gmgr.pFieldManager->GetDynamicFieldMasterCount());

		// Checksum 검사
		const MUID uidDynamicFieldMaster = pDynamicFieldMaster->GetUID();
		CHECK_EQUAL(uidDynamicFieldMaster.High + uidDynamicFieldMaster.Low + uidDynamicFieldMaster.Low
					, gmgr.pFieldManager->GetChecksumDynamicFieldMasters());
	}

	TEST_FIXTURE(FFieldManager, TestFieldManager_DestroyDynamicFieldMaster)
	{
		/// Fixture 설치
		int nFieldGroupID = 1;
		int nGameServerID = 1;
		DYNAMIC_FIELD_TYPE eTestDynamicFieldType = DFT_TRIAL;
		MUID uidFieldGroupCreator = 100;

		ZDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, nGameServerID, eTestDynamicFieldType, uidFieldGroupCreator);

		/// SUT 실행
		gmgr.pFieldManager->DestroyDynamicFieldMaster(pDynamicFieldMaster->GetUID());

		/// 검증
		CHECK_EQUAL(0, gmgr.pFieldManager->GetDynamicFieldMasterCount());
		CHECK_EQUAL(0, gmgr.pFieldManager->GetChecksumDynamicFieldMasters());
	}

	TEST_FIXTURE(FFieldManager, TestGameServerFieldMap_AddDynamicFieldMaster)
	{
		/// Fixture 설치
		int nFieldGroupID = 1;
		int nGameServerID = 1;
		DYNAMIC_FIELD_TYPE eTestDynamicFieldType = DFT_TRIAL;
		MUID uidFieldGroupCreator = 100;

		ZDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, nGameServerID, eTestDynamicFieldType, uidFieldGroupCreator);

		ZGameServerFieldMap fieldMap;

		CHECK_EQUAL(1, pDynamicFieldMaster->GetReferenceCount());

		/// SUT 실행
		fieldMap.AddDynamicFieldMaster(pDynamicFieldMaster);	///< fieldMap은 레퍼런스 카운터만 증가시킨다.

		/// 검증
		CHECK_EQUAL(2, pDynamicFieldMaster->GetReferenceCount());
		CHECK_EQUAL(1, fieldMap.GetDynamicFieldMasterCount());
	}

	TEST_FIXTURE(FFieldManager, TestGameServerFieldMap_DelDynamicFieldMaster)
	{
		/// Fixture 설치
		int nFieldGroupID = 1;
		int nGameServerID = 1;
		DYNAMIC_FIELD_TYPE eTestDynamicFieldType = DFT_TRIAL;
		MUID uidFieldGroupCreator = 100;

		ZDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, nGameServerID, eTestDynamicFieldType, uidFieldGroupCreator);
		ZGameServerFieldMap fieldMap;
		// fieldMap은 레퍼런스 카운터만 증가시킨다.
		fieldMap.AddDynamicFieldMaster(pDynamicFieldMaster);

		CHECK_EQUAL(2, pDynamicFieldMaster->GetReferenceCount());
		CHECK_EQUAL(1, fieldMap.GetDynamicFieldMasterCount());

		/// SUT 실행
		fieldMap.DelDynamicFieldMaster(pDynamicFieldMaster->GetUID());

		/// 검증
		CHECK_EQUAL(1, pDynamicFieldMaster->GetReferenceCount());
		CHECK_EQUAL(0, fieldMap.GetDynamicFieldMasterCount());
	}

	TEST(TestFieldChecksum)
	{
		SFieldChecksum fieldChecksum;

		MUID uidField1 = MUID(1, 15);
		MUID uidField2 = MUID(9, 50);

		fieldChecksum.Reset();
		fieldChecksum.Add(uidField1);

		// 각 UID별 high + (Low * 2)
		CHECK_EQUAL((1+15*2), fieldChecksum.GetValue());

		fieldChecksum.Add(uidField2);

		CHECK_EQUAL((1+15*2+9+50*2), fieldChecksum.GetValue());
	}


	//////////////////////////////////////////////////////////////////////////
	struct FFieldManager_Command : public FBaseMockLink2, FBasePlayer
	{
		FFieldManager_Command()
		{
			InitDefaultGameServer();
			
			m_nGameServerID = m_nServerID;

			AddFieldInfo();
			AddFiledGroupInfo();
			AddFiledGroupInfo2();

			m_pLink = NewLink(m_uidClient);
		}
		~FFieldManager_Command()
		{
			
		}
		void AddFieldInfo()
		{
			ZFieldInfo* pFieldInfo = new ZFieldInfo();
			pFieldInfo->m_nFieldID = 1;

			gmgr.pFieldInfoManager->Insert(pFieldInfo);
		}
		void AddFiledGroupInfo()
		{
			m_pFieldGroupInfo = new FIELD_GROUP_INFO();
			m_pFieldGroupInfo->nID = 1;
			m_pFieldGroupInfo->pStartFieldInfo = gmgr.pFieldInfoManager->Find(1);
			m_pFieldGroupInfo->vecFieldInfo.push_back(gmgr.pFieldInfoManager->Find(1));

			gmgr.pFieldInfoManager->InsertFieldGroup(m_pFieldGroupInfo);
		}
		void AddFiledGroupInfo2()
		{
			ZFieldInfo* pFieldInfo = new ZFieldInfo();
			pFieldInfo->m_nFieldID = 2;

			gmgr.pFieldInfoManager->Insert(pFieldInfo);

			m_pFieldGroupInfo2 = new FIELD_GROUP_INFO();
			m_pFieldGroupInfo2->nID = 2;
			m_pFieldGroupInfo2->pStartFieldInfo = gmgr.pFieldInfoManager->Find(1);
			m_pFieldGroupInfo2->vecFieldInfo.push_back(gmgr.pFieldInfoManager->Find(1));
			m_pFieldGroupInfo2->vecFieldInfo.push_back(gmgr.pFieldInfoManager->Find(2));

			gmgr.pFieldInfoManager->InsertFieldGroup(m_pFieldGroupInfo2);
		}
		MUID AddFieldGroup(int nFieldGroupID, int nGameServerID, MUID uidCreator)
		{
			ZDynamicFieldMaster* pFieldGroup = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, nGameServerID, DFT_TRIAL, uidCreator);
			return pFieldGroup->GetUID();
		}

		int m_nGameServerID;

		FIELD_GROUP_INFO* m_pFieldGroupInfo;
		FIELD_GROUP_INFO* m_pFieldGroupInfo2;
		MockLink* m_pLink;
	};

	TEST_FIXTURE(FFieldManager_Command, TestCmdHandler_MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL)
	{
		/// SUT 실행
		MCommandResult nCommandResult = m_pLink->OnRecv(MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL, 0, NULL);

		/// 검증
		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MMC_FIELD_SHARED_FIELD_INFO_ALL, m_pLink->GetCommand(0).GetID());

		uint32 nParamChecksum = m_pLink->GetParam<unsigned int>(0, 1);
		CHECK_EQUAL(gmgr.pFieldManager->GetChecksumFields(), nParamChecksum);
	}

	TEST_FIXTURE(FFieldManager_Command, TestCmdPost_MMC_FIELD_DEL_SHARED_FIELD)
	{
		/// Fixture 설치
		ZTestMgrWrapper<ZFieldManager>		fmgr;

		const int nTestFieldID = 1;
		const int nTestChannelID = 1;
		const int nTestServerID = 1;

		ZSharedField* pField = gmgr.pFieldManager->CreateSharedField(nTestFieldID, nTestChannelID, nTestServerID);

		MUID uidField = pField->GetUID();
		m_pLink->ResetCommands();

		/// SUT 실행
		gmgr.pFieldManager->DestroySharedField(uidField, true);

		/// 검증
		uint32 nCheckSum = gmgr.pFieldManager->GetChecksumFields();
		CHECK_EQUAL(MMC_FIELD_DEL_SHARED_FIELD, m_pLink->GetCommandID(0));
		CHECK_EQUAL(uidField, m_pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(nCheckSum, m_pLink->GetParam<unsigned int>(0, 1));
	}

	TEST_FIXTURE(FFieldManager_Command, TestCmdPost_MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP)
	{
		/// Fixture 설치
		const int nTestServerID = 1;
		const int nTestCID = 1;
		const MUID uidCreator = 100;

		MUID uidFieldGrouop = AddFieldGroup(m_pFieldGroupInfo2->nID, nTestServerID, uidCreator);
		m_pLink->ResetCommands();

		/// SUT 실행
		gmgr.pFieldManager->DestroyDynamicFieldMaster(uidFieldGrouop, true);

		/// 검증
		uint32 nCheckSum = gmgr.pFieldManager->GetChecksumDynamicFieldMasters();
		CHECK_EQUAL(MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP, m_pLink->GetCommandID(0));
		CHECK_EQUAL(uidFieldGrouop, m_pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(nCheckSum, m_pLink->GetParam<unsigned int>(0, 1));
	}


	TEST_FIXTURE(FFieldManager_Command, TestCmdHandler_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = AddNewPlayerInWorld(m_nGameServerID);
		m_pLink->ResetCommands();

		TD_GATE_FIELD_GROUP_INFO tdGateFieldGroupInfo;
		tdGateFieldGroupInfo.nFieldGroupID = 1;
		tdGateFieldGroupInfo.nType = DFT_TRIAL;
		tdGateFieldGroupInfo.isSingleType = false;
		tdGateFieldGroupInfo.GateInfo.nFieldID = m_pFieldGroupInfo->pStartFieldInfo->m_nFieldID;

		/// SUT 실행
		MCommandResult nCommandResult = m_pLink->OnRecv(MMC_GATE_ASK_DYNAMIC_FIELD, 2, NEW_UID(pPlayer->GetUID()), NEW_SINGLE_BLOB(&tdGateFieldGroupInfo, sizeof(TD_GATE_FIELD_GROUP_INFO)));

		/// 검증
		CHECK_EQUAL(3, m_pLink->GetCommandCount());

		CHECK_EQUAL(MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP, m_pLink->GetCommand(0).GetID());
		{
			TDMG_DYNAMIC_FIELD_GROUP_INFO* pParam_FieldGroupInfo;
			pParam_FieldGroupInfo = (TDMG_DYNAMIC_FIELD_GROUP_INFO*)m_pLink->GetCommand(0).GetParameter(0)->GetPointer();
			{
				ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->GetFirstGameServer();
				CHECK_EQUAL(m_pFieldGroupInfo->nID, pParam_FieldGroupInfo->nFieldGroupID);
				CHECK_EQUAL(pGameServer->GetID(), pParam_FieldGroupInfo->nGameServerID);
				CHECK_EQUAL(DFT_TRIAL, pParam_FieldGroupInfo->nType);		
			}

			TDMG_DYNAMIC_FIELD_NODE* pParam_DynamicFieldNode;
			pParam_DynamicFieldNode	= (TDMG_DYNAMIC_FIELD_NODE*)m_pLink->GetCommand(0).GetParameter(1)->GetPointer();
			{
				int nNodeCount			= m_pLink->GetCommand(0).GetParameter(1)->GetBlobCount();

				CHECK_EQUAL(1, nNodeCount);
				CHECK_EQUAL(m_pFieldGroupInfo->vecFieldInfo[0]->m_nFieldID, pParam_DynamicFieldNode[0].nFieldID);
			}

			unsigned int nParam_Checksum;
			m_pLink->GetCommand(0).GetParameter(&nParam_Checksum, 2, MPT_UINT, sizeof(unsigned int));
			{
				CHECK_EQUAL(gmgr.pFieldManager->GetChecksumDynamicFieldMasters(), nParam_Checksum);
			}
		}
		CHECK_EQUAL(MMC_FIELD_UPDATE_PROXY_FIELD_INFO,	m_pLink->GetCommand(1).GetID());
		CHECK_EQUAL(MMC_GATE_RES,						m_pLink->GetCommand(2).GetID());
	}

	TEST_FIXTURE(FFieldManager_Command, TestCmdHandler_MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL)
	{
		/// Fixture 설치
		AddNewPlayerInWorld(m_nGameServerID);
		m_pLink->ResetCommands();

		const MUID uidCreator = 100;

		AddFieldGroup(m_pFieldGroupInfo2->nID, m_nGameServerID, uidCreator);
		AddFieldGroup(m_pFieldGroupInfo2->nID, m_nGameServerID, uidCreator);
		AddFieldGroup(m_pFieldGroupInfo2->nID, m_nGameServerID, uidCreator);

		/// SUT 실행
		MCommandResult nCommandResult = m_pLink->OnRecv(MMC_FIELD_REQUEST_DYNAMIC_FIELD_GROUP_INFO_ALL, 0, NULL);

		/// 검증
		CHECK_EQUAL(MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL, m_pLink->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FFieldManager_Command, TestCmdHandler_MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL_CheckParam)
	{
		/// Fixture 설치
		AddNewPlayerInWorld(m_nGameServerID);
		m_pLink->ResetCommands();

		const MUID uidCreator = 100;

		AddFieldGroup(m_pFieldGroupInfo2->nID, m_nGameServerID, uidCreator);
		AddFieldGroup(m_pFieldGroupInfo2->nID, m_nGameServerID, uidCreator);

		/// SUT 실행
		MCommandResult nCommandResult = m_pLink->OnRecv(MMC_FIELD_REQUEST_DYNAMIC_FIELD_GROUP_INFO_ALL, 0, NULL);

		/// 검증
		CHECK_EQUAL(MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL, m_pLink->GetCommand(0).GetID());

		ZFieldManager::ZDynamicFieldMasterMap::iterator itrFieldGroup = gmgr.pFieldManager->GetDynamicFieldMasterMap().begin();
		ZDynamicFieldMaster* pFieldGroup1 = itrFieldGroup->second;
		ZDynamicFieldMaster* pFieldGroup2 = (++itrFieldGroup)->second;

		{	///< 검증 : 첫번째 파라미터
			int nFieldGroupCount = m_pLink->GetCommand(0).GetParameter(0)->GetBlobCount();
			CHECK_EQUAL(2, nFieldGroupCount);

			TDMG_DYNAMIC_FIELD_GROUP_INFO* pParam_FieldGroupInfo;
			pParam_FieldGroupInfo = (TDMG_DYNAMIC_FIELD_GROUP_INFO*)m_pLink->GetCommand(0).GetParameter(0)->GetPointer();

			CHECK_EQUAL(pFieldGroup1->GetUID(),		pParam_FieldGroupInfo[0].uidDynamicFieldGroup);
			CHECK_EQUAL(m_pFieldGroupInfo2->nID,	pParam_FieldGroupInfo[0].nFieldGroupID);
			CHECK_EQUAL(m_nGameServerID,			pParam_FieldGroupInfo[0].nGameServerID);

			CHECK_EQUAL(pFieldGroup2->GetUID(),		pParam_FieldGroupInfo[1].uidDynamicFieldGroup);
			CHECK_EQUAL(m_pFieldGroupInfo2->nID,	pParam_FieldGroupInfo[1].nFieldGroupID);
			CHECK_EQUAL(m_nGameServerID,			pParam_FieldGroupInfo[1].nGameServerID);
		}

		{	///< 검증 : 두번째 파라미터
			int nNodeCount = m_pLink->GetCommand(0).GetParameter(1)->GetBlobCount();
			CHECK_EQUAL(4, nNodeCount);

			ZDynamicFieldMaster::VEC_DYNAMICFIELD::iterator itrField;
			itrField = pFieldGroup1->GetFieldVector().begin();
			ZDynamicField* pField1_1 = *itrField;
			ZDynamicField* pField1_2 = *(++itrField);

			itrField = pFieldGroup2->GetFieldVector().begin();
			ZDynamicField* pField2_1 = *itrField;
			ZDynamicField* pField2_2 = *(++itrField);

			TDMG_DYNAMIC_FIELD_NODE* pParam_DynamicFieldNode;
			pParam_DynamicFieldNode	= (TDMG_DYNAMIC_FIELD_NODE*)m_pLink->GetCommand(0).GetParameter(1)->GetPointer();

			CHECK_EQUAL(pField1_1->GetUID(), pParam_DynamicFieldNode[0].uidDynamicField);
			CHECK_EQUAL(pField1_1->GetFieldGroupUID(), pParam_DynamicFieldNode[0].uidFieldGroup);

			CHECK_EQUAL(pField1_2->GetUID(), pParam_DynamicFieldNode[1].uidDynamicField);
			CHECK_EQUAL(pField1_2->GetFieldGroupUID(), pParam_DynamicFieldNode[1].uidFieldGroup);

			CHECK_EQUAL(pField2_1->GetUID(), pParam_DynamicFieldNode[2].uidDynamicField);
			CHECK_EQUAL(pField2_1->GetFieldGroupUID(), pParam_DynamicFieldNode[2].uidFieldGroup);

			CHECK_EQUAL(pField2_2->GetUID(), pParam_DynamicFieldNode[3].uidDynamicField);
			CHECK_EQUAL(pField2_2->GetFieldGroupUID(), pParam_DynamicFieldNode[3].uidFieldGroup);
		}

		{	///< 검증 : 세번째 파라미터
			unsigned int nParam_Checksum;
			m_pLink->GetCommand(0).GetParameter(&nParam_Checksum, 2, MPT_UINT, sizeof(unsigned int));
			CHECK_EQUAL(gmgr.pFieldManager->GetChecksumDynamicFieldMasters(), nParam_Checksum);
		}
	}

}