#include "StdAfx.h"
#include "DBTestHelper.h"
#include "UTestDatabaseContainer.h"
#include "UTestDatabase.h"
#include "MDBRecordSet.h"
#include "SUnitTestUtil.h"
#include "CSGuild.h"



using namespace mdb;


DBTestHelper::DBTestHelper()
{
}

DBTestHelper::~DBTestHelper()
{
}

int64 DBTestHelper::InsertAccount(const wchar_t* szUserID, const wchar_t* szSITE_USER_ID, const wchar_t* szPasswd)
{
	static int64 ACCN_ID = 1;
	_ASSERT(NULL != szUserID);
	
	if (NULL == szUserID)
		return 0;
	
	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_ACCOUNT (ACCN_ID, USER_ID, SITE_CODE, SITE_USER_ID, PWD) VALUES (%I64d, '%s', '', '%s', '%s');"
		, ACCN_ID++, szUserID, szSITE_USER_ID, szPasswd)) 
		return 0;

	TDBRecordSet rs;
	if (!UTestDB.Execute(rs, L"SELECT ACCN_ID FROM dbo.RZ_ACCOUNT WHERE SITE_USER_ID = '%s'", szSITE_USER_ID))
		return 0;
	
	_ASSERT(0 != rs.GetFetchedRowCount());

	if (0 == rs.GetFetchedRowCount())
		return 0;
	
	return rs.Field(L"ACCN_ID").AsInt64();
}


int DBTestHelper::GetAID(const wchar_t* szAccountName)
{
	CString strSQL;
	strSQL.Format(L"SELECT ACCN_ID FROM dbo.RZ_ACCOUNT WHERE USER_ID = '%s'", szAccountName);

	TDBRecordSet rs;

	if (!UTestDB.Execute(rs, strSQL))
	{
		return 0;
	}

	if (0 == rs.GetFetchedRowCount())
	{
		return 0;
	}

	return static_cast<uint32>(rs.Field(L"ACCN_ID").AsInt());
}


int64 DBTestHelper::InsertCharacter(const int64 nAID, int nWorld, const wchar_t* szName, const int8 nTotalTP/*=0*/, SEX nSex, RACE nRace)
{
	static int64 CSN = 1;

	wstring strName;
	strName = (NULL == szName) ? SUnitTestUtil::NewName() : szName;

	CString strSQL;
	strSQL.Format(L"\
				  INSERT INTO dbo.RZ_CHARACTER (CHAR_ID, ACCN_ID, WORLD_ID, NAME, RACE, SEX \
				  , FEAT_HAIR, FEAT_FACE, FEAT_HAIR_COLOR, FEAT_SKIN_COLOR \
				  , REMAIN_TP \
				  , LEV, EXP, MONEY\
				  , HP, EN, STA\
				  , REG_DATE)\
				  VALUES(%I64d, %I64d, %d, N'%s', %d, %d\
				  , %d, %d, %d, %d\
				  , %d\
				  , %d, %d, %d\
				  , 100, 100, 100\
				  , GETDATE())",
				  CSN++, nAID, nWorld, strName.c_str(), nRace, nSex, 0, 0, 1, 1, nTotalTP, 1, 0, 0);
	if (!UTestDB.Execute(strSQL)) return 0;

	strSQL.Format(L"SELECT CHAR_ID FROM dbo.RZ_CHARACTER WHERE ACCN_ID = %I64d AND NAME = '%s'", nAID, szName);	
	TDBRecordSet rs;
	if (!UTestDB.Execute(rs, strSQL)) return 0;
	if (0 == rs.GetFetchedRowCount()) return 0;

	const int64 nCID = static_cast<uint32>(rs.Field(L"CHAR_ID").AsInt64());

	return nCID;
}

int DBTestHelper::GetCID(const wchar_t* szCharName)
{
	if (!UTestDB.IsValid())
	{
		return 0;
	}

	CString strSQL;
	strSQL.Format(L"SELECT CHAR_ID FROM dbo.RZ_CHARACTER WHERE Name = '%s'", szCharName);


	TDBRecordSet rs;
	if (!UTestDB.Execute(rs, strSQL))
	{
		return 0;
	}

	if (0 == rs.GetFetchedRowCount())
	{
		return 0;
	}

	return static_cast<uint32>(rs.Field(L"CHAR_ID").AsInt());
}


