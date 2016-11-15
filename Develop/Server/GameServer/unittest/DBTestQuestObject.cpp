#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestHelper_Quest.h"
#include "GConfig.h"


SUITE(DBTestQuestObject)
{
	class FIXTURE
	{
	public :
		wstring MakeProcName()
		{
			wchar_t szProc[1024] = {0,};
			swprintf_s(szProc, L"dbo.USP_RZ_QUEST_UPDATE_OBJECT");
			return szProc;
		}

		wstring MakeObjColumnName(const int nQuestObjectID)
		{
			wchar_t szColumnName[1024] = {0,};
			swprintf_s(szColumnName, L"OBJ%d_PROG", nQuestObjectID);
			return szColumnName;
		}

		void USP_RZ_QUEST_UPDATE_OBJECT(DBT_QUEST_CHAR& qc, const int nQuestObjectID, const int16 nProgress, const bool bComplete)
		{
			CHECK(UTestDB.Execute(L"{CALL %s (%d, %I64d, %I64d, %d, %d, %d, %d, %d)}"
				, MakeProcName().c_str()
				, GConfig::m_nMyWorldID
				, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, nQuestObjectID, nProgress, bComplete));
		}

		void CHECK_NOT_COMPLETED_QUEST_OBJECT(DBT_QUEST_CHAR& qc, const int nQuestObjectID, const int16 nProgress)			
		{
			TDBRecordSet rsQuest;
			CHECK(UTestDB.Execute(rsQuest, L"SELECT [STATE], %s FROM dbo.RZ_QUEST WHERE CHAR_ID = %I64d AND SLOT_ID = %d AND QUEST_ID = %d;"
				, MakeObjColumnName(nQuestObjectID).c_str()
				, qc.nCID, qc.nSlotID, qc.nQuestID));
			CHECK_EQUAL(QST_EXIST, rsQuest.Field(L"STATE").AsByte());
			CHECK_EQUAL(nProgress, rsQuest.Field(MakeObjColumnName(nQuestObjectID)).AsShort());
			rsQuest.Close();			
		}

		void CHECK_COMPLETED_QUEST_OBJECT(DBT_QUEST_CHAR& qc, const int nQuestObjectID, const int16 nProgress)
		{
			TDBRecordSet rsQuest;
			CHECK(UTestDB.Execute(rsQuest, L"SELECT [STATE], %s FROM dbo.RZ_QUEST WHERE CHAR_ID = %I64d AND SLOT_ID = %d AND QUEST_ID = %d;"
				, MakeObjColumnName(nQuestObjectID).c_str()
				, qc.nCID, qc.nSlotID, qc.nQuestID));
			CHECK_EQUAL(QST_COMPLETE, rsQuest.Field(L"STATE").AsByte());
			CHECK_EQUAL(nProgress, rsQuest.Field(MakeObjColumnName(nQuestObjectID)).AsShort());
			rsQuest.Close();
		}
	};

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT1_ONLY_UPDATE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 1;
		const int16		nProgress	= 1;
		
		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, false);

		CHECK_NOT_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT1_ONLY_UPDATE_AND_COMPLETE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 1;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, true);

		CHECK_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);				
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT2_ONLY_UPDATE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 2;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, false);

		CHECK_NOT_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);			
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT2_ONLY_UPDATE_AND_COMPLETE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 2;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, true);

		CHECK_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);				
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT3_ONLY_UPDATE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 3;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, false);

		CHECK_NOT_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);			
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT3_ONLY_UPDATE_AND_COMPLETE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 3;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, true);

		CHECK_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);				
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT4_ONLY_UPDATE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 4;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, false);

		CHECK_NOT_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);				
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT4_ONLY_UPDATE_AND_COMPLETE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 4;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, true);

		CHECK_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);				
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT5_ONLY_UPDATE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 5;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, false);

		CHECK_NOT_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);				
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_QUEST_UPDATE_OBJECT5_ONLY_UPDATE_AND_COMPLETE)
	{
		DBT_QUEST_CHAR	qc			= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nQuestObjID	= 5;
		const int16		nProgress	= 1;

		USP_RZ_QUEST_UPDATE_OBJECT(qc, nQuestObjID, nProgress, true);

		CHECK_COMPLETED_QUEST_OBJECT(qc, nQuestObjID, nProgress);				
	}
}
