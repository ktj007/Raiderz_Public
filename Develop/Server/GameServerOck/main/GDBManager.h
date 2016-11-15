#ifndef MMATCHDBMGR_H
#define MMATCHDBMGR_H

#include "SDBManager.h"
#include "MAsyncDatabase.h"
#include "CSPalette.h"
#include "CSDef.h"
#include "GITEM_STACK_AMT.h"
#include "GDBTaskNPCShopData.h"
#include "SDBTaskID.h"
#include "GDef.h"
#include "GPlayerTrade.h"
#include "GPlayerDBCacheMgr.h"
#include "CSTypes.h"

#include "GItemLogCache.h"
#include "GMoneyLogCache.h"
#include "GCraftLogCache.h"
#include "GQuestItemLogCache.h"
#include "GItemMergeLogCache.h"
#include "GNpcShopLogCache.h"
#include "GXPLogCache.h"
#include "GItemDeleteLogCache.h"
#include "GNpcKillerData.h"


class GEntityPlayer;
class GPlayerQuests;
class GTalentInfo;
class GInventory;
class GQuestInfoMgr;
class GRecipeInfo;
class GItemData;
class GItem;
class SAsyncDB;
class GPlayerDBCache;

namespace mdb
{
	class MDBRecordSet;

	struct MDatabaseDesc;
}


class GDBAsyncTask;
class GDBT_NPC_SHOP_TRADE_DATA;
class GDBT_SELL_ITEM_VERY_COMMON;
class GDBT_ITEM_REPAIR_CHAR;
class GDBT_REPAIR_ITEM;
class GDBT_REPAIR_ALL_ITEM;
class GDBT_CRAFT_DATA;
class GDBT_CRAFT_UPDATE_DATA;
class GDBT_CHAR_XP_DATA;
class GDBT_CHAR_LEVEL_UP_DATA;
class GDBT_ITEM_DATA;
class GDBT_ITEM_DEC_STACK_AMT_DATA;
class GDBT_ITEM_DEC_DURA_DATA;
class GDBT_QEUST_DONE;
class GDBT_QUEST_ACCEPT;
class GDBT_QUEST_COMMON;
class GDBT_QUEST_OBJECT;
class GDBT_QUEST_GIVEUP;
class GDBT_QUEST_VAR;
class GDBT_ITEM_MOVE;
class GDBT_ITEM;
class GDBT_ITEM_MERGE_AND_SPLIT;
class GDBT_CHAR_ENTER_FILED;
class GDBT_GUILD;
class GDBT_GUILD_CHANGE_MASTER;
class GDBT_TALENT;
class GDBT_ITEM_LEARN_TALENT;
class GDBT_ACC_INSERT;
class GDBT_ACC_LOGIN;
class GDBT_CHAR_DELETE;
class GDBT_CHAR_SERIALIZE;
class GDBT_CHAR_SAVE_SOUL_BINDING;
class GDBT_CHAR_REGIST_INN_ROOM;
class GDBT_PALETTE_SET;
class GDBT_PALETTE_CHANGE;
class GDBT_PALETTE_RESET;
class GDBT_RECIPE;
class GDBT_DATA_FACTION;
class GDBT_EMBLEM;
class GDBT_ITEM_ENCH;
class GDBT_ITEM_DYE;
class GDBT_EFF_REMAIN_TIME_INSERT;
class GDBT_QUESTPVP_EVENT_REWARD;
class GDBT_QPER_TOINVEN;
class GDBT_QPER_TOMAIL;
class GDBT_CHAR_MONEY_INC_DEC;
class GDBT_CHAR_KILL;
class GDBT_PARTY;
class GDBT_CHAR_SELECT_LOG;
class GDBT_DISCONN_LOG;
class GDBT_STORAGE_UPDATE_MONEY;
class GDBT_GUILD_UPDATE_STORAGEMONEY;