int DBTestHelper::GetGID(const wchar_t* szGuildName)
{
	if (!UTestDB.IsValid())
	{
		return 0;
	}

	CString strSQL;
	strSQL.Format(L"SELECT GUILD_ID FROM dbo.RZ_GUILD WHERE Name='%s'", szGuildName);

	TDBRecordSet rs;

	if (!UTestDB.Execute(rs, strSQL))
	{
		return 0;
	}

	if (0 == rs.GetFetchedRowCount()) 
	{
		return 0;
	}

	return static_cast<uint32>(rs.Field(L"GUILD_ID").AsInt());
}

const uint64 DBTestHelper::InsertItem(int64 nAID, const uint64 nCID, int nWorld, const uint32 nItemID, const uint32 nStackAmt, const uint8 nSlotType, const int16 nSlotID, bool bClaimed
											  , const int nUsagePeriod, const int nAddExpiMin)
{
	bool bPeirodItem = false;
	if ((0 < nUsagePeriod) || (0 < nAddExpiMin))
		bPeirodItem = true;

	CString strSQL;
	strSQL.Format(L"DECLARE @IUID BIGINT; INSERT INTO dbo.RZ_IUID DEFAULT VALUES; SET @IUID = SCOPE_IDENTITY(); \
				   INSERT INTO dbo.RZ_INVEN(IUID, OWNER_ID, ACCN_ID, WORLD_ID, ITEM_ID\
				   , STACK_AMT, CLAIMED, DURA, MAX_DURA, SLOT_TYPE\
				   , SLOT_ID, CHAR_PTM, SOUL_COUNT, COLOR, PERIOD\
				   , USAGE_PERIOD, EFF_END_DATE)\
		Values (@IUID, %I64d, %I64d, %d, %d\
		, %d, %d, 10, 10, %d\
		, %d, 1, 0, 0, %d\
		, %d, CASE %d WHEN -1 THEN NULL ELSE DATEADD(mi, %d, GETDATE()) END)",
		nCID, nAID, nWorld, nItemID
		, nStackAmt, bClaimed, nSlotType
		, nSlotID, bPeirodItem
		, nUsagePeriod, nAddExpiMin, nAddExpiMin);
	if (!UTestDB.Execute(strSQL)) return 0;

	strSQL.Format(L"SELECT TOP (1) IUID FROM dbo.RZ_INVEN WHERE OWNER_ID=%I64d AND ITEM_ID=%d ORDER BY IUID DESC", nCID, nItemID);
	TDBRecordSet rs;
	if (!UTestDB.Execute(rs, strSQL)) return 0;
	if (0 == rs.GetFetchedRowCount()) return 0;

	return rs.Field(L"IUID").AsInt64();
}


int64 DBTestHelper::InsertGuildStorageItem( int WORLD, int64 GUILD_ID, int SLOT_ID, int ITEM_ID, int16 STACK_AMT, uint8 DURA, uint8 MAX_DURA , int COLOR, uint8 SOUL_COUNT )
{
	CString strSQL;
	strSQL.Format(L"DECLARE @IUID BIGINT; INSERT INTO dbo.RZ_IUID DEFAULT VALUES; SET @IUID = SCOPE_IDENTITY(); \
				   INSERT INTO dbo.RZ_GUILD_STORAGE(WORLD_ID, GUILD_ID, SLOT_ID, IUID, ITEM_ID, STACK_AMT, DURA, MAX_DURA, COLOR, SOUL_COUNT)\
				   Values (%d, %I64d, %d, @IUID, %d, %d, %d, %d, %d, %d);"
				   
				   , WORLD, GUILD_ID, SLOT_ID, ITEM_ID, STACK_AMT, DURA, MAX_DURA, COLOR, SOUL_COUNT);
	if (!UTestDB.Execute(strSQL)) 
		return -1;
	
	CString strIUID;
	strIUID.Format(L"SELECT TOP (1) IUID FROM dbo.RZ_GUILD_STORAGE WHERE GUILD_ID = %I64d AND SLOT_ID = %d;", GUILD_ID, SLOT_ID);	
	TDBRecordSet rs;	
	if (!UTestDB.Execute(rs, strIUID))
		return -2;
	if (0 == rs.GetFetchedRowCount())
		return -3;

	const int64 IUID = rs.Field(L"IUID").AsInt64();

	return IUID;
}

