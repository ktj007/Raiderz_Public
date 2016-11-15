#ifndef _MOCK_DB_MANAGER_H_
#define _MOCK_DB_MANAGER_H_


#include "GDBManager.h"
#include "GTradeExchanger.h"
#include "CSChrInfo.h"

class GFakeDBManager : public GDBManager, public MTestMemPool<GFakeDBManager>
{
private:
	//virtual	bool BeginTrans() override { return true; }
	//virtual	bool CommitTrans() override { return true; }
	//virtual	bool Rollback() override { return true; }

public:
	GFakeDBManager();
	virtual ~GFakeDBManager();

	//virtual bool Connect(MDatabaseDesc& desc) override { return true; }
	//virtual void Disconnect() override {}
	//virtual bool Execute(LPCTSTR szSQL) override { return true; }
	//virtual bool Execute(CODBCRecordset& outRecordSet, LPCTSTR szSQL) override { return true; }
	
	// GM
	virtual void GM_QUEST_HISTORY_RESET_ALL(const int64 nAID, const int64 nCID) override {}
	virtual void GM_QUEST_RESET_ALL(const int64 nAID, const int64 nCID) override {}

	virtual void FieldPlayerLog(vector<pair<int, int>>& FieldPlayerCount) override {}

public:	
	virtual bool LoginGetInfo(GDBT_ACC_LOGIN& data);

	virtual void CharSelectLog(const GDBT_CHAR_SELECT_LOG& data) override {}
	virtual void DisconnLogInsert(const GDBT_DISCONN_LOG& data) override {}

	virtual bool GetCharInfo(const int nCID, GEntityPlayer* pPlayer, int& nBlankTime) { return true; }
	virtual bool GetEquipSlotList(const uint32 nCID, PLAYER_INFO* poutPlayerInfo) { return true; }

	virtual void UpdateDB_FromItemInfo() override {}

