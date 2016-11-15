#include "stdafx.h"
#include "MockDBManager.h"
#include "GDBTaskHeaderList.h"
#include "GPlayerObjectManager.h"
#include "GDBCacheData.h"
#include "GDBTaskData.h"
#include "GDBTaskDataEmblem.h"
#include "GDBTaskMailWrite.h"
#include "GDB_CODE.h"
#include "GEntityPlayer.h"
#include "GDBTaskStorageSerialize.h"
#include "GDBTaskStorageUpdateMoney.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GDBTaskItemSplit.h"
#include "GDBTaskGuildUpdateStorageMoney.h"
#include "GDBTaskTradeMarketData.h"
#include "GDBTaskTradeMarketPutOn.h"



void MockDBManager::InitRef( MUID uidPlayer )
{
	m_uidPlayer = uidPlayer;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pPlayer) return;

	m_nRefCount = pPlayer->GetDBAsyncMonitor().GetRefCount();
}

bool MockDBManager::CheckRef()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidPlayer);
	if (NULL == pPlayer) return false;

	return m_nRefCount == pPlayer->GetDBAsyncMonitor().GetRefCount();
}


MockDBManager::MockDBManager()
{

}


MockDBManager::~MockDBManager()
{

}

bool MockDBManager::QuestDone( GDBT_QEUST_DONE& data )
{
	InitRef(data.m_QuestComm.m_uidPlayer);

	GDBTaskQuestDone t(data.m_QuestComm.m_uidPlayer);
	
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::QuestAccept( GDBT_QUEST_ACCEPT& data )
{
	InitRef(data.m_QuestComm.m_uidPlayer);

	GDBTaskQuestAccept t(data.m_QuestComm.m_uidPlayer);

	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::QuestGiveup( GDBT_QUEST_GIVEUP& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskQuestGiveup t(data.m_uidPlayer);
	
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::QuestFail( const GDBT_QUEST_COMMON& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskQuestFail t(data.m_uidPlayer);
	
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::Training_Train(GDBT_TALENT& data)
{
	InitRef(data.m_uidPlayer);

	GDBTaskTalentLearn t(data.m_uidPlayer);
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();
	
	return CheckRef();
}

bool MockDBManager::Training_TrainSync( GDBT_TALENT& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskTalentLearn t(data.m_uidPlayer);
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::CharAddMoney( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskCharIncMoney t(data.m_uidPlayer, SDBTID_CHAR_INC_MONEY);

	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::CharMinusMoney( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskCharDecMoney t(data.m_uidPlayer, SDBTID_CHAR_DEC_MONEY);

	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

class MockDBTaskItemBuy : public GDBTaskItemBuyInsert
{
public :
	MockDBTaskItemBuy(const MUID& uidReqPlayer) : GDBTaskItemBuyInsert(uidReqPlayer)
	{
		
	}

	void SetData(const int64 nNewIUID)
	{
		m_Result.nIUID = nNewIUID;
	}
};

bool MockDBManager::NPCShop_BuyInsert( const GDBT_NPC_SHOP_TRADE_DATA& data  )
{
	InitRef(data.m_uidPlayer);

	MockDBTaskItemBuy t(data.m_uidPlayer);

	t.Input(data);
	t.SetData(123456);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::NPCShop_BuyUpdate( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemBuyUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

//bool MockDBManager::ItemInsert( const MUID& uidPlayer, const int nCID, const int nItemID, const int nQty, const int nMaxQty, const int nDura, const int nColor, bool bBind, const uint8 nUsageState , const int nUsagePeriod, const int nAddExpiTimeMin, const int nPlayTime, const uint8 nCreateType, const bool bWriteLog , vector<DB_ITEM_UPDATE_QUANTITY>& vecRet )
//{
//	const int nCnt	= nQty / nMaxQty;
//	const int nRQty = nQty % nMaxQty;
//
//	int i;
//	for (i = 0; i < nCnt; ++i)
//	{
//		DB_ITEM_UPDATE_QUANTITY item;
//
//		item.m_nID			= nItemID;
//		item.m_nIUID		= 10000 + i;
//		item.m_nQuantity	= nMaxQty;
//		item.m_nType			= 0;
//
//		vecRet.push_back(item);
//	}
//
//	if (0 > nRQty)
//	{
//		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
//		_ASSERT(NULL != pPlayer);
//		if (NULL == pPlayer)
//			return false;
//
//		DB_ITEM_UPDATE_QUANTITY item;
//
//		GItem* pItem = pPlayer->GetItemInventory().GetItemByIDAndMinQuantity(nItemID);
//		if (NULL == pItem)
//		{
//			item.m_nID			= nItemID;
//			item.m_nIUID		= 10000 + (i++);
//			item.m_nQuantity	= nMaxQty;
//			item.m_nType			= 0;
//
//			vecRet.push_back(item);
//		}
//		else if (pItem->GetAmount() + nRQty <= nMaxQty)
//		{
//			pItem->GetAmount() += nRQty;
//		}
//		else
//		{
//			item.m_nID			= nItemID;
//			item.m_nIUID		= 10000 + (i++);
//			item.m_nQuantity	= nMaxQty;
//			item.m_nType			= 0;
//
//			vecRet.push_back(item);
//		}
//	}
//
//	return CheckRef();
//}

class MockDBTaskItemInsert : public GDBTaskItemInsert
{
public :
	MockDBTaskItemInsert(const MUID& uidReqPlayer) : GDBTaskItemInsert(uidReqPlayer) {}

	void SetResultIUID(const IUID nIUID)
	{
		m_Result.m_nIUID = nIUID;
	}
};

bool MockDBManager::ItemInsert( GDBT_ITEM_DATA& data )
{
	InitRef(data.m_uidPlayer);

	static IUID nIUID = 1;
	MockDBTaskItemInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetResultIUID(nIUID++);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::NPCShop_SellPartUpdate( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemSellUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

bool MockDBManager::NPCShop_SellPartDelete( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemSellDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

bool MockDBManager::NPCShop_SellVeryCommon( GDBT_SELL_ITEM_VERY_COMMON& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemSellVeryCommon t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}


bool MockDBManager::Trade( GTRADE_CHAR_VEC& vTradeChar, const uint8 nItemInstnaceCnt )
{
	GDBTaskTrade t;

	t.Input(vTradeChar);
	t.SetTaskSuccess();		
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

class MockDBTAskCraftInsert : public GDBTaskCraftInsert 
{
public :
	MockDBTAskCraftInsert(const MUID& uidReqPlayer) : GDBTaskCraftInsert(uidReqPlayer) {}

	void SetNewIUID(const int64 nIUID) { m_Result.nIUID = nIUID; }
};

bool MockDBManager::Craft_Insert( GDBT_CRAFT_DATA& data )
{
	InitRef(data.m_uidPlayer);

	MockDBTAskCraftInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.SetNewIUID(100001);
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::Craft_Update( GDBT_CRAFT_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskCraftUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return false;
}

bool MockDBManager::Training_UntrainAll( GDBT_TALENT& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskTalentResetAll t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

class MockDBTaskItemLootInsert : public GDBTaskItemLootInsert
{
public :
	MockDBTaskItemLootInsert(const MUID& uidPlayer) : GDBTaskItemLootInsert(uidPlayer) {}
	void SetIUID(const int64 nIUID)
	{
		m_Result.m_nIUID = nIUID;
	}
};

bool MockDBManager::ItemLootInsert(const GDBT_ITEM_DATA& data)
{
	InitRef(data.m_uidPlayer);

	MockDBTaskItemLootInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetIUID(1);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

bool MockDBManager::ItemLootUpdate(const GDBT_ITEM_DATA& data)
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemLootUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

bool MockDBManager::ItemLearnTalent(GDBT_ITEM_LEARN_TALENT& data)
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemLearnTalent t(data.m_uidPlayer);

	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

class MockDBTaskGuildSerialize : public GDBTaskGuildSerialize
{
public :
	MockDBTaskGuildSerialize(const MUID& uidReqPlayer) : GDBTaskGuildSerialize(uidReqPlayer)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidReqPlayer);
		if (NULL == pPlayer) return;

		GDBTASK_GUILD_MEMBER gm;
		gm.nAID = pPlayer->GetAID();
		gm.nCID = pPlayer->GetCID();
		gm.strCharName = pPlayer->GetName();
		gm.nLevel = pPlayer->GetLevel();
		gm.nGuildGrade = GMG_NORMAL;
		m_Data.vecGuildMember.push_back(gm);
	}
};


bool MockDBManager::GuildSerialize( const MUID& uidPlayer, const int64 nGID )
{
	InitRef(uidPlayer);

	MockDBTaskGuildSerialize t(uidPlayer);
	t.Input(nGID);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

class MockDBTaskGuildInsert : public GDBTaskGuildInsert
{
public :
	MockDBTaskGuildInsert(const MUID& uidReqPlayer) : GDBTaskGuildInsert(uidReqPlayer)
	{
		m_Data.m_nGID = 1005;
	}
};

bool MockDBManager::GuildInsert( GDBT_GUILD& data )
{
	InitRef(data.m_uidPlayer);

	data.m_nGID = SUnitTestUtil::NewGID();

	MockDBTaskGuildInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

class MockDBTaskGuildDelete : public GDBTaskGuildDelete
{
public :
	MockDBTaskGuildDelete(const MUID& uidReqPlayer) : GDBTaskGuildDelete(uidReqPlayer) {}
};

bool MockDBManager::GuildDelete( GDBT_GUILD& data )
{
	InitRef(data.m_uidPlayer);

	MockDBTaskGuildDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

class MockDBTaskGuildMemberInsert : public GDBTaskGuildMemberInsert
{
public :
	MockDBTaskGuildMemberInsert(const MUID& uidReqPlayer) : GDBTaskGuildMemberInsert(uidReqPlayer) {}
};

bool MockDBManager::GuildMemberInsert( GDBT_GUILD& data )
{
	InitRef(data.m_uidPlayer);

	MockDBTaskGuildMemberInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return CheckRef();
}

class MockDBTaskItemSwapSlot : public GDBTaskItemSwapSlot
{
public :
	MockDBTaskItemSwapSlot(const MUID& uidReqPlayer) : GDBTaskItemSwapSlot(uidReqPlayer)
	{

	}
};
bool MockDBManager::ItemMove(GDBT_ITEM_MOVE& data)
{
	InitRef(data.m_uidPlayer);

	MockDBTaskItemSwapSlot t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::ItemSplit( GDBT_ITEM_MERGE_AND_SPLIT& data )
{
	InitRef(data.m_uidPlayer);

	data.m_ToSlot.m_nIUID = SUnitTestUtil::NewIUID();

	GDBTaskItemSplit t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

class MockDBTaskGuildMemberDelete : public GDBTaskGuildMemberDelete
{
public :
	MockDBTaskGuildMemberDelete(const MUID& uidReqPlayer) : GDBTaskGuildMemberDelete(uidReqPlayer) {}

};

bool MockDBManager::GuildDeleteMember( GDBT_GUILD& data )
{
	InitRef(data.m_uidPlayer);

	MockDBTaskGuildMemberDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

class MockDBTaskGuildMasterChange : public GDBTaskGuildMasterChange
{
public :
	MockDBTaskGuildMasterChange(const MUID& uidCurMasger) : GDBTaskGuildMasterChange(uidCurMasger) {}
};

bool MockDBManager::GuildChangeMaster( GDBT_GUILD_CHANGE_MASTER& data )
{
	InitRef(data.m_uidPreMaster);

	MockDBTaskGuildMasterChange t(data.m_uidPreMaster);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::GuildUpdateStorageMoney( GDBT_GUILD_UPDATE_STORAGEMONEY& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskGuildUpdateStorageMoney t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}


bool MockDBManager::ItemMerge( const GDBT_ITEM_MERGE_AND_SPLIT& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemMerge t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::EmblemInsert( const GDBT_EMBLEM& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskEmblemInsert t(data.m_uidPlayer);
	t.Input(data.m_nEmblemID);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::EmblemDelete( const GDBT_EMBLEM& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskEmblemDelete t(data.m_uidPlayer);
	t.Input(data.m_nEmblemID);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::ItemDelete( const GDBT_ITEM_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::ItemDescDurability( GDBT_ITEM_DEC_DURA_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemDecDura t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::NPCShop_Repair( GDBT_REPAIR_ITEM& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemRepair t(data.m_uidPlayer);
	
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::NPCShop_RepairAll( GDBT_REPAIR_ALL_ITEM& data )
{
	InitRef(data.m_uidPlayer);

	if (0 == data.m_vRepair.size())
		return CheckRef();

	GDBTaskItemRepairAll t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::PartyLootIncreaseMoney( const vector<GDBTASKDATA_LOOTINCREASEMONEY>& vecTaskData, const vector<GDBCACHEDATA_CHARACTER>& vecCacheData )
{
	vector<GDBTaskCharIncMoney*> vTask;
	vTask.reserve(vecTaskData.size());
	bool bSuccess = true;
	for (size_t i = 0; i < vecTaskData.size(); ++i)
	{
		LootIncreaseMoney(vecTaskData[i], vecCacheData[i]);
	}

	return true;
}

bool MockDBManager::ItemEquip( GDBT_ITEM_EQUIP& data )
{
	InitRef(data.uidPlayer);

	GDBTaskItemEquip t(data.uidPlayer);

	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();	

	return CheckRef();
}

bool MockDBManager::ItemUnequip( GDBT_ITEM_UNEQUIP& data )
{
	InitRef(data.uidPlayer);

	GDBTaskItemUnequip t(data.uidPlayer);

	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();	

	return CheckRef();
}

bool MockDBManager::LootIncreaseMoney( const GDBTASKDATA_LOOTINCREASEMONEY& taskData, const GDBCACHEDATA_CHARACTER& cacheData )
{
	InitRef(taskData.uidPlayer);

	GDBTaskCharIncMoney t(taskData.uidPlayer, SDBTID_CHARLOOTMONEY);
	
	if (!t.Input(GDBT_CHAR_MONEY_INC_DEC(taskData.uidPlayer, taskData.nAID, taskData.nCID, GDB_CODE::CD_L_MONEY_LOOT, taskData.nCharPlayTime, taskData.nDeltaCharPtm
		, taskData.nLevel, taskData.nLevel, taskData.nMoney, taskData.nModMoney, false, taskData.nNpcID)))
		return false;

	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();	

	return CheckRef();
}

bool MockDBManager::CharLevelUp( GDBT_CHAR_LEVEL_UP_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskCharLevelUp Task(data.m_uidPlayer);

	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return CheckRef();
}

bool MockDBManager::CharLevelUpSync( GDBT_CHAR_LEVEL_UP_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskCharLevelUp Task(data.m_uidPlayer);

	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return CheckRef();
}

bool MockDBManager::ItemIncStackAmt( GDBT_ITEM_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemIncStackAmt Task(data.m_uidPlayer);
	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return CheckRef();
}

bool MockDBManager::ItemDecStackAmt( GDBT_ITEM_DEC_STACK_AMT_DATA& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemDecStackAmt Task(data.m_uidPlayer);
	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return CheckRef();
}

bool MockDBManager::Post( GDBAsyncTask* pTask )
{
	if (NULL == pTask)
		return false;

	if (SDBTID_CACHE_SYNC == pTask->GetID())
	{
		GDBTaskCacheFlush* pSyncTask = (GDBTaskCacheFlush*)pTask;

		pSyncTask->GetCharQuery().SetSuccess();

		for (size_t i = 0; i < pSyncTask->GetItemQueryVec().size(); ++i)
			pSyncTask->GetItemQueryVec()[i].SetSuccess();

	}

	pTask->OnPrePost();
	pTask->SetTaskSuccess();
	pTask->OnCompleted();
	SAFE_DELETE(pTask);

	return true;
}

bool MockDBManager::ItemEnchant( const GDBT_ITEM_ENCH& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemEnchant t(data.m_uidPlayer);
	if (!t.Input(data))
		return false;

	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::ItemDye( const GDBT_ITEM_DYE& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskItemDye Task(data.m_uidPlayer);
	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return CheckRef();
}

void BuildQPvPRewardInvenString(CString& strSQL, GDBT_QPER_TOINVEN& data)
{

}

class MockDBTaskQuestPvPRewardInven : public GDBTaskQuestPvPRewardInven
{
public :
	MockDBTaskQuestPvPRewardInven(const MUID& uidPlayer) : GDBTaskQuestPvPRewardInven(uidPlayer) {}

	GDBT_QPER_TOINVEN& GetData() { return m_Data; }
};

bool MockDBManager::QuestPVPERewardInven( GDBT_QPER_TOINVEN& data )
{
	InitRef(data.m_uidPlayer);

	MockDBTaskQuestPvPRewardInven t(data.m_uidPlayer);

	GDBT_QPER_TOINVEN& in_data = t.GetData();
	for (size_t i = 0; i < data.m_vecItem.size(); ++i)
	{
		in_data.m_vecRewardItemIUID.push_back(pair<int16, int64>(data.m_vecItem[i].m_nSlotID, SUnitTestUtil::NewIUID()));
	}

	if (!t.Input(data))
		return false;

	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::QuestPVPERewardMail( GDBT_QPER_TOMAIL& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskQuestPvPRewardMail t(data.m_uidPlayer);

	if (!t.Input(data))
		return false;

	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return CheckRef();
}

bool MockDBManager::StorageSerialize( const MUID& uidPlayer, const int64 nCID )
{
	InitRef(uidPlayer);

	GDBTaskStorageSerialize Task(uidPlayer);

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return CheckRef();
}

bool MockDBManager::StorageUpdateMoney( const GDBT_STORAGE_UPDATE_MONEY& data )
{
	InitRef(data.m_uidPlayer);

	GDBTaskStorageUpdateMoney Task(data.m_uidPlayer);
	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return CheckRef();
}

bool MockDBManager::TradeMarketPutOn(const GDBT_TRADEMARKET_PUT_ON& data)
{
	InitRef(data.playerUID);

	GDBTaskTradeMarketPutOn task(data.playerUID);
	if (!task.Input(data))
		return false;

	task.SetTaskSuccess();
	task.OnPrePost();
	task.OnCompleted();

	return CheckRef();
}