void DBTestHelper::InsertEmptyGuildStorage( int WORLD, int64 GUILD_ID, int SLOT_ID )
{
	UTestDB.Execute(L"INSERT INTO dbo.RZ_GUILD_STORAGE (WORLD_ID, GUILD_ID, SLOT_ID, IUID, STACK_AMT, DURA, MAX_DURA, COLOR) \
					 VALUES (%d, %I64d, %d, 0, 0, 0, 0, 0)"
		, WORLD, GUILD_ID, SLOT_ID);
}

bool DBTestHelper::IsBindedItem(const uint64 nIUID)
{
	CString strSQL;
	strSQL.Format(L"SELECT CLAIMED FROM dbo.RZ_INVEN WHERE IUID = %I64d", nIUID);
	TDBRecordSet rs;

	if (!UTestDB.Execute(rs, strSQL)) return 0;
	if (0 == rs.GetFetchedRowCount()) return 0;

	return rs.Field(L"Bind").AsBool();
}

int DBTestHelper::InsertItemInfo( const int nItemID, const wstring& strName, const int16 nStackAmt /*= 10*/, const bool bUnique /*= false*/, const bool bTradable /*= true */, const uint8 nType /*= ITEMTYPE_NONE*/, const uint8 nSlot /*= ITEMSLOT_NONE*/, const uint8 nTier /*= ITEM_TIER_VERY_COMMON */, const uint8 nMaxDura /*= 100*/, const int nBuyingPrice /*= 100*/, const int nSellingPrice /*= 25*/ )
{
	CString strSQL;
	strSQL.Format(L"INSERT INTO dbo.RZ_ITEM_INFO(ITEM_ID, NAME, TYPE, SLOT, TIER, MAX_DURA, STACK_AMT, [UNIQUE], TRADABLE, BUY_PRICE, SELL_PRICE, COLOR)\
				  VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);"
				  , nItemID, strName.c_str(), nType, nSlot, nTier, nMaxDura, nStackAmt, bUnique, bTradable, nBuyingPrice, nSellingPrice, 0);

	if (!UTestDB.Execute(strSQL))
		return 0;

	return nItemID;
}

int64 DBTestHelper::InsertGuild( int nWorld, const wstring& strGuildName, int nStorageMoney )
{
	static int64 GUILD_ID = 100000000;

	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_GUILD (WORLD_ID, GUILD_ID, NAME, STORAGE_MONEY) \
						  VALUES (%d, %I64d, N'%s', %d);"
						  , nWorld, GUILD_ID, strGuildName.c_str(), nStorageMoney))
		return -1;

	return GUILD_ID;
}

int DBTestHelper::InsertTalent( const int nTaltID, const wstring& strName, const int16 nConsumeTP /*= 1*/, const bool bUntrainable /*= true*/ )
{
	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_TALENT_INFO(TALENT_ID, NAME, CONSUME_TP, UNTRAINABLE)\
					VALUES (%d, N'%s', %d, %d);"
					, nTaltID, strName.c_str(), nConsumeTP, bUntrainable))
					return -1;

	return nTaltID;
}

int DBTestHelper::InsertQuestInfo( const int nQuestID, const wstring& strName /*= "test_quest"*/, const bool bChallenge /*= false*/)
{
	uint8 nType = 1;
	if (bChallenge)
		nType = 2;

	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_QUEST_INFO(QUEST_ID, NAME, [TYPE]) VALUES (%d, N'%s', %d);", nQuestID, strName.c_str(), nType))
		return 0;

	return nQuestID;
}