struct TD_ITEM;
struct GDBTASKDATA_LOOTINCREASEMONEY;
struct GDBCACHEDATA_CHARACTER;
struct GDBT_ITEM_EQUIP;
struct GDBT_ITEM_UNEQUIP;
struct GDBT_TRADE_DATA;
struct GDBT_MAIL_GET_MAILBOX_LIST;
struct GDBT_MAIL_WRITE;
struct GDBT_MAIL_DELETE;
struct GDBT_MAIL_GET_ITEM;
struct GDBT_MAIL_GET_MONEY;
struct GDBT_MAIL_WRITE;
struct GDBT_MAIL_WRITE_RESULT;
struct GDBT_MAIL_GET_MONEY;

class GLOG_DATA_MONEY;
class GLogCacheContainer;

class GEffRemainTimeSqlBuilder;

enum PROFILE_DATA_TYPE
{
	PDT_DBTASK	= 1,
	PDT_COMMAND = 11
};


/// DB 관리자
class GDBManager : public SDBManager
{
public:
	GDBManager();
	virtual ~GDBManager();
public:
	// 서비스때는 사용하지 않음 ---------------
	/// DB의 Item 테이블의 내용을 item.xml에 맞게 변경한다. - 디버그만 사용. 나중에 서비스될 때 다른 방법으로 변경할 예정 - birdkr
	virtual void UpdateDB_FromItemInfo();
	virtual void DeleteItemInstanceForDebug(int nItemID);

	virtual void UpdateDB_FromQeustInfo();
	// 서비스때는 사용하지 않음 --------------^
	// 

	virtual void FieldPlayerLog(vector<pair<int, int>>& FieldPlayerCount);

	// GM
	virtual void GM_QUEST_HISTORY_RESET_ALL(const int64 nAID, const int64 nCID);
	virtual void GM_QUEST_RESET_ALL(const int64 nAID, const int64 nCID);

	// 계정 관련 ------------------------------------------------
	virtual bool LoginGetInfo(GDBT_ACC_LOGIN& data);
	virtual bool AccInsert(unsigned int& outAID,		// 반환값 : AID 
		GDBT_ACC_INSERT& data
	);

	virtual bool AccDelete(const unsigned int nAID);

	virtual void CharSelectLog(const GDBT_CHAR_SELECT_LOG& data);
	virtual void DisconnLogInsert(const GDBT_DISCONN_LOG& data);

	// 캐릭터 관리 관련 ----------------------------------------
	virtual bool CharGetLookList(const MUID& uidPlayer, const int64 nAID);
	virtual bool CharSerialize(const GDBT_CHAR_SERIALIZE& data);

	virtual bool CharInsert(const MUID& uidPlayer,
		const int nAID,						// AID
		const wchar_t* szName,					// 캐릭터 이름
		const uint8 nLevel, 
		const int nXP, 
		const int nMoney,
		const int nRace,					// 종족
		const int nSex,						// 성별
		const int nFeatureHair,				// 머리 스타일
		const int nFeatureFace,				// 얼굴
		const short nFeatureHairColor,		// 머리색
		const short nFeatureSkinColor,		// 피부색
		const int nVoice,					// 목소리
		const int nHP,
		const int nEN,
		const int nSTA,
		const int8 nTotalTP,
		const int nFatigue,
		const int nSoulBindingID,
		const int nInnRoomID,
		const int nPrimaryPalettedID,
		const int nSecondaryPalettedID,
		const TALENT_STYLE nTalentStyle,
		const uint8 nItemSet,
		const int	nItemColor		
	);	
	virtual bool CharDelete(const GDBT_CHAR_DELETE& data);

	// 캐릭터 ----------------------------------------
	virtual bool PostEffRemainTimeInsert(const GDBT_EFF_REMAIN_TIME_INSERT& data, GEffRemainTimeSqlBuilder& efb);
	virtual bool EffRemainTimeInsert(const GDBT_EFF_REMAIN_TIME_INSERT& data);
	virtual bool Logout(int64 nAID, int64 nCID, GEntityPlayer* pPlayer, vector<REMAIN_BUFF_TIME>& vecBuffRemainTime, vector<pair<int, float>>& vecTalentCoolTime);

