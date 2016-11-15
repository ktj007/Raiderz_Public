#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestHelper_Quest.h"
#include "GConfig.h"


SUITE(DBTestQuestAccept)
{
	const static uint8 NOMAL_TYPE		= 1;
	const static uint8 CHALLENT_TYPE	= 2;

	void usp_rz_quest_accept_not_challenge_and_not_completed(bool bCreeateEmptySlot)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter();

		if (bCreeateEmptySlot)
			DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_ACCEPT (%d, %I64d, %I64d, %d, %d, %d, %d, '%s', '%s'\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , '%s', %d)}"
							   , GConfig::m_nMyWorldID
							   , qc.nAID, qc.nCID, qc.nCharPtm, qc.nSlotID, qc.nQuestID, false, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , L"", 0));

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT [STATE], CASE EXPI_DATE WHEN '9999-12-31 04:30' THEN 0 ELSE 1 END IsChallenge \
										FROM dbo.RZ_QUEST \
										WHERE CHAR_ID = %I64d AND QUEST_ID = %d AND ACCP_DATE = '%s' AND EXPI_DATE = '%s'"
										, qc.nCID, qc.nQuestID, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		CHECK_EQUAL(QST_EXIST, rsQuest.Field(L"STATE").AsByte());
		CHECK_EQUAL(false, rsQuest.Field(L"IsChallenge").AsBool());
		rsQuest.Close();		
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_NOT_CHALLENGE_AND_NOT_COMPLETED)
	{
		usp_rz_quest_accept_not_challenge_and_not_completed(true);
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_NOT_CHALLENGE_AND_NOT_COMPLETED_CREATE)
	{
		usp_rz_quest_accept_not_challenge_and_not_completed(false);
	}

	void usp_rz_quest_accept_challenge_and_not_completed(bool bCreateEmptySlot)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter(true);

		if (bCreateEmptySlot)
			DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_ACCEPT (%d, %I64d, %I64d, %d, %d, %d, %d, '%s', '%s'\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , '%s', %d)}"
							   , GConfig::m_nMyWorldID
							   , qc.nAID, qc.nCID, qc.nCharPtm, qc.nSlotID, qc.nQuestID, false, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , L"", 0));

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT [STATE], CASE EXPI_DATE WHEN '9999-12-31 04:30' THEN 0 ELSE 1 END IsChallenge \
										FROM dbo.RZ_QUEST \
										WHERE CHAR_ID = %I64d AND QUEST_ID = %d AND ACCP_DATE = '%s' AND EXPI_DATE = '%s'"
										, qc.nCID, qc.nQuestID, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		CHECK_EQUAL(QST_EXIST, rsQuest.Field(L"STATE").AsByte());
		CHECK_EQUAL(true, rsQuest.Field(L"IsChallenge").AsBool());
		rsQuest.Close();		
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_CHALLENGE_AND_NOT_COMPLETED)
	{
		usp_rz_quest_accept_challenge_and_not_completed(true);
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_CHALLENGE_AND_NOT_COMPLETED_CREATE)
	{
		usp_rz_quest_accept_challenge_and_not_completed(false);
	}

	void  usp_rz_quest_accept_not_challenge_and_completed(bool bCreateEmptySlot)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter();

		if (bCreateEmptySlot)
			DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_ACCEPT (%d, %I64d, %I64d, %d, %d, %d, %d, '%s', '%s'\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , '%s', %d)}"
							   , GConfig::m_nMyWorldID
							   , qc.nAID, qc.nCID, qc.nCharPtm, qc.nSlotID, qc.nQuestID, true, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , L"", 0));

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT [STATE], CASE EXPI_DATE WHEN '9999-12-31 04:30' THEN 0 ELSE 1 END IsChallenge \
										FROM dbo.RZ_QUEST \
										WHERE CHAR_ID = %I64d AND QUEST_ID = %d AND ACCP_DATE = '%s' AND EXPI_DATE = '%s'"
										, qc.nCID, qc.nQuestID, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		CHECK_EQUAL(QST_COMPLETE, rsQuest.Field(L"STATE").AsByte());
		CHECK_EQUAL(false, rsQuest.Field(L"IsChallenge").AsBool());
		rsQuest.Close();		
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_NOT_CHALLENGE_AND_COMPLETED)
	{
		usp_rz_quest_accept_not_challenge_and_completed(true);
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_NOT_CHALLENGE_AND_COMPLETED_CREATE)
	{
		usp_rz_quest_accept_not_challenge_and_completed(false);
	}

	void usp_rz_quest_accept_challenge_and_completed(bool bCreateEmptySlot)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter(true);

		if (bCreateEmptySlot)
			DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_ACCEPT (%d, %I64d, %I64d, %d, %d, %d, %d, '%s', '%s'\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , '%s', %d)}"
							   , GConfig::m_nMyWorldID
							   , qc.nAID, qc.nCID, qc.nCharPtm, qc.nSlotID, qc.nQuestID, true, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , L"", 0));

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT [STATE], CASE EXPI_DATE WHEN '9999-12-31 04:30' THEN 0 ELSE 1 END IsChallenge \
										FROM dbo.RZ_QUEST \
										WHERE CHAR_ID = %I64d AND QUEST_ID = %d AND ACCP_DATE = '%s' AND EXPI_DATE = '%s'"
										, qc.nCID, qc.nQuestID, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		CHECK_EQUAL(QST_COMPLETE, rsQuest.Field(L"STATE").AsByte());
		CHECK_EQUAL(true, rsQuest.Field(L"IsChallenge").AsBool());
		rsQuest.Close();
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_CHALLENGE_AND_COMPLETED)
	{
		usp_rz_quest_accept_challenge_and_completed(true);	
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_CHALLENGE_AND_COMPLETED_CREATE)
	{
		usp_rz_quest_accept_challenge_and_completed(false);	
	}

	class IN_QITEM
	{
	public :
		IN_QITEM() {}

		void InsertEmptySlot()
		{
			DBTestHelper::InsertEmptyItemSlot(m_GSN, m_CSN, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, m_SLOT_ID);
			UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ACCN_ID = %I64d WHERE OWNER_ID = %I64d AND SLOT_ID = %d;"
				, m_GSN, m_CSN, m_SLOT_ID);
		}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT ITEM_ID, STACK_AMT, MAX_DURA, DURA, CLAIMED, PERIOD, EFF_END_DATE\
								 FROM dbo.RZ_INVEN\
								 WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d AND IUID = %I64d;"
								 , m_GSN, m_CSN, m_SLOT_ID, m_NEW_IUID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(m_ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(m_STACK_AMT, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_MAX_DURA, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_MAX_DURA, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_CLAIMED, rs.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_PERIOD, rs.Field(L"PERIOD").AsBool());
			CHECK(rs.Field(L"EFF_END_DATE").IsNull());
			rs.Close();
		}

	public :
		int64	m_GSN;
		int64	m_CSN;
		int64	m_IUID;
		int		m_SLOT_ID;
		int		m_ITEM_ID;
		int		m_STACK_AMT;
		uint8	m_MAX_DURA;
		bool	m_CLAIMED;
		bool	m_PERIOD;
		wstring m_EFF_END_DATE;
		IUID	m_NEW_IUID;
	};

	void CHECK_ACCEPT_WITH_QUEST_ITEM(bool bExistEmptySlot)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter(true);

		if (bExistEmptySlot)
			DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID);
		/*const int64 IUID1 =  DBTestHelper::InsertItem(qc.nCID, 1, 1, SLOTTYPE_INVENTORY, 50);
		const int64 IUID2 =  DBTestHelper::InsertItem(qc.nCID, 2, 10, SLOTTYPE_INVENTORY, 51);
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ACCN_ID = %I64d;", qc.nAID);*/

		const wstring QITEM_LIST(L"0100000001001005");

		IN_QITEM qitem[QBEGIN_ITEM_COUNT];
		for (int i = 0; i < QBEGIN_ITEM_COUNT; ++i)
		{
			qitem[i].m_GSN			= qc.nAID;
			qitem[i].m_CSN			= qc.nCID;
			qitem[i].m_IUID			= 0;
			qitem[i].m_SLOT_ID		= 10 + i;
			qitem[i].m_ITEM_ID		= i + 2;
			qitem[i].m_STACK_AMT	= i + 3;
			qitem[i].m_MAX_DURA		= i + 4;
			qitem[i].m_CLAIMED		= 1 == (i % 2);
			qitem[i].m_PERIOD		= 0 == (i % 2);
			qitem[i].m_EFF_END_DATE = L"NULL";
			qitem[i].m_NEW_IUID		= 0;

			if (0 < i && bExistEmptySlot)
				qitem[i].InsertEmptySlot();
		}		

		qitem[0].m_IUID = DBTestHelper::InsertItem(qitem[0].m_GSN, qitem[0].m_CSN, GConfig::m_nMyWorldID, qitem[0].m_ITEM_ID, 1, 1, qitem[0].m_SLOT_ID, false, 1, -1);
		 UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = %d, MAX_DURA = %d WHERE IUID = %I64d;", qitem[0].m_MAX_DURA, qitem[0].m_MAX_DURA, qitem[0].m_IUID);
		qitem[0].m_NEW_IUID = qitem[0].m_IUID;

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_QUEST_ACCEPT (%d, %I64d, %I64d, %d, %d, %d, %d, '%s', '%s'\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , '%s', %d)}"
							   , GConfig::m_nMyWorldID
							   , qc.nAID, qc.nCID, qc.nCharPtm, qc.nSlotID, qc.nQuestID, true, qc.strAcceptDt.c_str(), qc.strExpiDt.c_str()
							   , qitem[0].m_SLOT_ID, qitem[0].m_IUID, qitem[0].m_ITEM_ID, qitem[0].m_STACK_AMT, qitem[0].m_MAX_DURA, qitem[0].m_CLAIMED, qitem[0].m_PERIOD , qitem[0].m_EFF_END_DATE.c_str()
							   , qitem[1].m_SLOT_ID, qitem[1].m_IUID, qitem[1].m_ITEM_ID, qitem[1].m_STACK_AMT, qitem[1].m_MAX_DURA, qitem[1].m_CLAIMED, qitem[1].m_PERIOD , qitem[1].m_EFF_END_DATE.c_str()
							   , qitem[2].m_SLOT_ID, qitem[2].m_IUID, qitem[2].m_ITEM_ID, qitem[2].m_STACK_AMT, qitem[2].m_MAX_DURA, qitem[2].m_CLAIMED, qitem[2].m_PERIOD , qitem[2].m_EFF_END_DATE.c_str()
							   , qitem[3].m_SLOT_ID, qitem[3].m_IUID, qitem[3].m_ITEM_ID, qitem[3].m_STACK_AMT, qitem[3].m_MAX_DURA, qitem[3].m_CLAIMED, qitem[3].m_PERIOD , qitem[3].m_EFF_END_DATE.c_str()
							   , qitem[4].m_SLOT_ID, qitem[4].m_IUID, qitem[4].m_ITEM_ID, qitem[4].m_STACK_AMT, qitem[4].m_MAX_DURA, qitem[4].m_CLAIMED, qitem[4].m_PERIOD , qitem[4].m_EFF_END_DATE.c_str()
							   , QITEM_LIST.c_str(), 7));

		for (; !rs.IsEOF(); rs.MoveNext())
		{
			CHECK((int64)0 < rs.Field(L"NEW_IUID").AsInt64());
			for (int i = 0; i < QBEGIN_ITEM_COUNT; ++i)
			{
				if (qitem[i].m_SLOT_ID == rs.Field(L"SLOT_ID").AsInt())
				{
					qitem[i].m_NEW_IUID = rs.Field(L"NEW_IUID").AsInt64();
					break;
				}
			}
		}
		rs.Close();

		for (int i = 0; i < QBEGIN_ITEM_COUNT; ++i)
			qitem[i].Check();
		
		TDBRecordSet rs1;
		UTestDB.Execute(rs1, L"SELECT ITEM_ID, IUID, STACK_AMT FROM dbo.RZ_INVEN WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND SLOT_ID = %d;"
			, qc.nAID, qc.nCID, qc.vObjItemInst[0].nSlotID);
		CHECK_EQUAL(1, rs1.GetFetchedRowCount());
		CHECK(rs1.Field(L"ITEM_ID").IsNull());
		CHECK_EQUAL(0, rs1.Field(L"IUID").AsInt64());
		CHECK_EQUAL(0, rs1.Field(L"STACK_AMT").AsShort());
		rs1.Close();

		TDBRecordSet rs2;
		UTestDB.Execute(rs2, L"SELECT ITEM_ID, IUID, STACK_AMT FROM dbo.RZ_INVEN WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND SLOT_ID = %d;"
			, qc.nAID, qc.nCID, qc.vObjItemInst[1].nSlotID);
		CHECK_EQUAL(1, rs2.GetFetchedRowCount());
		CHECK_EQUAL(qc.vObjItemInst[1].nItemID, rs2.Field(L"ITEM_ID").AsInt());
		CHECK_EQUAL(qc.vObjItemInst[1].nIUID, rs2.Field(L"IUID").AsInt64());
		CHECK_EQUAL(5, rs2.Field(L"STACK_AMT").AsShort());
		rs2.Close();
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_ITEM)
	{
		CHECK_ACCEPT_WITH_QUEST_ITEM(true);
	}

	GAMEDB_TEST(USP_RZ_QUEST_ACCEPT_ITEM_CREATE)
	{
		CHECK_ACCEPT_WITH_QUEST_ITEM(false);
	}
}