bool DBTestHelper::InsertQuest( int nWorld, const int64 nAID, const int64 nCID, const uint8 nSlotID, const int nQuestID, const wstring& strAcceptDt , const wstring& strExpiDt )
{
	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_QUEST(WORLD_ID, ACCN_ID, CHAR_ID, SLOT_ID, QUEST_ID, ACCP_DATE, EXPI_DATE) \
					VALUES (%d, %I64d, %I64d, %d, NULL, '%s', '%s');", nWorld, nAID, nCID, nSlotID, strAcceptDt.c_str(), strExpiDt.c_str()))
		return false;

	if (0 < nQuestID)
		if (!UTestDB.Execute(L"UPDATE dbo.RZ_QUEST SET QUEST_ID = %d WHERE CHAR_ID = %I64d AND SLOT_ID = %d;", nQuestID, nCID, nSlotID))
			return false;

	return true;
}

bool DBTestHelper::InsertQuestHistory( int nWorld, const int64 nAID, const int64 nCID, const int nQuestID, const wstring& strExpiDt )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_QUEST_HISTORY(WORLD_ID, ACCN_ID, CHAR_ID, QUEST_ID, EXPI_DATE) \
							VALUES (%d, %I64d, %I64d, %d, '%s');", nWorld, nAID, nCID, nQuestID, strExpiDt.c_str());
}

bool DBTestHelper::SetPalette( const int64 nCID, const uint8 nOrdNum, uint8 nType, int nID )
{
	return UTestDB.Execute(L"UPDATE dbo.RZ_PALETTE SET ID = %d, ID_TYPE = %d WHERE CHAR_ID = %I64d AND SLOT_ID = %d;", nID, nType, nCID, nOrdNum);
}

bool DBTestHelper::InsertPalette( int nWorld, const int64 nAID, const int64 nCID, const uint8 nSize )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_PALETTE(WORLD_ID, ACCN_ID, CHAR_ID, SLOT_ID, ID_TYPE, ID)\
						 SELECT %d, %I64d, %I64d, num, 0, 0\
						 FROM   dbo.RZ_CPT WITH (NOLOCK)\
						 WHERE  num < %d;", nWorld, nAID, nCID, nSize);
}

bool DBTestHelper::InsertGuildMember( int nWorld, const int64 nGID, int64 nAID, const int64 nCID, const uint8 nGrade )
{
	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_GUILD_MEMBER (WORLD_ID, ACCN_ID, CHAR_ID, GRADE, GUILD_ID) \
							 VALUES (%d, %I64d, %I64d, %d, %I64d);"
							 , nWorld, nAID, nCID, nGrade, nGID))
	{
		return false;
	}
	
	return true;
}

bool DBTestHelper::InsertTalentInstance( int nWorld, const int64 nAID, const int64 nCID, const int nTaltID )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_TALENT(WORLD_ID, ACCN_ID, CHAR_ID, TALENT_ID) VALUES (%d, %I64d, %I64d, %d);", nWorld	, nAID, nCID, nTaltID);
}

bool DBTestHelper::InsertCutscene( int nWorld, const int64 nAID, const int64 nCID, const int nCutScnID )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_CUTSCENE(WORLD_ID, ACCN_ID, CHAR_ID, CUTSCN_ID) VALUES (%d, %I64d, %I64d, %d);", nWorld, nAID, nCID, nCutScnID);
}

int DBTestHelper::InsertEmblemInfo( const int nEmblID )
{
	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_EMBLEM_INFO(EMBLEM_ID, NAME) VALUES (%d, N'%d');", nEmblID, nEmblID))
		return 0;

	return nEmblID;
}

bool DBTestHelper::InsertEmblem( int nWorld, const int64 nAID, const int64 nCID, const int nEmblID, const bool bSelected )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_EMBLEM(WORLD_ID, ACCN_ID, CHAR_ID, EMBLEM_ID, SELECTED) VALUES (%d, %I64d, %I64d, %d, %d);", nWorld, nAID, nCID, nEmblID, bSelected);
}

bool DBTestHelper::InsertFaction( int nWorld, const int64 nAID, const int64 nCID, const int nFactID, const int16 nVal )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_FACTION(WORLD_ID, ACCN_ID, CHAR_ID, FACT_ID, VAL) VALUES (%d, %I64d, %I64d, %d, %d);", nWorld, nAID, nCID, nFactID, nVal);		
}