	vec3 GetPlayerPos( GEntityPlayer* pPlayer );
	virtual bool CharAddXP(GDBT_CHAR_XP_DATA& data);
	virtual void CharAddXPLog(GDBT_CHAR_XP_DATA& data);
	virtual bool CharLevelUp(GDBT_CHAR_LEVEL_UP_DATA& data);	// 캐릭터의 레벨 업데이트
	virtual bool CharLevelUpSync(GDBT_CHAR_LEVEL_UP_DATA& data);
	virtual void CharLevelUpLog(GDBT_CHAR_LEVEL_UP_DATA& data);
	virtual bool CharAddMoney(const GDBT_CHAR_MONEY_INC_DEC& data);
	virtual bool CharMinusMoney(const GDBT_CHAR_MONEY_INC_DEC& data);
	virtual bool CharRegistInnRoom(const GDBT_CHAR_REGIST_INN_ROOM& data);				// 캐릭터의 여관방 업데이트
	virtual bool CharUpdatePlayerGrade(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nPlayerGrade);
	virtual bool CharDie(GDBT_CHAR_KILL& die, GDBT_CHAR_KILL& killer, int nKillerNpcID);
		
		/*const int64 nGSN, const int nWorldID, const MUID& uidPlayer, const int64 nPlayerCID, int nCode, const int nPlayTime
		, const int nLevel, const int nXP, const int nMoney, const int nFieldID, const float fX, const float fY, const float fZ
		, int nKillerNpcID, const MUID& uidKillerPlayer, int nKillerCode);*/

	virtual bool GM_CharUpdateTP(const int64 nAID, const int64 nCID, const int16 nRemainTP, const int16 nTotalTP, const int nCharPtm);		// 캐릭터의 TP 추가
	virtual void CharUpdateMoneyLog(const GLOG_DATA_MONEY& data);
	
	// 아이템 --------------------------------------------------
	virtual bool ItemInsert(GDBT_ITEM_DATA& data);
	virtual void ItemInsertLog(GDBT_ITEM_DATA& data);
	virtual bool ItemLootInsert(const GDBT_ITEM_DATA& data);
	virtual bool ItemLootUpdate(const GDBT_ITEM_DATA& data);
	virtual void ItemLootLog(const GDBT_ITEM_DATA& data);
	virtual bool ItemDelete(const GDBT_ITEM_DATA& data);						/// 아이템 삭제	
	virtual void ItemDeleteLog(const GDBT_ITEM_DATA& data);
	virtual bool ItemEquip(GDBT_ITEM_EQUIP& data);	/// 아이템 장비
	virtual bool ItemUnequip(GDBT_ITEM_UNEQUIP& data);	/// 아이템 장비 해제
	virtual bool ItemDescDurability(GDBT_ITEM_DEC_DURA_DATA& data);	
	virtual void ItemDyeLog(const GDBT_ITEM_DYE& data);
	virtual bool ItemDye(const GDBT_ITEM_DYE& data);
	virtual bool ItemUpdateSoulCnt(const int64 nCID, const uint8 nSlotType, const int16 nSlotID, const int64 nIUID, const uint8 nSoulCnt);
	virtual bool ItemLearnTalent(GDBT_ITEM_LEARN_TALENT& data);
	virtual bool ItemMove(GDBT_ITEM_MOVE& data);
	virtual void ItemMoveLog(GDBT_ITEM_MOVE& data);
	virtual bool ItemSplit(GDBT_ITEM_MERGE_AND_SPLIT& data);
	virtual bool ItemMerge(const GDBT_ITEM_MERGE_AND_SPLIT& data);
	virtual void ItemMergeAndSplitLog(const GDBT_ITEM_MERGE_AND_SPLIT& data);
	virtual bool ItemIncStackAmt(GDBT_ITEM_DATA& data);
	virtual bool ItemDecStackAmt(GDBT_ITEM_DEC_STACK_AMT_DATA& data);
	virtual void ItemDecStackAmtLog(GDBT_ITEM_DEC_STACK_AMT_DATA& data);
	virtual bool ItemEnchant(const GDBT_ITEM_ENCH& data);
	virtual void ItemEnchLog(const GDBT_ITEM_ENCH& data);
	
