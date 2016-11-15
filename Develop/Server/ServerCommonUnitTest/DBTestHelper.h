#ifndef _TEST_DB_HELPER_H_
#define _TEST_DB_HELPER_H_

#include "ServerCommonUnitTestLib.h"
#include "CSChrInfo.h"

class SCOMMON_UNITTEST_API DBTestHelper
{
public:
	DBTestHelper();
	~DBTestHelper();

	static int64 MAIL_UID;

	static int64 InsertAccount(const wchar_t* szUserID = L"test_user", const wchar_t* szSITE_USER_ID = L"test_usc", const wchar_t* szPasswd = L"test_passwd");
	static int GetAID(const wchar_t* szAccountName);
	
	static int64 InsertCharacter(const int64 nAID, int nWorld, const wchar_t* szName = L"test_char", const int8 nTotalTP=0, SEX nSex = SEX_MALE, RACE nRace = RACE_HUMAN);
	static bool SetMoney(const int64 nGSN, const int64 nCSN, const int nMoney = 100);
	static int GetCID(const wchar_t* szCharName);

	static int64 InsertCharStorageItem(int WORLD, int64 GSN, int64 CSN, int SLOT_ID, int ITEM_ID, int16 STACK_AMT, uint8 DURA, uint8 MAX_DURA
		, int COLOR, uint8 SOUL_COUNT, uint8 CLAIMED, int CHAR_PTM);
	static void InsertEmptyCharStorage(int WORLD, int64 GSN, int64 CSN, int SLOT_ID);

	static int InsertItemInfo(const int nItemID, const wstring& strName = L"test_item", const int16 nStackAmt = 10, const bool bUnique = false, const bool bTradable = true
		, const uint8 nType = ITEMTYPE_NONE, const uint8 nSlot = ITEMSLOT_NONE, const uint8 nTier = ITEM_TIER_VERY_COMMON
		, const uint8 nMaxDura = 100, const int nBuyingPrice = 100, const int nSellingPrice = 25);
	
	static const uint64 InsertItem(int64 nAID, const uint64 nCID, int nWorld, const uint32 nItemID, const uint32 nStackAmt=1, const uint8 nSlotType = 1, const int16 nSlotID = 0, bool bClaimed=false
		, const int nUsagePeriod = 5, const int nAddExpiMin = 10);
	static bool IsBindedItem(const uint64 nIUID);
	static bool InsertEmptyItemSlot(int64 nAID, const int64 nCID, int nWorld, const uint8 nSlotType, const int16 nSlotID, const int64 nIUID = 0);
	static bool InsertEmptyItemEnch(int WORLD_ID, int64 IUID);

	static int64	InsertGuild(int nWorld, const wstring& strGuildName, int nStorageMoney = 0);
	static int	GetGID(const wchar_t* szGuildName);
	static bool	InsertGuildMember(int nWorld, const int64 nGID, int64 nAID, const int64 nCID, const uint8 nGrade);
	static int64 InsertGuildStorageItem(int WORLD, int64 GUILD_ID, int SLOT_ID, int ITEM_ID, int16 STACK_AMT, uint8 DURA, uint8 MAX_DURA
		, int COLOR, uint8 SOUL_COUNT);
	static void InsertEmptyGuildStorage(int WORLD, int64 GUILD_ID, int SLOT_ID);

	static int	InsertTalent(const int nTaltID, const wstring& strName = L"", const int16 nConsumeTP = 1, const bool bUntrainable = true);
	static bool	InsertTalentInstance(int nWorld, const int64 nAID, const int64 nCID, const int nTaltID);

	static int	InsertQuestInfo(const int nQID, const wstring& strName = L"test_quest", const bool bChallenge = false);
	static bool InsertQuest(int nWorld, const int64 nAID, const int64 nCID, const uint8 nSlotID, const int nQuestID = 0, const wstring& strAcceptDt = L"2010-01-01 00:00:00", const wstring& strExpiDt = L"2010-01-01 01:00:00");
	static bool	InsertQuestHistory(int nWorld, const int64 nAID, const int64 nCID, const int nQuestID, const wstring& strExpiDt = L"9999-12-31 04:30");

	static bool InsertPalette(int nWorld, const int64 nAID, const int64 nCID, const uint8 nSize);
	static bool SetPalette(const int64 nCID, const uint8 nOrdNum, uint8 nType, int nID);

	static bool	InsertCutscene(int nWorld, const int64 nAID, const int64 nCID, const int nCutScnID);

	static int	InsertEmblemInfo(const int nEmblID);
	static bool	InsertEmblem(int nWorld, const int64 nAID, const int64 nCID, const int nEmblID, const bool bSelected = false);

	static bool	InsertFaction(int nWorld, const int64 nAID, const int64 nCID, const int nFactID, const int16 nVal = 0);

	static bool	InsertRecp(int nWorld, const int64 nAID, const int64 nCID, const int nRecpID);

	static int64 InsertMail(int nWorld, const int64 nGSN, const int64 nCSN, const uint8 nType = 1, const wstring& strSndrName = L"", const wstring& strTitle = L"", const bool bHaveText = false
		, const int nMoney = 100, const bool bRead = false, const int nDefItemID = 1);

	static int64 InsertDeleteMail(int nWorld, const int64 nGSN, const int64 nCSN, const uint8 nType = 1, const wstring& strSndrName = L"", const wstring& strTitle = L"", const bool bHaveText = false
		, const int nMoney = 100, const bool bRead = false, const int nDefItemID = 1);

	static void InsertMailItem(int nWorld, const int64 nMAIL_UID, const int nSLOT_ID, const int64 nIUID, const int nITEM_ID, const int16 nSTACK_AMT, const uint8 nDURA, const uint8 nMAX_DURA
		, const int nENCH_ITEM_ID_1, const int nENCH_ITEM_ID_2, const int nENCH_ITEM_ID_3, const int nENCH_ITEM_ID_4, const int nENCH_ITEM_ID_5, const int nENCH_ITEM_ID_6
		, const int nDEL_REMAIN_DAY);
};

#endif