bool DBTestHelper::InsertRecp( int nWorld, const int64 nAID, const int64 nCID, const int nRecpID )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_RECIPE(WORLD_ID, ACCN_ID, CHAR_ID, RECP_ID) VALUES (%d, %I64d, %I64d, %d);", nWorld, nAID, nCID, nRecpID);
}

bool DBTestHelper::InsertEmptyItemSlot( int64 nAID, const int64 nCID, int nWorld, const uint8 nSlotType, const int16 nSlotID, const int64 nIUID )
{
	if (!UTestDB.Execute(L"INSERT INTO dbo.RZ_INVEN(OWNER_ID, ACCN_ID, WORLD_ID, SLOT_TYPE, SLOT_ID, IUID, CHAR_PTM, COLOR, CLAIMED) \
						  VALUES (%I64d, %I64d, %d, %d, %d, %I64d, 0, 0, 0);"
						  , nCID, nAID, nWorld, nSlotType, nSlotID, nIUID))
		return false;

	return true;
}

int64 DBTestHelper::MAIL_UID = 1;

int64 DBTestHelper::InsertMail( int nWorld, const int64 nGSN, const int64 nCSN, const uint8 nType /*= 1*/, const wstring& strSndrName /*= L""*/, const wstring& strTitle /*= L""*/, const bool bHaveText
							   , const int nMoney, const bool bRead, const int nDefItemID )
{
	UTestDB.Execute(L"INSERT INTO dbo.RZ_MAIL(WORLD_ID, ACCN_ID, CHAR_ID, MAIL_ID, MAIL_TYPE, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, READ_FLAG, DEL_DATE, DEF_ITEM_ID) \
					 VALUES (%d, %I64d, %I64d, %I64d, %d, N'%s', N'%s', %d, %d, %d, DATEADD(dd, 1, GETDATE()), %d);"
		, nWorld, nGSN, nCSN, MAIL_UID++, nType, strSndrName.c_str(), strTitle.c_str(), bHaveText
		, nMoney, bRead, nDefItemID);

	return MAIL_UID - 1;
}

int64 DBTestHelper::InsertDeleteMail( int nWorld,const int64 nGSN, const int64 nCSN, const uint8 nType /*= 1*/, const wstring& strSndrName /*= L""*/, const wstring& strTitle /*= L""*/, const bool bHaveText, const int nMoney /*= 100*/, const bool bRead /*= false*/, const int nDefItemID /*= 1*/ )
{
	UTestDB.Execute(L"INSERT INTO dbo.RZ_MAIL(WORLD_ID, ACCN_ID, CHAR_ID, MAIL_ID, MAIL_TYPE, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, READ_FLAG, DEL_DATE, DEF_ITEM_ID) \
					 VALUES (%d, %I64d, %I64d, %I64d, %d, N'%s', N'%s', %d, %d, %d, DATEADD(dd, -1, GETDATE()), %d);"
					 , nWorld, nGSN, nCSN, MAIL_UID++, nType, strSndrName.c_str(), strTitle.c_str(), bHaveText
					 , nMoney, bRead, nDefItemID);

	return MAIL_UID - 1;
}

bool DBTestHelper::SetMoney( const int64 nGSN, const int64 nCSN, const int nMoney )
{
	return UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET [MONEY] = %d WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", nMoney, nGSN, nCSN);
}