	// 트레이드 --------------------------------------------------
	virtual bool Trade(GTRADE_CHAR_VEC& vTradeChar, const uint8 nItemInstanceCnt);
	virtual void TradeLog(const GDBT_TRADE_DATA& data);

	// 엔피씨 상점 --------------------------------------------------	
	virtual bool NPCShop_BuyInsert(const GDBT_NPC_SHOP_TRADE_DATA& data);
	virtual bool NPCShop_BuyUpdate(const GDBT_NPC_SHOP_TRADE_DATA& data);		
	virtual void NPCShop_BuyLog(const GDBT_NPC_SHOP_TRADE_DATA& data);
	virtual bool NPCShop_SellPartUpdate(const GDBT_NPC_SHOP_TRADE_DATA& data);
	virtual bool NPCShop_SellPartDelete(const GDBT_NPC_SHOP_TRADE_DATA& data);
	bool MakeSellVeryCommonList(GITEM_STACK_AMT_VEC& vec, wstring& strString);
	virtual bool NPCShop_SellVeryCommon(GDBT_SELL_ITEM_VERY_COMMON& data);
	virtual void NPCShop_SellLog(const GDBT_NPC_SHOP_TRADE_DATA& data);
	virtual void NPCShop_SellVeryCommonLog(GDBT_SELL_ITEM_VERY_COMMON& data);
	virtual bool NPCShop_Repair(GDBT_REPAIR_ITEM& data);
	virtual bool NPCShop_RepairAll(GDBT_REPAIR_ALL_ITEM& data);
	bool MakeRepairAllList(GDBT_ITEM_REPAIR_VEC& vec, wstring& strString);
	virtual void NPCShop_RepairLog(const GDBT_ITEM_REPAIR_CHAR& data);
	


	// 트레이닝 ----------------------------------------	
	virtual bool Training_Train(GDBT_TALENT& data);	/// 탤런트 훈련	
	virtual bool Training_TrainSync(GDBT_TALENT& data);
	virtual bool Training_UntrainAll(GDBT_TALENT& data);	/// 전체 탤런트 초기화
	virtual void TalentLearnLog(GDBT_TALENT& data);
	virtual void TalentResetLog(GDBT_TALENT& data);

	// 루팅 --------------------------------------------------
	virtual bool LootIncreaseMoney(const GDBTASKDATA_LOOTINCREASEMONEY& taskData, const GDBCACHEDATA_CHARACTER& cacheData);
	virtual bool PartyLootIncreaseMoney(const vector<GDBTASKDATA_LOOTINCREASEMONEY>& vecTaskData, const vector<GDBCACHEDATA_CHARACTER>& vecCacheData);

	// 소울 바인딩 --------------------------------------------
	virtual bool SoulBindingSave(const GDBT_CHAR_SAVE_SOUL_BINDING& data);

	bool MakeStackAmtList(GITEM_STACK_AMT_VEC& vecItems, const wstring& strObject, const size_t nMaxStringLen, const size_t nMaxItemCnt, wstring& strList);

	// 크래프트 ----------------------------------------------
	virtual bool Craft_Insert(GDBT_CRAFT_DATA& data);
	virtual bool Craft_Update(GDBT_CRAFT_DATA& data);
	virtual void CraftLog(GDBT_CRAFT_DATA& data, int64 nProdIUID);
	virtual bool Craft_RecpInsert(const GDBT_RECIPE& data);
	virtual bool Craft_RecpDelete(const GDBT_RECIPE& data);

