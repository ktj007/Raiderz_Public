#include "stdafx.h"
#include "GFieldFactory.h"
#include "GMasterServerFacade.h"
#include "GFieldMgr.h"
#include "GFieldInfoMgr.h"
#include "GTestWrapper.h"
#include "GManagedFieldInfoMgr.h"
#include "GUTHelper_Field.h"
#include "GTestSystem.h"
#include "GField.h"

SUITE(FieldFactory)
{
	const int TEST_FIELD_ID_1 = 1;
	const int TEST_FIELD_ID_2 = 2;

	struct Fixture
	{
		Fixture()
		{
			m_pTestFieldFactory = new GFieldFactory(&m_FieldMgr);
			
			GFieldInfo* pFieldInfo = NULL;
			pFieldInfo = GUTHelper_Field::NewFieldInfo(TEST_FIELD_ID_1);
			pFieldInfo->m_nDefaultChannelCount = 1;

			pFieldInfo = GUTHelper_Field::NewFieldInfo(TEST_FIELD_ID_2);
			pFieldInfo->m_nDefaultChannelCount = 1;
		}

		~Fixture()
		{
			SAFE_DELETE(m_pTestFieldFactory);
		}

		GFieldMgr m_FieldMgr;
		GFieldFactory* m_pTestFieldFactory;

		GTestMgrWrapper<GFieldInfoMgr> m_FieldInfoMgrWrapper;
		GTestSysWrapper<GMasterServerFacade> m_MasterServerFacadeWrapper;
	};

	TEST_FIXTURE(Fixture, TestFieldFactory_CreateAllSharedField)
	{
		TDMG_MANAGED_SHARED_FIELD fIelds[2];
		int nFieldCount = 2;

		fIelds[0].nChannelID[0] = 1;
		fIelds[0].nChannelCount = 1;
		fIelds[0].nFieldID = TEST_FIELD_ID_1;
		fIelds[0].uidField[0] = MUID(2, 100);

		fIelds[1].nChannelID[0] = 1;
		fIelds[1].nChannelCount = 1;
		fIelds[1].nFieldID = TEST_FIELD_ID_2;
		fIelds[1].uidField[0] = MUID(2, 200);

		gsys.pMasterServerFacade->GetManagedFieldInfoMgr()->AssignSharedField(fIelds, nFieldCount);


		CHECK_EQUAL(0, m_FieldMgr.GetFieldQty());

		GTestSystem testSystem;
		m_pTestFieldFactory->CreateAllSharedField(gmgr.pFieldInfoMgr, &testSystem, NULL);

		CHECK_EQUAL(2, m_FieldMgr.GetFieldQty());

		vector<GField*> vecFIeld;
		vecFIeld = m_FieldMgr.GetFieldList(TEST_FIELD_ID_1);
		CHECK_EQUAL(MUID(2, 100), vecFIeld[0]->GetUID());

		vecFIeld = m_FieldMgr.GetFieldList(TEST_FIELD_ID_2);
		CHECK_EQUAL(MUID(2, 200), vecFIeld[0]->GetUID());
	}
}