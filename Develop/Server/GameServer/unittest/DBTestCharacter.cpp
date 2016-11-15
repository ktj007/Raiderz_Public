#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "SAccountInfo.h"
#include "GDB_CODE.h"
#include "GConfig.h"


SUITE(RZ_CHARACTER)
{
	GAMEDB_TEST(USP_RZ_CHAR_GET_INFO)
	{
		const wstring strName(L"tset_character");
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, strName.c_str());
		CHECK(0 != nCID);

		const float fPosX = 1.0f;
		const float fPosY = 2.0f;
		const float fPosZ = 3.0f;

		const int nHair = 1;
		const int nFace = 2;
		const int nHairColor = 3;
		const int nSkinColor = 4;
		const int nEyeColor = 5;

		const int nTatoo = 11;
		const int nTatooX = 12;
		const int nTatooY = 13;
		const int nTatooScale = 14;
		const int nMakeup = 15;

		CHECK(UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET POS_X = %f, POS_Y = %f, POS_Z = %f\
									, FEAT_HAIR = %d, FEAT_FACE = %d, FEAT_HAIR_COLOR = %d, FEAT_SKIN_COLOR = %d, FEAT_EYE_COLOR = %d\
									, TATOO = %d, TATOO_POS_X = %d, TATOO_POS_Y = %d, TATOO_SCALE = %d, MAKEUP = %d\
							   WHERE CHAR_ID = %I64d;"
							   , fPosX, fPosY, fPosZ
							   , nHair, nFace, nHairColor, nSkinColor, nEyeColor
							   , nTatoo, nTatooX, nTatooY, nTatooScale, nMakeup
							   , nCID));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_INFO (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(nCID, rs.Field(L"CHAR_ID").AsInt());
		CHECK(strName == rs.Field(L"NAME").AsWString());
		CHECK_EQUAL(fPosX, rs.Field(L"POS_X").AsFloat());
		CHECK_EQUAL(fPosY, rs.Field(L"POS_Y").AsFloat());
		CHECK_EQUAL(fPosZ, rs.Field(L"POS_Z").AsFloat());
		CHECK_EQUAL(nHair, rs.Field(L"FEAT_HAIR").AsInt());
		CHECK_EQUAL(nFace, rs.Field(L"FEAT_FACE").AsInt());
		CHECK_EQUAL(nHairColor, rs.Field(L"FEAT_HAIR_COLOR").AsInt());
		CHECK_EQUAL(nSkinColor, rs.Field(L"FEAT_SKIN_COLOR").AsInt());
		CHECK_EQUAL(nEyeColor, rs.Field(L"FEAT_EYE_COLOR").AsInt());
		CHECK_EQUAL(nTatoo, rs.Field(L"TATOO").AsInt());
		CHECK_EQUAL(nTatooX, rs.Field(L"TATOO_POS_X").AsInt());
		CHECK_EQUAL(nTatooY, rs.Field(L"TATOO_POS_Y").AsInt());
		CHECK_EQUAL(nTatooScale, rs.Field(L"TATOO_SCALE").AsInt());
		CHECK_EQUAL(nMakeup, rs.Field(L"MAKEUP").AsInt());
		rs.Close();
		
	}

	GAMEDB_TEST(USP_RZ_CHAR_DELETE_NOT_GUILD_MEMBER)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const wstring strGuildName = L"test_guild";

		int ret;
		CHECK(UTestDB.Execute(ret, L"{? = CALL dbo.USP_RZ_CHAR_DELETE (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));	
		CHECK_EQUAL(0, ret);
	}

	GAMEDB_TEST(USP_RZ_CHAR_DELETE_GUILD_MASTER)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const wstring strGuildName = L"test_guild";

		const int64 nGID = DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, strGuildName);
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID, nCID, GMG_MASTER);
		CHECK(nGID > 0);

		int ret;
		CHECK(UTestDB.Execute(ret, L"{? = CALL dbo.USP_RZ_CHAR_DELETE (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(-1, ret);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT COUNT(*) CNT FROM dbo.RZ_GUILD_MEMBER WHERE WORLD_ID = %d AND GUILD_ID = %I64d AND CHAR_ID = %I64d;"
			, GConfig::m_nMyWorldID, nGID, nCID);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(1, rs.Field(L"CNT").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_DELETE_GUILD_MEMBER)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const wstring strGuildName = L"test_guild";

		const int64 nGID = DBTestHelper::InsertGuild(GConfig::m_nMyWorldID, strGuildName);
		DBTestHelper::InsertGuildMember(GConfig::m_nMyWorldID, nGID, nAID, nCID, GMG_NORMAL);
		CHECK(nGID > 0);

		int ret;
		CHECK(UTestDB.Execute(ret, L"{? = CALL dbo.USP_RZ_CHAR_DELETE (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(0, ret);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT COUNT(*) CNT FROM dbo.RZ_GUILD_MEMBER;");
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"CNT").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_GET_SIMPLE_INFO)
	{
		const int64		nAID = 1;
		const wstring	strName(L"test_character");
		const int64		nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, strName.c_str());

		const int nHair = 1;
		const int nFace = 2;
		const int nHairColor = 3;
		const int nSkinColor = 4;
		const int nEyeColor = 5;

		const int nTatoo = 11;
		const int nTatooX = 12;
		const int nTatooY = 13;
		const int nTatooScale = 14;
		const int nMakeup = 15;

		CHECK(UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET FEAT_HAIR = %d, FEAT_FACE = %d, FEAT_HAIR_COLOR = %d, FEAT_SKIN_COLOR = %d, FEAT_EYE_COLOR = %d\
							   , TATOO = %d, TATOO_POS_X = %d, TATOO_POS_Y = %d, TATOO_SCALE = %d, MAKEUP = %d\
							   WHERE CHAR_ID = %I64d;"
							   , nHair, nFace, nHairColor, nSkinColor, nEyeColor
							   , nTatoo, nTatooX, nTatooY, nTatooScale, nMakeup
							   , nCID));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_SIMPLE_INFO (%d, %I64d)}", GConfig::m_nMyWorldID, nAID));
		CHECK(strName == rs.Field(L"NAME").AsWString());
		CHECK_EQUAL(1, rs.Field(L"LEV").AsByte());
		CHECK_EQUAL(0, rs.Field(L"RACE").AsByte());
		CHECK_EQUAL(0, rs.Field(L"SEX").AsByte());
		CHECK_EQUAL(nHair, rs.Field(L"FEAT_HAIR").AsInt());
		CHECK_EQUAL(nFace, rs.Field(L"FEAT_FACE").AsInt());
		CHECK_EQUAL(nHairColor, rs.Field(L"FEAT_HAIR_COLOR").AsInt());
		CHECK_EQUAL(nSkinColor, rs.Field(L"FEAT_SKIN_COLOR").AsInt());
		CHECK_EQUAL(nEyeColor, rs.Field(L"FEAT_EYE_COLOR").AsInt());
		CHECK_EQUAL(nTatoo, rs.Field(L"TATOO").AsInt());
		CHECK_EQUAL(nTatooX, rs.Field(L"TATOO_POS_X").AsInt());
		CHECK_EQUAL(nTatooY, rs.Field(L"TATOO_POS_Y").AsInt());
		CHECK_EQUAL(nTatooScale, rs.Field(L"TATOO_SCALE").AsInt());
		CHECK_EQUAL(nMakeup, rs.Field(L"MAKEUP").AsInt());
		rs.Field(L"SHR_FIELD_ID");
		rs.Field(L"DYN_FIELD_GROUP_UID");
		rs.Field(L"DYN_FIELD_ID");
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_GET_EQUIPMENT_INFO)
	{
		const int64	nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nItem_1		= 1;
		const int	nItem_2		= 2;
		const int16 nStackAmt	= 1;
		const uint8 nSlotType	= 2;
		const int16 nSlotID_1	= 1;
		const int16 nSlotID_2	= 2;

		CHECK(0 < DBTestHelper::InsertItemInfo(nItem_1, L"test_1"));
		CHECK(0 < DBTestHelper::InsertItemInfo(nItem_2, L"test_2"));

		CHECK(0 < DBTestHelper::InsertItem(nAID, nCID, GConfig::m_nMyWorldID, nItem_1, nStackAmt, nSlotType, nSlotID_1));
		CHECK(0 < DBTestHelper::InsertItem(nAID, nCID, GConfig::m_nMyWorldID, nItem_2, nStackAmt, nSlotType, nSlotID_2));		

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_EQUIPMENT_INFO (%d, %I64d)}", GConfig::m_nMyWorldID, nAID));

		bool bEquipItem1 = false;
		bool bEquipItem2 = false;
		for (; !rs.IsEOF(); rs.MoveNext())
		{
			switch (rs.Field(L"ITEM_ID").AsInt())
			{
			case nItem_1 :
				{
					bEquipItem1 = true;		
				}
				break;

			case nItem_2 :
				{
					bEquipItem2 = true;
				}
				break;
			}
		}		
		rs.Close();

		CHECK(bEquipItem1);
		CHECK(bEquipItem2);
	}

	GAMEDB_TEST(USP_RZ_CHAR_LEVEL_UP)
	{
		const int64 nAID			= 1;
		const int64	nCID			= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const u_int nNewXP			= 10000;
		const u_int nNewLevel		= 2;
		const int	nMoney			= 5012;
		const u_int nRemainTP		= 10;
		const u_int nTotalTP		= 20;
		const int	nCharPlayTime	= 100;
		const int	nFieldID		= 1;
		const float	nX				= 10;
		const float	nY				= 20;
		const float	nZ				= 30;	

		CHECK(UTestDB.Execute(L"{CALL USP_RZ_CHAR_LEVEL_UP (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %f, %f, %f)}"
			, GConfig::m_nMyWorldID
			, nAID, nCID, nNewXP, nNewLevel, nMoney, nCharPlayTime, nRemainTP
			, nX, nY, nZ));

		TDBRecordSet rs1;
		CHECK(UTestDB.Execute(rs1, L"SELECT [EXP], LEV, REMAIN_TP FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d", nCID));
		CHECK_EQUAL(1, rs1.GetFetchedRowCount());
		CHECK_EQUAL(nNewXP, rs1.Field(L"EXP").AsInt());
		CHECK_EQUAL(nNewLevel, rs1.Field(L"LEV").AsInt());
		CHECK_EQUAL(nRemainTP, rs1.Field(L"REMAIN_TP").AsInt());
		rs1.Close();		
	}

	void USP_RZ_CHAR_UPDATE_MONEY(const int64 nCID, const int nMoney, const int nXP, const int nCharPtm, const int16 nLogType)
	{
		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_UPDATE_MONEY (%d, %I64d, %I64d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, (int64)1, nCID, nMoney, nXP, nCharPtm));

		TDBRecordSet rsChar;
		CHECK(UTestDB.Execute(rsChar, L"SELECT CHAR_PTM, [EXP], [MONEY] FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d", nCID));
		CHECK_EQUAL(1, rsChar.GetFetchedRowCount());
		CHECK_EQUAL(nXP, rsChar.Field(L"EXP").AsInt());
		CHECK_EQUAL(nMoney, rsChar.Field(L"MONEY").AsInt());
		CHECK_EQUAL(nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
		rsChar.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_UPDATE_MONEY_INCREASE)
	{
		const int64	nCID		= DBTestHelper::InsertCharacter(1, GConfig::m_nMyWorldID);
		const int	nMoney		= 100;
		const int	nXP			= 1023;
		const int	nCharPtm	= 123;
		const int16	nIncLogType	= 102;

		USP_RZ_CHAR_UPDATE_MONEY(nCID, nMoney, nXP, nCharPtm, nIncLogType);
	}

	GAMEDB_TEST(USP_RZ_CHAR_UPDATE_MONEY_DECREASE)
	{
		const int64	nCID		= DBTestHelper::InsertCharacter(1, GConfig::m_nMyWorldID);
		const int	nMoney		= 100;
		const int	nXP			= 1023;
		const int	nCharPtm	= 123;
		const int16	nDecLogType	= 103;

		USP_RZ_CHAR_UPDATE_MONEY(nCID, nMoney, nXP, nCharPtm, nDecLogType);
	}

	GAMEDB_TEST(USP_RZ_CHAR_LOGOUT)
	{
		const int64		nAID				= 1;
		const int64		nCID				= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const uint32	nXP					= 1231;
		const int		nLevel				= 1;
		const uint32	nMoney				= 64154231;
		const uint16	nFatigue			= 1324;
		const int		nWeaponSet			= 1;
		const int		nActivePaletteID	= 0;
		const int		nPrimaryPaletteID	= 3;
		const int		nSecondaryPaletteID	= 5;
		const int		nHP					= 100;
		const int		nEN					= 101;
		const int		nSTA				= 102;
		const float		fPosX				= 1.0f;
		const float		fPosY				= 2.0f;
		const float		fPosZ				= 3.0f;
		const int		nSharedFieldID		= 1;
		const float		fEnterPosX			= 4.0f;
		const float		fEnterPosY			= 5.0f;
		const float		fEnterPosZ			= 6.0f;
		const MUID		uidDFieldGroup(10, 11);
		const int		nDFieldID			= 10;
		const int		nCPointFieldID		= 11;
		const int		nCPointMarkerID		= 12;
		const MUID		uidParty			= 101010;
		const MUID		uidArena			= 101011;
		const MUID		uidArenaTeam		= 101012;
		const int		nPlayTime			= 54;
		const int		nElapsedDeadTimeSec	= 87;
		const wstring	strEffect(L"1.1.10,2.2.20");
		
		CHECK(UTestDB.Execute(L"{CALL USP_RZ_CHAR_LOGOUT (%d, %I64d, %I64d, %d, %d, %d\
													   , %d, %d, %d\
													   , %d, %d, %d, %d, %d\
													   , %f, %f, %f\
													   , %d, %f, %f, %f\
													   , %I64d, %d, %d, %d\
													   , %I64d, %I64d, %I64d\
													   , %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nXP, nLevel, nMoney
			, nHP, nEN, nSTA
			, nFatigue, nWeaponSet, nActivePaletteID, nPrimaryPaletteID, nSecondaryPaletteID
			, fPosX, fPosY, fPosZ
			, nSharedFieldID, fEnterPosX, fEnterPosY, fEnterPosZ
			, uidDFieldGroup.Value, nDFieldID, nCPointFieldID, nCPointMarkerID
			, uidParty.Value, uidArena, uidArenaTeam			
			, nPlayTime, nElapsedDeadTimeSec));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT * FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d", nCID));

		CHECK_EQUAL(nXP, rs.Field(L"EXP").AsInt());
		CHECK_EQUAL(nLevel, rs.Field(L"LEV").AsInt());
		CHECK_EQUAL(nMoney, rs.Field(L"MONEY").AsInt());
		CHECK_EQUAL(nFatigue, rs.Field(L"FATIGUE").AsInt());
		CHECK_EQUAL(nHP, rs.Field(L"HP").AsInt());
		CHECK_EQUAL(nEN, rs.Field(L"EN").AsInt());
		CHECK_EQUAL(nSTA, rs.Field(L"STA").AsInt());
		CHECK_EQUAL(nWeaponSet, rs.Field(L"WEAPON_SET").AsInt());
		CHECK_EQUAL(nActivePaletteID, rs.Field(L"ACT_PALETTE_ID").AsInt());
		CHECK_EQUAL(nPrimaryPaletteID, rs.Field(L"PRI_PALETTE_ID").AsInt());
		CHECK_EQUAL(nSecondaryPaletteID, rs.Field(L"SEC_PALETTE_ID").AsInt());
		CHECK_EQUAL(fPosX, rs.Field(L"POS_X").AsFloat());
		CHECK_EQUAL(fPosY, rs.Field(L"POS_Y").AsFloat());
		CHECK_EQUAL(fPosZ, rs.Field(L"POS_Z").AsFloat());
		CHECK_EQUAL(nSharedFieldID, rs.Field(L"SHR_FIELD_ID").AsInt());
		CHECK_EQUAL(fEnterPosX, rs.Field(L"ENT_POS_X").AsFloat());
		CHECK_EQUAL(fEnterPosY, rs.Field(L"ENT_POS_Y").AsFloat());
		CHECK_EQUAL(fEnterPosZ, rs.Field(L"ENT_POS_Z").AsFloat());
		CHECK_EQUAL(uidDFieldGroup.Value, rs.Field(L"DYN_FIELD_GROUP_UID").AsInt64());
		CHECK_EQUAL(nDFieldID, rs.Field(L"DYN_FIELD_ID").AsInt());
		CHECK_EQUAL(nCPointFieldID, rs.Field(L"CHK_POINT_FIELD_ID").AsInt());
		CHECK_EQUAL(nCPointMarkerID, rs.Field(L"CHK_POINT_MKR_ID").AsInt());
		CHECK_EQUAL(uidParty.Value, rs.Field(L"PARTY_UID").AsInt64());
		CHECK_EQUAL(uidArena.Value, rs.Field(L"ARENA_UID").AsInt64());
		CHECK_EQUAL(uidArenaTeam.Value, rs.Field(L"ARENA_TEAM_UID").AsInt64());
		CHECK_EQUAL(nPlayTime, rs.Field(L"CHAR_PTM").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_UPDATE_PLAYER_GRADE)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_UPDATE_PLAYER_GRADE (%d, %I64d, %I64d, 2)}"
			, GConfig::m_nMyWorldID, nAID, nCID));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT PLAYER_GRADE FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", nCID));
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(2, rs.Field(L"PLAYER_GRADE").AsByte());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_UPDATE_TP)
	{
		const int64 nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int16 nRemainTP	= 10;
		const int	nCharPtm	= 30;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_UPDATE_TP (%d, %I64d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nRemainTP, nCharPtm));
		
		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT REMAIN_TP, CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", nCID));
		CHECK_EQUAL(1, rs.GetFetchedCount());
		CHECK_EQUAL(nRemainTP, rs.Field(L"REMAIN_TP").AsShort());
		CHECK_EQUAL(nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_UPDATE_XP)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nXP		= 10210;
		const int nMoney	= 12311;
		const int nCharPtm	= 532;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_UPDATE_XP (%d, %I64d, %I64d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nXP, nMoney, nCharPtm));

		TDBRecordSet rsChar;
		UTestDB.Execute(rsChar, L"SELECT [EXP], [MONEY], CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %d;", nCID);
		CHECK_EQUAL(nXP, rsChar.Field(L"EXP").AsInt());
		CHECK_EQUAL(nMoney, rsChar.Field(L"MONEY").AsInt());
		CHECK_EQUAL(nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
		rsChar.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_MOVE_FIELD)
	{
		const int64		nAID			= 1;
		const int64		nCID			= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int		nXP				= 1;
		const uint8		nLevel			= 2;
		const int		nMoney			= 3;
		const int		nCharPtm		= 4;
		const int		nSharedFieldID	= 5;
		const float		fPosX			= 10.f;
		const float		fPosY			= 20.f;
		const float		fPosZ			= 30.f;

		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_ENTER_FIELD (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %f, %f, %f)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nXP, nLevel, nMoney, nCharPtm, nSharedFieldID, fPosX, fPosY, fPosZ);

		TDBRecordSet rsChar;
		UTestDB.Execute(rsChar, L"SELECT [EXP], LEV, [MONEY], CHAR_PTM, SHR_FIELD_ID, POS_X, POS_Y, POS_Z, DYN_FIELD_ID, DYN_FIELD_GROUP_UID \
								 , ENT_POS_X, ENT_POS_Y, ENT_POS_Z FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", nCID);
		CHECK_EQUAL(1, rsChar.GetFetchedRowCount());
		CHECK_EQUAL(nXP, rsChar.Field(L"EXP").AsInt());
		CHECK_EQUAL(nLevel, rsChar.Field(L"LEV").AsByte());
		CHECK_EQUAL(nMoney, rsChar.Field(L"MONEY").AsInt());
		CHECK_EQUAL(nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(nSharedFieldID, rsChar.Field(L"SHR_FIELD_ID").AsInt());
		CHECK_EQUAL(fPosX, rsChar.Field(L"POS_X").AsFloat());
		CHECK_EQUAL(fPosY, rsChar.Field(L"POS_Y").AsFloat());
		CHECK_EQUAL(fPosZ, rsChar.Field(L"POS_Z").AsFloat());
		CHECK_EQUAL(0, rsChar.Field(L"DYN_FIELD_ID").AsInt());
		CHECK_EQUAL(0, rsChar.Field(L"DYN_FIELD_GROUP_UID").AsInt64());
		CHECK_EQUAL(0.0f, rsChar.Field(L"ENT_POS_X").AsFloat());
		CHECK_EQUAL(0.0f, rsChar.Field(L"ENT_POS_Y").AsFloat());
		CHECK_EQUAL(0.0f, rsChar.Field(L"ENT_POS_Z").AsFloat());
	}
}