	// 보관함
	virtual bool StorageSerialize(const MUID& uidPlayer, const int64 nCID);
	virtual bool StorageUpdateMoney(const GDBT_STORAGE_UPDATE_MONEY& data);


	// 길드 --------------------------------------------------
	virtual bool GuildSerialize(const MUID& uidPlayer, const int64 nGID);
	virtual bool GuildInsert(GDBT_GUILD& data);
	virtual bool GuildMemberInsert(GDBT_GUILD& data);
	virtual bool GuildDeleteMember(GDBT_GUILD& data);
	virtual bool GuildDelete(GDBT_GUILD& data);
	virtual bool GuildChangeMaster(GDBT_GUILD_CHANGE_MASTER& data);
	virtual void GuildLog(GDBT_GUILD& data);
	virtual void GuildMemberLog(GDBT_GUILD& data);
	virtual bool GuildUpdateStorageMoney(GDBT_GUILD_UPDATE_STORAGEMONEY& data);

	// 파티
	virtual void PartyCreateLog(const GDBT_PARTY& data);
	virtual void PartyEnterLog(const GDBT_PARTY& data);
	virtual void PartyLeaveLog(const GDBT_PARTY& data);
	virtual void PartyKickOffLog(const GDBT_PARTY& data);
	virtual void PartyChangeLeaderLog(const GDBT_PARTY& data);
	virtual void PartyDeleteLog(const GDBT_PARTY& data);

	// 팔레트 --------------------------------------------------
	virtual bool PalettePutUp(const GDBT_PALETTE_SET& data);
	virtual bool PalettePutDown(const GDBT_PALETTE_RESET& data);
	virtual bool PaletteChange(const GDBT_PALETTE_CHANGE& data);	

