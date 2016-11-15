#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "MTypes.h"
#include "GConfig.h"

SUITE(DBTestTrade)
{
	struct _CHAR_POSITION
	{
		_CHAR_POSITION(const int nFieldID, const float fPosX, const float fPosY, const float fPosZ)
		{
			nFieldID_	= nFieldID;
			fPosX_		= fPosX;
			fPosY_		= fPosY;
			fPosZ_		= fPosZ;
		}

		int		nFieldID_;
		float	fPosX_;
		float	fPosY_;
		float	fPosZ_;
	};

	struct _CHAR
	{
		_CHAR(const int nAID, const int nXP, const int nMoney, const int nCharPtm, const int nModMoney, const int nFieldID, const float fPosX, const float fPosY, const float fPosZ)
			: Pos(nFieldID, fPosX, fPosY, fPosZ)
		{
			nAID_			= nAID;
			nCID_			= 0;
			nXP_			= nXP;
			nMoney_			= nMoney;
			nCharPtm_		= nCharPtm;
			nModMoney_		= nModMoney;
			nDoingCharPtm_	= nCharPtm + 1000;
		}

		void InsertChar()
		{
			wchar_t szName[32] = {0,};
			swprintf_s(szName, L"test_char_%d", nCID_);
			nCID_ = DBTestHelper::InsertCharacter(nAID_, GConfig::m_nMyWorldID, szName);
			UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET CHAR_PTM = %d, [MONEY] = %d WHERE CHAR_ID = %I64d;", nCharPtm_, nMoney_, nCID_);
		}

		int64			nAID_;
		int64			nCID_;
		int				nXP_;
		int				nMoney_;
		int				nCharPtm_;
		int				nModMoney_;
		int				nDoingCharPtm_;
		_CHAR_POSITION	Pos;
	};

	typedef vector<_CHAR> CHAR_VEC;

	struct _ITEM
	{
		_ITEM(int64 nAID, const int64 nCID, const int nItemID, const int16 nSlotID, const int16 nSlotIDTo
			, const int16 nStackAmt, const int nCharPtm, const int nUsagePeriod, const int nModStackAmt
			, const uint8 nDura, const uint8 nMaxDura, const uint8 nEnchCnt, const int nColor, const uint8 nSoulCnt)
		{
			nAID_			= nAID;
			nCID_			= nCID;
			nItemID_		= nItemID;
			nSlotID_		= nSlotID;
			nSlotIDTo_		= nSlotIDTo;
			nStackAmt_		= nStackAmt;
			nCharPtm_		= nCharPtm;
			nUsagePeriod_	= nUsagePeriod;
			nBefStackAmt_	= nModStackAmt;
			nAddIUID_		= 0;
			nDura_			= nDura;
			nMaxDura_		= nMaxDura;
			nEnchCnt_		= nEnchCnt;
			nColor_			= nColor;
			nSoulCnt_		= nSoulCnt;
		}

		void InsertItem()
		{
			DBTestHelper::InsertItemInfo(nItemID_, L"test_item");

			nIUID_ = DBTestHelper::InsertItem(nAID_, nCID_, GConfig::m_nMyWorldID, nItemID_, nStackAmt_, SLOTTYPE_INVENTORY, nSlotID_, false, nUsagePeriod_);
			UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET CHAR_PTM = %d, DURA = %d, MAX_DURA = %d, COLOR = %d, SOUL_COUNT = %d WHERE IUID = %I64d"
				, nCharPtm_, nDura_, nMaxDura_, nColor_, nSoulCnt_, nIUID_);
		}

		int64	nAID_;
		int64	nCID_;
		int		nItemID_;
		int16	nSlotID_;
		int16	nSlotIDTo_;
		int16	nStackAmt_;
		int		nCharPtm_;
		int		nUsagePeriod_;
		int16	nBefStackAmt_;
		int64	nIUID_;
		int64	nAddIUID_;
		uint8	nDura_;
		uint8	nMaxDura_;
		uint8	nEnchCnt_;
		int		nColor_;
		uint8	nSoulCnt_;
	};

	typedef vector<_ITEM> ITEM_VEC;

	struct _ADD_ITEM
	{
		_ADD_ITEM(const CID nCID, const int16 nSlotID, const CID nToCID, const int16 nToSlotID, const IUID nIUID, const int16 nStackAmt, const uint8 nDura, const int nRemainUsagePeriod)
			: nCID_(nCID), nSlotID_(nSlotID), nToCID_(nCID), nToSlotID_(nSlotID), nIUID_(nIUID), nStackAmt_(nStackAmt), nDura_(nDura), nRemainUsagePeriod_(nRemainUsagePeriod) {}

		int		nCID_;
		int		nToCID_;
		int16	nSlotID_;
		int16	nToSlotID_;
		int64	nIUID_;
		int16	nStackAmt_;
		uint8	nDura_;
		int		nRemainUsagePeriod_;
	};

	typedef vector<_ADD_ITEM> ADD_ITEM_VEC;

	class FIXTURE
	{
	public :
		FIXTURE()
		{
			m_vChar.push_back(_CHAR(1, 4312, 100, 1, 0, 1, 1.0f, 2.0f, 3.0f));
			m_vChar.push_back(_CHAR(2, 3644, 200, 2, 0, 1, 11.0f, 12.0f, 13.0f));

			m_vChar[0].InsertChar();
			m_vChar[1].InsertChar();

			m_vItem[0].push_back(_ITEM(m_vChar[0].nAID_, m_vChar[0].nCID_, 1, 0, 11, 0, 10, 1000, 10, 10, 100, 11, 1, 4));
			m_vItem[0].push_back(_ITEM(m_vChar[0].nAID_, m_vChar[0].nCID_, 2, 1, 12, 20, 10, 1500, 25, 11, 110, 22, 2, 3));
			m_vItem[1].push_back(_ITEM(m_vChar[1].nAID_, m_vChar[1].nCID_, 3, 2, 21, 0, 11, 1000, 10, 50, 60, 33, 3, 10));
			m_vItem[1].push_back(_ITEM(m_vChar[1].nAID_, m_vChar[1].nCID_, 4, 3, 22, 20, 11, 1500, 30, 4, 10, 44, 5, 7));

			m_vItem[0][0].InsertItem();
			m_vItem[0][1].InsertItem();
			m_vItem[1][0].InsertItem();
			m_vItem[1][1].InsertItem();

			DBTestHelper::InsertEmptyItemSlot(m_vItem[1][0].nAID_, m_vItem[1][0].nCID_, GConfig::m_nMyWorldID, 1, m_vItem[0][0].nSlotIDTo_);
			DBTestHelper::InsertEmptyItemSlot(m_vItem[1][1].nAID_, m_vItem[1][1].nCID_, GConfig::m_nMyWorldID, 1, m_vItem[0][1].nSlotIDTo_);
			DBTestHelper::InsertEmptyItemSlot(m_vItem[0][0].nAID_, m_vItem[0][0].nCID_, GConfig::m_nMyWorldID, 1, m_vItem[1][0].nSlotIDTo_);
			DBTestHelper::InsertEmptyItemSlot(m_vItem[0][1].nAID_, m_vItem[0][1].nCID_, GConfig::m_nMyWorldID, 1, m_vItem[1][1].nSlotIDTo_);
		}

		void SetUpFixture(const int nChar1ModMoney, const int nChar2ModMoney)
		{
			m_vChar[0].nModMoney_ = nChar1ModMoney;
			m_vChar[1].nModMoney_ = nChar2ModMoney;
		}

		void CHECK_TRADE_MONEY()
		{
			TDBRecordSet rs;
			CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_TRADE (\
													   %d, %I64d, %I64d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %I64d, %I64d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d, %I64d, %d, %d, %d, %d, %d, %d \
													   , %d)}"
													   , GConfig::m_nMyWorldID
													   , m_vChar[0].nAID_, m_vChar[0].nCID_, m_vChar[0].nXP_, CalcChar1Money(), m_vChar[0].nDoingCharPtm_
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , m_vChar[1].nAID_, m_vChar[1].nCID_,  m_vChar[1].nXP_, CalcChar2Money(), m_vChar[1].nDoingCharPtm_
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0, (int64)0, 0, 0, 0, 0, 0, 0
													   , 0));

			CHECK_EQUAL(0, rs.GetFetchedRowCount());
		}

		void CHECK_TRADE()
		{
			const int nItemInstCnt = (m_vItem[0].size() * 2) + (m_vItem[1].size() * 2);
			TDBRecordSet rs;
			CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_TRADE ( \
														  %d, %I64d, %I64d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %I64d, %I64d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d, %I64d, %d, %d, %d, %d, %d, %d \
														, %d)}"
														, GConfig::m_nMyWorldID
														, m_vChar[0].nAID_, m_vChar[0].nCID_, m_vChar[0].nXP_, CalcChar1Money(), m_vChar[0].nDoingCharPtm_
														, m_vItem[0][0].nSlotID_, m_vItem[0][0].nIUID_, m_vItem[0][0].nItemID_, m_vItem[0][0].nStackAmt_, m_vItem[0][0].nBefStackAmt_ - m_vItem[0][0].nStackAmt_, m_vItem[0][0].nDura_, m_vItem[0][0].nUsagePeriod_, m_vItem[0][0].nSlotIDTo_
														, m_vItem[0][1].nSlotID_, m_vItem[0][1].nIUID_, m_vItem[0][1].nItemID_, m_vItem[0][1].nStackAmt_, m_vItem[0][1].nBefStackAmt_ - m_vItem[0][1].nStackAmt_, m_vItem[0][1].nDura_, m_vItem[0][1].nUsagePeriod_, m_vItem[0][1].nSlotIDTo_
														, 0, (int64)0, 0, 0, 0, 0, 0, 0
														, 0, (int64)0, 0, 0, 0, 0, 0, 0
														, 0, (int64)0, 0, 0, 0, 0, 0, 0
														, m_vChar[1].nAID_, m_vChar[1].nCID_,  m_vChar[1].nXP_, CalcChar2Money(), m_vChar[1].nDoingCharPtm_
														, m_vItem[1][0].nSlotID_, m_vItem[1][0].nIUID_, m_vItem[1][0].nItemID_, m_vItem[1][0].nStackAmt_, m_vItem[1][0].nBefStackAmt_ - m_vItem[1][0].nStackAmt_, m_vItem[1][0].nDura_, m_vItem[1][0].nUsagePeriod_, m_vItem[1][0].nSlotIDTo_
														, m_vItem[1][1].nSlotID_, m_vItem[1][1].nIUID_, m_vItem[1][1].nItemID_, m_vItem[1][1].nStackAmt_, m_vItem[1][1].nBefStackAmt_ - m_vItem[1][1].nStackAmt_, m_vItem[1][1].nDura_, m_vItem[1][1].nUsagePeriod_, m_vItem[1][1].nSlotIDTo_
														, 0, (int64)0, 0, 0, 0, 0, 0, 0
														, 0, (int64)0, 0, 0, 0, 0, 0, 0
														, 0, (int64)0, 0, 0, 0, 0, 0, 0
														, nItemInstCnt));
			
			
			for (; !rs.IsEOF(); rs.MoveNext())
			{
				m_vAddItem.push_back(_ADD_ITEM(rs.Field(L"CHAR_ID").AsInt(), rs.Field(L"SLOT_ID").AsShort(), rs.Field(L"TO_CHAR_ID").AsInt(), rs.Field(L"TO_SLOT_ID").AsShort()
					, rs.Field(L"IUID").AsInt64(), rs.Field(L"STACK_AMT").AsShort(), rs.Field(L"DURA").AsByte(), rs.Field(L"REMAIN_USAGE_PERIOD").AsInt()));
			}

			CHECK_EQUAL(8 /*아이템 하나당 2개의 결과가 생성된다.*/, rs.GetFetchedRowCount());
		}

		int CalcChar1Money()
		{
			return CalcTradeMoney(m_vChar[0], m_vChar[1]);
		}

		int CalcChar2Money()
		{
			return CalcTradeMoney(m_vChar[1], m_vChar[0]);
		}

		int CalcTradeMoney(const _CHAR& Recver, const _CHAR& Sender)
		{
			return Recver.nMoney_ + (Sender.nModMoney_ - Recver.nModMoney_);
		}

		void CHECK_CHARACTER_XP_AND_MONEY()
		{
			const wchar_t* szCharSQL = L"SELECT EXP, [MONEY] FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;";

			TDBRecordSet rsChar1;
			UTestDB.Execute(rsChar1, szCharSQL, m_vChar[0].nCID_);
			CHECK_EQUAL(m_vChar[0].nXP_, rsChar1.Field(L"EXP").AsInt());
			CHECK_EQUAL(CalcChar1Money(), rsChar1.Field(L"MONEY").AsInt());
			rsChar1.Close();

			TDBRecordSet rsChar2;
			UTestDB.Execute(rsChar2, szCharSQL, m_vChar[1].nCID_);
			CHECK_EQUAL(m_vChar[1].nXP_, rsChar2.Field(L"EXP").AsInt());
			CHECK_EQUAL(CalcChar2Money(), rsChar2.Field(L"MONEY").AsInt());
			rsChar2.Close();
		}

		void CHECK_ITEM_MOVE_TO()
		{
			const wchar_t* szSQL = L"SELECT OWNER_ID, SLOT_TYPE, SLOT_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM, USAGE_PERIOD\
							   FROM dbo.RZ_INVEN \
							   WHERE IUID = %I64d;";
			TDBRecordSet rs1;
			UTestDB.Execute(rs1, szSQL, m_vItem[0][0].nIUID_);
			CHECK_EQUAL(1, rs1.GetFetchedRowCount());
			CHECK_EQUAL(m_vChar[1].nCID_, rs1.Field(L"OWNER_ID").AsInt64());
			CHECK_EQUAL(SLOTTYPE_INVENTORY, rs1.Field(L"SLOT_TYPE").AsInt());
			CHECK_EQUAL(m_vItem[0][0].nSlotIDTo_, rs1.Field(L"SLOT_ID").AsInt());
			CHECK_EQUAL(m_vItem[0][0].nBefStackAmt_ - m_vItem[0][0].nStackAmt_, rs1.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_vItem[0][0].nDura_, rs1.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_vItem[0][0].nMaxDura_, rs1.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_vItem[0][0].nColor_, rs1.Field(L"COLOR").AsInt());
			CHECK_EQUAL(false, rs1.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_vChar[1].nDoingCharPtm_, rs1.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_vChar[1].nDoingCharPtm_ + m_vItem[0][0].nUsagePeriod_, rs1.Field(L"USAGE_PERIOD").AsInt());
			rs1.Close();

			TDBRecordSet rs2;
			UTestDB.Execute(rs2, szSQL, m_vItem[1][0].nIUID_);
			CHECK_EQUAL(1, rs2.GetFetchedRowCount());
			CHECK_EQUAL(m_vChar[0].nCID_, rs2.Field(L"OWNER_ID").AsInt64());
			CHECK_EQUAL(SLOTTYPE_INVENTORY, rs2.Field(L"SLOT_TYPE").AsInt());
			CHECK_EQUAL(m_vItem[1][0].nSlotIDTo_, rs2.Field(L"SLOT_ID").AsInt());
			CHECK_EQUAL(m_vItem[1][0].nBefStackAmt_ - m_vItem[1][0].nStackAmt_, rs2.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_vItem[1][0].nDura_, rs2.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_vItem[1][0].nMaxDura_, rs2.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_vItem[1][0].nColor_, rs2.Field(L"COLOR").AsInt());
			CHECK_EQUAL(false, rs2.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_vChar[0].nDoingCharPtm_, rs2.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_vChar[0].nDoingCharPtm_ + m_vItem[1][0].nUsagePeriod_, rs2.Field(L"USAGE_PERIOD").AsInt());
			rs2.Close();
		}

		void CHECK_ITEM_SPLIT_UPDATE()
		{
			const wchar_t* szSQL = L"SELECT OWNER_ID, SLOT_TYPE, SLOT_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM, USAGE_PERIOD\
								FROM dbo.RZ_INVEN \
								WHERE IUID = %I64d;";
			TDBRecordSet rs1;
			CHECK(UTestDB.Execute(rs1, szSQL, m_vItem[0][1].nIUID_));
			CHECK_EQUAL(m_vChar[0].nCID_, rs1.Field(L"OWNER_ID").AsInt64());
			CHECK_EQUAL(1, rs1.Field(L"SLOT_TYPE").AsInt());
			CHECK_EQUAL(m_vItem[0][1].nSlotID_, rs1.Field(L"SLOT_ID").AsInt());
			CHECK_EQUAL(m_vItem[0][1].nStackAmt_, rs1.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_vItem[0][1].nSoulCnt_, rs1.Field(L"SOUL_COUNT").AsByte());
			CHECK_EQUAL(m_vItem[0][1].nDura_, rs1.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_vItem[0][1].nMaxDura_, rs1.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_vItem[0][1].nColor_, rs1.Field(L"COLOR").AsInt());
			CHECK_EQUAL(false, rs1.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_vItem[0][1].nCharPtm_, rs1.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_vItem[0][1].nUsagePeriod_, rs1.Field(L"USAGE_PERIOD").AsInt());
			rs1.Close();

			TDBRecordSet rs2;
			CHECK(UTestDB.Execute(rs2, szSQL, m_vItem[1][1].nIUID_));
			CHECK_EQUAL(m_vChar[1].nCID_, rs2.Field(L"OWNER_ID").AsInt64());
			CHECK_EQUAL(1, rs2.Field(L"SLOT_TYPE").AsInt());
			CHECK_EQUAL(m_vItem[1][1].nSlotID_, rs2.Field(L"SLOT_ID").AsInt());
			CHECK_EQUAL(m_vItem[1][1].nStackAmt_, rs2.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_vItem[1][1].nSoulCnt_, rs2.Field(L"SOUL_COUNT").AsByte());
			CHECK_EQUAL(m_vItem[1][1].nDura_, rs2.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_vItem[1][1].nMaxDura_, rs2.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_vItem[1][1].nColor_, rs2.Field(L"COLOR").AsInt());
			CHECK_EQUAL(false, rs2.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_vItem[1][1].nCharPtm_, rs2.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_vItem[1][1].nUsagePeriod_, rs2.Field(L"USAGE_PERIOD").AsInt());
			rs2.Close();
		}

		void CHECK_SPLIT_ADD()
		{
			const wchar_t* szSQL = L"SELECT OWNER_ID, SLOT_TYPE, SLOT_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM, USAGE_PERIOD\
								FROM dbo.RZ_INVEN \
								WHERE ITEM_ID = %d AND IUID <> %I64d;";

			// Char_2
			TDBRecordSet rs1;
			CHECK(UTestDB.Execute(rs1, szSQL, m_vItem[0][1].nItemID_, m_vItem[0][1].nIUID_));
			CHECK_EQUAL(m_vChar[1].nCID_, rs1.Field(L"OWNER_ID").AsInt64());
			CHECK_EQUAL(1, rs1.Field(L"SLOT_TYPE").AsInt());
			CHECK_EQUAL(m_vItem[0][1].nSlotIDTo_, rs1.Field(L"SLOT_ID").AsInt());
			CHECK_EQUAL(m_vItem[0][1].nBefStackAmt_ - m_vItem[0][1].nStackAmt_, rs1.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_vItem[0][1].nDura_, rs1.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_vItem[0][1].nMaxDura_, rs1.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_vItem[0][1].nColor_, rs1.Field(L"COLOR").AsInt());
			CHECK_EQUAL(false, rs1.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_vChar[1].nDoingCharPtm_, rs1.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_vItem[0][1].nUsagePeriod_ + m_vChar[1].nDoingCharPtm_ , rs1.Field(L"USAGE_PERIOD").AsInt());
			rs1.Close();

			// Char_1
			TDBRecordSet rs2;
			CHECK(UTestDB.Execute(rs2, szSQL, m_vItem[1][1].nItemID_, m_vItem[1][1].nIUID_));
			CHECK_EQUAL(m_vChar[0].nCID_, rs2.Field(L"OWNER_ID").AsInt64());
			CHECK_EQUAL(1, rs2.Field(L"SLOT_TYPE").AsInt());
			CHECK_EQUAL(m_vItem[1][1].nSlotIDTo_, rs2.Field(L"SLOT_ID").AsInt());
			CHECK_EQUAL(m_vItem[1][1].nBefStackAmt_ - m_vItem[1][1].nStackAmt_, rs2.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_vItem[1][1].nDura_, rs2.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_vItem[1][1].nMaxDura_, rs2.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_vItem[1][1].nColor_, rs2.Field(L"COLOR").AsInt());
			CHECK_EQUAL(false, rs2.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_vChar[0].nDoingCharPtm_, rs2.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_vItem[1][1].nUsagePeriod_ + m_vChar[0].nDoingCharPtm_, rs2.Field(L"USAGE_PERIOD").AsInt());
			rs2.Close();
		}

		
	public :
		CHAR_VEC		m_vChar;
		ITEM_VEC		m_vItem[2];
		ADD_ITEM_VEC	m_vAddItem;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE, CALC_TRADE_MONEY)
	{
		SetUpFixture(5, 7);

		CHECK_EQUAL(102, CalcChar1Money());
		CHECK_EQUAL(198, CalcChar2Money());

		CHECK_EQUAL(CalcChar1Money(), CalcTradeMoney(m_vChar[0], m_vChar[1]));
		CHECK_EQUAL(CalcChar2Money(), CalcTradeMoney(m_vChar[1], m_vChar[0]));
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_TRADE_MONEY_EACH_OTHER)
	{
		SetUpFixture(5, 7);

		CHECK_TRADE_MONEY();

		CHECK_CHARACTER_XP_AND_MONEY();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_TRADE_MONEY_CHAR1_ONLY_RECV_MONEY)
	{
		SetUpFixture(0, 7);

		CHECK_TRADE_MONEY();

		CHECK_CHARACTER_XP_AND_MONEY();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_TRADE_MONEY_CHAR2_ONLY_RECV_MONEY)
	{
		SetUpFixture(5, 0);

		CHECK_TRADE_MONEY();

		CHECK_CHARACTER_XP_AND_MONEY();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ONLY_TRADE_ITEM)
	{
		SetUpFixture(0, 0);

		CHECK_TRADE();

		CHECK_ITEM_MOVE_TO();
		CHECK_ITEM_SPLIT_UPDATE();
		CHECK_SPLIT_ADD();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_TRADE)
	{
		SetUpFixture(5, 7);

		CHECK_TRADE();

		CHECK_CHARACTER_XP_AND_MONEY();

		CHECK_ITEM_MOVE_TO();
		CHECK_ITEM_SPLIT_UPDATE();
		CHECK_SPLIT_ADD();
	}
} 