	virtual void DeleteItemInstanceForDebug(int nItemID) override {}
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
		const uint8 nItemSet,				// 기본 장비 세트 인덱스
		const int	nItemColor				// 기본 장비 세트 컬러
		
		) override { return true; }

	virtual bool CharLevelUp(GDBT_CHAR_LEVEL_UP_DATA& data) override;	// 캐릭터의 레벨 업데이트
	virtual bool CharLevelUpSync(GDBT_CHAR_LEVEL_UP_DATA& data) override;
	virtual void CharLevelUpLog(GDBT_CHAR_LEVEL_UP_DATA& data) override {}
	virtual void CharAddXPLog(GDBT_CHAR_XP_DATA& data) override {}

	virtual bool CharUpdatePlayerGrade(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nPlayerGrade) override { return true; }
	virtual bool CharAddMoney(const GDBT_CHAR_MONEY_INC_DEC& data) override;
	virtual bool CharMinusMoney(const GDBT_CHAR_MONEY_INC_DEC& data) override;
	virtual bool CharDie(GDBT_CHAR_KILL& die, GDBT_CHAR_KILL& killer, int nKillerNpcID) override { return true; }
	virtual void CharUpdateMoneyLog(const GLOG_DATA_MONEY& data) override {}

	virtual void FieldEnter(const GDBT_CHAR_ENTER_FILED& data) override {}

	virtual bool ItemLootInsert(const GDBT_ITEM_DATA& data) override;
	virtual bool ItemLootUpdate(const GDBT_ITEM_DATA& data) override;
	virtual void ItemLootLog(const GDBT_ITEM_DATA& data) override {}
	virtual bool ItemInsert(GDBT_ITEM_DATA& data) override;
	virtual void ItemInsertLog(GDBT_ITEM_DATA& data) override {}
	virtual bool ItemDelete(const GDBT_ITEM_DATA& data) override;
	virtual void ItemDeleteLog(const GDBT_ITEM_DATA& data) override {}
	virtual bool ItemDescDurability(GDBT_ITEM_DEC_DURA_DATA& data) override;
	virtual bool ItemUpdateSoulCnt(const int64 nCID, const uint8 nSlotType, const int16 nSlotID, const int64 nIUID, const uint8 nSoulCnt) override { return true; }
	virtual bool ItemLearnTalent(GDBT_ITEM_LEARN_TALENT& data) override;
	virtual bool ItemMove(GDBT_ITEM_MOVE& data) override;
	virtual void ItemMoveLog(GDBT_ITEM_MOVE& data) override {}
	virtual bool ItemMerge(const GDBT_ITEM_MERGE_AND_SPLIT& data) override;
	virtual bool ItemIncStackAmt(GDBT_ITEM_DATA& data) override;
	virtual bool ItemDecStackAmt(GDBT_ITEM_DEC_STACK_AMT_DATA& data) override;
	virtual void ItemDecStackAmtLog(GDBT_ITEM_DEC_STACK_AMT_DATA& data) override {};
	virtual bool ItemEnchant(const GDBT_ITEM_ENCH& data) override;
	virtual void ItemMergeAndSplitLog(const GDBT_ITEM_MERGE_AND_SPLIT& data) override {}


	// 크래프트 ----------------------------------------------
	virtual bool Craft_Insert(GDBT_CRAFT_DATA& data) override;
	virtual bool Craft_Update(GDBT_CRAFT_DATA& data) override;
	virtual void CraftLog(GDBT_CRAFT_DATA& data, int64 nProdIUID) override {}

	virtual bool Craft_RecpInsert(const GDBT_RECIPE& data) override {return true; }
	virtual bool Craft_RecpDelete(const GDBT_RECIPE& data) override {return true; }

	virtual bool Trade(GTRADE_CHAR_VEC& vTradeChar, const uint8 nItemInstnaceCnt) override;

	virtual bool NPCShop_BuyInsert(const GDBT_NPC_SHOP_TRADE_DATA& data) override;
	virtual bool NPCShop_BuyUpdate(const GDBT_NPC_SHOP_TRADE_DATA& data) override;		
	virtual void NPCShop_BuyLog(const GDBT_NPC_SHOP_TRADE_DATA& data) override {}

	virtual bool NPCShop_SellPartUpdate(const GDBT_NPC_SHOP_TRADE_DATA& data) override;
	virtual bool NPCShop_SellPartDelete(const GDBT_NPC_SHOP_TRADE_DATA& data) override;
	virtual bool NPCShop_SellVeryCommon(GDBT_SELL_ITEM_VERY_COMMON& data) override;
	virtual void NPCShop_SellLog(const GDBT_NPC_SHOP_TRADE_DATA& data) override {}
	virtual void NPCShop_SellVeryCommonLog(GDBT_SELL_ITEM_VERY_COMMON& data) override {}
	virtual void NPCShop_RepairLog(const GDBT_ITEM_REPAIR_CHAR& data) override {}

	virtual void TalentLearnLog(GDBT_TALENT& data) override {}
	virtual void TalentResetLog(GDBT_TALENT& data) override {}
	virtual bool Training_Train(GDBT_TALENT& data) override;
	virtual bool Training_TrainSync(GDBT_TALENT& data) override;
	virtual bool Training_UntrainAll(GDBT_TALENT& data) override;

	virtual bool LootIncreaseMoney(const GDBTASKDATA_LOOTINCREASEMONEY& taskData, const GDBCACHEDATA_CHARACTER& cacheData) { return true; }
	virtual bool PartyLootIncreaseMoney(const vector<GDBTASKDATA_LOOTINCREASEMONEY>& vecTaskData, const vector<GDBCACHEDATA_CHARACTER>& vecCacheData) { return true; }
	virtual bool ItemEquip(GDBT_ITEM_EQUIP& data) override;
	virtual bool ItemUnequip(GDBT_ITEM_UNEQUIP& data) override;

	virtual void ItemDyeLog(const GDBT_ITEM_DYE& data) override {}
	virtual bool ItemDye(const GDBT_ITEM_DYE& data) override;
	virtual bool SoulBindingSave(const GDBT_CHAR_SAVE_SOUL_BINDING& data) override { return true; }

	virtual bool GuildInsert(GDBT_GUILD& data) override;
	virtual bool GuildMemberInsert(GDBT_GUILD& data) override;
	virtual bool GuildDeleteMember(GDBT_GUILD& data) override;
	virtual bool GuildDelete(GDBT_GUILD& data) override;
	virtual bool GuildChangeMaster(GDBT_GUILD_CHANGE_MASTER& data) override;	
	virtual void GuildLog(GDBT_GUILD& data) override {}
	virtual void GuildMemberLog(GDBT_GUILD& data) override {}

	virtual void PartyCreateLog(const GDBT_PARTY& data) override {}
	virtual void PartyEnterLog(const GDBT_PARTY& data) override {}
	virtual void PartyLeaveLog(const GDBT_PARTY& data) override {}
	virtual void PartyKickOffLog(const GDBT_PARTY& data) override {}
	virtual void PartyChangeLeaderLog(const GDBT_PARTY& data) override {}
	virtual void PartyDeleteLog(const GDBT_PARTY& data) override {}

	virtual bool PalettePutUp(const GDBT_PALETTE_SET& data) override { return true; }
	virtual bool PalettePutDown(const GDBT_PALETTE_RESET& data) override { return true; }
	virtual bool PaletteChange(const GDBT_PALETTE_CHANGE& data) override { return true; }

	virtual bool GetCharTalent(GEntityPlayer* pPlayer) { return true; }
	virtual bool GetCharPalette(GEntityPlayer* pPlayer) { return true; }

	virtual bool GetBuffAndTalentCoolTime(int nCID, vector<REMAIN_BUFF_TIME>& outvecBuffRemainTime, vector<pair<int, float>>& outvecTalentCoolTime) { return true; }

	virtual bool CharSerialize(const GDBT_CHAR_SERIALIZE& data) override;
	virtual bool CharGetLookList(const MUID& uidPlayer, const int64 nAID) override;

	virtual bool QuestDone(GDBT_QEUST_DONE& data) override;
	virtual void QuestDoneLog(GDBT_QEUST_DONE& data) override {}
	virtual bool QuestAccept(GDBT_QUEST_ACCEPT& data) override;
	virtual void QuestAcceptLog(GDBT_QUEST_ACCEPT& data) override {}
	virtual bool QuestUpdateObject(const GDBT_QUEST_OBJECT& data) override {return true;}
	virtual void QuestGiveupLog(GDBT_QUEST_GIVEUP& data) override {}
	virtual bool QuestGiveup(GDBT_QUEST_GIVEUP& data) override;
	virtual bool QuestFail(const GDBT_QUEST_COMMON& data) override;
	virtual void QuestFailLog(const GDBT_QUEST_COMMON& data) override {}
	virtual bool QuestUpdateVar(const GDBT_QUEST_VAR& data) override {return true;}
	virtual bool QuestComplete(const GDBT_QUEST_COMMON& data ) override { return true; }
	virtual void QuestCompleteLog(const GDBT_QUEST_COMMON& data) override {}
	virtual bool QuestPVPERewardInven(GDBT_QPER_TOINVEN& data) override;
	virtual void QuestPVPERewardInvenLog(GDBT_QPER_TOINVEN& data) override {}
	virtual void PostQPvPInvenLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate) override {}
	virtual void PostQPvPInvenRewardXPLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate) override {}
	virtual void PostQPvPInvenRewardMoneyLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate) override {}
	virtual void PostQPvPInvenRewardItemLog(GDBT_QPER_TOINVEN& data, const wstring& strRegDate) override {}
	virtual bool QuestPVPERewardMail(GDBT_QPER_TOMAIL& data) override;
	virtual void QuestPVPERewardMailLog(GDBT_QPER_TOMAIL& data) override {}
	virtual void PostQPvPMailLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate) override {}
	virtual void PostQPvPMailRewardXPLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate) override {}
	virtual void PostQPvPMailRewardMoneyLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate) override {}
	virtual void PostQPvPMailRewardItemLog(GDBT_QPER_TOMAIL& data, const wstring& strRegDate) override {}

	virtual bool Logout(int64 nAID, int64 nCID, GEntityPlayer* pPlayer, vector<REMAIN_BUFF_TIME>& vecBuffRemainTime, vector<pair<int, float>>& vecTalentCoolTime) override { return true; }

	// server status
	virtual bool ServerStatusStart(const int nWorldID, const int nServerID, const wstring& strServerName, const wstring& strServerVersion, const wstring& strIP, const uint16 nPort, const int nMaxUser , const uint8 nType, const int nUpdateElapsedTimeSec, const int nAllowDelayTm) override { return true; }
	virtual bool ServerStatusUpdate(const int nWordID, const int nServerID, const int nCurUserCount, const bool bIsServable) override { return true; }

	// 우호도.
	virtual bool FactionInsert(const GDBT_DATA_FACTION& data) override { return true; }
	virtual bool FactionUpdate(const GDBT_DATA_FACTION& data) override { return true; }
	
	// 컷씬
	virtual bool CutsceneSawnInsert(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nCutsceneID) override { return true; }

	// 자격(Qualify)
	virtual bool EmblemInsert(const GDBT_EMBLEM& data) override;
	virtual bool EmblemDelete(const GDBT_EMBLEM& data) override;

	virtual void NPCKilledLog(const wstring& strRegDate, int nNpcID, const vector<GNPC_KILLER>& vKiller) override {}

private:
	void Test_PushDefaultItem(GEntityPlayer* pPlayer);

	static IUID FAKE_IUID;

public :
	static IUID NewIUID();	
};

#endif //_MOCK_DB_MANAGER_H_