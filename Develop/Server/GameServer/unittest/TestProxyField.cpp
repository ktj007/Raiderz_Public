#include "stdafx.h"
#include "GProxyField.h"
#include "GProxyFieldManager.h"
#include "GProxyDynamicFieldMaster.h"
#include "FBaseFieldInfo.h"
#include "FBaseNetClient.h"
#include "STransData_M2G.h"
#include "GCommandTable.h"
#include "GMasterServerFacade.h"
#include "GFieldMgr.h"

SUITE(ProxyField)
{
	const int DYNAMIC_FIELD_ID_1 = 101;
	const int DYNAMIC_FIELD_ID_2 = 102;

	struct FProxyField : public FBaseFieldInfo, public FBaseNetClient
	{
		FProxyField()
		{
			m_pFieldInfo = AddFieldInfo(1);

			AddFieldInfo(2);

			GFieldInfo* pDynamicFieldInfo = NULL;
			pDynamicFieldInfo = AddFieldInfo(DYNAMIC_FIELD_ID_1);
			pDynamicFieldInfo->m_bDynamic = true;

			pDynamicFieldInfo = AddFieldInfo(DYNAMIC_FIELD_ID_2);
			pDynamicFieldInfo->m_bDynamic = true;

			m_pFieldGroupInfo = AddFieldGroupInfo(1, 1);
			
		}
		virtual ~FProxyField()
		{

		}

		//
		GFieldInfo*					m_pFieldInfo;
		FIELD_GROUP_INFO*			m_pFieldGroupInfo;

		GProxyFieldManager			mgr;
		GTestMgrWrapper<GFieldMgr>	m_FieldMgrWrapper;
	};

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_ClearSharedFields)
	{
		const MUID uidField = MUID(5, 6);
		const int nGameServerID = 1;
		const int nChannelID = 1;

		GProxyField* pField = mgr.AddSharedField(uidField, m_pFieldInfo, nGameServerID, nChannelID);

		CHECK_EQUAL(1, mgr.GetFieldCount());
		CHECK(0 != mgr.CalcChecksumSharedFields());

		mgr.ClearProxySharedFields();

		CHECK_EQUAL(0, mgr.GetFieldCount());
		CHECK_EQUAL(0, mgr.CalcChecksumSharedFields());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_AddSharedFIeld)
	{
		const MUID uidField = MUID(5, 6);
		const int nGameServerID = 1;
		const int nChannelID = 1;

		CHECK_EQUAL(0, mgr.GetFieldCount());
		CHECK_EQUAL(0, mgr.CalcChecksumSharedFields());

		GProxyField* pField = mgr.AddSharedField(uidField, m_pFieldInfo, nGameServerID, nChannelID);

		CHECK_EQUAL(1, mgr.GetFieldCount());
		CHECK(0 != mgr.CalcChecksumSharedFields());
		CHECK_EQUAL(uidField, pField->GetUID());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_DeleteSharedFIeld)
	{
		const MUID uidField = MUID(5, 6);
		const int nGameServerID = 1;
		const int nChannelID = 1;

		GProxyField* pField = mgr.AddSharedField(uidField, m_pFieldInfo, nGameServerID, nChannelID);

		CHECK_EQUAL(1, mgr.GetFieldCount());

		mgr.DeleteSharedField(pField->GetUID());

		CHECK_EQUAL(0, mgr.GetFieldCount());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_Find)
	{
		const MUID uidField = MUID(5, 6);
		const int nGameServerID = 1;
		const int nChannelID = 1;

		GProxyField* pField = mgr.AddSharedField(uidField, m_pFieldInfo, nGameServerID, nChannelID);

		/// SUT 실행
		GProxyField* pFindedField = mgr.Find(uidField);

		/// 검증
		CHECK_EQUAL(pField->GetUID(), pFindedField->GetUID());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_SharedFieldsChecksum)
	{
		MUID uidField1 = MUID(1, 15);
		MUID uidField2 = MUID(9, 50);
		const int nGameServerID = 1;
		const int nChannelID1 = 1;
		const int nChannelID2 = 2;

		CHECK_EQUAL(0, mgr.CalcChecksumSharedFields());

		mgr.AddSharedField(uidField1, m_pFieldInfo, nGameServerID, nChannelID1);

		// 각 UID별 high + (Low * 2)
		CHECK_EQUAL((1+15*2), mgr.CalcChecksumSharedFields());

		mgr.AddSharedField(uidField2, m_pFieldInfo, nGameServerID, nChannelID2);

		CHECK_EQUAL((1+15*2+9+50*2), mgr.CalcChecksumSharedFields());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_AddFieldGroup)
	{
		const MUID uidFieldGroup = MUID(5, 6);
		const int nGameServerID = 1;
		const DYNAMIC_FIELD_TYPE eDynamicFieldType = DFT_TRIAL;

		GProxyDynamicFieldMaster* pFieldGroup = mgr.AddFieldGroup(uidFieldGroup, m_pFieldGroupInfo, nGameServerID, eDynamicFieldType);

		CHECK_EQUAL(1, mgr.GetFieldGroupCount());
		CHECK_EQUAL(uidFieldGroup, pFieldGroup->GetUID());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_DeleteFieldGroup)
	{
		const MUID uidFieldGroup = MUID(5, 6);
		const int nGameServerID = 1;
		const DYNAMIC_FIELD_TYPE eDynamicFieldType = DFT_TRIAL;

		GProxyDynamicFieldMaster* pFieldGroup = mgr.AddFieldGroup(uidFieldGroup, m_pFieldGroupInfo, nGameServerID, eDynamicFieldType);

		CHECK_EQUAL(1, mgr.GetFieldGroupCount());

		mgr.DeleteFieldGroup(pFieldGroup->GetUID());

		CHECK_EQUAL(0, mgr.GetFieldGroupCount());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_ClearFieldGroup)
	{
		const MUID uidFieldGroup = MUID(5, 6);
		const int nGameServerID = 1;
		const DYNAMIC_FIELD_TYPE eDynamicFieldType = DFT_TRIAL;

		GProxyDynamicFieldMaster* pFieldGroup = mgr.AddFieldGroup(uidFieldGroup, m_pFieldGroupInfo, nGameServerID, eDynamicFieldType);
		
		MUID uidDynamicField1 = MUID(5, 11);
		GFieldInfo* pDynamicFieldInfo1 = gmgr.pFieldInfoMgr->Find(DYNAMIC_FIELD_ID_1);
		mgr.AddDynamicField(uidDynamicField1, pDynamicFieldInfo1, uidFieldGroup);

		MUID uidDynamicField2 = MUID(5, 12);
		GFieldInfo* pDynamicFieldInfo2 = gmgr.pFieldInfoMgr->Find(DYNAMIC_FIELD_ID_2);
		mgr.AddDynamicField(uidDynamicField2, pDynamicFieldInfo2, uidFieldGroup);

		CHECK_EQUAL(2, mgr.GetFieldCount());
		CHECK_EQUAL(1, mgr.GetFieldGroupCount());

		mgr.ClearProxyFieldGroups();

		CHECK_EQUAL(0, mgr.GetFieldCount());
		CHECK_EQUAL(0, mgr.GetFieldGroupCount());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_FindFieldGroup)
	{
		const MUID uidFieldGroup = MUID(5, 6);
		const int nGameServerID = 1;
		const DYNAMIC_FIELD_TYPE eDynamicFieldType = DFT_TRIAL;

		GProxyDynamicFieldMaster* pFieldGroup = mgr.AddFieldGroup(uidFieldGroup, m_pFieldGroupInfo, nGameServerID, eDynamicFieldType);

		CHECK_EQUAL(1, mgr.GetFieldGroupCount());

		GProxyDynamicFieldMaster* pFindedpFieldGroup = mgr.FindFieldGroup(uidFieldGroup);

		CHECK_EQUAL(pFieldGroup->GetUID(), pFindedpFieldGroup->GetUID());
	}

	TEST_FIXTURE(FProxyField, TestProxyFieldManager_ChecksumFieldGroup)
	{
		MUID uidFieldGroup1 = MUID(1, 15);
		MUID uidFieldGroup2 = MUID(9, 50);
		const int nGameServerID = 1;
		const DYNAMIC_FIELD_TYPE eDynamicFieldType = DFT_TRIAL;

		CHECK_EQUAL(0, mgr.CalcChecksumFieldGroups());

		mgr.AddFieldGroup(uidFieldGroup1, m_pFieldGroupInfo, nGameServerID, eDynamicFieldType);

		// 각 UID별 high + (Low * 2)
		CHECK_EQUAL((1+15*2), mgr.CalcChecksumFieldGroups());

		mgr.AddFieldGroup(uidFieldGroup2, m_pFieldGroupInfo, nGameServerID, eDynamicFieldType);

		CHECK_EQUAL((1+15*2+9+50*2), mgr.CalcChecksumFieldGroups());
	}


	//////////////////////////////////////////////////////////////////////////
	struct FProxyField_OnCommand : public FProxyField
	{
		FProxyField_OnCommand() 
		{
			m_uidField = MUID(1, 15);
			m_uidFieldGroup = MUID(1, 50);
		}
		virtual ~FProxyField_OnCommand() {}
		void OnRecv_MMC_FIELD_ADD_SHARED_FIELD(uint32 nChecksum)
		{
			MUID uidField = m_uidField;

			TDMG_SHARED_FIELD_INFO paramBlob1;
			paramBlob1.nGameServerID = 1;
			paramBlob1.uidField = uidField;
			paramBlob1.nFieldID = 1;
			paramBlob1.nChannelID = 0;

			m_pNetAgent->OnRecv(MMC_FIELD_ADD_SHARED_FIELD, 2, NEW_BLOB(&paramBlob1, sizeof(TDMG_SHARED_FIELD_INFO), 1), NEW_UINT(nChecksum) );
		}
		void OnRecv_MMC_FIELD_DEL_SHARED_FIELD(MUID uidField, uint32 nChecksum)
		{
			m_pNetAgent->OnRecv(MMC_FIELD_DEL_SHARED_FIELD, 2, NEW_UID(uidField), NEW_UINT(nChecksum) );
		}
		void OnRecv_MMC_FIELD_SHARED_FIELD_INFO_ALL()
		{
			const int BLOB_COUNT = 3;

			TDMG_SHARED_FIELD_INFO* paramBlob = new TDMG_SHARED_FIELD_INFO[BLOB_COUNT];

			paramBlob[0].nGameServerID = 1;
			paramBlob[0].uidField = MUID(1, 1);
			paramBlob[0].nFieldID = 1;
			paramBlob[0].nChannelID = 1;

			paramBlob[1].nGameServerID = 1;
			paramBlob[1].uidField = MUID(1, 2);
			paramBlob[1].nFieldID = 1;
			paramBlob[1].nChannelID = 2;

			paramBlob[2].nGameServerID = 1;
			paramBlob[2].uidField = MUID(1, 3);
			paramBlob[2].nFieldID = 2;
			paramBlob[2].nChannelID = 1;

			vector<MUID> vecUID;
			for (int i = 0; i < BLOB_COUNT; i++)
				vecUID.push_back(paramBlob[i].uidField);

			m_pNetAgent->OnRecv(MMC_FIELD_SHARED_FIELD_INFO_ALL, 2, NEW_BLOB(paramBlob, sizeof(TDMG_SHARED_FIELD_INFO), BLOB_COUNT), NEW_UINT(CalcChecksum(vecUID)) );

			delete [] paramBlob;
		}
		void OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(MUID uidCreator, uint32 nChecksum)
		{
			MUID uidFieldGroup = m_uidFieldGroup;

			TDMG_DYNAMIC_FIELD_GROUP_INFO paramBlob1;
			paramBlob1.uidDynamicFieldGroup = uidFieldGroup;
			paramBlob1.uidCreator = uidCreator;
			paramBlob1.nType = DFT_TRIAL;
			paramBlob1.nFieldGroupID = 1;
			paramBlob1.nGameServerID = 1;

			TDMG_DYNAMIC_FIELD_NODE paramBlob2[2];
			paramBlob2[0].uidFieldGroup = uidFieldGroup;
			paramBlob2[0].uidDynamicField = 1;
			paramBlob2[0].nFieldID = DYNAMIC_FIELD_ID_1;

			paramBlob2[1].uidFieldGroup = uidFieldGroup;
			paramBlob2[1].uidDynamicField = 2;
			paramBlob2[1].nFieldID = DYNAMIC_FIELD_ID_2;

			std::vector<TDMG_DYNAMIC_FIELD_NODE > vecDynamicFieldNode;
			vecDynamicFieldNode.push_back(paramBlob2[0]);
			vecDynamicFieldNode.push_back(paramBlob2[1]);

			m_pNetAgent->OnRecv(MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP, 3, NEW_SINGLE_BLOB(&paramBlob1, sizeof(TDMG_DYNAMIC_FIELD_GROUP_INFO)), NEW_BLOB(vecDynamicFieldNode), NEW_UINT(nChecksum));
		}
		void OnRecv_MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP(MUID uidFieldGroup, uint32 nChecksum)
		{
			m_pNetAgent->OnRecv(MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP, 2, NEW_UID(uidFieldGroup), NEW_UINT(nChecksum));
		}
		void OnRecv_MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL(vector<TDMG_DYNAMIC_FIELD_GROUP_INFO>& vecFieldGroupList, vector<TDMG_DYNAMIC_FIELD_NODE>& vecDynamicFieldList, uint32 nChecksum)
		{
			m_pNetAgent->OnRecv(MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL, 3, NEW_BLOB(vecFieldGroupList), NEW_BLOB(vecDynamicFieldList), NEW_UINT(nChecksum));
		}
		void OnRecv_MMC_FIELD_UPDATE_PROXY_FIELD_INFO(MUID uidField, int nPlayerCount)
		{
			TDMG_PROXY_FIELD_UPDATE_INFO tTDProxyFieldUpdateInfo;
			tTDProxyFieldUpdateInfo.nPlayerCount = nPlayerCount;

			m_pNetAgent->OnRecv(MMC_FIELD_UPDATE_PROXY_FIELD_INFO, 2, NEW_UID(uidField), NEW_SINGLE_BLOB(&tTDProxyFieldUpdateInfo, sizeof(TDMG_PROXY_FIELD_UPDATE_INFO)));
		}


		uint32 CalcChecksum(vector<MUID> vecUID)
		{
			SFieldChecksum fieldChecksum;

			for (size_t i = 0; i < vecUID.size(); i++)
			{
				fieldChecksum.Add(vecUID[i]);
			}
			return fieldChecksum.GetValue();
		}

		MUID m_uidField;
		MUID m_uidFieldGroup;
	};

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_ADD_SHARED_FIELD)
	{
		vector<MUID> vecUID; vecUID.push_back(m_uidField);
		uint32 nChecksum = CalcChecksum(vecUID);

		OnRecv_MMC_FIELD_ADD_SHARED_FIELD(nChecksum);

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(1, pProxyFieldManager->GetFieldCount());

		GProxyField* pProxyField = pProxyFieldManager->Find(m_uidField);
		CHECK_EQUAL(m_uidField, pProxyField->GetUID());
		CHECK_EQUAL(false, pProxyField->IsDynamic());
		CHECK_EQUAL(1, pProxyField->GetFieldInfo()->m_nFieldID);
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_ADD_SHARED_FIELD_With_WrongChecksum)
	{
		uint32 nChecksum = 14145;

		OnRecv_MMC_FIELD_ADD_SHARED_FIELD(nChecksum);

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(1, pProxyFieldManager->GetFieldCount());

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL, m_pNetAgent->GetSendCommand(0).GetID());
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_DEL_SHARED_FIELD)
	{
		vector<MUID> vecUID; vecUID.push_back(m_uidField);
		uint32 nChecksum = CalcChecksum(vecUID);

		OnRecv_MMC_FIELD_ADD_SHARED_FIELD(nChecksum);

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(1, pProxyFieldManager->GetFieldCount());

		// del

		OnRecv_MMC_FIELD_DEL_SHARED_FIELD(m_uidField, 0);
		CHECK_EQUAL(0, pProxyFieldManager->GetFieldCount());
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_DEL_SHARED_FIELD_With_WrongChecksum)
	{
		vector<MUID> vecUID; vecUID.push_back(m_uidField);
		uint32 nChecksum = CalcChecksum(vecUID);

		OnRecv_MMC_FIELD_ADD_SHARED_FIELD(nChecksum);

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(1, pProxyFieldManager->GetFieldCount());

		// del
		uint32 nWrongChecksum = 14145;

		OnRecv_MMC_FIELD_DEL_SHARED_FIELD(m_uidField, nWrongChecksum);
		CHECK_EQUAL(0, pProxyFieldManager->GetFieldCount());

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL, m_pNetAgent->GetSendCommand(0).GetID());
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_SHARED_FIELD_INFO_ALL)
	{
		OnRecv_MMC_FIELD_SHARED_FIELD_INFO_ALL();

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(3, pProxyFieldManager->GetFieldCount());

	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP)
	{
		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
		MUID uidFieldGroupCreator = 100;

		vector<MUID> vecUID; vecUID.push_back(m_uidFieldGroup);
		uint32 nChecksum = CalcChecksum(vecUID);

		CHECK_EQUAL(0, pProxyFieldManager->CalcChecksumFieldGroups());
		CHECK_EQUAL(0, pProxyFieldManager->CalcChecksumSharedFields());

		OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(uidFieldGroupCreator, nChecksum);

		CHECK_EQUAL(nChecksum, pProxyFieldManager->CalcChecksumFieldGroups());
		CHECK_EQUAL(0, pProxyFieldManager->CalcChecksumSharedFields());

		CHECK_EQUAL(1, pProxyFieldManager->GetFieldGroupCount());

		GProxyDynamicFieldMaster* pProxyFieldGroup = pProxyFieldManager->FindFieldGroup(m_uidFieldGroup);
		CHECK_EQUAL(m_uidFieldGroup, pProxyFieldGroup->GetUID());
		CHECK_EQUAL(1, pProxyFieldGroup->GetInfo()->nID);
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP_With_WrongChecksum)
	{
		MUID uidFieldGroupCreator = 100;
		uint32 nChecksum = 14145;

		OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(uidFieldGroupCreator, nChecksum);

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(1, pProxyFieldManager->GetFieldGroupCount());

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_FIELD_REQUEST_DYNAMIC_FIELD_GROUP_INFO_ALL, m_pNetAgent->GetSendCommand(0).GetID());
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP)
	{
		MUID uidFieldGroupCreator = 100;
		vector<MUID> vecUID; vecUID.push_back(m_uidFieldGroup);
		uint32 nChecksum = CalcChecksum(vecUID);

		OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(uidFieldGroupCreator, nChecksum);

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(1, pProxyFieldManager->GetFieldGroupCount());

		// del

		OnRecv_MMC_FIELD_DEL_DYNAMIC_FIELD_GROUP(m_uidFieldGroup, nChecksum);
		CHECK_EQUAL(0, pProxyFieldManager->GetFieldGroupCount());
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL)
	{
		AddFieldGroupInfo(1, 1);
		AddFieldGroupInfo(2, 2);

		vector<TDMG_DYNAMIC_FIELD_GROUP_INFO> vecFieldGroupList;
		TDMG_DYNAMIC_FIELD_GROUP_INFO tdFieldGroup[2];
		tdFieldGroup[0].uidDynamicFieldGroup = 1;
		tdFieldGroup[0].nFieldGroupID = 1;
		tdFieldGroup[0].nGameServerID = 1;

		tdFieldGroup[1].uidDynamicFieldGroup = 2;
		tdFieldGroup[1].nFieldGroupID = 2;
		tdFieldGroup[1].nGameServerID = 2;

		vecFieldGroupList.push_back(tdFieldGroup[0]);
		vecFieldGroupList.push_back(tdFieldGroup[1]);

		vector<TDMG_DYNAMIC_FIELD_NODE> vecDynamicFieldList;
		TDMG_DYNAMIC_FIELD_NODE tdField[2];
		tdField[0].uidFieldGroup = 1;
		tdField[0].uidDynamicField = 1;
		tdField[0].nFieldID = 1;

		tdField[0].uidFieldGroup = 2;
		tdField[1].uidDynamicField = 2;
		tdField[1].nFieldID = 2;

		vecDynamicFieldList.push_back(tdField[0]);
		vecDynamicFieldList.push_back(tdField[1]);


		vector<MUID> vecUID;
		vecUID.push_back(vecFieldGroupList[0].uidDynamicFieldGroup);
		vecUID.push_back(vecFieldGroupList[1].uidDynamicFieldGroup);
		uint32 nChecksum = CalcChecksum(vecUID);


		OnRecv_MMC_FIELD_DYNAMIC_FIELD_GROUP_INFO_ALL(vecFieldGroupList, vecDynamicFieldList, nChecksum);

		GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();

		CHECK_EQUAL(2, pProxyFieldManager->GetFieldGroupCount());
		CHECK_EQUAL(1, pProxyFieldManager->FindFieldGroup(1)->GetInfo()->nID);
		CHECK_EQUAL(2, pProxyFieldManager->FindFieldGroup(2)->GetInfo()->nID);
		CHECK_EQUAL(nChecksum, pProxyFieldManager->CalcChecksumFieldGroups());
	}

	TEST_FIXTURE(FProxyField_OnCommand, TestCmdHandler_OnRecv_MMC_FIELD_UPDATE_PROXY_FIELD_INFO)
	{
		GTestSysWrapper<GMasterServerFacade> wrapperMasterServerFacade;

		const MUID uidField = MUID(5, 6);
		const int nGameServerID = 1;
		const int nChannelID = 1;

		CHECK_EQUAL(0, gsys.pMasterServerFacade->GetProxyFieldManager()->GetFieldCount());

		GProxyField* pField = gsys.pMasterServerFacade->GetProxyFieldManager()->AddSharedField(uidField, m_pFieldInfo, nGameServerID, nChannelID);

		CHECK_EQUAL(1, gsys.pMasterServerFacade->GetProxyFieldManager()->GetFieldCount());
		CHECK_EQUAL(0, pField->GetPlayerCount());

		OnRecv_MMC_FIELD_UPDATE_PROXY_FIELD_INFO(uidField, 1);

		CHECK_EQUAL(1, pField->GetPlayerCount());

		OnRecv_MMC_FIELD_UPDATE_PROXY_FIELD_INFO(uidField, 5);

		CHECK_EQUAL(5, pField->GetPlayerCount());
	}

}