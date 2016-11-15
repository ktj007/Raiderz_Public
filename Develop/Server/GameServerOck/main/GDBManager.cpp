#include "stdafx.h"
#include "GDBManager.h"
#include "GDef.h"
#include "GItemManager.h"
#include "GItemData.h"
#include "CCommandResultTable.h"
#include "GEntityPlayer.h"
#include "GGuildMgr.h"
#include "GGuild.h"
#include "GPaletteItem.h"
#include "GPalette.h"
#include "GPaletteSystem.h"
#include "GGlobal.h"
#include "MDBRecordSet.h"
#include "MDatabaseDesc.h"
#include "GDBTaskHeaderList.h"
#include "GQuestInfoMgr.h"
#include "GPlayerBattleArena.h"
#include "GSystem.h"
#include "GDBLogoutFieldBuilder.h"
#include "GPlayerFieldDynamic.h"
#include "GItemHolder.h"
#include "SAsyncDB.h"
#include "GConfig.h"
#include "SDsnFactory.h"
#include "SBaseDsnFactory.h"
#include "GPlayerDBCacheMgr.h"
#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GItemDBCacheMgr.h"
#include "GDBTaskData.h"
#include "GDBCacheData.h"
#include "GDBTaskDataTalent.h"
#include "GDBTaskNPCShopData.h"
#include "GDBTaskCharData.h"
#include "GDBTaskDataItemEquip.h"
#include "GDBTaskDataPalette.h"
#include "GDBTaskDataRecipe.h"
#include "GDBTaskDataFaction.h"
#include "GDBTaskDataEmblem.h"
#include "GDBTaskDataEffRemainTime.h"
#include "GDBTaskDataParty.h"
#include "GDBTaskDataConnection.h"
#include "MLocale.h"
#include "GConst.h"
#include "GMailSystem.h"
#include "GItemConvertPrefixedString.h"
#include "GEffRemainTimeSqlBuilder.h"
#include "GDBTaskNPCKillLogBuilder.h"
#include "GTimeCalculator.h"
#include "GDB_CODE.h"
#include "GPlayerObjectManager.h"
#include "GLogCacheContainer.h"
#include "GDBTaskStorageSerialize.h"
#include "GDBTaskStorageUpdateMoney.h"
#include "GDBTaskItemSplit.h"
#include "GEquipmentSlot.h"
#include "GDBTaskGuildUpdateStorageMoney.h"
#include "GDBCacheSystem.h"


#ifdef _DEBUG
#include "GPlayerObjectManager.h"
#endif

#define MAX_STR_PARAM_LEN 8000


// 계정 - 기본 정보 읽기
// 캐릭터
// 아이템
// 퀘스트

GDBManager::GDBManager() : SDBManager(), m_pGameDB(NULL), m_pGameDBSelectChar(NULL), m_pLogDB(NULL), m_nGameDBQCount(0), m_nGameDBSelectCharQCount(0), m_nLogDBQCount(0)
, m_pLogCaches(NULL)
{
}

GDBManager::~GDBManager()
{

}


//////////////////////////////////////////////////////////////////////////////////////////////////

// 계정 추가 - UserID, Name
wchar_t g_szDB_ACC_INSERT[] = L"{CALL dbo.USP_RZ_ACCOUNT_INSERT (N'%s', N'%s', N'%s', N'%s')}";
bool GDBManager::AccInsert(unsigned int& outAID, GDBT_ACC_INSERT& data)
{
	CString strSQL;
	strSQL.Format(g_szDB_ACC_INSERT, data.m_strUSER_ID.c_str(), data.m_strSITE_CODE.c_str(), data.m_strSITE_USER_ID.c_str(), data.m_strPWD.c_str());

	mdb::MDBRecordSet rs(&m_DB);

	if (Execute(rs, strSQL) == false)
	{
		return false;
	}
	if (rs.GetFetchedRowCount() <= 0)
	{
		_ASSERT(0);
		return false;
	}

	outAID = (uint32)rs.FieldW(L"ACCN_ID").AsInt64();

	return true;
}

// 계정 삭제 - AccountID
wchar_t g_szDB_DELETE_ACCOUNT[] = L"{CALL spDeleteAccount (%u)}";

bool GDBManager::AccDelete(const unsigned int nAID)
{
	//CString strSQL;
	//strSQL.Format(g_szDB_DELETE_ACCOUNT, nAID);

	//CODBCRecordset rs(&m_DB);

	//if (Execute(rs, strSQL) == false)
	//{
	//	return false;
	//}

	//return true;
	return false;
}


