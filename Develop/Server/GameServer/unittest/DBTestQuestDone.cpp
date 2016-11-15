#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestHelper_Quest.h"
#include "GQuestDateCalculator.h"
#include "GItemConvertPrefixedString.h"
#include "GConfig.h"


SUITE(DBTestQuestDone)
{
	class IN_QITEM
	{
	public :
		IN_QITEM() {}

		void InsertEmptySlot()
		{
			DBTestHelper::InsertEmptyItemSlot(m_GSN, m_CSN, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, m_SLOT_ID);
			/*UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ACCN_ID = %I64d WHERE OWNER_ID = %I64d AND SLOT_ID = %d;"
				, m_GSN, m_CSN, m_SLOT_ID);*/
		}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, STACK_AMT, MAX_DURA, DURA, CLAIMED, PERIOD, EFF_END_DATE\
								 FROM dbo.RZ_INVEN\
								 WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d AND IUID = %I64d;"
								 , m_GSN, m_CSN, m_SLOT_ID, m_NEW_IUID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(m_IUID, rs.Field(L"IUID").AsInt64());
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
		int64	m_NEW_IUID;
	};

	void CHECK_QUEST_DONE_WITH_QUEST_ITEM(bool bExistEmptySlot)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter();

		const int16 nRemoveStackAmt_1 = qc.vObjItemInst[0].nStackAmt;
		const int16 nRemoveStackAmt_2 = 2;
		wstring		strAcceptDt;
		wstring		strExpiDt;

		GQuestDateCalculator::GetAccpetDt(strAcceptDt);
		GQuestDateCalculator::GetExpiDt(true, strExpiDt);

		DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, strAcceptDt, strExpiDt);

		wstring strString;
		GItemConvertPrefixedString cov;
		strString += cov.Convert((SH_ITEM_SLOT_TYPE)1, qc.vObjItemInst[0].nSlotID, qc.vObjItemInst[0].nStackAmt - nRemoveStackAmt_1);
		strString += cov.Convert((SH_ITEM_SLOT_TYPE)1, qc.vObjItemInst[1].nSlotID, qc.vObjItemInst[1].nStackAmt - nRemoveStackAmt_2);

		IN_QITEM qitem[QBEGIN_ITEM_COUNT];
		for (int i = 0; i < QBEGIN_ITEM_COUNT; ++i)
		{
			qitem[i].m_GSN			= qc.nAID;
			qitem[i].m_CSN			= qc.nCID;
			qitem[i].m_SLOT_ID		= 10 + i;
			qitem[i].m_IUID			= i + 1;
			qitem[i].m_ITEM_ID		= i + 2;
			qitem[i].m_STACK_AMT	= i + 3;
			qitem[i].m_MAX_DURA		= i + 4;
			qitem[i].m_CLAIMED		= 1 == (i % 2);
			qitem[i].m_PERIOD		= 0 == (i % 2);
			qitem[i].m_EFF_END_DATE = L"NULL";
			qitem[i].m_NEW_IUID		= i + 1; // 새로 추가된 아이템은 이값이 새로운 IUID로 업데이트 된다.

			if (0 < i)
				qitem[i].InsertEmptySlot();
		}		

		qitem[0].m_IUID = DBTestHelper::InsertItem(qitem[0].m_GSN, qitem[0].m_CSN, GConfig::m_nMyWorldID, qitem[0].m_ITEM_ID, 1, 1, qitem[0].m_SLOT_ID, false, 1, -1);
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = %d, MAX_DURA = %d WHERE IUID = %I64d;", qitem[0].m_MAX_DURA, qitem[0].m_MAX_DURA, qitem[0].m_IUID);
		qitem[0].m_NEW_IUID = qitem[0].m_IUID;

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_QUEST_DONE (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, '%s', '%s'\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , '%s', %d)}"
								   , GConfig::m_nMyWorldID
								   , qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, qc.nCharPtm, qc.nLevel, qc.nXP, qc.nMoney, strAcceptDt.c_str(), strExpiDt.c_str()
								   , qitem[0].m_SLOT_ID, qitem[0].m_IUID, qitem[0].m_ITEM_ID, qitem[0].m_STACK_AMT, qitem[0].m_MAX_DURA, qitem[0].m_CLAIMED, qitem[0].m_PERIOD , qitem[0].m_EFF_END_DATE.c_str()
								   , qitem[1].m_SLOT_ID, qitem[1].m_IUID, qitem[1].m_ITEM_ID, qitem[1].m_STACK_AMT, qitem[1].m_MAX_DURA, qitem[1].m_CLAIMED, qitem[1].m_PERIOD , qitem[1].m_EFF_END_DATE.c_str()
								   , qitem[2].m_SLOT_ID, qitem[2].m_IUID, qitem[2].m_ITEM_ID, qitem[2].m_STACK_AMT, qitem[2].m_MAX_DURA, qitem[2].m_CLAIMED, qitem[2].m_PERIOD , qitem[2].m_EFF_END_DATE.c_str()
								   , qitem[3].m_SLOT_ID, qitem[3].m_IUID, qitem[3].m_ITEM_ID, qitem[3].m_STACK_AMT, qitem[3].m_MAX_DURA, qitem[3].m_CLAIMED, qitem[3].m_PERIOD , qitem[3].m_EFF_END_DATE.c_str()
								   , qitem[4].m_SLOT_ID, qitem[4].m_IUID, qitem[4].m_ITEM_ID, qitem[4].m_STACK_AMT, qitem[4].m_MAX_DURA, qitem[4].m_CLAIMED, qitem[4].m_PERIOD , qitem[4].m_EFF_END_DATE.c_str()
								   , strString.c_str(), 7));

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

		CHECK_EQUAL(qitem[0].m_IUID, qitem[0].m_NEW_IUID);

		for (int i = 0; i < QBEGIN_ITEM_COUNT; ++i)
			qitem[i].Check();

		TDBRecordSet rsChar;
		UTestDB.Execute(rsChar, L"SELECT [EXP], LEV, [MONEY], CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", qc.nCID);
		CHECK_EQUAL(1, rsChar.GetFetchedRowCount());
		CHECK_EQUAL(qc.nXP, rsChar.Field(L"EXP").AsInt());
		CHECK_EQUAL(qc.nLevel, rsChar.Field(L"LEV").AsByte());
		CHECK_EQUAL(qc.nMoney, rsChar.Field(L"MONEY").AsInt());
		rsChar.Close();

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT QUEST_ID, [STATE] FROM dbo.RZ_QUEST WHERE CHAR_ID = %I64d AND SLOT_ID = %d;", qc.nCID, qc.nSlotID));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		CHECK(rsQuest.Field(L"QUEST_ID").IsNull());
		CHECK_EQUAL(0, rsQuest.Field(L"STATE").AsByte());
		rsQuest.Close();

		TDBRecordSet rsQuestHis;
		CHECK(UTestDB.Execute(rsQuestHis, L"SELECT QUEST_ID FROM dbo.RZ_QUEST_HISTORY WHERE CHAR_ID = %I64d AND QUEST_ID = %d AND EXPI_DATE = '%s';"
			, qc.nCID, qc.nQuestID, strExpiDt.c_str()));
		CHECK_EQUAL(qc.nQuestID, rsQuestHis.Field(L"QUEST_ID").AsInt());
		rsQuestHis.Close();

		TDBRecordSet rsItem_1;
		CHECK(UTestDB.Execute(rsItem_1, L"SELECT IUID, STACK_AMT, ITEM_ID FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d;", qc.nCID, qc.vObjItemInst[0].nSlotID));
		CHECK_EQUAL(qc.vObjItemInst[0].nStackAmt - nRemoveStackAmt_1, rsItem_1.Field(L"STACK_AMT").AsShort());
		rsItem_1.Close();

		TDBRecordSet rsItem_2;
		CHECK(UTestDB.Execute(rsItem_2, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", qc.vObjItemInst[1].nIUID));
		CHECK_EQUAL(qc.vObjItemInst[1].nStackAmt - nRemoveStackAmt_2, rsItem_2.Field(L"STACK_AMT").AsInt64());
		rsItem_2.Close();

		for (int i = 0; i < QBEGIN_ITEM_COUNT; ++i)
		{
			qitem[i].Check();
		}
	}

	GAMEDB_TEST(USP_RZ_QUEST_DONE)
	{
		CHECK_QUEST_DONE_WITH_QUEST_ITEM(true);	
	}

	GAMEDB_TEST(USP_RZ_QUEST_DONE_CREATE)
	{
		CHECK_QUEST_DONE_WITH_QUEST_ITEM(false);	
	}

	GAMEDB_TEST(USP_RZ_QUEST_DONE_UPDATE_HISTORY)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter();

		const int16 nRemoveStackAmt_1 = qc.vObjItemInst[0].nStackAmt;
		const int16 nRemoveStackAmt_2 = 2;
		wstring		strAcceptDt;
		wstring		strExpiDt;

		GQuestDateCalculator::GetAccpetDt(strAcceptDt);
		GQuestDateCalculator::GetExpiDt(true, strExpiDt);

		DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, strAcceptDt, strExpiDt);
		DBTestHelper::InsertQuestHistory(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nQuestID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_DONE (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, '%s', '%s'\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , %d, %I64d, %d, %d, %d, %d, %d, %s\
							   , '%s', %d)}"
							   , GConfig::m_nMyWorldID
							   , qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, qc.nCharPtm, qc.nLevel, qc.nXP, qc.nMoney, strAcceptDt.c_str(), strExpiDt.c_str()
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , 0, (int64)0, 0, 0, 0, 0, 0, L"NULL"
							   , L"", 0));

		TDBRecordSet rsQuestHis;
		CHECK(UTestDB.Execute(rsQuestHis, L"SELECT QUEST_ID FROM dbo.RZ_QUEST_HISTORY WHERE CHAR_ID = %I64d AND QUEST_ID = %d AND EXPI_DATE = '%s';"
			, qc.nCID, qc.nQuestID, strExpiDt.c_str()));
		CHECK_EQUAL(qc.nQuestID, rsQuestHis.Field(L"QUEST_ID").AsInt());
	}
}