	// 퀘스트 --------------------------------------------------
	virtual bool QuestAccept(GDBT_QUEST_ACCEPT& data);
	virtual void QuestAcceptLog(GDBT_QUEST_ACCEPT& data);
	virtual bool MakeQuestGiveupList(GITEM_STACK_AMT_VEC& vec, wstring& strString);
	virtual void QuestGiveupLog(GDBT_QUEST_GIVEUP& data);
	virtual bool QuestGiveup(GDBT_QUEST_GIVEUP& data);
	virtual bool QuestFail(const GDBT_QUEST_COMMON& data);
	virtual void QuestFailLog(const GDBT_QUEST_COMMON& data);
	virtual bool QuestComplete(const GDBT_QUEST_COMMON& data);
	virtual void QuestCompleteLog(const GDBT_QUEST_COMMON& data);
	virtual bool MakeQuestRemoveItemList(GITEM_STACK_AMT_VEC& vec, wstring& strString);
	virtual bool QuestDone(GDBT_QEUST_DONE& data);
	virtual void QuestDoneLog(GDBT_QEUST_DONE& data);
	virtual bool QuestUpdateObject(const GDBT_QUEST_OBJECT& data);
	virtual bool QuestUpdateVar(const GDBT_QUEST_VAR& data);
	virtual bool QuestPVPERewardInven(GDBT_QPER_TOINVEN& data);
	virtual bool QuestPVPERewardMail(GDBT_QPER_TOMAIL& data);
	virtual void QuestPVPERewardInvenLog(GDBT_QPER_TOINVEN& data);
	virtual void PostQPvPInvenLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate);
	virtual void PostQPvPInvenRewardXPLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate);
	virtual void PostQPvPInvenRewardMoneyLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate);
	virtual void PostQPvPInvenRewardItemLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate);
	virtual void QuestPVPERewardMailLog(GDBT_QPER_TOMAIL& data);
	virtual void PostQPvPMailLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate);
	virtual void PostQPvPMailRewardXPLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate);
	virtual void PostQPvPMailRewardMoneyLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate);
	virtual void PostQPvPMailRewardItemLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate);
	
	
	// 우호도
	virtual bool FactionInsert(const GDBT_DATA_FACTION& data);
	virtual bool FactionUpdate(const GDBT_DATA_FACTION& data);
	
	// 컷씬
	virtual bool CutsceneSawnInsert(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nCutsceneID);

	// 우편
	virtual bool MailGetList(const GDBT_MAIL_GET_MAILBOX_LIST& data, bool bOpenMaibox);
	virtual bool MailCheckReceiver(const GEntityPlayer* pPlayer, const GDBT_MAIL_WRITE& data);
	virtual bool MailWrite(const MUID& uidPlayer, const GDBT_MAIL_WRITE& data);
	virtual bool MailGetContent(const MUID& uidPlayer, int64 nMUID, bool bHasText);
	virtual bool MailSetRead(const MUID& uidPlayer, int64 nAID, int64 nCID, int64 nMUID);
	virtual bool MailDelete(const MUID& uidPlayer, const GDBT_MAIL_DELETE& data);
	virtual bool MailGetAppendedItem(const GDBT_MAIL_GET_ITEM& data);
	virtual bool MailGetAppendedMoney(const MUID& uidPlayer, const GDBT_MAIL_GET_MONEY& data);
	virtual void MailGetAppendedMoneyLog(const GDBT_MAIL_GET_MONEY& data);
	virtual void MailGetAppendedItemLog(const GDBT_MAIL_GET_ITEM& data);
	virtual void MailWriteLog(const GDBT_MAIL_WRITE& mw, const GDBT_MAIL_WRITE_RESULT& res);
	virtual void MailDeleteLog(const GDBT_MAIL_DELETE& data);

	// emblem
	virtual bool EmblemInsert(const GDBT_EMBLEM& data);
	virtual bool EmblemDelete(const GDBT_EMBLEM& data);
	
	// log.
	virtual bool ServerProfileInsert(const int nWorldID, const int nServerID, const PROFILE_DATA_TYPE nType, const std::wstring& strRegDt, const int nID, const int nCnt, const int64 nWorkTm/*millisecond*/);

	// field move
	virtual void FieldEnter(const GDBT_CHAR_ENTER_FILED& data);

	// npc kill.
	virtual void NPCKilledLog(const wstring& strRegDate, int nNpcID, const vector<GNPC_KILLER>& vKiller);

	// server status
	virtual bool ServerStatusStart(const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm);
	virtual bool ServerStatusUpdate(const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable);

	// Expo 대비
	virtual bool ResetExpoCharacters();
	virtual bool ResetExpoPublicBoothCharacter(CID nCID);

private :
	int CalculateElapsedDeadTimeSec(GEntityPlayer* pPlayer);
	

public :
	bool				InitAsyncDB();

	virtual bool		Post(GDBAsyncTask* pTask);
	
	bool				InitGameDB();
	bool				InitGameDBSelectChar();
	bool				InitLogDB();
	void				InitLogCache();

	void				RegistTraceTaskID();
	
	void				Update();
	void				Release();
	void				LogingDBTaskExecCnt();

	void				ReleaseGameDB();
	void				ReleaseGameDBSelectChar();
	void				ReleaseLogDB();
	void				ReleaseLogCaches();
	
	DWORD				GetAsyncQSize();
	
	void				ExecuteAsyncForDev(const MUID& uidPlayer, wchar_t* szSQL);
	
	uint32				GetGameDBQCount()			{ return m_nGameDBQCount; }
	uint32				GetGameDBSelectCharQCount() { return m_nGameDBSelectCharQCount; }
	uint32				GetLogDBQCount()			{ return m_nLogDBQCount; }

	bool				IsDirectPostItem(int nItemID);

protected :
	SAsyncDB*						m_pGameDB;
	SAsyncDB*						m_pGameDBSelectChar;
	SAsyncDB*						m_pLogDB;	
	uint32							m_nGameDBQCount;
	uint32							m_nGameDBSelectCharQCount;
	uint32							m_nLogDBQCount;

	GLogCacheContainer*				m_pLogCaches;
};




#endif