void GDBManager::CharSelectLog(const GDBT_CHAR_SELECT_LOG& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_CHAR_SELECT_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_CHAR_SELECT_LOG (%I64d, %d, %I64d, %d, %d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nMoney, data.m_nFieldID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}


void GDBManager::DisconnLogInsert(const GDBT_DISCONN_LOG& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_DISCONN_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_DISCONN_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nMoney, data.m_nSumDeltaMoney, data.m_nFieldID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

// 캐릭터 관련 ----------------------------------------

// 캐릭터 생성
wchar_t g_szDB_CHAR_INSERT[] = L"{CALL dbo.USP_RZ_CHAR_INSERT (%d, %I64d, N'%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}";
bool GDBManager::CharInsert(const MUID& uidPlayer
								 , const int nAID, const wchar_t* szName
								 , const uint8 nLevel, const int nXP, const int nMoney
								 , const int nRace, const int nSex
								 , const int nFeatureHair, const int nFeatureFace, const short nFeatureHairColor, const short nFeatureSkinColor, const int nVoice
								 , const int nHP, const int nEN, const int nSTA
								 , const int8 nTotalTP
								 , const int nFatigue
								 , const int nSoulBindingID
								 , const int nInnRoomID
								 , const int nPrimaryPalettedID, const int nSecondaryPalettedID
								 , const TALENT_STYLE nTalentStyle
								 , const uint8 nItemSet
								 , const int nItemColor)
{
	GDBTaskCharInsert* pTask = new GDBTaskCharInsert(uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQLInsertCharacter;
	strSQLInsertCharacter.Format(g_szDB_CHAR_INSERT
		, GConfig::m_nMyWorldID
		, (int64)nAID
		, szName
		, nLevel
		, nXP
		, nMoney
		, nRace
		, nSex
		, nFeatureHair
		, nFeatureFace
		, nFeatureHairColor
		, nFeatureSkinColor
		, nTotalTP
		, nHP
		, nEN
		, nSTA
		, nFatigue
		, nSoulBindingID
		, nInnRoomID
		, nPrimaryPalettedID
		, nSecondaryPalettedID
		, nTalentStyle
		, nItemSet
		, nItemColor);

	pTask->Input(nAID);
	pTask->PushSQL(strSQLInsertCharacter);

	return Post(pTask);
}

wchar_t g_szDB_CHAR_DELETE[] = L"{? = CALL dbo.USP_RZ_CHAR_DELETE (%d, %I64d, %I64d)}";
// 캐릭터 삭제
bool GDBManager::CharDelete(const GDBT_CHAR_DELETE& data)
{
	CString strSQL;
	strSQL.Format(g_szDB_CHAR_DELETE, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);

	GDBTaskCharDelete* pTask = new GDBTaskCharDelete(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	pTask->Input(data.m_nIndex);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}


// 계정 로그인 정보 받아오기
wchar_t g_szDB_LOGIN_GETINFO[] = L"{CALL dbo.USP_RZ_LOGIN_GET_INFO (N'%s', N'%s')}";
bool GDBManager::LoginGetInfo(GDBT_ACC_LOGIN& data)
{
	GDBTaskLogin* pTask = new GDBTaskLogin(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_LOGIN_GETINFO, data.m_strSITE_CODE.c_str(), data.m_strSITE_USER_ID.c_str());

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

wchar_t g_szDB_ITEM_DELETE[] = L"{CALL dbo.USP_RZ_ITEM_DELETE (%d, %I64d, %d, %d, %I64d)}";
bool GDBManager::ItemDelete( const GDBT_ITEM_DATA& data )
{
	GDBTaskItemDelete* pTask = new GDBTaskItemDelete(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_DELETE
		, GConfig::m_nMyWorldID
		, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID);	
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::ItemDeleteLog( const GDBT_ITEM_DATA& data )
{
	m_pLogCaches->m_ItemDeleteLogCache.PushBack(GLOG_DATA_ITEM_DELETE(data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, data.m_Item.m_nIUID, data.m_Item.m_nItemID
		, -data.m_nModStackAmt, data.m_Item.m_nMaxDura, 0, data.m_Item.m_strExpiDt, GTimeCalculator::GetTimeAsString(GTimeSec(0))), IsDirectPostItem(data.m_Item.m_nItemID));
}

// 캐릭터 아이템 장비하기
wchar_t g_szDB_ITEM_EQUIP[] = L"{CALL dbo.USP_RZ_ITEM_UPDATE_EQUIPMENT (%d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}";
bool GDBManager::ItemEquip(GDBT_ITEM_EQUIP& data)
{
	GDBTaskItemEquip* pTask = new GDBTaskItemEquip(data.uidPlayer);
	if (NULL == pTask)
		return false;

	uint8 nUneqSlotType1 = 0;
	uint8 nUneqSlotType2 = 0;

	if (data.unequip1.m_nSlotType > 0)
		nUneqSlotType1 = SLOTTYPE_INVENTORY;
	if (data.unequip2.m_nSlotType > 0)
		nUneqSlotType2 = SLOTTYPE_INVENTORY;

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_EQUIP
		, GConfig::m_nMyWorldID
		, data.nCID
		, data.equip.m_nSlotType, data.equip.m_nSlotID, data.equip.m_nIUID, data.equip.m_nDura, data.equip.m_bClaimed, data.equip.m_nToSlotID
		, data.unequip1.m_nSlotID, data.unequip1.m_nIUID, data.unequip1.m_nDura, data.unequip1.m_nToSlotID
		, data.unequip2.m_nSlotID, data.unequip2.m_nIUID, data.unequip2.m_nDura, data.unequip2.m_nToSlotID);

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 캐릭터 장비된 아이템 해제하기
wchar_t g_szDB_ITEM_UNEQUIP[] = L"{CALL dbo.USP_RZ_ITEM_UPDATE_EQUIPMENT (%d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}";
bool GDBManager::ItemUnequip(GDBT_ITEM_UNEQUIP& data)
{
	GDBTaskItemUnequip* pTask = new GDBTaskItemUnequip(data.uidPlayer);
	if (NULL == pTask)
		return false;

	uint8 nUneqSlotType1 = 0;
	uint8 nUneqSlotType2 = 0;

	if (data.unequip1.m_nSlotType > 0)
		nUneqSlotType1 = SLOTTYPE_INVENTORY;
	if (data.unequip2.m_nSlotType > 0)
		nUneqSlotType2 = SLOTTYPE_INVENTORY;

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_EQUIP
		, GConfig::m_nMyWorldID
		, data.nCID
		, 0, 0, (int64)0, 0, false, 0
		, data.unequip1.m_nSlotID, data.unequip1.m_nIUID, data.unequip1.m_nDura, data.unequip1.m_nToSlotID
		, data.unequip2.m_nSlotID, data.unequip2.m_nIUID, data.unequip2.m_nDura, data.unequip2.m_nToSlotID);

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 아이템 내구도 변경 - IUID, nDurability
wchar_t g_szDB_ITEM_UPDATE_DURABILITY[] = L"{CALL dbo.USP_RZ_ITEM_UPDATE_DURA (%d, %I64d, %d, %d, %I64d, %d)}";
bool GDBManager::ItemDescDurability(GDBT_ITEM_DEC_DURA_DATA& data)
{
	GDBTaskItemDecDura* pTask = new GDBTaskItemDecDura(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_UPDATE_DURABILITY
		, GConfig::m_nMyWorldID
		, data.m_nCID, data.m_nSlotType, data.m_nSlotID, data.m_nIUID, data.m_nDura);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::ItemDyeLog(const GDBT_ITEM_DYE& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_ITEM_DYE_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_ITEM_DYE_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %I64d, %d, %I64d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney
		, data.m_nTargetIUID, data.m_nTargetItemID
		, data.m_nDyeIUID, data.m_nDyeItemID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

// 아이템 염색 - IUID, nDyeColor
wchar_t g_szDB_ITEM_DYE[] = L"{CALL dbo.USP_RZ_ITEM_DYE (%d, %I64d, %d, %I64d, %d, %d, %I64d, %d)}";
bool GDBManager::ItemDye( const GDBT_ITEM_DYE& data )
{
	GDBTaskItemDye* pTask = new GDBTaskItemDye(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_DYE
		, GConfig::m_nMyWorldID
		, data.m_nCID
		, data.m_nDyeItemSlotID, data.m_nDyeIUID
		, data.m_nTargetItemSlotType, data.m_nTargetItemSlotID, data.m_nTargetIUID
		, data.m_nColor);

	pTask->PushSQL(strSQL);
	
	return Post(pTask);
}


// 아이템 영혼량 변경 - IUID, nSoulQuantity
wchar_t g_szDB_ITEM_UPDATE_SOUL_COUNT[] = L"{CALL dbo.USP_RZ_ITEM_UPDATE_SOUL_CNT (%d, %I64d, %d, %d, %I64d, %d)}";
bool GDBManager::ItemUpdateSoulCnt(const int64 nCID, const uint8 nSlotType, const int16 nSlotID, const int64 nIUID, const uint8 nSoulCnt)
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(0, (SDBTASK_ID)0);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_UPDATE_SOUL_COUNT, GConfig::m_nMyWorldID, nCID, nSlotType, nSlotID, nIUID, nSoulCnt);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_ITEM_LEARN_TALENT[] = L"{CALL dbo.USP_RZ_ITEM_LEARN_TALENT (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %d, %d)}";
bool GDBManager::ItemLearnTalent( GDBT_ITEM_LEARN_TALENT& data )
{
	GDBTaskItemLearnTalent* pTask = new GDBTaskItemLearnTalent(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_LEARN_TALENT
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID, data.m_nSlotType, data.m_nSlotID, data.m_nIUID, data.m_nItemID
		, data.m_nTalentID[0], data.m_nTalentID[1], data.m_nTalentID[2], data.m_nTalentID[3], data.m_nTalentID[4]);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}


bool GDBManager::CharAddXP( GDBT_CHAR_XP_DATA& data )
{	
	GDBTaskCharUpdateXP* pTask = new GDBTaskCharUpdateXP(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_CHAR_UPDATE_XP (%d, %I64d, %I64d, %d, %d, %d)}"
		, GConfig::m_nMyWorldID, data.m_nAID, (int64)data.m_nCID, data.m_nXP, data.m_nMoney, data.m_nCharPtm);
	
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::CharAddXPLog( GDBT_CHAR_XP_DATA& data )
{
	m_pLogCaches->m_XPLogCache.PushBack(GLOG_DATA_XP(data.m_nAID, data.m_nCID, GDB_CODE::CD_L_KILL_NPC_GAIN_XP, data.m_nCharPtm, data.m_nDeltaCharPtm
		, data.m_nLevel, data.m_nXP, data.m_nModXP
		, GTimeCalculator::GetTimeAsString(GTimeSec(0)), data.m_nNpcID));
}

// 캐릭터의 레벨 업데이트 - CID, XP, Level
wchar_t g_szDB_CHAR_LEVEL_UP[] = L"{CALL dbo.USP_RZ_CHAR_LEVEL_UP (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %f, %f, %f)}";
bool GDBManager::CharLevelUp( GDBT_CHAR_LEVEL_UP_DATA& data)
{
	GDBTaskCharLevelUp* pTask = new GDBTaskCharLevelUp(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_CHAR_LEVEL_UP
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID, data.m_nXP, data.m_nLevel, data.m_nMoney, data.m_nCharPtm, data.m_nRemainTP
		, data.m_vPos.x, data.m_vPos.y, data.m_vPos.z);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool GDBManager::CharLevelUpSync( GDBT_CHAR_LEVEL_UP_DATA& data )
{
	CString strSQL;
	strSQL.Format(g_szDB_CHAR_LEVEL_UP
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID, data.m_nXP, data.m_nLevel, data.m_nMoney, data.m_nCharPtm, data.m_nRemainTP
		, data.m_vPos.x, data.m_vPos.y, data.m_vPos.z);

	GDBTaskCharLevelUp Task(data.m_uidPlayer);

	Task.Input(data);

	Task.PushSQL(strSQL);
	Task.OnPrePost();
	Task.OnExecute(m_DB);
	Task.OnCompleted();

	return true;
}

void GDBManager::CharLevelUpLog( GDBT_CHAR_LEVEL_UP_DATA& data )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_CHAR_LEVEL_UP_LOG);
	if (NULL == pTask)
		return;

	CString strNpcID = L"NULL";
	if (0 < data.m_nNpcID)
		strNpcID.Format(L"%d", data.m_nNpcID);

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_CHAR_LEVEL_UP_LOG (%I64d, %d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s, %d, %f, %f, %f)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCode, data.m_strRegDate.c_str(), data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nXP, data.m_nModXP, strNpcID
		, data.m_nFieldID, data.m_vPos.x, data.m_vPos.y, data.m_vPos.z);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

wchar_t g_szDB_INN_REGISTROOM[] = L"{CALL dbo.USP_RZ_INN_ROOM_REGIST (%d, %I64d, %I64d, %d, %d)}";
bool GDBManager::CharRegistInnRoom( const GDBT_CHAR_REGIST_INN_ROOM& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_INNROOMREIST);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_INN_REGISTROOM, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nRoomID, data.m_nCharPtm);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

wchar_t g_szDB_CHAR_UPDATE_TP[] = L"{CALL dbo.USP_RZ_CHAR_UPDATE_TP (%d, %I64d, %I64d, %d, %d)}";
bool GDBManager::GM_CharUpdateTP(const int64 nAID, const int64 nCID, const int16 nRemainTP, const int16 nTotalTP, const int nCharPtm)
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(0, SDBTID_CHAR_UPDATE_TP);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_CHAR_UPDATE_TP, GConfig::m_nMyWorldID, nAID, nCID, nRemainTP, nCharPtm);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_NPCShop_BuyInsert[] =L"{CALL dbo.USP_RZ_ITEM_BUY_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d, %d, %d)}";
bool GDBManager::NPCShop_BuyInsert( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	GDBTaskItemBuyInsert* pTask = new GDBTaskItemBuyInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_NPCShop_BuyInsert
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt
		, data.m_Item.m_nDura, data.m_Item.m_nMaxDura, data.m_Item.m_nColor, data.m_Item.m_bClaimed, data.m_Item.m_bPeriodItem
		, data.m_Item.m_nUsagePeriod, data.m_Item.m_strExpiDt.c_str(), data.m_Item.m_nCharPtm, data.m_nXP, data.m_nMoney);
	pTask->PushSQL(strSQL);

	return Post(pTask);	
}

const wchar_t g_szDB_NPCShop_BuyUpdate[] = L"{CALL dbo.USP_RZ_ITEM_BUY_UPDATE (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d, %d)}";
bool GDBManager::NPCShop_BuyUpdate( const GDBT_NPC_SHOP_TRADE_DATA& data)
{
	GDBTaskItemBuyUpdate* pTask = new GDBTaskItemBuyUpdate(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_NPCShop_BuyUpdate
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID, data.m_Item.m_nStackAmt
		, data.m_Item.m_nCharPtm, data.m_nXP, data.m_nMoney);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::NPCShop_BuyLog( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	m_pLogCaches->m_NpcShopLogCache.PushBack(GLOG_DATA_NPC_SHOP(data.m_nAID, data.m_Item.m_nCID
		, GDB_CODE::CD_L_NPC_SHOP_BUY, data.m_Item.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, -data.m_nModMoney
		, data.m_Item.m_nIUID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt, data.m_nModStackAmt,GTimeCalculator::GetTimeAsString(GTimeSec(0)))
		, IsDirectPostItem(data.m_Item.m_nItemID));
}

const wchar_t g_szDB_NPCShop_SellPartUpdate[] = L"{CALL dbo.USP_RZ_ITEM_SELL_UPDATE (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d, %d)}";
bool GDBManager::NPCShop_SellPartUpdate( const GDBT_NPC_SHOP_TRADE_DATA& data)
{
	GDBTaskItemSellUpdate* pTask = new GDBTaskItemSellUpdate(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_NPCShop_SellPartUpdate
		, GConfig::m_nMyWorldID
		, data.m_nAID,  data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID
		, data.m_Item.m_nStackAmt, data.m_Item.m_nCharPtm, data.m_nXP, data.m_nMoney);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_NPCShop_SellPartDelete[] = L"{CALL dbo.USP_RZ_ITEM_SELL_DELETED (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d)}";
bool GDBManager::NPCShop_SellPartDelete( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	GDBTaskItemSellDelete* pTask = new GDBTaskItemSellDelete(data.m_uidPlayer);
	if (NULL == pTask)
		return FALSE;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_NPCShop_SellPartDelete
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID
		, data.m_Item.m_nCharPtm, data.m_nXP, data.m_nMoney);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool GDBManager::MakeSellVeryCommonList(GITEM_STACK_AMT_VEC& vec, wstring& strString)
{
	GItemConvertPrefixedString cov;

	wstring str;
	for each (const GITEM_STACK_AMT& st in vec)
	{
		 str = cov.Convert((SH_ITEM_SLOT_TYPE)st.nSlotType, st.nSlotID);
		if (GItemConvertPrefixedString::m_strFalse == str)
			return false;

		strString += str;
	}

	return true;
}

const wchar_t g_szDB_NPCShop_SellVeryCommon[] = L"{CALL dbo.USP_RZ_ITEM_SELL_VERY_COMMON (%d, %I64d, %I64d, '%s', %d, %d, %d, %d)}";
bool GDBManager::NPCShop_SellVeryCommon( GDBT_SELL_ITEM_VERY_COMMON& data )
{
	wstring strString;
	if (!MakeSellVeryCommonList(data.m_vVeryComm, strString))
		return false;

	GDBTaskItemSellVeryCommon* pTask = new GDBTaskItemSellVeryCommon(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	const size_t nItemInstCnt = data.m_vVeryComm.size();

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_NPCShop_SellVeryCommon
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID, strString.c_str(), nItemInstCnt, data.m_nCharPtm, data.m_nXP, data.m_nMoney);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::NPCShop_SellLog( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	m_pLogCaches->m_NpcShopLogCache.PushBack(GLOG_DATA_NPC_SHOP(data.m_nAID, data.m_Item.m_nCID
		, GDB_CODE::CD_L_NPC_SHOP_SELL, data.m_Item.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, data.m_nModMoney
		, data.m_Item.m_nIUID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt, -data.m_nModStackAmt,GTimeCalculator::GetTimeAsString(GTimeSec(0)))
		, IsDirectPostItem(data.m_Item.m_nItemID));
	
}

void GDBManager::NPCShop_SellVeryCommonLog( GDBT_SELL_ITEM_VERY_COMMON& data )
{
	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	m_pLogCaches->m_NpcShopLogCache.PushBack(GLOG_DATA_NPC_SHOP(data.m_nAID, data.m_nCID
		, GDB_CODE::CD_L_NPC_SHOP_SELL_VC, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, data.m_nModMoney
		, 0, 0, 0, 0,strRegDate));
	
	for each (const GITEM_STACK_AMT& amt in data.m_vVeryComm)
	{
		m_pLogCaches->m_NpcShopLogCache.PushBack(GLOG_DATA_NPC_SHOP(data.m_nAID, data.m_nCID
			, GDB_CODE::CD_L_NPC_SHOP_SELL_VC, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, 0
			, amt.nIUID, amt.nItemID, amt.nToStackAmt, -amt.nModStackAmt,strRegDate)
			, IsDirectPostItem(amt.nItemID));
	}
}

const wchar_t g_szDB_ITEM_REPAIR[] = L"{CALL dbo.USP_RZ_ITEM_REPAIR (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d)}";
bool GDBManager::NPCShop_Repair( GDBT_REPAIR_ITEM& data )
{
	GDBTaskItemRepair* pTask = new GDBTaskItemRepair(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_REPAIR
		, GConfig::m_nMyWorldID
		, data.m_Char.m_nAID, data.m_Char.m_nCID, data.m_nSlotType, data.m_nSlotID, data.m_nIUID, data.m_Char.m_nCharPtm, data.m_Char.m_nXP, data.m_Char.m_nMoney);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool GDBManager::MakeRepairAllList(GDBT_ITEM_REPAIR_VEC& vec, wstring& strString)
{
	GItemConvertPrefixedString cov;

	wstring str;
	for each (const GDBT_ITEM_REPAIR_ALL& st in vec)
	{
		str = cov.Convert((SH_ITEM_SLOT_TYPE)st.m_nSlotType, st.m_nSlotID);
		if (GItemConvertPrefixedString::m_strFalse == str)
			return false;

		strString += str;
	}

	return true;
}


const wchar_t g_szDB_ITEM_REPAIR_ALL[] = L"{CALL dbo.USP_RZ_ITEM_REPAIR_ALL (%d, %I64d, %I64d, '%s', %d, %d, %d, %d)}";
bool GDBManager::NPCShop_RepairAll( GDBT_REPAIR_ALL_ITEM& data )
{
	if (0 == data.m_vRepair.size())
		return true;

	wstring strString;
	if (!MakeRepairAllList(data.m_vRepair, strString))
		return false;

	GDBTaskItemRepairAll* pTask = new GDBTaskItemRepairAll(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	const size_t nItemCnt = data.m_vRepair.size();

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_REPAIR_ALL
		, GConfig::m_nMyWorldID
		, data.m_Char.m_nAID, data.m_Char.m_nCID, strString.c_str(), nItemCnt, data.m_Char.m_nCharPtm, data.m_Char.m_nXP, data.m_Char.m_nMoney);
	pTask->PushSQL(strSQL);
	return Post(pTask);
}

void GDBManager::NPCShop_RepairLog( const GDBT_ITEM_REPAIR_CHAR& data )
{
	m_pLogCaches->m_NpcShopLogCache.PushBack(GLOG_DATA_NPC_SHOP(data.m_nAID, data.m_nCID
		, GDB_CODE::CD_L_NPC_SHOP_REPAIR, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, -data.m_nModMoney
		, 0, 0, 0, 0,GTimeCalculator::GetTimeAsString(GTimeSec(0))));
}

// 탤런트 훈련 - nCID, nTalentID
wchar_t g_szDB_TALENT_LEARN[] = L"{CALL dbo.USP_RZ_TALENT_LEARN (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d)}";
bool GDBManager::Training_Train( GDBT_TALENT& data )
{
	// TODO : 탤런트 기획 픽스되면, 하위 랭크의 탤런트 ID는 저장하지 않도록 변경

	GDBTaskTalentLearn* pTask = new GDBTaskTalentLearn(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_TALENT_LEARN, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nCharPtm, data.m_nTalentID, data.m_nRemainTP, data.m_bOnPalette, data.m_nLowRankTalentID, data.m_vNumAndSlot.size());

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool GDBManager::Training_TrainSync(GDBT_TALENT& data)
{
	CString strSQL;
	strSQL.Format(g_szDB_TALENT_LEARN, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nCharPtm, data.m_nTalentID, data.m_nRemainTP, data.m_bOnPalette, data.m_nLowRankTalentID, data.m_vNumAndSlot.size());

	GDBTaskTalentLearn Task(data.m_uidPlayer);
	Task.Input(data);
	Task.PushSQL(strSQL);

	Task.OnPrePost();

	Task.OnExecute(m_DB);
	Task.OnCompleted();

	return true;
}

// 모든 탤런트 초기화 - nCID
wchar_t g_szDB_TALENT_RESET_ALL[] = L"{CALL dbo.USP_RZ_TALENT_RESET_ALL (%d, %I64d, %I64d, %d, %d, %d, %d, %I64d)}";
bool GDBManager::Training_UntrainAll( GDBT_TALENT& data )
{
	// TODO : 배운 탤런트 목록 DB에서 관리 하게 되면 모든 탤런트 삭제

	GDBTaskTalentResetAll* pTask = new GDBTaskTalentResetAll(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_TALENT_RESET_ALL, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID, data.m_nCharPtm, data.m_nRemainTP, data.m_nResetAllTalentCountOnPalette, data.m_nSlotID, data.m_nIUID);

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::TalentLearnLog( GDBT_TALENT& data )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_TALENT_LEARN_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_TALENT_LEARN_LOG (%I64d, %d, %I64d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nTalentID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

void GDBManager::TalentResetLog(GDBT_TALENT& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_TALENT_RESET_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_TALENT_RESET_LOG (%I64d, %d, %I64d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nTalentID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}


const wchar_t g_szDB_CHAR_LOOT_MONEY[] = L"{CALL dbo.USP_RZ_CHAR_UPDATE_MONEY (%d, %I64d, %I64d, %d, %d, %d)}";
bool GDBManager::LootIncreaseMoney(const GDBTASKDATA_LOOTINCREASEMONEY& taskData, const GDBCACHEDATA_CHARACTER& cacheData)
{
	GDBTaskCharIncMoney* pTask = new GDBTaskCharIncMoney(taskData.uidPlayer, SDBTID_CHARLOOTMONEY);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(
		GDBT_CHAR_MONEY_INC_DEC(taskData.uidPlayer, taskData.nAID, taskData.nCID, GDB_CODE::CD_L_MONEY_LOOT, taskData.nCharPlayTime, taskData.nDeltaCharPtm
		, taskData.nLevel, taskData.nXP, taskData.nMoney, taskData.nModMoney, false, taskData.nNpcID))) 
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_CHAR_LOOT_MONEY
		, GConfig::m_nMyWorldID
		, taskData.nAID, taskData.nCID, taskData.nMoney, cacheData.nXP, taskData.nCharPlayTime); 

	pTask->PushSQL(strSQL);

	return Post(pTask);	
}

bool GDBManager::PartyLootIncreaseMoney(const vector<GDBTASKDATA_LOOTINCREASEMONEY>& vecTaskData, const vector<GDBCACHEDATA_CHARACTER>& vecCacheData)
{
	if (vecTaskData.size() != vecCacheData.size())
		return false;

	for (size_t i = 0; i < vecTaskData.size(); ++i)
	{
		GDBTaskCharIncMoney* pTask = new GDBTaskCharIncMoney(vecTaskData[i].uidPlayer, SDBTID_CHARLOOTMONEY);
		if (NULL == pTask)
			return false;

		if (!pTask->Input(GDBT_CHAR_MONEY_INC_DEC(vecTaskData[i].uidPlayer, vecTaskData[i].nAID, vecTaskData[i].nCID, GDB_CODE::CD_L_MONEY_LOOT
				, vecTaskData[i].nCharPlayTime, vecTaskData[i].nDeltaCharPtm, vecTaskData[i].nLevel, vecTaskData[i].nXP
				, vecTaskData[i].nMoney, vecTaskData[i].nModMoney, true, vecTaskData[i].nNpcID)))
		{
			SAFE_DELETE(pTask);
			continue;;
		}
		
		CString strSQL;
		strSQL.Format(g_szDB_CHAR_LOOT_MONEY, vecTaskData[i].nCID, vecTaskData[i].nMoney, vecCacheData[i].nXP, vecTaskData[i].nCharPlayTime);
		pTask->PushSQL(strSQL);

		Post(pTask);
	}

	return true;
}

// 바인딩 위치 저장
wchar_t g_szDB_SOUL_BINDING_SAVE[] = L"{CALL dbo.USP_RZ_SOUL_BINDING_SAVE (%d, %I64d, %I64d, %u, %d)}";
bool GDBManager::SoulBindingSave(const GDBT_CHAR_SAVE_SOUL_BINDING& data)
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_SOULBINDINGSAVE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_SOUL_BINDING_SAVE
		, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nSoulBindingID, data.m_nCharPtm);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::UpdateDB_FromItemInfo()
{
	if (!IsRunForTest())
	{
		return;
	}

	return; // 사용되지 않는 코드 (교성씨 백)
#ifdef _DEBUG
	return;
#endif

	// 파일로 저장하고 DB에 입력은 수동으로 한다.
	MLogger logger;
	logger.Init(MLOG_LEVEL_DEBUG, MLOG_FILE, "system/item.sql.txt");

	for (GItemManager::GItemDataMap::iterator itor = gmgr.pItemManager->Begin(); itor != gmgr.pItemManager->End(); ++itor)
	{
		GItemData* pItemData = (*itor).second;
		
		CString strSQL;
		strSQL.Format(L"{CALL spInsertItemData (%d, '%s', '%s', %d, %d, %d, %d)}",
			pItemData->m_nID, 
			pItemData->m_strName.c_str(), 
			pItemData->m_strDesc.c_str(),
			pItemData->m_nStackAmount,
			pItemData->m_nMaxDurability,
			pItemData->m_nBaseBuyingPrice,
			pItemData->m_nBaseSellingPrice);

		logger.Output(strSQL);
		logger.Output(L"\n");

		Execute( strSQL );
	}

	logger.Output(L"\n\n\n");

	for (GItemManager::GItemDataMap::iterator itor = gmgr.pItemManager->Begin(); itor != gmgr.pItemManager->End(); ++itor)
	{
		GItemData* pItemData = (*itor).second;

		CString strSQL;
		strSQL.Format(L"UPDATE Item SET Name='%s' , Description='%s' , StackAmount=%d , MaxDurability=%d, BuyPrice=%d, SellPrice=%d WHERE ItemID=%d",
			pItemData->m_strName.c_str(), 
			pItemData->m_strDesc.c_str(), 
			pItemData->m_nStackAmount,
			pItemData->m_nMaxDurability,
			pItemData->m_nBaseBuyingPrice,
			pItemData->m_nBaseSellingPrice,
			pItemData->m_nID);

		logger.Output(strSQL);
		logger.Output(L"\n");

		Execute( strSQL );
	}

}

void GDBManager::DeleteItemInstanceForDebug(int nItemID)
{
#ifndef _DEBUG
	return;
#endif

	// 따로 DB에 접속한다. --

	mdb::MDatabase db;
	if(db.Connect(m_DBDesc.BuildDSNString()) == false) return;

	// 바인딩 위치 저장
	wchar_t g_szDB_DELETE_ITEM_INSTANCE_FROM_ITEMID[] = L"DELETE FROM ItemInstance WHERE ItemID=%d";

	CString strSQL;
	strSQL.Format(g_szDB_DELETE_ITEM_INSTANCE_FROM_ITEMID, nItemID);

	mdb::MDatabaseQuery dbq(&db);
	dbq.Execute(MLocale::ConvUTF16ToAnsi(strSQL.GetString()));
}

// 보관함 가져오기
wchar_t g_szDB_STORAGE_GET[] = L"{CALL dbo.USP_RZ_STORAGE_GET_ITEM (%d, %I64d, %d)}";
bool GDBManager::StorageSerialize(const MUID& uidPlayer, const int64 nCID)
{
	GDBTaskStorageSerialize* pTask = new GDBTaskStorageSerialize(uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_STORAGE_GET, GConfig::m_nMyWorldID, nCID, SLOTTYPE_STORAGE);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 보관함 돈 갱신
wchar_t g_szDB_STORAGE_UPDATE_MONEY[] = L"{CALL dbo.USP_RZ_CHAR_UPDATE_STORAGE_MONEY (%d, %I64d, %I64d, %d, %d)}";
bool GDBManager::StorageUpdateMoney(const GDBT_STORAGE_UPDATE_MONEY& data)
{
	GDBTaskStorageUpdateMoney* pTask = new GDBTaskStorageUpdateMoney(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_STORAGE_UPDATE_MONEY, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nMoney, data.m_nStorageMoney);
	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 길드 가져오기
wchar_t g_szDB_GUILD_GET[] = L"{CALL dbo.USP_RZ_GUILD_GET_INFO (%d, %I64d)}";
// 길드 멤버 가져오기
wchar_t g_szDB_GUILD_GET_MEMBER_LIST[] = L"{CALL dbo.USP_RZ_GUILD_GET_MEMBER (%d, %I64d)}";
bool GDBManager::GuildSerialize( const MUID& uidPlayer, const int64 nGID )
{
	GDBTaskGuildSerialize* pTask = new GDBTaskGuildSerialize(uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL_GuildInfo;
	CString strSQL_GuildMemberList;
	CString strSQL_GuildStorage;

	strSQL_GuildInfo.Format(g_szDB_GUILD_GET, GConfig::m_nMyWorldID, nGID);
	strSQL_GuildMemberList.Format(g_szDB_GUILD_GET_MEMBER_LIST, GConfig::m_nMyWorldID, nGID);
	strSQL_GuildStorage.Format(g_szDB_STORAGE_GET, GConfig::m_nMyWorldID, nGID, SLOTTYPE_GUILDSTORAGE);


	pTask->Input(nGID);

	pTask->PushSQL(strSQL_GuildInfo);
	pTask->PushSQL(strSQL_GuildMemberList);
	pTask->PushSQL(strSQL_GuildStorage);

	return Post(pTask);
}

// 길드 생성
wchar_t g_szDB_GUILD_INSERT[] = L"{CALL dbo.USP_RZ_GUILD_INSERT (%d, %I64d, %I64d, %d, N'%s')}";
bool GDBManager::GuildInsert(GDBT_GUILD& data)
{
	GDBTaskGuildInsert* pTask = new GDBTaskGuildInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_GUILD_INSERT, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nMoney, data.m_strName.c_str());

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 길드 멤버 추가
wchar_t g_szDB_GUILD_MEMBER_INSERT[] = L"{CALL dbo.USP_RZ_GUILD_MEMBER_INSERT (%d, %I64d, %I64d, %I64d)}";
bool GDBManager::GuildMemberInsert(GDBT_GUILD& data)
{
	GDBTaskGuildMemberInsert* pTask = new GDBTaskGuildMemberInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_GUILD_MEMBER_INSERT, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nGID);

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);	
}

// 길드 멤버 삭제
wchar_t g_szDB_GUILD_MEMBER_DELETE[] = L"{CALL dbo.USP_RZ_GUILD_MEMBER_DELETE (%d, %I64d, %I64d, %I64d)}";
bool GDBManager::GuildDeleteMember(GDBT_GUILD& data)
{
	GDBTaskGuildMemberDelete* pTask = new GDBTaskGuildMemberDelete(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_GUILD_MEMBER_DELETE
		, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nGID);

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 길드 해체
wchar_t g_szDB_GUILD_DELETE[] = L"{CALL dbo.USP_RZ_GUILD_DELETE (%d, %I64d, %I64d, %I64d)}";
bool GDBManager::GuildDelete(GDBT_GUILD& data)
{
	GDBTaskGuildDelete* pTask = new GDBTaskGuildDelete(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_GUILD_DELETE, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nGID);

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 길마 변경
wchar_t g_szDB_GUILD_MASTER_CHANGE[] = L"{CALL dbo.USP_RZ_GUILD_CHANGE_MASTER (%d, %I64d, %I64d, %I64d)}";
bool GDBManager::GuildChangeMaster(GDBT_GUILD_CHANGE_MASTER& data)
{
	GDBTaskGuildMasterChange* pTask = new GDBTaskGuildMasterChange(data.m_uidPreMaster);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_GUILD_MASTER_CHANGE
		, GConfig::m_nMyWorldID, data.m_NewMaster.m_nGID, data.m_nPreMasterCID, data.m_NewMaster.m_nCID);

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::GuildLog(GDBT_GUILD& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_GUILD_LOG);
	if (NULL == pTask)
		return;

	wstring strNAME = L"NULL";
	if (!data.m_strName.empty())
		strNAME = wstring(L"N'") + data.m_strName + wstring(L"'");
	
	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_GUILD_LOG (%I64d, %d, %I64d, %d, %I64d, %s)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCODE, data.m_nGID, strNAME.c_str());

	pTask->PushSQL(strSQL);

	Post(pTask);
}

void GDBManager::GuildMemberLog(GDBT_GUILD& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_GUILD_MEMBER_LOG);
	if (NULL == pTask)
		return;

	wstring strNAME = L"NULL";
	if (!data.m_strName.empty())
		strNAME = wstring(L"N'") + data.m_strName + wstring(L"'");

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_GUILD_MEMBER_LOG (%I64d, %d, %I64d, %d, %I64d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCODE, data.m_nGID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

// 길드 보관함 돈 갱신
wchar_t g_szDB_GUILD_UPDATE_STORAGEMONEY[] = L"{CALL dbo.USP_RZ_GUILD_UPDATE_STORAGE_MONEY (%d,\
											  %I64d, %d, %d,\
											  %I64d, %d, %d)}";
bool GDBManager::GuildUpdateStorageMoney( GDBT_GUILD_UPDATE_STORAGEMONEY& data )
{
	GDBTaskGuildUpdateStorageMoney* pTask = new GDBTaskGuildUpdateStorageMoney(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	int nBefMoney = 0;
	int nBefStorageMoney = 0;

	if (GDBT_GUILD_UPDATE_STORAGEMONEY::DEPOSIT == data.m_nType)
	{
		nBefMoney = (data.m_nMoney + data.m_nModMoney) - gsys.pDBCacheSystem->GetCharCacheModMoney(data.m_uidPlayer);
		nBefStorageMoney = data.m_nStorageMoney - data.m_nModMoney;
	}
	else if (GDBT_GUILD_UPDATE_STORAGEMONEY::WITHDRAW == data.m_nType)
	{
		nBefMoney = (data.m_nMoney - data.m_nModMoney) - gsys.pDBCacheSystem->GetCharCacheModMoney(data.m_uidPlayer);
		nBefStorageMoney = data.m_nStorageMoney + data.m_nModMoney;
	}

	CString strSQL;
	strSQL.Format(g_szDB_GUILD_UPDATE_STORAGEMONEY, GConfig::m_nMyWorldID,
		data.m_nCID, nBefMoney, data.m_nMoney, 
		data.m_nGID, nBefStorageMoney, data.m_nStorageMoney);
	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}


// 팔레트에 아이템이나 탤런트를 올린다.
wchar_t g_szDB_PALETTE_SET[] = L"{CALL dbo.USP_RZ_PALETTE_SET (%d, %I64d, %I64d, %d, %d, %d)}";
bool GDBManager::PalettePutUp( const GDBT_PALETTE_SET& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_PALETTEPUTUP);
	if (NULL == pTask)
		return false;
	
	uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(data.m_Slot.m_nNum, data.m_Slot.m_nSlot);

	CString strSQL;
	strSQL.Format(g_szDB_PALETTE_SET, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, nIndex, data.m_Slot.m_nID, data.m_Slot.m_nIdType);

	pTask->PushSQL(strSQL);
	
	return Post(pTask);
}

// 팔레트에 올려진걸 내린다.
wchar_t g_szDB_PALETTE_RESET[] = L"{CALL dbo.USP_RZ_PALETTE_RESET (%d, %I64d, %I64d, %d)}";
bool GDBManager::PalettePutDown( const GDBT_PALETTE_RESET& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_PALETTEDOWN);
	if (NULL == pTask)
		return false;

	uint8 nIndex = gsys.pPaletteSystem->NumAndSlotToIndex(data.m_nNum, data.m_nSlot);

	CString strSQL;
	strSQL.Format(g_szDB_PALETTE_RESET, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, nIndex);
	
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

// 팔레트에 올려진걸 서로 바꾼다.
wchar_t g_szDB_PALETTE_CHANGE[] = L"{CALL dbo.USP_RZ_PALETTE_CHANGE (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d)}";
bool GDBManager::PaletteChange( const GDBT_PALETTE_CHANGE& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_PALETTECHANGE);
	if (NULL == pTask)
		return false;

	uint8 nIndex1 = gsys.pPaletteSystem->NumAndSlotToIndex(data.m_Slot.m_nNum, data.m_Slot.m_nSlot);
	uint8 nIndex2 = gsys.pPaletteSystem->NumAndSlotToIndex(data.m_ToSlot.m_nNum, data.m_ToSlot.m_nSlot);

	CString strSQL;
	strSQL.Format(g_szDB_PALETTE_CHANGE
		, GConfig::m_nMyWorldID
		, data.m_nAID
		, data.m_nCID
		, nIndex1, data.m_Slot.m_nID, data.m_Slot.m_nIdType
		, nIndex2, data.m_ToSlot.m_nID, data.m_ToSlot.m_nIdType);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}


void GDBManager::Update()
{
	if (NULL != m_pLogCaches)
		m_pLogCaches->Update();
	
	if (NULL != m_pGameDB)
		m_pGameDB->Update();

	if (NULL != m_pGameDBSelectChar)
		m_pGameDBSelectChar->Update();

	if (NULL != m_pLogDB)
		m_pLogDB->Update();
}


bool GDBManager::InitAsyncDB()
{
	_ASSERT(NULL == m_pGameDB);
	_ASSERT(NULL == m_pLogDB);

	if (!InitGameDB())
		return false;

	if (!InitGameDBSelectChar())
		return false;

	if (!InitLogDB())
		return false;

	InitLogCache();
	RegistTraceTaskID();

	_ASSERT(NULL != m_pGameDB);
	_ASSERT(NULL != m_pLogDB);

	return true;
}


void GDBManager::Release()
{
	MLog1("START DB RELEASE\n");

	LogingDBTaskExecCnt();

	ReleaseLogCaches();
	ReleaseGameDB();
	ReleaseGameDBSelectChar();
	ReleaseLogDB();	

	MLog1("COMPLETED DB RELEASE\n");
}

const wchar_t g_szDB_CHAR_GETINFO[]					= L"{CALL dbo.USP_RZ_CHAR_GET_INFO (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_ITEM[]				= L"{CALL dbo.USP_RZ_CHAR_GET_ITEM (%d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_TALENT[]				= L"{CALL dbo.USP_RZ_CHAR_GET_TALENT (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_PALETTE[]				= L"{CALL dbo.USP_RZ_CHAR_GET_PALETTE (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_EFFECT_REMAIND_TIME[]	= L"{CALL dbo.USP_RZ_CHAR_GET_EFF_REMAIN_TIME (%d, %I64d, %I64d, %d)}";
const wchar_t g_szDB_CHAR_GET_QUEST[]				= L"{CALL dbo.USP_RZ_CHAR_GET_QUEST (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_QUEST_HISTORY[]		= L"{CALL dbo.USP_RZ_CHAR_GET_QUEST_HISTORY (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_FACTION[]				= L"{CALL dbo.USP_RZ_CHAR_GET_FACTION (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_CUTSCENE[]			= L"{CALL dbo.USP_RZ_CHAR_GET_CUTSECNE (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_RECIPE[]				= L"{CALL dbo.USP_RZ_CHAR_GET_RECIPE (%d, %I64d, %I64d)}";
const wchar_t g_szDB_CHAR_GET_EMBLEM[]				= L"{CALL dbo.USP_RZ_CHAR_GET_EMBLEM (%d, %I64d, %I64d)}";
const wchar_t g_szDB_MAIL_GET_SUMMARY[]				= L"{CALL dbo.USP_RZ_MAIL_GET_SUMMARY (%d, %I64d, %I64d)}";
bool GDBManager::CharSerialize(const GDBT_CHAR_SERIALIZE& data)
{
	CString strSQLCharGetInfo;
	CString strSQLCharGetItem;
	CString strSQLCharGetTalent;
	CString strSQLCharGetPalette;
	CString strSQLCharGetEffectRemainTime;
	CString strSQLCharGetQuest;
	CString strSQLCharGetQuestHistory;
	CString strSQLCharGetFaction;
	CString strSQLCharGetCutscene;
	CString strSQLCharGetRecipe;
	CString strSQLCharGetEmblem;
	CString strSQLMailGetSummary;

	strSQLCharGetInfo.Format(g_szDB_CHAR_GETINFO, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetItem.Format(g_szDB_CHAR_GET_ITEM, GConfig::m_nMyWorldID, data.m_nCID);
	strSQLCharGetTalent.Format(g_szDB_CHAR_GET_TALENT, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetPalette.Format(g_szDB_CHAR_GET_PALETTE, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetEffectRemainTime.Format(g_szDB_CHAR_GET_EFFECT_REMAIND_TIME, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, 0);
	strSQLCharGetQuest.Format(g_szDB_CHAR_GET_QUEST, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetQuestHistory.Format(g_szDB_CHAR_GET_QUEST_HISTORY, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetFaction.Format(g_szDB_CHAR_GET_FACTION, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetCutscene.Format(g_szDB_CHAR_GET_CUTSCENE, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetRecipe.Format(g_szDB_CHAR_GET_RECIPE, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLCharGetEmblem.Format(g_szDB_CHAR_GET_EMBLEM, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);
	strSQLMailGetSummary.Format(g_szDB_MAIL_GET_SUMMARY, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID);

	GDBTaskCharSerialize* pTask = new GDBTaskCharSerialize(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	pTask->Input(data.m_nCID, data.m_nUIID, data.m_bReload);
	
	pTask->PushSQL(strSQLCharGetInfo)
			.PushSQL(strSQLCharGetItem)
			.PushSQL(strSQLCharGetTalent)
			.PushSQL(strSQLCharGetPalette)
			.PushSQL(strSQLCharGetEffectRemainTime)
			.PushSQL(strSQLCharGetQuest)
			.PushSQL(strSQLCharGetQuestHistory)
			.PushSQL(strSQLCharGetFaction)
			.PushSQL(strSQLCharGetCutscene)
			.PushSQL(strSQLCharGetRecipe)
			.PushSQL(strSQLCharGetEmblem)
			.PushSQL(strSQLMailGetSummary);

	return Post(pTask);		
}


static const wchar_t g_szDB_CHAR_GET_SIMPLE_INFO[] = L"{CALL dbo.USP_RZ_CHAR_GET_SIMPLE_INFO (%d, %I64d)}";
static const wchar_t g_szDB_CHAR_GET_EQUIPMENT_INFO[] = L"{CALL dbo.USP_RZ_CHAR_GET_EQUIPMENT_INFO (%d, %I64d)}";
bool GDBManager::CharGetLookList(const MUID& uidPlayer, const int64 nAID)
{
	GDBTaskCharGetLookList* pTask = new GDBTaskCharGetLookList(uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQLCharGetSimpleInfo;
	CString strSQLCharGetEquipmentInfo;

	strSQLCharGetSimpleInfo.Format(g_szDB_CHAR_GET_SIMPLE_INFO, GConfig::m_nMyWorldID, nAID);
	strSQLCharGetEquipmentInfo.Format(g_szDB_CHAR_GET_EQUIPMENT_INFO, GConfig::m_nMyWorldID, nAID);	

	pTask->PushSQL(strSQLCharGetSimpleInfo)
		.PushSQL(strSQLCharGetEquipmentInfo);	

	return Post(pTask);
}


bool GDBManager::PostEffRemainTimeInsert(const GDBT_EFF_REMAIN_TIME_INSERT& data, GEffRemainTimeSqlBuilder& efb)
{
	CString& strSQL = efb.BuildSQL(data.m_nAID, data.m_nCID);

	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_EFF_REMAIN_TIME_INSERT);
	if (NULL == pTask)
		return false;

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool GDBManager::EffRemainTimeInsert(const GDBT_EFF_REMAIN_TIME_INSERT& data)
{
	int idx = 0;
	int ORD_NUM = 0;
	GEffRemainTimeSqlBuilder efb;

	for each (const pair<int, float>& tt in data.m_vecTalentCoolTime)
	{
		efb.Set(idx, ORD_NUM, 0, tt.first, data.m_nCharPtm + (int)tt.second);

		++idx;
		++ORD_NUM;

		if (idx >= GEffRemainTimeSqlBuilder::MAX_EFF_REMAIN_COUNT)
		{
			idx = 0;

			if (!PostEffRemainTimeInsert(data, efb))
				return false;

			efb.Reset();
		}
	}

	for each (const REMAIN_BUFF_TIME& bt in data.m_vecBuffRemainTime)
	{
		efb.Set(idx, ORD_NUM, 1, bt.nID, data.m_nCharPtm + (int)bt.fRemainDurationSeconds);

		++idx;
		++ORD_NUM;

		if (idx >= GEffRemainTimeSqlBuilder::MAX_EFF_REMAIN_COUNT)
		{
			idx = 0;

			if (!PostEffRemainTimeInsert(data, efb))
				return false;

			efb.Reset();
		}
	}

	if (0 < idx)
	{
		if (!PostEffRemainTimeInsert(data, efb))
			return false;
	}

	return true;
}


wchar_t g_szDB_CHAR_LOGOUT[] = L"{CALL dbo.USP_RZ_CHAR_LOGOUT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %d, %f, %f, %f, \
							%I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d, %d)}";
bool GDBManager::Logout(int64 nAID, int64 nCID, GEntityPlayer* pPlayer, vector<REMAIN_BUFF_TIME>& vecBuffRemainTime, vector<pair<int, float>>& vecTalentCoolTime )
{
	_ASSERT(NULL != pPlayer);
	
	if (NULL == pPlayer)
		return false;

	PLAYER_INFO* pPlayerInfo = pPlayer->GetPlayerInfo();
	if (NULL == pPlayerInfo)
		return false;

	GDBT_EFF_REMAIN_TIME_INSERT data(nAID, pPlayer->GetUID(), nCID, pPlayerInfo->nPlayTimeSec, vecBuffRemainTime, vecTalentCoolTime);
	EffRemainTimeInsert(data);

	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(pPlayer->GetUID(), SDBTID_LOGOUT);
	if (NULL == pTask)
		return false;

	GDBLogoutFieldBuilder LogoutField(pPlayer);
	LogoutField.Build();	

	CString strSQL_UCharLastStatus;
	strSQL_UCharLastStatus.Format(g_szDB_CHAR_LOGOUT, 
		GConfig::m_nMyWorldID,
		nAID, 
		nCID, 
		pPlayer->GetPlayerInfo()->nXP, 
		pPlayer->GetPlayerInfo()->nLevel,
		pPlayer->GetPlayerInfo()->GetMoney(),
		pPlayer->GetHP(),
		pPlayer->GetEN(),
		pPlayer->GetSTA(),
		pPlayer->GetPlayerInfo()->nFatigueQuantity,
		pPlayer->GetItemHolder()->GetEquipment().GetWeaponSet(),
		pPlayer->GetPalette().GetSelectedPalette(), 
		pPlayer->GetPalette().GetPrimary(),
		pPlayer->GetPalette().GetSecondary(),
		LogoutField.GetPlayerPos().x,
		LogoutField.GetPlayerPos().y,
		LogoutField.GetPlayerPos().z,
		LogoutField.GetSharedFieldID(),
		LogoutField.GetEnterPos().x,
		LogoutField.GetEnterPos().y,
		LogoutField.GetEnterPos().z,
		LogoutField.GetDyanmicFieldGroupUID().Value,
		LogoutField.GetDynamicFieldID(),
		pPlayer->GetPlayerField().GetFieldDynamic().GetCheckpoint().nFieldID,
		pPlayer->GetPlayerField().GetFieldDynamic().GetCheckpoint().nID,
		pPlayer->GetPartyUID().Value,
		pPlayer->GetPlayerBattleArena().GetBattleArenaUID().Value,
		static_cast<int64>(0), // ArenaTeamUID		
		pPlayer->GetPlayerInfo()->nPlayTimeSec,
		CalculateElapsedDeadTimeSec(pPlayer) /* ElapsedDeadTimeSec : 죽은 시점으로부터 경과된 시간(초단위). 죽은적이 없는 캐릭터는 -1으로 넘겨주면 됨.*/		
		);

	pTask->PushSQL(strSQL_UCharLastStatus);
	///////////////////////////////////////////////////////////////////////////

	return Post(pTask);
}

int GDBManager::CalculateElapsedDeadTimeSec( GEntityPlayer* pPlayer )
{
	if (false == pPlayer->IsDead())
		return -1;

	int nElapsedDeadTimeSec = static_cast<int>((gsys.pSystem->GetNowTime() - pPlayer->GetPlayerInfo()->nDeadTime) / 1000);

	return nElapsedDeadTimeSec;
}



bool GDBManager::MakeQuestRemoveItemList(GITEM_STACK_AMT_VEC& vec, wstring& strString)
{
	wstring str;
	GItemConvertPrefixedString cov;
	for each(const GITEM_STACK_AMT& st in vec)
	{
		str = cov.Convert((SH_ITEM_SLOT_TYPE)st.nSlotType, st.nSlotID, st.nToStackAmt);
		if (GItemConvertPrefixedString::m_strFalse == str)
			return false;

		strString += str;
	}

	return true;
}

const wchar_t g_szDB_QuestDone[] = L"{CALL dbo.USP_RZ_QUEST_DONE (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, '%s', '%s'\
									, %d, %I64d, %d, %d, %d, %d, %d, %s\
									, %d, %I64d, %d, %d, %d, %d, %d, %s\
									, %d, %I64d, %d, %d, %d, %d, %d, %s\
									, %d, %I64d, %d, %d, %d, %d, %d, %s\
									, %d, %I64d, %d, %d, %d, %d, %d, %s\
									, '%s', %d)}";
bool GDBManager::QuestDone( GDBT_QEUST_DONE& data )
{
	wstring strString;
	if (!MakeQuestRemoveItemList(data.m_vecRemoveItem, strString))
		return false;

	GDBTaskQuestDone* pTask = new GDBTaskQuestDone(data.m_QuestComm.m_uidPlayer);
	if (NULL == pTask)
		return false;

	const size_t nQItemCnt = data.m_vecAddItem.size() + data.m_vecRemoveItem.size();

	vector<GDBT_QUEST_ADDITEM> vecAddItem;		
	for (size_t i=0; i < QREWARD_ITEM_COUNT; i++)
	{
		if (i < data.m_vecAddItem.size())
		{
			vecAddItem.push_back(data.m_vecAddItem[i]);
		}
		else
		{
			vecAddItem.push_back(GDBT_QUEST_ADDITEM());
		}
	}

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_QuestDone
		, GConfig::m_nMyWorldID
		, data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID, data.m_QuestComm.m_nSlotID, data.m_QuestComm.m_nQuestID, data.m_QuestComm.m_nCharPtm
		, data.m_QuestComm.m_nLevel, data.m_QuestComm.m_nXP, data.m_nMoney, data.m_QuestComm.m_strAcceptDt.c_str(), data.m_strExpiDt.c_str()
		, vecAddItem[0].m_nSlotID, vecAddItem[0].m_nIUID, vecAddItem[0].m_nItemID, vecAddItem[0].m_nStackAmt, vecAddItem[0].m_nMaxDura, vecAddItem[0].m_bClaimed, vecAddItem[0].m_bPeriod ,  L"NULL"
		, vecAddItem[1].m_nSlotID, vecAddItem[1].m_nIUID, vecAddItem[1].m_nItemID, vecAddItem[1].m_nStackAmt, vecAddItem[1].m_nMaxDura, vecAddItem[1].m_bClaimed, vecAddItem[1].m_bPeriod ,  L"NULL"
		, vecAddItem[2].m_nSlotID, vecAddItem[2].m_nIUID, vecAddItem[2].m_nItemID, vecAddItem[2].m_nStackAmt, vecAddItem[2].m_nMaxDura, vecAddItem[2].m_bClaimed, vecAddItem[2].m_bPeriod ,  L"NULL"
		, vecAddItem[3].m_nSlotID, vecAddItem[3].m_nIUID, vecAddItem[3].m_nItemID, vecAddItem[3].m_nStackAmt, vecAddItem[3].m_nMaxDura, vecAddItem[3].m_bClaimed, vecAddItem[3].m_bPeriod ,  L"NULL"
		, vecAddItem[4].m_nSlotID, vecAddItem[4].m_nIUID, vecAddItem[4].m_nItemID, vecAddItem[4].m_nStackAmt, vecAddItem[4].m_nMaxDura, vecAddItem[4].m_bClaimed, vecAddItem[4].m_bPeriod ,  L"NULL"
		, strString.c_str(), nQItemCnt);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::QuestDoneLog( GDBT_QEUST_DONE& data )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_QUEST_DONE_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_QUEST_DONE_LOG (%I64d, %d, %I64d, %d, %d, '%s')}"
		, data.m_QuestComm.m_nAID, GConfig::m_nMyWorldID, data.m_QuestComm.m_nCID, data.m_QuestComm.m_nCharPtm
		, data.m_QuestComm.m_nQuestID, data.m_QuestComm.m_strAcceptDt.c_str());

	pTask->PushSQL(strSQL);

	Post(pTask);

	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	if (0 < data.m_nDeltaLevel)
	{
		CharLevelUpLog(GDBT_CHAR_LEVEL_UP_DATA(data.m_QuestComm.m_nAID, data.m_QuestComm.m_uidPlayer, data.m_QuestComm.m_nCID, GDB_CODE::CD_L_QREWARD_XP_GAIN_LEVEL_UP
			, strRegDate, data.m_QuestComm.m_nXP, data.m_nIncXP, data.m_QuestComm.m_nLevel, data.m_nMoney, 0, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nDeltaCharPtm, 0 /*NPC_ID*/
			, data.m_QuestComm.m_nFieldID, data.m_vPos));
	}
	else if (0 < data.m_nIncXP)
	{
		CharAddXPLog(GDBT_CHAR_XP_DATA(data.m_QuestComm.m_nAID, data.m_QuestComm.m_uidPlayer, data.m_QuestComm.m_nCID, GDB_CODE::CD_L_QREWARD_XP_GAIN
			, strRegDate, data.m_QuestComm.m_nXP, data.m_nIncXP, data.m_QuestComm.m_nLevel, data.m_nMoney, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nDeltaCharPtm, 0));
	}	

	// reward money.
	if (0 != data.m_nIncMoney)
	{
		m_pLogCaches->m_QuestItemLogCache.PushBack(
			GLOG_DATA_QUEST_ITEM(data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID
			, GDB_CODE::CD_L_QREWARD_MONEY_GAIN, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nDeltaCharPtm, data.m_QuestComm.m_nLevel, data.m_nMoney, data.m_nIncMoney
			, 0, 0, 0, 0, strRegDate, 0, data.m_QuestComm.m_nQuestID, 0));
	}

	// reward item.
	for each (const GDBT_QUEST_ADDITEM& ai in data.m_vecAddItem)
	{
		for each (const pair<int, int64>& iuid in data.m_mapIUID)
		{
			if (iuid.first == ai.m_nSlotID)
			{
				const IUID nIUID = iuid.second;

				m_pLogCaches->m_QuestItemLogCache.PushBack(
					GLOG_DATA_QUEST_ITEM(data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID
					, GDB_CODE::CD_L_QREWARD_ITEM_GAIN, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nDeltaCharPtm, data.m_QuestComm.m_nLevel
						, data.m_nMoney, 0, nIUID, ai.m_nItemID, ai.m_nStackAmt, ai.m_nModStackAmt, strRegDate
						, 0, data.m_QuestComm.m_nQuestID, 0), IsDirectPostItem(ai.m_nItemID));
			}			
		}		
	}

	// remove quest item.
	for each (const GITEM_STACK_AMT& amt in data.m_vecRemoveItem)
	{
		m_pLogCaches->m_QuestItemLogCache.PushBack(GLOG_DATA_QUEST_ITEM(data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID
			, GDB_CODE::CD_L_QREWARD_ITEM_DEL, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nDeltaCharPtm, data.m_QuestComm.m_nLevel, data.m_nMoney, 0
			, amt.nIUID, amt.nItemID, amt.nToStackAmt, -amt.nModStackAmt
			, strRegDate, 0, data.m_QuestComm.m_nQuestID, 0), IsDirectPostItem(amt.nItemID));
	}
}

const wchar_t g_szDB_QuestAccept[] = L"{CALL dbo.USP_RZ_QUEST_ACCEPT (%d, %I64d, %I64d, %d, %d, %d, %d, '%s', '%s'\
											, %d, %I64d, %d, %d, %d, %d, %d, %s\
											, %d, %I64d, %d, %d, %d, %d, %d, %s\
											, %d, %I64d, %d, %d, %d, %d, %d, %s\
											, %d, %I64d, %d, %d, %d, %d, %d, %s\
											, %d, %I64d, %d, %d, %d, %d, %d, %s\
											, '%s', %d)}";
bool GDBManager::QuestAccept( GDBT_QUEST_ACCEPT& data )
{
	wstring strString;
	if (!MakeQuestRemoveItemList(data.m_vecRemoveItem, strString))
		return false;

	GDBTaskQuestAccept* pTask = new GDBTaskQuestAccept(data.m_QuestComm.m_uidPlayer);
	if (NULL == pTask)
		return false;

	const size_t nQItemCnt = data.m_vecAddItem.size() + data.m_vecRemoveItem.size();

	vector<GDBT_QUEST_ADDITEM> vecAddItem;	
	for (size_t i=0; i < QBEGIN_ITEM_COUNT; i++)
	{
		if (i < data.m_vecAddItem.size())
		{
			vecAddItem.push_back(data.m_vecAddItem[i]);
		}
		else
		{
			vecAddItem.push_back(GDBT_QUEST_ADDITEM());
		}
	}

	uint8 nType;
	if (data.m_bChallange)
		nType = 2;
	else
		nType = 1;

	CString strSQL;
	strSQL.Format(g_szDB_QuestAccept
		, GConfig::m_nMyWorldID
		, data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nSlotID, data.m_QuestComm.m_nQuestID, data.m_bComplete
		, data.m_QuestComm.m_strAcceptDt.c_str(), data.m_strExpiDt.c_str()
		, vecAddItem[0].m_nSlotID, vecAddItem[0].m_nIUID, vecAddItem[0].m_nItemID, vecAddItem[0].m_nStackAmt, vecAddItem[0].m_nMaxDura, vecAddItem[0].m_bClaimed, vecAddItem[0].m_bPeriod ,  L"NULL"
		, vecAddItem[1].m_nSlotID, vecAddItem[1].m_nIUID, vecAddItem[1].m_nItemID, vecAddItem[1].m_nStackAmt, vecAddItem[1].m_nMaxDura, vecAddItem[1].m_bClaimed, vecAddItem[1].m_bPeriod ,  L"NULL"
		, vecAddItem[2].m_nSlotID, vecAddItem[2].m_nIUID, vecAddItem[2].m_nItemID, vecAddItem[2].m_nStackAmt, vecAddItem[2].m_nMaxDura, vecAddItem[2].m_bClaimed, vecAddItem[2].m_bPeriod ,  L"NULL"
		, vecAddItem[3].m_nSlotID, vecAddItem[3].m_nIUID, vecAddItem[3].m_nItemID, vecAddItem[3].m_nStackAmt, vecAddItem[3].m_nMaxDura, vecAddItem[3].m_bClaimed, vecAddItem[3].m_bPeriod ,  L"NULL"
		, vecAddItem[4].m_nSlotID, vecAddItem[4].m_nIUID, vecAddItem[4].m_nItemID, vecAddItem[4].m_nStackAmt, vecAddItem[4].m_nMaxDura, vecAddItem[4].m_bClaimed, vecAddItem[4].m_bPeriod ,  L"NULL"
		, strString.c_str(), nQItemCnt);

	if (!pTask->Input(data))
		return false;

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::QuestAcceptLog( GDBT_QUEST_ACCEPT& data )
{
	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	for each (const GDBT_QUEST_ADDITEM& ai in data.m_vecAddItem)
	{
		int64 nIUID = 0;
		for each (const pair<int, int64>& iuid in data.m_mapIUID)
		{
			if (iuid.first == ai.m_nSlotID)
				nIUID = iuid.second;

			if (0 == nIUID)
				continue;

			m_pLogCaches->m_QuestItemLogCache.PushBack(
				GLOG_DATA_QUEST_ITEM(data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID
				, GDB_CODE::CD_L_QACCEPT_ITEM_GAIN, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nDeltaCharPtm, data.m_QuestComm.m_nLevel, data.m_nMoney, 0
				, nIUID, ai.m_nItemID, ai.m_nStackAmt, ai.m_nModStackAmt
				, strRegDate, 0, data.m_QuestComm.m_nQuestID, 0), IsDirectPostItem(ai.m_nItemID));
		}		
	}

	for each (const GITEM_STACK_AMT& amt in data.m_vecRemoveItem)
	{
		m_pLogCaches->m_QuestItemLogCache.PushBack(GLOG_DATA_QUEST_ITEM(data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID
			, GDB_CODE::CD_L_QACCEPT_ITEM_DEL, data.m_QuestComm.m_nCharPtm, data.m_QuestComm.m_nDeltaCharPtm, data.m_QuestComm.m_nLevel, data.m_nMoney, 0
			, amt.nIUID, amt.nItemID, amt.nToStackAmt, -amt.nModStackAmt
			, strRegDate, 0, data.m_QuestComm.m_nQuestID, 0), IsDirectPostItem(amt.nItemID));
	}
}

const wchar_t g_szDB_QuestUpdateObject[] = L"{CALL dbo.USP_RZ_QUEST_UPDATE_OBJECT (%d, %I64d, %I64d, %d, %d, %d, %d, %d)}";
bool GDBManager::QuestUpdateObject( const GDBT_QUEST_OBJECT& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_QuestComm.m_uidPlayer, SDBTID_QUESTUPDATEOBJECT);
	if (NULL == pTask)
		return false;

	CString strSQL;

	strSQL.Format(g_szDB_QuestUpdateObject
		, GConfig::m_nMyWorldID
		, data.m_QuestComm.m_nAID, data.m_QuestComm.m_nCID, data.m_QuestComm.m_nSlotID, data.m_QuestComm.m_nQuestID, data.m_nObjID, data.m_nProgress, data.m_bComplete);
	
	pTask->PushSQL(strSQL);

	if (!Post(pTask))
		return false;

	return true;
}

void GDBManager::UpdateDB_FromQeustInfo()
{
	if (!IsRunForTest())
	{
		return;
	}


	return; // 사용되지 않는 코드 (교성씨 백)

#ifdef _DEBUG
	return;
#endif


	MLogger logger;
	logger.Init(MLOG_LEVEL_DEBUG, MLOG_FILE, "system/Quest.sql.txt");

	vector<int> vecAllQuestID = gmgr.pQuestInfoMgr->GetAllQuestID();

	for each (const int& nQuestID in vecAllQuestID)
	{
		CString strSQL;
		strSQL.Format(L"{CALL spInsertQuestData (%d)}", nQuestID);

		logger.Output(strSQL);
		logger.Output(L"\n");

		Execute(strSQL);
	}	
}

bool GDBManager::MakeQuestGiveupList(GITEM_STACK_AMT_VEC& vec, wstring& strString)
{
	GItemConvertPrefixedString cov;
	wstring str;
	for each (const GITEM_STACK_AMT& st in vec)
	{
		str = cov.Convert((SH_ITEM_SLOT_TYPE)st.nSlotType, st.nSlotID, st.nToStackAmt);
		if (GItemConvertPrefixedString::m_strFalse == str)
			return false;

		strString += str;
	}

	return true;
}


void GDBManager::QuestGiveupLog( GDBT_QUEST_GIVEUP& data )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_QUEST_FAIL_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_QUEST_GIVEUP_LOG (%I64d, %d, %I64d, %d, %d, '%s', %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nQuestID, data.m_strAcceptDt.c_str(), data.m_nFieldID);

	pTask->PushSQL(strSQL);

	Post(pTask);

	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	for each (const GITEM_STACK_AMT& amt in data.m_vecQItems)
	{
		m_pLogCaches->m_QuestItemLogCache.PushBack(GLOG_DATA_QUEST_ITEM(data.m_nAID, data.m_nCID
			, GDB_CODE::CD_L_QGIVEUP_ITEM_DEL, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, 0, 0
			, amt.nIUID, amt.nItemID, amt.nToStackAmt, -amt.nModStackAmt
			, strRegDate, 0, data.m_nQuestID, 0), IsDirectPostItem(amt.nItemID));		
	}
}


const wchar_t g_szDB_QuestGiveup[] = L"{CALL dbo.USP_RZ_QUEST_GIVEUP (%d, %I64d, %I64d, %d, %d, '%s', %d)}";
bool GDBManager::QuestGiveup( GDBT_QUEST_GIVEUP& data )
{
	wstring strString;
	if (!MakeQuestGiveupList(data.m_vecQItems, strString))
		return false;
	
	GDBTaskQuestGiveup* pTask = new GDBTaskQuestGiveup(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	const size_t  nQItemCnt = data.m_vecQItems.size();

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_QuestGiveup
		, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nSlotID, data.m_nQuestID, strString.c_str(), nQItemCnt);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_QuestFail[] = L"{CALL dbo.USP_RZ_QUEST_FAIL (%d, %I64d, %I64d, %d, %d)}";
bool GDBManager::QuestFail( const GDBT_QUEST_COMMON& data )
{
	GDBTaskQuestFail* pTask = new GDBTaskQuestFail(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_QuestFail, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nSlotID, data.m_nQuestID);

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::QuestFailLog( const GDBT_QUEST_COMMON& data )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_QUEST_FAIL_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_QUEST_FAIL_LOG (%I64d, %d, %I64d, %d, %d, '%s', %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nQuestID, data.m_strAcceptDt.c_str(), data.m_nFieldID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}


const wchar_t g_szDB_QuestUpdateVar[] = L"{CALL dbo.USP_RZ_QUEST_UPDATE_VAR (%d, %I64d, %I64d, %d, %d, %d)}";
bool GDBManager::QuestUpdateVar( const GDBT_QUEST_VAR& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_QUESTUPDATEVAR);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_QuestUpdateVar
		, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nSlotID, data.m_nQuestID, data.m_nVar);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_QuestComplete[] = L"{CALL dbo.USP_RZ_QUEST_COMPLETE (%d, %I64d, %I64d, %d, %d)}";
bool GDBManager::QuestComplete( const GDBT_QUEST_COMMON& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_QUESTCOMPLETE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_QuestComplete
		, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nSlotID, data.m_nQuestID);

	pTask->PushSQL(strSQL);

	if (!Post(pTask))
		return false;

	QuestCompleteLog(data);

	return true;
}

void GDBManager::QuestCompleteLog( const GDBT_QUEST_COMMON& data )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_QUEST_COMPLETE_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;

	strSQL.Format(L"{CALL dbo.USP_RZ_QUEST_COMPLETE_LOG (%I64d, %d, %I64d, %d, %d, '%s', %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nQuestID, data.m_strAcceptDt.c_str(), data.m_nFieldID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

void BuildQPvPRewardInvenString(CString& strSQL, const GDBT_QPER_TOINVEN& data)
{
	GDBT_QPER_ITEM pitem[MAX_MAIL_APPENDED_ITEM_COUNT];

	for (size_t i = 0; i < data.m_vecItem.size(); ++i)
		pitem[i].Set(data.m_vecItem[i]);	

	strSQL.Format(L"{CALL dbo.USP_RZ_QPVP_REWARD_INVEN (%d, %I64d, %I64d, %d, %d, %d, %d\
				   , %d, %I64d, %d, %d, %d, %d, %d, %s\
				   , %d, %I64d, %d, %d, %d, %d, %d, %s\
				   , %d, %I64d, %d, %d, %d, %d, %d, %s\
				   , %d, %I64d, %d, %d, %d, %d, %d, %s\
				   , %d, %I64d, %d, %d, %d, %d, %d, %s)}"
				   , GConfig::m_nMyWorldID
				   , data.m_nAID, data.m_nCID, data.m_nCharPtm, data.m_nMoney, data.m_nXP, data.m_nLevel
				   , pitem[0].m_nSlotID, pitem[0].m_nIUID, pitem[0].m_nItemID, pitem[0].m_nStackAmt, pitem[0].m_bClaimed, pitem[0].m_nMaxDura, 0, L"NULL"					
				   , pitem[1].m_nSlotID, pitem[1].m_nIUID, pitem[1].m_nItemID, pitem[1].m_nStackAmt, pitem[0].m_bClaimed, pitem[1].m_nMaxDura, 0, L"NULL"						
				   , pitem[2].m_nSlotID, pitem[2].m_nIUID, pitem[2].m_nItemID, pitem[2].m_nStackAmt, pitem[0].m_bClaimed, pitem[2].m_nMaxDura, 0, L"NULL"
				   , pitem[3].m_nSlotID, pitem[3].m_nIUID, pitem[3].m_nItemID, pitem[3].m_nStackAmt, pitem[0].m_bClaimed, pitem[3].m_nMaxDura, 0, L"NULL"
				   , pitem[4].m_nSlotID, pitem[4].m_nIUID, pitem[4].m_nItemID, pitem[4].m_nStackAmt, pitem[0].m_bClaimed, pitem[4].m_nMaxDura, 0, L"NULL");
}

bool GDBManager::QuestPVPERewardInven( GDBT_QPER_TOINVEN& data )
{
	GDBTaskQuestPvPRewardInven* pTask = new GDBTaskQuestPvPRewardInven(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	BuildQPvPRewardInvenString(strSQL, data);

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::PostQPvPInvenLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_QPVP_LOG);
	if (NULL == pTask)
		return;

	const uint8 QUEST_PVP_TYPE = 3;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_QPVP_INVEN_LOG (%I64d, %d, %I64d, %s, %d, %d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, strRegDate.c_str(), data.m_nCharPtm, data.m_nEventID, data.m_nFieldID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

void GDBManager::PostQPvPInvenRewardXPLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate)
{
	if (0 < data.m_nDeltaLevel)
	{
		CharLevelUpLog(GDBT_CHAR_LEVEL_UP_DATA(data.m_nAID, data.m_uidPlayer, data.m_nCID, GDB_CODE::CD_L_QPVP_REWARD_XP_GAIN_LEVEL_UP
			, strRegDate, data.m_nXP, data.m_nDeltaXP, data.m_nLevel, data.m_nMoney, 0, data.m_nCharPtm, data.m_nDeltaCharPtm, 0 /*NPC_ID*/
			, data.m_nFieldID, data.m_vPos));
	}
	else if (0 < data.m_nDeltaXP)
	{
		CharAddXPLog(GDBT_CHAR_XP_DATA(data.m_nAID, data.m_uidPlayer, data.m_nCID, GDB_CODE::CD_L_QPVP_REWARD_XP_GAIN
			, strRegDate, data.m_nXP, data.m_nDeltaXP, data.m_nLevel, data.m_nMoney, data.m_nCharPtm, data.m_nDeltaCharPtm, 0));
	}	
}

void GDBManager::PostQPvPInvenRewardMoneyLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate)
{
	// reward money.
	if (0 != data.m_nDeltaMoney)
	{
		m_pLogCaches->m_QuestItemLogCache.PushBack(
			GLOG_DATA_QUEST_ITEM(data.m_nAID, data.m_nCID
			, GDB_CODE::CD_L_QPVP_REWARD_MONEY_GAIN, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, data.m_nDeltaMoney
			, 0, 0, 0, 0, strRegDate, 0, 0, data.m_nEventID));
	}
}

void GDBManager::PostQPvPInvenRewardItemLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate)
{
	// reward item to inven
	for each (const GDBT_QPER_ITEM& qi in data.m_vecItem)
	{
		for each (const pair<int16, int64>& ai in data.m_vecRewardItemIUID)
		{
			if (ai.first == qi.m_nSlotID)
			{
				m_pLogCaches->m_QuestItemLogCache.PushBack(GLOG_DATA_QUEST_ITEM(data.m_nAID, data.m_nCID
					, GDB_CODE::CD_L_QPVP_REWARD_ITEM_GAIN, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, 0, ai.second
					, qi.m_nItemID, qi.m_nStackAmt, qi.m_nModStackAmt
					, strRegDate, 0, 0, data.m_nEventID), IsDirectPostItem(qi.m_nItemID));
			}
		}
	}	
}

void GDBManager::QuestPVPERewardInvenLog( GDBT_QPER_TOINVEN& data )
{
	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	PostQPvPInvenLog(data, strRegDate);
	PostQPvPInvenRewardXPLog(data, strRegDate);
	PostQPvPInvenRewardMoneyLog(data, strRegDate);
	PostQPvPInvenRewardItemLog(data, strRegDate);	
}

void BuildQPvPRewardMailString(CString& strSQL, const GDBT_QPER_TOMAIL& data)
{
	GDBT_QPER_ITEM pitem[MAX_MAIL_APPENDED_ITEM_COUNT];

	for (size_t i = 0; i < data.m_vecItem.size(); ++i)
		pitem[i].Set(data.m_vecItem[i]);	

	strSQL.Format(L"{CALL dbo.USP_RZ_QPVP_REWARD_MAIL (%d, %I64d, %I64d, N'%s', N'%s', %d, %d, %d, %d, %d\
				   , %d, %d, %d\
				   , %d, %d, %d\
				   , %d, %d, %d\
				   , %d, %d, %d\
				   , %d, %d, %d)}"
				   , GConfig::m_nMyWorldID
				   , data.m_nAID, data.m_nCID, L"", data.m_strTitle.c_str(), data.m_nRemainDeleteSeconds, data.m_nCharPtm, data.m_nMoney, data.m_nXP, data.m_nLevel
				   , pitem[0].m_nItemID, pitem[0].m_nStackAmt, pitem[0].m_nMaxDura
				   , pitem[1].m_nItemID, pitem[1].m_nStackAmt, pitem[1].m_nMaxDura
				   , pitem[2].m_nItemID, pitem[2].m_nStackAmt, pitem[2].m_nMaxDura
				   , pitem[3].m_nItemID, pitem[3].m_nStackAmt, pitem[3].m_nMaxDura
				   , pitem[4].m_nItemID, pitem[4].m_nStackAmt, pitem[4].m_nMaxDura);
}

bool GDBManager::QuestPVPERewardMail( GDBT_QPER_TOMAIL& data )
{
	GDBTaskQuestPvPRewardMail* pTask = new GDBTaskQuestPvPRewardMail(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	BuildQPvPRewardMailString(strSQL, data);

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::PostQPvPMailLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_QPVP_LOG);
	if (NULL == pTask)
		return;

	const uint8 QUEST_PVP_TYPE = 3;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_QPVP_MAIL_LOG (%I64d, %d, %I64d, %s, %d, %d, %d, %I64d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, strRegDate.c_str(), data.m_nCharPtm, data.m_nEventID, data.m_nFieldID, data.m_nMailUID, MT_QPVP);

	pTask->PushSQL(strSQL);

	gsys.pDBManager->Post(pTask);
}

void GDBManager::PostQPvPMailRewardXPLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate)
{
	if (0 < data.m_nDeltaLevel)
	{
		CharLevelUpLog(GDBT_CHAR_LEVEL_UP_DATA(data.m_nAID, data.m_uidPlayer, data.m_nCID, GDB_CODE::CD_L_QPVP_REWARD_XP_GAIN_LEVEL_UP
			, strRegDate, data.m_nXP, data.m_nDeltaXP, data.m_nLevel, data.m_nMoney, 0, data.m_nCharPtm, data.m_nDeltaCharPtm, 0 /*NPC_ID*/
			, data.m_nFieldID, data.m_vPos));
	}
	else if (0 < data.m_nDeltaXP)
	{
		CharAddXPLog(GDBT_CHAR_XP_DATA(data.m_nAID, data.m_uidPlayer, data.m_nCID, GDB_CODE::CD_L_QPVP_REWARD_XP_GAIN
			, strRegDate, data.m_nXP, data.m_nDeltaXP, data.m_nLevel, data.m_nMoney, data.m_nCharPtm, data.m_nDeltaCharPtm, 0));
	}	
}

void GDBManager::PostQPvPMailRewardMoneyLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate)
{
	// reward money.
	if (0 != data.m_nDeltaMoney)
	{
		m_pLogCaches->m_QuestItemLogCache.PushBack(
			GLOG_DATA_QUEST_ITEM(data.m_nAID, data.m_nCID
			, GDB_CODE::CD_L_QPVP_REWARD_MONEY_GAIN, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, data.m_nDeltaMoney
			, 0, 0, 0, 0, strRegDate, 0, 0, data.m_nEventID));
	}
}

void GDBManager::PostQPvPMailRewardItemLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate)
{
	for each (const GDBT_QPER_ITEM& qi in data.m_vecItem)
	{
		m_pLogCaches->m_QuestItemLogCache.PushBack(GLOG_DATA_QUEST_ITEM(data.m_nAID, data.m_nCID, GDB_CODE::CD_L_QPVP_REWARD_ITEM_GAIN, data.m_nCharPtm
			, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, 0, 0
			, qi.m_nItemID, qi.m_nStackAmt, qi.m_nModStackAmt
			, strRegDate, data.m_nMailUID, 0, data.m_nEventID), IsDirectPostItem(qi.m_nItemID));
	}	
}

void GDBManager::QuestPVPERewardMailLog(GDBT_QPER_TOMAIL& data)
{
	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	PostQPvPMailLog(data, strRegDate);
	PostQPvPMailRewardXPLog(data, strRegDate);	
	PostQPvPMailRewardMoneyLog(data, strRegDate);
	PostQPvPMailRewardItemLog(data, strRegDate);	
}

const wchar_t g_szDB_CharUpdatePlayerGrade[] = L"{CALL dbo.USP_RZ_CHAR_UPDATE_PLAYER_GRADE (%d, %I64d, %I64d, %d)}";
bool GDBManager::CharUpdatePlayerGrade( const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nPlayerGrade )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(uidPlayer, SDBTID_CHARUPDATEPLAYERGRADE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_CharUpdatePlayerGrade, GConfig::m_nMyWorldID, nAID, nCID, nPlayerGrade);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_Faction_Insert[] = L"{CALL dbo.USP_RZ_FACTION_INSERT (%d, %I64d, %I64d, %d, %d)}";
bool GDBManager::FactionInsert( const GDBT_DATA_FACTION& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_FACTIONINSERT);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_Faction_Insert, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nFactID, data.m_nVal);
	
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_Faction_Update[] = L"{CALL dbo.USP_RZ_FACTION_UPDATE (%d, %I64d, %I64d, %d, %d)}";
bool GDBManager::FactionUpdate( const GDBT_DATA_FACTION& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_FACTIONUPDATE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_Faction_Update, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nFactID, data.m_nVal);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}


DWORD GDBManager::GetAsyncQSize()
{
	DWORD dwQSize = 0;

	return dwQSize;
}

void GDBManager::CharUpdateMoneyLog(const GLOG_DATA_MONEY& data)									
{
	if (0 == data.m_nDeltaMoney)
		return;

	m_pLogCaches->m_MoneyLogCache.PushBack(data);	
}

const wchar_t g_szDB_CHAR_UPDATE_MONEY[] = L"{CALL dbo.USP_RZ_CHAR_UPDATE_MONEY (%d, %I64d, %I64d, %d, %d, %d)}";
bool GDBManager::CharAddMoney( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	_ASSERT(0 < data.m_nDeltaMoney);

	if (0 >= data.m_nDeltaMoney)
		return false;

	GDBTaskCharIncMoney* pTask = new GDBTaskCharIncMoney(data.m_uidPlayer, SDBTID_CHAR_INC_MONEY);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_CHAR_UPDATE_MONEY
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID, data.m_nMoney, data.m_nXP, data.m_nCharPtm); 
	pTask->PushSQL(strSQL);

	return Post(pTask);	
}

bool GDBManager::CharMinusMoney( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	_ASSERT(0 < data.m_nDeltaMoney);

	if (0 >= data.m_nDeltaMoney)
		return false;

	GDBTaskCharDecMoney* pTask = new GDBTaskCharDecMoney(data.m_uidPlayer, SDBTID_CHAR_DEC_MONEY);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_CHAR_UPDATE_MONEY
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID, data.m_nMoney, data.m_nXP, data.m_nCharPtm);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_Trade[] = L"{CALL dbo.USP_RZ_TRADE (\
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
, %d)}";
bool GDBManager::Trade(GTRADE_CHAR_VEC& vTradeChar, const uint8 nItemInstanceCnt)
{
	GDBTaskTrade* pTask = new GDBTaskTrade();
	if (NULL == pTask)
		return false;

	GTRADE_ITEM_VEC& vItem1 = vTradeChar[0].m_vItem;
	GTRADE_ITEM_VEC& vItem2 = vTradeChar[1].m_vItem;

	CString strSQL;
	strSQL.Format(g_szDB_Trade
		, GConfig::m_nMyWorldID
		, vTradeChar[0].m_nAID, vTradeChar[0].m_nCID, vTradeChar[0].m_nXP, vTradeChar[0].m_nMoney, vTradeChar[0].m_nCharPtm
		
		, vItem1[0].m_nSlotID, vItem1[0].m_nIUID, vItem1[0].m_nItemID, vItem1[0].m_nStackAmt, vItem1[0].m_nTradeAmt, vItem1[0].m_nDura, vItem1[0].m_nRemainUsagePeriod, vItem1[0].m_nSlotIDTo
		, vItem1[1].m_nSlotID, vItem1[1].m_nIUID, vItem1[1].m_nItemID, vItem1[1].m_nStackAmt, vItem1[1].m_nTradeAmt, vItem1[1].m_nDura, vItem1[1].m_nRemainUsagePeriod, vItem1[1].m_nSlotIDTo
		, vItem1[2].m_nSlotID, vItem1[2].m_nIUID, vItem1[2].m_nItemID, vItem1[2].m_nStackAmt, vItem1[2].m_nTradeAmt, vItem1[2].m_nDura, vItem1[2].m_nRemainUsagePeriod, vItem1[2].m_nSlotIDTo
		, vItem1[3].m_nSlotID, vItem1[3].m_nIUID, vItem1[3].m_nItemID, vItem1[3].m_nStackAmt, vItem1[3].m_nTradeAmt, vItem1[3].m_nDura, vItem1[3].m_nRemainUsagePeriod, vItem1[3].m_nSlotIDTo
		, vItem1[4].m_nSlotID, vItem1[4].m_nIUID, vItem1[4].m_nItemID, vItem1[4].m_nStackAmt, vItem1[4].m_nTradeAmt, vItem1[4].m_nDura, vItem1[4].m_nRemainUsagePeriod, vItem1[4].m_nSlotIDTo
		
		, vTradeChar[1].m_nAID, vTradeChar[1].m_nCID, vTradeChar[1].m_nXP, vTradeChar[1].m_nMoney, vTradeChar[1].m_nCharPtm
		
		, vItem2[0].m_nSlotID, vItem2[0].m_nIUID, vItem2[0].m_nItemID, vItem2[0].m_nStackAmt, vItem2[0].m_nTradeAmt, vItem2[0].m_nDura, vItem2[0].m_nRemainUsagePeriod, vItem2[0].m_nSlotIDTo
		, vItem2[1].m_nSlotID, vItem2[1].m_nIUID, vItem2[1].m_nItemID, vItem2[1].m_nStackAmt, vItem2[1].m_nTradeAmt, vItem2[1].m_nDura, vItem2[1].m_nRemainUsagePeriod, vItem2[1].m_nSlotIDTo
		, vItem2[2].m_nSlotID, vItem2[2].m_nIUID, vItem2[2].m_nItemID, vItem2[2].m_nStackAmt, vItem2[2].m_nTradeAmt, vItem2[2].m_nDura, vItem2[2].m_nRemainUsagePeriod, vItem2[2].m_nSlotIDTo
		, vItem2[3].m_nSlotID, vItem2[3].m_nIUID, vItem2[3].m_nItemID, vItem2[3].m_nStackAmt, vItem2[3].m_nTradeAmt, vItem2[3].m_nDura, vItem2[3].m_nRemainUsagePeriod, vItem2[3].m_nSlotIDTo
		, vItem2[4].m_nSlotID, vItem2[4].m_nIUID, vItem2[4].m_nItemID, vItem2[4].m_nStackAmt, vItem2[4].m_nTradeAmt, vItem2[4].m_nDura, vItem2[4].m_nRemainUsagePeriod, vItem2[4].m_nSlotIDTo

		, nItemInstanceCnt);

	if (!pTask->Input(vTradeChar))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::TradeLog( const GDBT_TRADE_DATA& data )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_CHAR_TRADE_LOG);
	if (NULL == pTask)
		return;

	const GTRADE_CHAR& tch_1 = data.vTradeChar[TRADE_CHAR_FIRSET];
	const GTRADE_CHAR& tch_2 = data.vTradeChar[TRADE_CHAR_SECOND];

	const GTRADE_ITEM_LOG* ti_1 = data.ItemLog[TRADE_CHAR_FIRSET];
	const GTRADE_ITEM_LOG* ti_2 = data.ItemLog[TRADE_CHAR_SECOND];

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_TRADE_LOG (%d \
				   , %I64d, %I64d, %d, %d, %d, %d, %d \
				   , %d, %f, %f, %f \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %I64d, %I64d, %d, %d, %d, %d, %d \
				   , %d, %f, %f, %f \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d \
				   , %d, %I64d, %I64d, %d, %d, %d)}"
				   , GConfig::m_nMyWorldID
				   , tch_1.m_nAID, tch_1.m_nCID, tch_1.m_nCharPtm, tch_1.m_nDeltaCharPtm, tch_1.m_nLevel, tch_1.m_nMoney, tch_1.m_nModMoney
				   , tch_1.m_nFieldID, tch_1.m_fPosX, tch_1.m_fPosY, tch_1.m_fPosZ
				   , ti_1[0].m_nItemID, ti_1[0].m_nIUID, ti_1[0].m_nToIUID, ti_1[0].m_nStackAmt, ti_1[0].m_nModStackAmt, ti_1[0].m_nUsagePeriod
				   , ti_1[1].m_nItemID, ti_1[1].m_nIUID, ti_1[1].m_nToIUID, ti_1[1].m_nStackAmt, ti_1[1].m_nModStackAmt, ti_1[1].m_nUsagePeriod
				   , ti_1[2].m_nItemID, ti_1[2].m_nIUID, ti_1[2].m_nToIUID, ti_1[2].m_nStackAmt, ti_1[2].m_nModStackAmt, ti_1[2].m_nUsagePeriod
				   , ti_1[3].m_nItemID, ti_1[3].m_nIUID, ti_1[3].m_nToIUID, ti_1[3].m_nStackAmt, ti_1[3].m_nModStackAmt, ti_1[3].m_nUsagePeriod
				   , ti_1[4].m_nItemID, ti_1[4].m_nIUID, ti_1[4].m_nToIUID, ti_1[4].m_nStackAmt, ti_1[4].m_nModStackAmt, ti_1[4].m_nUsagePeriod
				   , tch_2.m_nAID, tch_2.m_nCID, tch_2.m_nCharPtm, tch_2.m_nDeltaCharPtm, tch_2.m_nLevel, tch_2.m_nMoney, tch_2.m_nModMoney
				   , tch_2.m_nFieldID, tch_2.m_fPosX, tch_2.m_fPosY, tch_2.m_fPosZ
				   , ti_2[0].m_nItemID, ti_2[0].m_nIUID, ti_2[0].m_nToIUID, ti_2[0].m_nStackAmt, ti_2[0].m_nModStackAmt, ti_2[0].m_nUsagePeriod
				   , ti_2[1].m_nItemID, ti_2[1].m_nIUID, ti_2[1].m_nToIUID, ti_2[1].m_nStackAmt, ti_2[1].m_nModStackAmt, ti_2[1].m_nUsagePeriod
				   , ti_2[2].m_nItemID, ti_2[2].m_nIUID, ti_2[2].m_nToIUID, ti_2[2].m_nStackAmt, ti_2[2].m_nModStackAmt, ti_2[2].m_nUsagePeriod
				   , ti_2[3].m_nItemID, ti_2[3].m_nIUID, ti_2[3].m_nToIUID, ti_2[3].m_nStackAmt, ti_2[3].m_nModStackAmt, ti_2[3].m_nUsagePeriod
				   , ti_2[4].m_nItemID, ti_2[4].m_nIUID, ti_2[4].m_nToIUID, ti_2[4].m_nStackAmt, ti_2[4].m_nModStackAmt, ti_2[4].m_nUsagePeriod);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

const wchar_t g_szDB_ItemInsert[] = L"{CALL dbo.USP_RZ_ITEM_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d)}";
bool GDBManager::ItemInsert( GDBT_ITEM_DATA& data )
{
	GDBTaskItemInsert* pTask = new GDBTaskItemInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_ItemInsert, GConfig::m_nMyWorldID, data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt
		, data.m_Item.m_nDura, data.m_Item.m_nMaxDura, data.m_Item.m_nColor, data.m_Item.m_bClaimed, data.m_Item.m_bPeriodItem, data.m_Item.m_nUsagePeriod
		, data.m_Item.m_strExpiDt.c_str(), data.m_Item.m_nCharPtm);

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::ItemInsertLog( GDBT_ITEM_DATA& data )
{
	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	GLOG_DATA_ITEM log_data(data.m_nAID, data.m_Item.m_nCID, GDB_CODE::CD_L_ITEM_GAIN, data.m_Item.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney
		, data.m_Item.m_nIUID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt, data.m_nModStackAmt, strRegDate, data.m_nNpcID);

	m_pLogCaches->m_ItemLootLogCache.PushBack(log_data, IsDirectPostItem(data.m_Item.m_nItemID));
}

bool GDBManager::CharDie( GDBT_CHAR_KILL& die, GDBT_CHAR_KILL& killer, int nKillerNpcID )
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_CHARDIE);
	if (NULL == pTask)
		return false;

	CString strSQL;

	if (0 == nKillerNpcID && 0 == killer.m_uidPlayer)
	{
		strSQL.Format(L"{CALL dbo.USP_RZ_CHAR_DIE_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d, NULL, %d, %f, %f, %f)}"
			, die.m_nAID, GConfig::m_nMyWorldID, die.m_nCID, die.m_nCode, die.m_nCharPtm, die.m_nDeltaCharPtm, die.m_nLevel, die.m_nXP, die.m_nFieldID, die.m_vPos.x, die.m_vPos.y, die.m_vPos.z);
	}
	else if (0 != nKillerNpcID && 0 == killer.m_uidPlayer)
	{
		strSQL.Format(L"{CALL dbo.USP_RZ_CHAR_DIE_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f)}"
			, die.m_nAID, GConfig::m_nMyWorldID, die.m_nCID, die.m_nCode, die.m_nCharPtm, die.m_nDeltaCharPtm, die.m_nLevel, die.m_nXP, nKillerNpcID, die.m_nFieldID, die.m_vPos.x, die.m_vPos.y, die.m_vPos.z);
	}
	else if (0 == nKillerNpcID && 0 != killer.m_uidPlayer)
	{
		// pvp die는 여기서 pvp용 프로시저를 호출한다.
		// 
		strSQL.Format(L"{CALL dbo.USP_RZ_CHAR_PVP_DIE_LOG (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %f, %f, %f)}"
			, GConfig::m_nMyWorldID
			, die.m_nAID, die.m_nCID, die.m_nCode, die.m_nCharPtm, die.m_nDeltaCharPtm, die.m_nLevel, die.m_nXP, die.m_nFieldID, die.m_vPos.x, die.m_vPos.y, die.m_vPos.z
			, killer.m_nAID, killer.m_nCID, killer.m_nCode, killer.m_nCharPtm, killer.m_nDeltaCharPtm, killer.m_nLevel, killer.m_nXP, killer.m_nFieldID, killer.m_vPos.x, killer.m_vPos.y, killer.m_vPos.z);
	}

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool MakeCraftItemList(GITEM_STACK_AMT_VEC& vec, wstring& strString)
{
	GItemConvertPrefixedString cov;
	wstring str;
	for each(const GITEM_STACK_AMT& st in vec)
	{
		str = cov.Convert((SH_ITEM_SLOT_TYPE)st.nSlotType, st.nSlotID, st.nToStackAmt);
		if (GItemConvertPrefixedString::m_strFalse == str)
			return false;

		strString += str;
	}

	return true;
}

const wchar_t g_sz_DB_CraftInsert[] = L"{CALL dbo.USP_RZ_ITEM_CRAFT_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d, '%s', %d, %d, %d)}";
bool GDBManager::Craft_Insert( GDBT_CRAFT_DATA& data )
{
	wstring strString;
	if (!MakeCraftItemList(data.m_vRecp, strString))
		return false;	

	GDBTaskCraftInsert* pTask = new GDBTaskCraftInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	const size_t nItemInstCnt = data.m_vRecp.size() + 1;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_sz_DB_CraftInsert, GConfig::m_nMyWorldID, data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt
		, data.m_Item.m_nDura, data.m_Item.m_nMaxDura, data.m_Item.m_nColor, data.m_Item.m_bClaimed, data.m_Item.m_bPeriodItem, data.m_Item.m_nUsagePeriod
		, data.m_Item.m_strExpiDt.c_str(), data.m_Item.m_nCharPtm, strString.c_str(), nItemInstCnt, data.m_nXP, data.m_nMoney);
	pTask->PushSQL(strSQL);	

	return Post(pTask);
}

const wchar_t g_sz_DB_CraftUpdate[] = L"{CALL dbo.USP_RZ_ITEM_CRAFT_UPDATE (%d, %I64d, %I64d, %d, %d, %d, %I64d, %d, %d, %d, %d, '%s', %d)}";
bool GDBManager::Craft_Update( GDBT_CRAFT_DATA& data )
{
	wstring strString;
	if (!MakeCraftItemList(data.m_vRecp, strString))
		return false;	

	GDBTaskCraftUpdate* pTask = new GDBTaskCraftUpdate(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	const size_t nItemInstCnt = data.m_vRecp.size() + 1;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_sz_DB_CraftUpdate
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nItemID, data.m_Item.m_nIUID
		, data.m_nXP, data.m_nMoney, data.m_Item.m_nStackAmt, data.m_Item.m_nCharPtm, strString.c_str(), nItemInstCnt);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::CraftLog( GDBT_CRAFT_DATA& data, int64 nProdIUID )
{
	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	m_pLogCaches->m_CraftLogCache.PushBack(GLOG_DATA_CRAFT(data.m_nAID, data.m_Item.m_nCID, GDB_CODE::CD_L_CRAFT_PRODUCT , data.m_Item.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, -data.m_nModMoney
		, nProdIUID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt, data.m_nModStackAmt, strRegDate, data.m_nNPCID, 0), IsDirectPostItem(data.m_Item.m_nItemID));

	for each (const GITEM_STACK_AMT& amt in data.m_vRecp)
	{
		m_pLogCaches->m_CraftLogCache.PushBack(GLOG_DATA_CRAFT(data.m_nAID, data.m_Item.m_nCID, GDB_CODE::CD_L_CRAFT_RECIPE, data.m_Item.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel
					, data.m_nMoney, 0, amt.nIUID, amt.nItemID, amt.nToStackAmt, -amt.nModStackAmt, strRegDate, 0, nProdIUID), IsDirectPostItem(amt.nItemID));
	}
}

const wchar_t g_sz_DB_RecpInsert[] = L"{CALL dbo.USP_RZ_RECP_INSERT (%d, %I64d, %I64d, %d)};";
bool GDBManager::Craft_RecpInsert( const GDBT_RECIPE& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_CRAFT_RECPINSERT);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_sz_DB_RecpInsert, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nRecpID);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_sz_DB_RecpDelete[] = L"{CALL dbo.USP_RZ_RECP_DELETE (%d, %I64d, %I64d, %d)};";
bool GDBManager::Craft_RecpDelete( const GDBT_RECIPE& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(data.m_uidPlayer, SDBTID_CRAFT_RECPDELETE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_sz_DB_RecpDelete, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nRecpID);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_sz_DB_CutsceneSawnInsert[] = L"{CALL dbo.USP_RZ_CUTSCENE_INSERT (%d, %I64d, %I64d, %d)}";
bool GDBManager::CutsceneSawnInsert( const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nCutsceneID )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(uidPlayer, SDBTID_CUTSCENESAWNINSERT);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_sz_DB_CutsceneSawnInsert, GConfig::m_nMyWorldID, nAID, nCID, nCutsceneID);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_MAIL_GET_MAILBOX_LIST[]		= L"{CALL dbo.USP_RZ_MAIL_GET_LIST (%d, %I64d, %I64d, %I64d, %I64d, %d)}";
bool GDBManager::MailGetList(const GDBT_MAIL_GET_MAILBOX_LIST& data, bool bOpenMaibox)
{
	CString strSQLMailGetMailboxList;

	strSQLMailGetMailboxList.Format(g_szDB_MAIL_GET_MAILBOX_LIST
									, GConfig::m_nMyWorldID
									, data.m_nAID
									, data.m_nCID
									, data.m_nTopMUID
									, data.m_nBottomMUID
									, data.m_nReqMailCount);

	GDBTaskMailGetList* pTask = new GDBTaskMailGetList(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data.m_nTopMUID, data.m_nBottomMUID, data.m_nReqMailCount, bOpenMaibox))
	{
		SAFE_DELETE(pTask);
		return false;
	}
	pTask->PushSQL(strSQLMailGetMailboxList);

	return Post(pTask);
}


const wchar_t g_szDB_MAIL_CHECK_RCVR[] = L"{CALL dbo.USP_RZ_MAIL_CHECK_RCVR (%d, %I64d, %I64d, '%s', %d, %d)}";
bool GDBManager::MailCheckReceiver(const GEntityPlayer* pPlayer, const GDBT_MAIL_WRITE& data)
{
	VALID_RET(pPlayer != NULL, false);


	CString strSQLMailCheckReceiver;

	strSQLMailCheckReceiver.Format(g_szDB_MAIL_CHECK_RCVR,
									GConfig::m_nMyWorldID,
									(int64)pPlayer->GetCID(),
									(int64)pPlayer->GetPlayerGrade(),
									data.m_strReceiverName.c_str(),									
									gsys.pMailSystem->GetMailType(pPlayer),
									MAX_MAILBOX_MAIL_COUNT
									);

	GDBTaskMailCheckReceiver* pTask = new GDBTaskMailCheckReceiver(pPlayer->GetUID());
	if (NULL == pTask)
		return false;

	pTask->Input(data);
	pTask->PushSQL(strSQLMailCheckReceiver);

	return Post(pTask);
}


const wchar_t g_szDB_MAIL_WRITE[] = L"{CALL dbo.USP_RZ_MAIL_SEND (%d,%I64d, %I64d, '%s', %d, %d, %d, \
																	'%s', \
																	%d, '%s', %d, '%s', %d, %d, \
																	%d, %I64d, %d, %d, %d\
																	%d, %I64d, %d, %d, %d \
																	%d, %I64d, %d, %d, %d \
																	%d, %I64d, %d, %d, %d \
																	%d, %I64d, %d, %d, %d \
																	%d)}";

bool GDBManager::MailWrite(const MUID& uidPlayer, const GDBT_MAIL_WRITE& data)
{
	CString strSQLMailWrite;

	strSQLMailWrite.Format(g_szDB_MAIL_WRITE,
							GConfig::m_nMyWorldID,
							data.m_nSenderAID,
							data.m_nSenderCID,
							data.m_strSenderName.c_str(),
							data.m_nSenderCharacterPlaySeconds,
							data.m_nMoney,
							data.m_nDeltaMoney,

							data.m_strReceiverName.c_str(),
		
							data.m_nMailType,
							data.m_strTitle.c_str(),
							data.m_hasText,
							data.m_strText.c_str(),
							data.m_nRemainDeleteSeconds,
							data.m_nDefaultItemID,
			
								data.m_appendedItemSlot[0].m_nSlotID,
								data.m_appendedItemSlot[0].m_nItemUID,
								data.m_appendedItemSlot[0].m_nStackAmount,
								data.m_appendedItemSlot[0].m_nDeltaStackAmount,
								data.m_appendedItemSlot[0].m_nDuration,
								
								data.m_appendedItemSlot[1].m_nSlotID,
								data.m_appendedItemSlot[1].m_nItemUID,
								data.m_appendedItemSlot[1].m_nStackAmount,
								data.m_appendedItemSlot[1].m_nDeltaStackAmount,
								data.m_appendedItemSlot[1].m_nDuration,
								
								data.m_appendedItemSlot[2].m_nSlotID,
								data.m_appendedItemSlot[2].m_nItemUID,
								data.m_appendedItemSlot[2].m_nStackAmount,
								data.m_appendedItemSlot[2].m_nDeltaStackAmount,
								data.m_appendedItemSlot[2].m_nDuration,
								
								data.m_appendedItemSlot[3].m_nSlotID,
								data.m_appendedItemSlot[3].m_nItemUID,
								data.m_appendedItemSlot[3].m_nStackAmount,
								data.m_appendedItemSlot[3].m_nDeltaStackAmount,
								data.m_appendedItemSlot[3].m_nDuration,
								
								data.m_appendedItemSlot[4].m_nSlotID,
								data.m_appendedItemSlot[4].m_nItemUID,
								data.m_appendedItemSlot[4].m_nStackAmount,
								data.m_appendedItemSlot[4].m_nDeltaStackAmount,
								data.m_appendedItemSlot[4].m_nDuration,
								
							data.m_nAppendedItemCount
						);


	GDBTaskMailWrite* pTask = new GDBTaskMailWrite(uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->PushSQL(strSQLMailWrite);

	return Post(pTask);
}

void GDBManager::MailWriteLog(const GDBT_MAIL_WRITE& mw, const GDBT_MAIL_WRITE_RESULT& res)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_MAIL_WRITE_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_MAIL_SEND_LOG (%I64d, %d, %I64d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %I64d\
				, %I64d, %I64d, %d, %d, %d \
				, %I64d, %I64d, %d, %d, %d \
				, %I64d, %I64d, %d, %d, %d \
				, %I64d, %I64d, %d, %d, %d \
				, %I64d, %I64d, %d, %d, %d)}"
				, mw.m_nSenderAID, GConfig::m_nMyWorldID, mw.m_nSenderCID, res.m_nAID, res.m_nCID, mw.m_nSenderCharacterPlaySeconds, mw.m_nDeltaCharPtm, mw.m_nLevel
				, mw.m_nMailType, mw.m_nMoney, -mw.m_nDeltaMoney, -mw.m_nPostage
				, res.m_nMailUID
				, mw.m_appendedItemSlot[0].m_nItemUID, res.GetNewIUID(mw.m_appendedItemSlot[0].m_nItemUID), mw.m_appendedItemSlot[0].m_nItemID, mw.m_appendedItemSlot[0].m_nStackAmount, mw.m_appendedItemSlot[0].m_nDeltaStackAmount
				, mw.m_appendedItemSlot[1].m_nItemUID, res.GetNewIUID(mw.m_appendedItemSlot[1].m_nItemUID), mw.m_appendedItemSlot[1].m_nItemID, mw.m_appendedItemSlot[1].m_nStackAmount, mw.m_appendedItemSlot[1].m_nDeltaStackAmount
				, mw.m_appendedItemSlot[2].m_nItemUID, res.GetNewIUID(mw.m_appendedItemSlot[2].m_nItemUID), mw.m_appendedItemSlot[2].m_nItemID, mw.m_appendedItemSlot[2].m_nStackAmount, mw.m_appendedItemSlot[2].m_nDeltaStackAmount
				, mw.m_appendedItemSlot[3].m_nItemUID, res.GetNewIUID(mw.m_appendedItemSlot[3].m_nItemUID), mw.m_appendedItemSlot[3].m_nItemID, mw.m_appendedItemSlot[3].m_nStackAmount, mw.m_appendedItemSlot[3].m_nDeltaStackAmount
				, mw.m_appendedItemSlot[4].m_nItemUID, res.GetNewIUID(mw.m_appendedItemSlot[4].m_nItemUID), mw.m_appendedItemSlot[4].m_nItemID, mw.m_appendedItemSlot[4].m_nStackAmount, mw.m_appendedItemSlot[4].m_nDeltaStackAmount);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

const wchar_t g_szDB_MAIL_GET_TEXT[]				= L"{CALL dbo.USP_RZ_MAIL_GET_TEXT (%d, %I64d)}";
const wchar_t g_szDB_MAIL_GET_APPENDED_ITEM_LIST[]	= L"{CALL dbo.USP_RZ_MAIL_GET_ITEM_LIST (%d, %I64d)}";
bool GDBManager::MailGetContent(const MUID& uidPlayer, int64 nMUID, bool bHasText)
{
	CString strSQLMailGetText;
	CString strSQLMailGetAppendedItemList;

	strSQLMailGetText.Format(g_szDB_MAIL_GET_TEXT, GConfig::m_nMyWorldID, nMUID);
	strSQLMailGetAppendedItemList.Format(g_szDB_MAIL_GET_APPENDED_ITEM_LIST, GConfig::m_nMyWorldID, nMUID);


	GDBTaskMailGetContent* pTask = new GDBTaskMailGetContent(uidPlayer);
	if (NULL == pTask)
		return false;

	pTask->Input(nMUID, bHasText);
	pTask->PushSQL(strSQLMailGetText);
	pTask->PushSQL(strSQLMailGetAppendedItemList);

	return Post(pTask);
}

const wchar_t g_szDB_MAIL_SET_READ[] = L"{CALL dbo.USP_RZ_MAIL_SET_READ (%d, %I64d, %I64d, %I64d)}";
bool GDBManager::MailSetRead(const MUID& uidPlayer, int64 nAID, int64 nCID, int64 nMUID)
{
	CString strSQLMailSetRead;
	strSQLMailSetRead.Format(g_szDB_MAIL_SET_READ, GConfig::m_nMyWorldID, nAID, nCID, nMUID);


	GDBTaskMailSetRead* pTask = new GDBTaskMailSetRead(uidPlayer);
	if (NULL == pTask)
		return false;

	pTask->PushSQL(strSQLMailSetRead);

	return Post(pTask);
}

void GDBManager::MailDeleteLog( const GDBT_MAIL_DELETE& data )
{
	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_MAIL_DELETE_LOG (%I64d, %d, %I64d, %d, %I64d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nMailUID);

	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_MAIL_DELETE_LOG);
	if (NULL == pTask)
		return;

	pTask->PushSQL(strSQL);

	Post(pTask);
}

const wchar_t g_szDB_MAIL_DELETE[] = L"{CALL dbo.USP_RZ_MAIL_DELETE (%d, %I64d, %I64d, %I64d)}";
bool GDBManager::MailDelete(const MUID& uidPlayer, const GDBT_MAIL_DELETE& data)
{
	VALID_RET(uidPlayer.IsValid(), false);

	CString strSQLMailDelete;
	strSQLMailDelete.Format(g_szDB_MAIL_DELETE,
							GConfig::m_nMyWorldID,
							data.m_nAID,
							data.m_nCID,
							data.m_nMailUID
							);

	GDBTaskMailDelete* pTask = new GDBTaskMailDelete(uidPlayer, data.m_nMailUID);
	if (NULL == pTask)
		return false;

	pTask->Input(data);
	pTask->PushSQL(strSQLMailDelete);

	return Post(pTask);
}

void GDBManager::MailGetAppendedItemLog(const GDBT_MAIL_GET_ITEM& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_MAIL_GET_ITEM_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_MAIL_GET_ITEM_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %I64d, %d, %d, %I64d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPlayTime, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney
		, data.m_nIUID, data.m_nItemID, data.m_nModStackAmt, data.m_nMailUID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

const wchar_t gszDB_MAIL_GET_ITEM[]	= L"{CALL dbo.USP_RZ_MAIL_GET_ITEM (%d, %I64d, %I64d, %I64d, %d, %I64d, %d, %d, %d)}";
bool GDBManager::MailGetAppendedItem(const GDBT_MAIL_GET_ITEM& data)
{
	VALID_RET(data.m_uidPlayer.IsValid(), false);

	GDBTaskMailGetItem* pTask = new GDBTaskMailGetItem(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQLMailGetItem;
	strSQLMailGetItem.Format(gszDB_MAIL_GET_ITEM,
							GConfig::m_nMyWorldID,
							data.m_nAID,
							data.m_nCID,
							data.m_nMailUID,
							data.m_nMailSlotID,
							data.m_nIUID,
							data.m_InvenSlotID,
							data.m_nNewMailDefaultItemID,
							data.m_nCharPlayTime
							);

	if (!pTask->Input(	data
						))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->PushSQL(strSQLMailGetItem);

	return Post(pTask);
}

void GDBManager::MailGetAppendedMoneyLog(const GDBT_MAIL_GET_MONEY& data)
{
	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_MAIL_GET_ITEM_LOG);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_MAIL_GET_MONEY_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d, %I64d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel
		, data.m_nResultMoney, data.m_nAppendedMoney, data.m_nMailUID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

const wchar_t gszDB_MAIL_GET_MONEY[]	= L"{CALL dbo.USP_RZ_MAIL_GET_MONEY (%d, %I64d, %I64d, %I64d, %d, %d)}";
bool GDBManager::MailGetAppendedMoney(const MUID& uidPlayer, const GDBT_MAIL_GET_MONEY& data)
{
	CString strSQLMailGetMoney;
	strSQLMailGetMoney.Format(gszDB_MAIL_GET_MONEY, 
								GConfig::m_nMyWorldID,
								data.m_nAID,
								data.m_nCID,
								data.m_nMailUID,
								data.m_nAppendedMoney,
								data.m_nResultMoney
								);

	GDBTaskMailGetMoney* pTask = new GDBTaskMailGetMoney(uidPlayer);

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->PushSQL(strSQLMailGetMoney);
	
	return Post(pTask);
}

void GDBManager::ExecuteAsyncForDev( const MUID& uidPlayer, wchar_t* szSQL )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(uidPlayer, SDBTID_DEVELOP);
	if (NULL == pTask)
		return;

	pTask->PushSQL(CString(szSQL));
	Post(pTask);
}

bool GDBManager::MakeStackAmtList( GITEM_STACK_AMT_VEC& vecItems, const wstring& strObject, const size_t nMaxStringLen, const size_t nMaxItemCnt, wstring& strList )
{
	_ASSERT(nMaxItemCnt >= vecItems.size());

	if (nMaxItemCnt < vecItems.size())
	{
		MLog3(L"DB ERROR : %s item instacne count is too many. cnt:%d.\n", strObject.c_str(), vecItems.size());
		return false;
	}

	wchar_t szStackAmt[32];

	for (size_t i = 0; i < vecItems.size(); ++i)
	{
		_snwprintf_s(szStackAmt, 32, 31, L"%d.%d.%d,", vecItems[i].nSlotType, vecItems[i].nSlotID, vecItems[i].nToStackAmt);
		strList.append(szStackAmt);	

		_ASSERT(nMaxStringLen >= strList.length());

		if (nMaxStringLen < strList.length())
		{
			MLog3(L"DB ERROR : %s item list wstring is long. length(%d)\n", strObject.c_str(), strList.length());
			return false;
		}
	}

	return true;
}

const wchar_t g_szDB_ItemLootInsert[] = L"{CALL dbo.USP_RZ_ITEM_LOOT_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s)}";
bool GDBManager::ItemLootInsert(const GDBT_ITEM_DATA& data)
{
	if (-1 == data.m_Item.m_nSlotID) return false;

	GDBTaskItemLootInsert* pTask = new GDBTaskItemLootInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_ItemLootInsert
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nItemID
		, data.m_Item.m_nStackAmt, data.m_Item.m_nDura, data.m_Item.m_nMaxDura, data.m_Item.m_nColor
		, data.m_Item.m_bClaimed, data.m_Item.m_nCharPtm, data.m_Item.m_bPeriodItem, data.m_Item.m_nUsagePeriod, data.m_Item.m_strExpiDt.c_str());

	pTask->Input(data);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_ItemLootUpdate[] = L"{CALL dbo.USP_RZ_ITEM_LOOT_UPDATE (%d, %I64d, %d, %d, %I64d, %d)}";
bool GDBManager::ItemLootUpdate(const GDBT_ITEM_DATA& data)
{
	if (-1 == data.m_Item.m_nSlotID) return false;

	GDBTaskItemLootUpdate* pTask = new GDBTaskItemLootUpdate(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{	
		SAFE_DELETE(pTask);
		return false;
	}
	
	CString strSQL;
	strSQL.Format(g_szDB_ItemLootUpdate
		, GConfig::m_nMyWorldID
		, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID, data.m_Item.m_nStackAmt);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}


void GDBManager::ItemLootLog( const GDBT_ITEM_DATA& data )
{
	const wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

	GLOG_DATA_ITEM log_data(data.m_nAID, data.m_Item.m_nCID, GDB_CODE::CD_L_ITEM_LOOT, data.m_Item.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel
		, data.m_nMoney, data.m_Item.m_nIUID, data.m_Item.m_nItemID, data.m_Item.m_nStackAmt, data.m_nModStackAmt, strRegDate, data.m_nNpcID);

	m_pLogCaches->m_ItemLootLogCache.PushBack(log_data, IsDirectPostItem(data.m_Item.m_nItemID));
}

void GDBManager::ReleaseGameDB()
{
	if (NULL != m_pGameDB)
	{
		m_pGameDB->Release();
		SAFE_DELETE(m_pGameDB);
	}
}

void GDBManager::ReleaseGameDBSelectChar()
{
	if (NULL != m_pGameDBSelectChar)
	{
		m_pGameDBSelectChar->Release();
		SAFE_DELETE(m_pGameDBSelectChar);
	}
}

void GDBManager::ReleaseLogDB()
{
	if (NULL != m_pLogDB)
	{
		m_pLogDB->Release();
		SAFE_DELETE(m_pLogDB);
	}
}

bool GDBManager::InitGameDB()
{
	if (NULL != m_pGameDB)
		return false;
	
	m_pGameDB = new SAsyncDB;
	if (NULL == m_pGameDB)
		return false;

	mdb::MDatabaseDesc DBDesc = SDsnFactory::GetInstance().Get()->GetGameDSN();

	const mdb::MDB_AYSNC_RESULT AsyncDBRes = m_pGameDB->Init(SDBT_DBTYPE_GAMEDB, DBDesc);		
	if (mdb::MDBAR_SUCCESS != AsyncDBRes)
	{
		mlog3 ("Create Async GameDB failed(errno %d).\n", AsyncDBRes);
		return false;
	}

	mlog ("Create Async GameDB success.\n");

	return true;
}

bool GDBManager::InitGameDBSelectChar()
{
	if (NULL != m_pGameDBSelectChar)
		return false;

	m_pGameDBSelectChar = new SAsyncDB();
	if (NULL == m_pGameDBSelectChar)
		return false;

	static const DWORD GMAEDB_SELECT_CHAR_THREAD_COUNT = 1;

	mdb::MDatabaseDesc DBDesc = SDsnFactory::GetInstance().Get()->GetGameDSN();

	const mdb::MDB_AYSNC_RESULT AsyncDBRes = m_pGameDBSelectChar->Init(SDBT_DBTYPE_GAMEDB_SELECT_CHAR, DBDesc, GMAEDB_SELECT_CHAR_THREAD_COUNT);		
	if (mdb::MDBAR_SUCCESS != AsyncDBRes)
	{
		mlog3 ("Create Async LoginDB failed(errno %d).\n", AsyncDBRes);
		return false;
	}

	mlog ("Create Async LoginDB success.\n");

	return true;
}

bool GDBManager::InitLogDB()
{
	if (NULL != m_pLogDB)
		return false;

	m_pLogDB = new SAsyncDB;
	if (NULL == m_pLogDB)
		return false;

	mdb::MDatabaseDesc DBDesc = SDsnFactory::GetInstance().Get()->GetLogDSN();

	const mdb::MDB_AYSNC_RESULT AsyncDBRes = m_pLogDB->Init(SDBT_DBTYPE_LOGDB, DBDesc);		
	if (mdb::MDBAR_SUCCESS != AsyncDBRes)
	{
		mlog3 ("Create Async LogDB failed(errno %d).\n", AsyncDBRes);
		return false;
	}

	mlog ("Create Async LogDB success.\n");

	return true;
}

bool GDBManager::Post( GDBAsyncTask* pTask )
{
	if (GConfig::m_bNoDB)
	{
		SAFE_DELETE(pTask);
		return true;
	}

	if (NULL == pTask)
		return false;	

	const SDBT_DBTYPE dbType = pTask->GetDBType();

	if (SDBT_DBTYPE_GAMEDB != dbType && SDBT_DBTYPE_LOGDB != dbType && SDBT_DBTYPE_GAMEDB_SELECT_CHAR != dbType)
	{
		SAFE_DELETE(pTask);
		return false;
	}

	pTask->OnPrePost();

	if (pTask->IsSync())
	{
		SAsyncDB* pAsyncDB = NULL;

		if (SDBT_DBTYPE_GAMEDB == pTask->GetDBType())
		{
			pAsyncDB = m_pGameDB;			
			m_nGameDBQCount++;
		}
		else if (SDBT_DBTYPE_LOGDB == pTask->GetDBType())
		{
			pAsyncDB = m_pLogDB;
			m_nLogDBQCount++;
		}
		else if (SDBT_DBTYPE_GAMEDB_SELECT_CHAR == pTask->GetDBType())
		{
			pAsyncDB = m_pGameDBSelectChar;
			m_nGameDBSelectCharQCount++;
		}	
		else
		{
			_ASSERT(false && L"Invalied db type.");	
			SAFE_DELETE(pTask);
			return false;
		}

		if (NULL == pAsyncDB)
		{
			SAFE_DELETE(pTask);
			return false;
		}
		else
		{
			if (!pAsyncDB->ExecuteAsync(pTask))
			{
				pTask->OnCompleted();
				SAFE_DELETE(pTask);
				return false;
			}
		}		
	}
	else
	{
		pTask->SetTaskSuccess();
		pTask->OnCompleted();
		SAFE_DELETE(pTask);		
	}

	return true;
	
}

const wchar_t g_szDB_Item_Swap_Slot[] = L"{CALL dbo.USP_RZ_ITEM_SWAP_SLOT (%d,\
										 %I64d, %I64d, %d, %d, %I64d, %d, %d,\
										 %I64d, %I64d, %d, %d, %I64d, %d, %d, %d)}";
bool GDBManager::ItemMove(GDBT_ITEM_MOVE& data)
{
	GDBTaskItemSwapSlot* pTask = new GDBTaskItemSwapSlot(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	int nFromItemCacheModAmt = gsys.pDBCacheSystem->GetItemCacheModAmt(data.m_uidPlayer, data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID);
	int nToItemCacheModAmt = gsys.pDBCacheSystem->GetItemCacheModAmt(data.m_uidPlayer, data.m_ToSlot.m_nSlotType, data.m_ToSlot.m_nSlotID, data.m_ToSlot.m_nIUID);

	CString strSQL;
	strSQL.Format(g_szDB_Item_Swap_Slot, GConfig::m_nMyWorldID,
		data.m_FromSlot.m_nAID, data.m_FromSlot.m_nCIDorGID, data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID, data.m_FromSlot.m_nStackAmt - nFromItemCacheModAmt, data.m_FromSlot.m_nStackAmt,
		data.m_ToSlot.m_nAID, data.m_ToSlot.m_nCIDorGID, data.m_ToSlot.m_nSlotType, data.m_ToSlot.m_nSlotID, data.m_ToSlot.m_nIUID, data.m_ToSlot.m_nStackAmt - nToItemCacheModAmt, data.m_ToSlot.m_nStackAmt,
		data.m_nCharPtm);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::ItemMoveLog( GDBT_ITEM_MOVE& data )
{
	if (GDB_CODE::CD_L_ITEM_MOVE_INVEN_TO_INVEN == data.m_nCode)
		return;

	m_pLogCaches->m_ItemMoveLogCache.PushBack(
		GLOG_DATA_ITEM_MOVE(data.m_FromSlot.m_nAID, data.m_FromSlot.m_nCIDorGID, data.m_nCode, data.m_nCharPtm, data.m_nDeltaCharPtm
			, data.m_nLev, data.m_nCurMoney, data.m_FromSlot.m_nIUID, data.m_nItemID, data.m_FromSlot.m_nStackAmt
			, data.m_ToSlot.m_nAID, data.m_ToSlot.m_nCIDorGID));
}

const wchar_t g_szDB_Item_Split[] = L"{CALL dbo.USP_RZ_ITEM_SPLIT (%d,\
									 %I64d, %I64d, %d, %d, %I64d, %d, %d,\
									 %I64d, %I64d, %d, %d, %d, %d)}";
bool GDBManager::ItemSplit(GDBT_ITEM_MERGE_AND_SPLIT& data)
{
	GDBTaskItemSplit* pTask = new GDBTaskItemSplit(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	int nFromItemCacheModAmt = gsys.pDBCacheSystem->GetItemCacheModAmt(data.m_uidPlayer, data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID);
	int nToItemCacheModAmt = gsys.pDBCacheSystem->GetItemCacheModAmt(data.m_uidPlayer, data.m_ToSlot.m_nSlotType, data.m_ToSlot.m_nSlotID, data.m_ToSlot.m_nIUID);

	CString strSQL;
	strSQL.Format(g_szDB_Item_Split, GConfig::m_nMyWorldID, 
		data.m_FromSlot.m_nAID, data.m_FromSlot.m_nCIDorGID, data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID, data.m_FromSlot.m_nStackAmt + data.m_nMergeAndSplitAmt - nFromItemCacheModAmt, data.m_FromSlot.m_nStackAmt, 
		data.m_ToSlot.m_nAID, data.m_ToSlot.m_nCIDorGID, data.m_ToSlot.m_nSlotType, data.m_ToSlot.m_nSlotID, data.m_ToSlot.m_nStackAmt,
		data.m_nCharPtm);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_Item_Merge[] = L"{CALL dbo.USP_RZ_ITEM_MERGE (%d, \
									 %I64d, %d, %d, %I64d, %d, %d, \
									 %I64d, %d, %d, %I64d, %d, %d)}";
bool GDBManager::ItemMerge(const GDBT_ITEM_MERGE_AND_SPLIT& data)						   
{
	GDBTaskItemMerge* pTask = new GDBTaskItemMerge(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	int nFromItemCacheModAmt = gsys.pDBCacheSystem->GetItemCacheModAmt(data.m_uidPlayer, data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID);
	int nToItemCacheModAmt = gsys.pDBCacheSystem->GetItemCacheModAmt(data.m_uidPlayer, data.m_ToSlot.m_nSlotType, data.m_ToSlot.m_nSlotID, data.m_ToSlot.m_nIUID);

	CString strSQL;
	strSQL.Format(g_szDB_Item_Merge, GConfig::m_nMyWorldID,
		data.m_FromSlot.m_nCIDorGID, data.m_FromSlot.m_nSlotType, data.m_FromSlot.m_nSlotID, data.m_FromSlot.m_nIUID, data.m_FromSlot.m_nStackAmt + data.m_nMergeAndSplitAmt - nFromItemCacheModAmt, data.m_FromSlot.m_nStackAmt,
		data.m_ToSlot.m_nCIDorGID, data.m_ToSlot.m_nSlotType, data.m_ToSlot.m_nSlotID, data.m_ToSlot.m_nIUID, data.m_ToSlot.m_nStackAmt - data.m_nMergeAndSplitAmt - nToItemCacheModAmt, data.m_ToSlot.m_nStackAmt);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}


void GDBManager::ItemMergeAndSplitLog( const GDBT_ITEM_MERGE_AND_SPLIT& data )
{
	m_pLogCaches->m_ItemMergeAndSplitLogCacahe.PushBack(
		GLOG_DATA_ITEM_MERGE_AND_SPLIT(data.m_FromSlot.m_nAID, data.m_FromSlot.m_nCIDorGID, data.m_nCode
		, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney
		, data.m_FromSlot.m_nIUID, data.m_FromSlot.m_nItemID, data.m_FromSlot.m_nStackAmt, data.m_nMergeAndSplitAmt
		, data.m_ToSlot.m_nAID, data.m_ToSlot.m_nCIDorGID, data.m_ToSlot.m_nIUID, data.m_ToSlot.m_nStackAmt));
}

const wchar_t g_szDB_DBTask_InsertExecCnt[] = L"{CALL dbo.USP_RZ_SERVER_PROFILE_INSERT (%d, %d, %d, '%s', %d, %d, %I64d)}";
bool GDBManager::ServerProfileInsert( const int nWorldID, const int nServerID, const PROFILE_DATA_TYPE nType, const std::wstring& strRegDt, const int nID, const int nCnt, const int64 nWorkTm )
{
	//GLogDBTaskQuery* pTask = new GLogDBTaskQuery(MUID(0), SDBTID_INSERTEXECCNT);
	//if (NULL == pTask)
	//	return false;

	//CString strSQL;
	//strSQL.Format(g_szDB_DBTask_InsertExecCnt, nWorldID, nServerID, nType, strRegDt.c_str(), nID, nCnt, nWorkTm);

	//pTask->PushSQL(strSQL);

	//return Post(pTask);

	return true;
}

void GDBManager::LogingDBTaskExecCnt()
{
	if (!GConfig::m_bDBTaskCount)
		return;

	wchar_t szRegDt[1024] = {0,};
	// LogDB에 저장.
	tm* t =  gsys.pSystem->GetLocalTime();
	if (NULL != t)
	{
		_snwprintf_s(szRegDt, 1024, 1023, L"%d-%d-%d %d:%d:%d"
			, t->tm_year + 1900
			, t->tm_mon
			, t->tm_mday + 1
			, t->tm_hour
			, t->tm_min
			, t->tm_sec);
	}

	GDBTaskSortedList lst = GDBAsyncTask::GetExecCountDecrSortedList();
	if (!lst.empty())
	{
		for (GDBTaskSortedList::iterator it = lst.begin(); it != lst.end(); ++it)
		{
			ServerProfileInsert(
				  GConfig::m_nMyWorldID
				, GConfig::m_nMyServerID
				, PDT_DBTASK
				, szRegDt
				, it->first
				, it->second.nCnt
				, it->second.nWorkTm);				
		}
	}
}

const wchar_t g_szDB_Emblem_Insert[] = L"{CALL dbo.USP_RZ_EMBLEM_INSERT (%d, %I64d, %I64d, %d)}";
bool GDBManager::EmblemInsert( const GDBT_EMBLEM& data )
{
	GDBTaskEmblemInsert* pTask = new GDBTaskEmblemInsert(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_Emblem_Insert, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nEmblemID);

	pTask->Input(data.m_nEmblemID);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

const wchar_t g_szDB_Emblem_Delete[] = L"{CALL dbo.USP_RZ_EMBLEM_DELETE (%d, %I64d, %I64d, %d)}";
bool GDBManager::EmblemDelete( const GDBT_EMBLEM& data )
{
	GDBTaskEmblemDelete* pTask = new GDBTaskEmblemDelete(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(g_szDB_Emblem_Delete, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nEmblemID);
	
	pTask->Input(data.m_nEmblemID);
	pTask->PushSQL(strSQL);

	return Post(pTask);
}

wchar_t g_szDB_ITEM_INC_STACK_AMOUNT[] = L"{CALL dbo.USP_RZ_ITEM_UPDATE_STACK_AMT (%d, %I64d, %d, %d, %I64d, %d)}";
bool GDBManager::ItemIncStackAmt( GDBT_ITEM_DATA& data )
{
	GDBTaskItemIncStackAmt* pTask = new GDBTaskItemIncStackAmt(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_INC_STACK_AMOUNT
		, GConfig::m_nMyWorldID
		, data.m_Item.m_nCID, data.m_Item.m_nSlotType, data.m_Item.m_nSlotID, data.m_Item.m_nIUID, data.m_Item.m_nStackAmt);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

wchar_t g_szDB_ITEM_DEC_STACK_AMOUNT[] = L"{CALL dbo.USP_RZ_ITEM_UPDATE_STACK_AMT (%d, %I64d, %d, %d, %I64d, %d)}";
bool GDBManager::ItemDecStackAmt( GDBT_ITEM_DEC_STACK_AMT_DATA& data )
{
	GDBTaskItemDecStackAmt* pTask = new GDBTaskItemDecStackAmt(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
	{
		SAFE_DELETE(pTask);
		return false;
	}

	CString strSQL;
	strSQL.Format(g_szDB_ITEM_DEC_STACK_AMOUNT
		, GConfig::m_nMyWorldID
		, data.m_nCID, data.m_nSlotType, data.m_nSlotID, data.m_nIUID, data.m_nStackAmt);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::ItemDecStackAmtLog( GDBT_ITEM_DEC_STACK_AMT_DATA& data )
{
	m_pLogCaches->m_ItemLootLogCache.PushBack(GLOG_DATA_ITEM(data.m_nAID, data.m_nCID, GDB_CODE::CD_L_ITEM_USE
		, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney, data.m_nIUID, data.m_nItemID, data.m_nStackAmt, -data.m_nModStackAmt
		, GTimeCalculator::GetTimeAsString(GTimeSec(0)), data.m_nNpcID), IsDirectPostItem(data.m_nItemID));
}

bool GDBManager::ServerStatusStart( const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(0, SDBTID_SERVER_START);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_SERVER_START (%d, %d, '%s', '%s', '%s', %d, %d, %d, %d, %d)}"
		, nWorldID, nServerID, strServerName.c_str(), strServerVersion.c_str(), strIP.c_str()
		, nPort, nType, nMaxUser, nUpdateElapsedTimeSec, nAllowDelayTm );

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

bool GDBManager::ServerStatusUpdate( const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(0, SDBTID_SERVER_UPDATE);
	if (NULL == pTask)
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_SERVER_UPDATE (%d, %d, %d, %d)}", nWordID, nServerID, nCurUserCount, bIsServable);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::FieldEnter( const GDBT_CHAR_ENTER_FILED& data )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(0, SDBTID_CHAR_ENTER_FIELD);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_CHAR_ENTER_FIELD (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %f, %f, %f)}"
		, GConfig::m_nMyWorldID, data.m_nAID, data.m_nCID, data.m_nXP, data.m_nLevel, data.m_nMoney, data.m_nCharPtm
		, data.m_nSharedFieldID, data.m_fPosX, data.m_fPosY, data.m_fPosZ);

	pTask->PushSQL(strSQL);

	Post(pTask);
}



void GDBManager::NPCKilledLog( const wstring& strRegDate, int nNpcID, const vector<GNPC_KILLER>& vKiller )
{
	GDBTaskNPCKillLogBuilder nkb;
	nkb.PostNpcKillLog(GConfig::m_nMyWorldID, strRegDate, nNpcID, vKiller);	
}

void GDBManager::ItemEnchLog(const GDBT_ITEM_ENCH& data)
{
	CString strSQL;
	int CODE;

	if (-1 != data.m_nTargetEnchItemID)
		CODE = GDB_CODE::CD_L_ENCH_SUCCESS;	
	else
		CODE = GDB_CODE::CD_L_ENCH_FAIL;

	strSQL.Format(L"{CALL dbo.USP_RZ_ITEM_ENCH_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %I64d, %d, %I64d, %d, %d, %d)}"
		, data.m_nAID, GConfig::m_nMyWorldID, data.m_nCID, CODE, data.m_nCharPtm, data.m_nDeltaCharPtm, data.m_nLevel, data.m_nMoney
		, data.m_nTargetIUID, data.m_nTargetItemID
		, data.m_nEStoneIUID, data.m_nEStoneItemID
		, data.m_nEAgentIUID, data.m_nEAgentItemID, data.m_nEAgentStackAmt, data.m_nEAgentDeltaStackAmt);

	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_ITEM_ENCHANT_LOG);
	if (NULL == pTask)
		return;

	pTask->PushSQL(strSQL);

	Post(pTask);
}

bool GDBManager::ItemEnchant( const GDBT_ITEM_ENCH& data )
{
	GDBTaskItemEnchant* pTask = new GDBTaskItemEnchant(data.m_uidPlayer);
	if (NULL == pTask)
		return false;

	if (!pTask->Input(data))
		return false;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_ITEM_ENCHANT (%d, %I64d, %I64d, %d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %d)}"
		, GConfig::m_nMyWorldID
		, data.m_nAID, data.m_nCID
		, data.m_nEStoneItemSlotID, data.m_nEStoneIUID
		, data.m_nEAgentItemSlotID, data.m_nEAgentIUID
		, data.m_nTargetItemSlotType, data.m_nTargetItemSlotID, data.m_nTargetIUID, data.m_nTargetDura, data.m_nTargetMaxDura
		, data.m_nTargetEnchSlotID, data.m_nTargetEnchItemID, data.m_bClaimed);

	pTask->PushSQL(strSQL);

	return Post(pTask);
}

void GDBManager::GM_QUEST_HISTORY_RESET_ALL( const int64 nAID, const int64 nCID )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(0, SDBTID_DEVELOP);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_GM_QUEST_HISTORY_RESET_ALL (%I64d, %I64d)}"
		, nAID, nCID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

void GDBManager::GM_QUEST_RESET_ALL( const int64 nAID, const int64 nCID )
{
	GGameDBTaskQuery* pTask = new GGameDBTaskQuery(0, SDBTID_DEVELOP);
	if (NULL == pTask)
		return;

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_GM_QUEST_RESET_ALL (%I64d, %I64d)}"
		, nAID, nCID);

	pTask->PushSQL(strSQL);

	Post(pTask);
}

void GDBManager::FieldPlayerLog( vector<pair<int, int>>& FieldPlayerCount )
{
	class FIELD_PLAYER_COUNT
	{
	public :
		enum {MAX_FILED_COUNT = 10, };

		FIELD_PLAYER_COUNT() : m_nFieldID(0), m_nPlayerCount(0) {}

		int m_nFieldID;
		int m_nPlayerCount;
	};

	FIELD_PLAYER_COUNT fpc[FIELD_PLAYER_COUNT::MAX_FILED_COUNT];

	for (size_t i = 0; i < FieldPlayerCount.size(); ++i)
	{
		fpc[i].m_nFieldID		= FieldPlayerCount[i].first;
		fpc[i].m_nPlayerCount	= FieldPlayerCount[i].second;
	}

	CString strSQL;
	strSQL.Format(L"{CALL dbo.USP_RZ_FIELD_PLAYER_LOG (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)}"
		, GConfig::m_nMyWorldID, GConfig::m_nMyServerID
		, fpc[0].m_nFieldID, fpc[0].m_nPlayerCount
		, fpc[1].m_nFieldID, fpc[1].m_nPlayerCount
		, fpc[2].m_nFieldID, fpc[2].m_nPlayerCount
		, fpc[3].m_nFieldID, fpc[3].m_nPlayerCount
		, fpc[4].m_nFieldID, fpc[4].m_nPlayerCount
		, fpc[5].m_nFieldID, fpc[5].m_nPlayerCount
		, fpc[6].m_nFieldID, fpc[6].m_nPlayerCount
		, fpc[7].m_nFieldID, fpc[7].m_nPlayerCount
		, fpc[8].m_nFieldID, fpc[8].m_nPlayerCount
		, fpc[9].m_nFieldID, fpc[9].m_nPlayerCount);

	GLogDBTaskQuery* pTask = new GLogDBTaskQuery(0, SDBTID_CBT_FILE_PLAYER_COUNT);
	if (NULL == pTask)
		return;

	pTask->PushSQL(strSQL);

	Post(pTask);
}

bool GDBManager::IsDirectPostItem( int nItemID )
{
	bool bDirectPost = true;
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL != pItemData)
	{
		if (ITEM_TIER_RARE > pItemData->m_nTier)
			bDirectPost = false;		
	}

	return bDirectPost;
}

void GDBManager::ReleaseLogCaches()
{
	if (NULL != m_pLogCaches)
	{
		m_pLogCaches->Release();
		SAFE_DELETE(m_pLogCaches);		
	}
}

void GDBManager::InitLogCache()
{
	if (NULL == m_pLogCaches)
		m_pLogCaches = new GLogCacheContainer;
}

void GDBManager::PartyCreateLog( const GDBT_PARTY& data )
{
	m_pLogCaches->m_PartyLogcache.PushBack(
		GLOG_DATA_PARTY(data.m_uidParty
			, data.m_nAID
			, data.m_nCID
			, GDB_CODE::CD_L_PARTY_CREATE
			, GTimeCalculator::GetTimeAsString(GTimeSec(0))
			, data.m_nType));
}

void GDBManager::PartyEnterLog( const GDBT_PARTY& data )
{
	m_pLogCaches->m_PartyLogcache.PushBack(
		GLOG_DATA_PARTY(data.m_uidParty
		, data.m_nAID
		, data.m_nCID
		, GDB_CODE::CD_L_PARTY_ENTER
		, GTimeCalculator::GetTimeAsString(GTimeSec(0))
		, data.m_nType));
}

void GDBManager::PartyLeaveLog( const GDBT_PARTY& data )
{
	m_pLogCaches->m_PartyLogcache.PushBack(
		GLOG_DATA_PARTY(data.m_uidParty
		, data.m_nAID
		, data.m_nCID
		, GDB_CODE::CD_L_PARTY_LEAVE
		, GTimeCalculator::GetTimeAsString(GTimeSec(0))
		, data.m_nType));
}

void GDBManager::PartyChangeLeaderLog( const GDBT_PARTY& data )
{
	m_pLogCaches->m_PartyLogcache.PushBack(
		GLOG_DATA_PARTY(data.m_uidParty
		, data.m_nAID
		, data.m_nCID
		, GDB_CODE::CD_L_PARTY_CHANGE_LEADER
		, GTimeCalculator::GetTimeAsString(GTimeSec(0))
		, data.m_nType));
}

void GDBManager::PartyDeleteLog( const GDBT_PARTY& data )
{
	m_pLogCaches->m_PartyLogcache.PushBack(
		GLOG_DATA_PARTY(data.m_uidParty
		, data.m_nAID
		, data.m_nCID
		, GDB_CODE::CD_L_PARTY_DELETE
		, GTimeCalculator::GetTimeAsString(GTimeSec(0))
		, data.m_nType));
}

void GDBManager::PartyKickOffLog( const GDBT_PARTY& data )
{
	m_pLogCaches->m_PartyLogcache.PushBack(
		GLOG_DATA_PARTY(data.m_uidParty
		, data.m_nAID
		, data.m_nCID
		, GDB_CODE::CD_L_PARTY_KICK_OFF
		, GTimeCalculator::GetTimeAsString(GTimeSec(0))
		, data.m_nType));
}

void GDBManager::RegistTraceTaskID()
{
	SDBAsyncTask::GetTaskTracer().PushTraceTaskID(SDBTID_LOGOUT);
	SDBAsyncTask::GetTaskTracer().PushTraceTaskID(SDBTID_CHARDIE);
	SDBAsyncTask::GetTaskTracer().PushTraceTaskID(SDBTID_CHARLOOTMONEY);
}


//////////////////////////////////////////////////////////////////////////
/// Expo 대비
bool GDBManager::ResetExpoCharacters()
{
	CString strSQL_Reset;
	strSQL_Reset.Format(L"{CALL dbo.USP_RZ_EXPO_RESET}");	///< Expo 캐릭터 데이터 날리기

	if (!Execute(strSQL_Reset))
		return false;

	CString strSQL_SetData;
	strSQL_SetData.Format(L"{CALL dbo.USP_RZ_EXPO_SET_DATA}");	///< Expo 캐릭터 데이터 셋팅

	if (!Execute(strSQL_SetData))
		return false;

	return true;
}

bool GDBManager::ResetExpoPublicBoothCharacter( CID nCID )
{
	CString strSQL_Reset;
	strSQL_Reset.Format(L"{CALL dbo.USP_RZ_EXPO_PUB_RESET_ME (%d)}", nCID);	///< Expo 캐릭터 데이터 날리기

	if (!Execute(strSQL_Reset))
		return false;

	CString strSQL_SetData;
	strSQL_SetData.Format(L"{CALL dbo.USP_RZ_EXPO_PUB_SET_ME_DATA (%d)}", nCID);	///< Expo 캐릭터 데이터 셋팅

	if (!Execute(strSQL_SetData))
		return false;

	return true;

}