void DBTestHelper::InsertMailItem( int nWorld, const int64 nMAIL_UID, const int nSLOT_ID, const int64 nIUID, const int nITEM_ID, const int16 nSTACK_AMT, const uint8 nDURA, const uint8 nMAX_DURA 
								  , const int nENCH_ITEM_ID_1, const int nENCH_ITEM_ID_2, const int nENCH_ITEM_ID_3, const int nENCH_ITEM_ID_4, const int nENCH_ITEM_ID_5, const int nENCH_ITEM_ID_6
								  , const int nDEL_REMAIN_DAY )
{
	UTestDB.Execute(L"INSERT INTO dbo.RZ_MAIL_ITEM(WORLD_ID, MAIL_ID, SLOT_ID, IUID, ITEM_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR \
					 , EFF_START_DATE, DEL_DATE, REG_DATE) \
					 VALUES (%d, %I64d, %d, %I64d, %d, %d, 1, %d, %d, 1, GETDATE(), DATEADD(dd, %d, GETDATE()), GETDATE());"
					 , nWorld, nMAIL_UID, nSLOT_ID, nIUID, nITEM_ID, nSTACK_AMT, nDURA, nMAX_DURA, nDEL_REMAIN_DAY);

	UTestDB.Execute(L"INSERT INTO dbo.RZ_ITEM_ENCHANT (WORLD_ID, IUID, ENCH_ITEM_ID_1, ENCH_ITEM_ID_2, ENCH_ITEM_ID_3, ENCH_ITEM_ID_4, ENCH_ITEM_ID_5, ENCH_ITEM_ID_6) \
					 VALUES (%d, %I64d, %d, %d, %d, %d, %d, %d);"
					 , nWorld, nIUID, nENCH_ITEM_ID_1, nENCH_ITEM_ID_2, nENCH_ITEM_ID_3, nENCH_ITEM_ID_4, nENCH_ITEM_ID_5, nENCH_ITEM_ID_6);
}

bool DBTestHelper::InsertEmptyItemEnch( int WORLD_ID, int64 IUID )
{
	return UTestDB.Execute(L"INSERT INTO dbo.RZ_ITEM_ENCHANT (WORLD_ID, IUID) VALUES (%d, %I64d);", WORLD_ID, IUID);
}

int64 DBTestHelper::InsertCharStorageItem( int WORLD, int64 GSN, int64 CSN, int SLOT_ID, int ITEM_ID, int16 STACK_AMT, uint8 DURA, uint8 MAX_DURA , int COLOR, uint8 SOUL_COUNT, uint8 CLAIMED, int CHAR_PTM )
{
	CString strSQL;
	strSQL.Format(L"DECLARE @IUID BIGINT; INSERT INTO dbo.RZ_IUID DEFAULT VALUES; SET @IUID = SCOPE_IDENTITY(); \
				   INSERT INTO dbo.RZ_CHAR_STORAGE(WORLD_ID, ACCN_ID, CHAR_ID, SLOT_ID, IUID, ITEM_ID, STACK_AMT, DURA, MAX_DURA, COLOR, SOUL_COUNT, CLAIMED, CHAR_PTM)\
				   Values (%d, %I64d, %I64d, %d, @IUID, %d, %d, %d, %d, %d, %d, %d, %d);"
				   , WORLD, GSN, CSN, SLOT_ID, ITEM_ID, STACK_AMT, DURA, MAX_DURA, COLOR, SOUL_COUNT, CLAIMED, CHAR_PTM);
	if (!UTestDB.Execute(strSQL)) 
		return -1;

	CString strIUID;
	strIUID.Format(L"SELECT TOP (1) IUID FROM dbo.RZ_CHAR_STORAGE WHERE CHAR_ID = %I64d AND SLOT_ID = %d;", CSN, SLOT_ID);	
	TDBRecordSet rs;	
	if (!UTestDB.Execute(rs, strIUID))
		return -2;
	if (0 == rs.GetFetchedRowCount())
		return -3;

	const int64 IUID = rs.Field(L"IUID").AsInt64();

	return IUID;
}

void DBTestHelper::InsertEmptyCharStorage( int WORLD, int64 GSN, int64 CSN, int SLOT_ID )
{
	UTestDB.Execute(L"INSERT INTO dbo.RZ_CHAR_STORAGE(WORLD_ID, ACCN_ID, CHAR_ID, SLOT_ID, IUID, ITEM_ID, STACK_AMT, DURA, MAX_DURA, COLOR, SOUL_COUNT, CLAIMED, CHAR_PTM)\
				   Values (%d, %I64d, %I64d, %d, 0, NULL, 0, 0, 0, 0, 0, 0, 0);"
				   , WORLD, GSN, CSN, SLOT_ID);

}
