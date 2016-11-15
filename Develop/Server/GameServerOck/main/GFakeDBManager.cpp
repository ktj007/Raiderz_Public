#include "stdafx.h"
#include "GFakeDBManager.h"
#include "GDBTaskHeaderList.h"
#include "GPlayerObjectManager.h"
#include "GItemManager.h"
#include "GItemData.h"
#include "GGlobal.h"
#include "GFakeDBIDGenerator.h"
#include "GDBTaskDataEmblem.h"

IUID GFakeDBManager::FAKE_IUID = 1;



GFakeDBManager::GFakeDBManager()
{

}


GFakeDBManager::~GFakeDBManager()
{

}

bool GFakeDBManager::QuestDone( GDBT_QEUST_DONE& data )
{
	GDBTaskQuestDone t(data.m_QuestComm.m_uidPlayer);
	
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::QuestAccept( GDBT_QUEST_ACCEPT& data )
{
	GDBTaskQuestAccept t(data.m_QuestComm.m_uidPlayer);

	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::QuestGiveup( GDBT_QUEST_GIVEUP& data )
{
	GDBTaskQuestGiveup t(data.m_uidPlayer);
	
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::QuestFail( const GDBT_QUEST_COMMON& data )
{
	GDBTaskQuestFail t(data.m_uidPlayer);
	
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::Training_Train(GDBT_TALENT& data)
{
	GDBTaskTalentLearn t(data.m_uidPlayer);
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();
	
	return true;
}

bool GFakeDBManager::Training_TrainSync( GDBT_TALENT& data )
{
	GDBTaskTalentLearn t(data.m_uidPlayer);
	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::CharAddMoney( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	GDBTaskCharIncMoney t(data.m_uidPlayer, SDBTID_CHAR_INC_MONEY);

	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::CharMinusMoney( const GDBT_CHAR_MONEY_INC_DEC& data )
{
	GDBTaskCharDecMoney t(data.m_uidPlayer, SDBTID_CHAR_DEC_MONEY);

	t.Input(data);

	t.SetTaskSuccess();

	t.OnPrePost();
	t.OnCompleted();

	return true;
}

class MockDBTaskItemBuy : public GDBTaskItemBuyInsert
{
public :
	MockDBTaskItemBuy(const MUID& uidReqPlayer) : GDBTaskItemBuyInsert(uidReqPlayer)
	{
		
	}

	void SetData(const IUID nNewIUID)
	{
		m_Result.nIUID = nNewIUID;
	}
};

bool GFakeDBManager::NPCShop_BuyInsert( const GDBT_NPC_SHOP_TRADE_DATA& data  )
{
	MockDBTaskItemBuy t(data.m_uidPlayer);

	t.Input(data);
	t.SetData(123456);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::NPCShop_BuyUpdate( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	GDBTaskItemBuyUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
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
//	return true;
//}

class MockDBTaskItemInsert : public GDBTaskItemInsert
{
public :
	MockDBTaskItemInsert(const MUID& uidReqPlayer) : GDBTaskItemInsert(uidReqPlayer) {		
	}

	virtual void OnExecute(mdb::MDatabase& rfDB) override
	{
		m_Result.m_nIUID = GFakeDBManager::NewIUID();
	}
};

bool GFakeDBManager::ItemInsert( GDBT_ITEM_DATA& data )
{
	MockDBTaskItemInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::NPCShop_SellPartUpdate( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	GDBTaskItemSellUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

bool GFakeDBManager::NPCShop_SellPartDelete( const GDBT_NPC_SHOP_TRADE_DATA& data )
{
	GDBTaskItemSellDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

bool GFakeDBManager::NPCShop_SellVeryCommon( GDBT_SELL_ITEM_VERY_COMMON& data )
{
	GDBTaskItemSellVeryCommon t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}


bool GFakeDBManager::Trade( GTRADE_CHAR_VEC& vTradeChar, const uint8 nItemInstnaceCnt )
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

bool GFakeDBManager::Craft_Insert( GDBT_CRAFT_DATA& data )
{
	MockDBTAskCraftInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.SetNewIUID(100001);
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::Craft_Update( GDBT_CRAFT_DATA& data )
{
	GDBTaskCraftUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return false;
}

bool GFakeDBManager::Training_UntrainAll( GDBT_TALENT& data )
{
	GDBTaskTalentResetAll t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
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

bool GFakeDBManager::ItemLootInsert(const GDBT_ITEM_DATA& data)
{
	MockDBTaskItemLootInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetIUID(1);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

bool GFakeDBManager::ItemLootUpdate(const GDBT_ITEM_DATA& data)
{
	GDBTaskItemLootUpdate t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

bool GFakeDBManager::ItemLearnTalent(GDBT_ITEM_LEARN_TALENT& data)
{
	GDBTaskItemLearnTalent t(data.m_uidPlayer);

	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

class MockDBTaskGuildInsert : public GDBTaskGuildInsert
{
public :
	MockDBTaskGuildInsert(const MUID& uidReqPlayer) : GDBTaskGuildInsert(uidReqPlayer)
	{
		m_Data.m_nGID = 1005;
	}
};

bool GFakeDBManager::GuildInsert( GDBT_GUILD& data )
{
	MockDBTaskGuildInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

class MockDBTaskGuildDelete : public GDBTaskGuildDelete
{
public :
	MockDBTaskGuildDelete(const MUID& uidReqPlayer) : GDBTaskGuildDelete(uidReqPlayer) {}
};

bool GFakeDBManager::GuildDelete( GDBT_GUILD& data )
{
	MockDBTaskGuildDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

class MockDBTaskGuildMemberInsert : public GDBTaskGuildMemberInsert
{
public :
	MockDBTaskGuildMemberInsert(const MUID& uidReqPlayer) : GDBTaskGuildMemberInsert(uidReqPlayer) {}
};

bool GFakeDBManager::GuildMemberInsert( GDBT_GUILD& data )
{
	MockDBTaskGuildMemberInsert t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

class MockDBTaskItemSwapSlot : public GDBTaskItemSwapSlot
{
public :
	MockDBTaskItemSwapSlot(const MUID& uidReqPlayer) : GDBTaskItemSwapSlot(uidReqPlayer)
	{

	}
};
bool GFakeDBManager::ItemMove(GDBT_ITEM_MOVE& data)
{
	MockDBTaskItemSwapSlot t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

class MockDBTaskGuildMemberDelete : public GDBTaskGuildMemberDelete
{
public :
	MockDBTaskGuildMemberDelete(const MUID& uidReqPlayer) : GDBTaskGuildMemberDelete(uidReqPlayer) {}

};

bool GFakeDBManager::GuildDeleteMember( GDBT_GUILD& data )
{
	MockDBTaskGuildMemberDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

class MockDBTaskGuildMasterChange : public GDBTaskGuildMasterChange
{
public :
	MockDBTaskGuildMasterChange(const MUID& uidCurMasger) : GDBTaskGuildMasterChange(uidCurMasger) {}
};

bool GFakeDBManager::GuildChangeMaster( GDBT_GUILD_CHANGE_MASTER& data )
{
	MockDBTaskGuildMasterChange t(data.m_uidPreMaster);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::ItemMerge( const GDBT_ITEM_MERGE_AND_SPLIT& data )
{
	GDBTaskItemMerge t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::EmblemInsert( const GDBT_EMBLEM& data )
{
	GDBTaskEmblemInsert t(data.m_uidPlayer);
	t.Input(data.m_nEmblemID);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::EmblemDelete( const GDBT_EMBLEM& data )
{
	GDBTaskEmblemDelete t(data.m_uidPlayer);
	t.Input(data.m_nEmblemID);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::ItemDelete( const GDBT_ITEM_DATA& data )
{
	GDBTaskItemDelete t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::ItemDescDurability( GDBT_ITEM_DEC_DURA_DATA& data )
{
	GDBTaskItemDecDura t(data.m_uidPlayer);
	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

class FakeLogin : public GDBTaskLogin
{
public :
	FakeLogin(const MUID& uidPlayer) : GDBTaskLogin(uidPlayer) {}
	void SetAID(const int64 nAID)
	{
		m_Result.m_nGSN = nAID;
	}
};

bool GFakeDBManager::LoginGetInfo( GDBT_ACC_LOGIN& data )
{
	static int64 AID = 1;

	FakeLogin t(data.m_uidPlayer);
	
	t.Input(data);
	t.SetAID(AID++);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

bool GFakeDBManager::CharGetLookList( const MUID& uidPlayer, const int64 nAID )
{
	GDBTaskCharGetLookList t(uidPlayer);

	SDBTASK_ACCOUNT_CAHR_FEATURE_LIST& l = t.GetData().GetData().CharList[0];
	t.GetData().GetData().nCharCount = 1;

	l.nCID = GFakeDBIDGenerator::NewCID();
	l.nFeatureFace = 1;
	l.nFeatureHair = 1;
	l.nFeatureHairColor = 1;
	l.nFeatureSkinColor = 1;
	l.nLevel = 50;
	l.nPlayTime = 0;
	l.nSharedFieldID = 3;
	l.Sex = SEX_MALE;
	l.strName = L"FakeDB";
	l.Race = RACE_HUMAN;
	
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

void GFakeDBManager::Test_PushDefaultItem(GEntityPlayer* pPlayer)
{
	GItemAdder adder;

	int nItemIDList[] = {
		10, 11, 12, 13, 14, 19,  // 무기들
		1010, 1011, 1012, 1013, 1014, // 방어구
	};

	for (int i=0; i<ArraySize(nItemIDList); ++i)
	{
		adder.Add(pPlayer, nItemIDList[i]);
	}
}

class FakeCharSerialize : public  GDBTaskCharSerialize
{
public :
	FakeCharSerialize(const MUID& uidPlayer) : GDBTaskCharSerialize(uidPlayer) {}

	GCHAR_SERIAL_DATA& GetData() { return m_Data; }

	void SetItem()
	{
		vector<int> vwi;
		vwi.push_back(10);
		vwi.push_back(11);
		vwi.push_back(12);
		vwi.push_back(13);
		vwi.push_back(14);
		vwi.push_back(19);

		vector<int> vdi;
		vdi.push_back(1010);
		vdi.push_back(1011);
		vdi.push_back(1012);
		vdi.push_back(1013);
		vdi.push_back(1014);

		for (size_t i = 0; i < vwi.size(); ++i)
		{
			GDBT_ITEMINSTANCE ii;
			ClearItemInstance(ii);
			
			ii.nDura = gmgr.pItemManager->GetItemData(vwi[i])->m_nMaxDurability;
			ii.nMaxDura = gmgr.pItemManager->GetItemData(vwi[i])->m_nMaxDurability;
			ii.nSlotType = 1;
			ii.nSlotID = i;
			ii.nItemID = vwi[i];
			ii.nStackAmt = 1;
			ii.nIUID = GFakeDBManager::NewIUID();

			m_Data.qItemInst.push_back(ii);
		}

		for (size_t i = 0; i < vdi.size(); ++i)
		{
			GDBT_ITEMINSTANCE ii;
			ClearItemInstance(ii);

			ii.nDura = gmgr.pItemManager->GetItemData(vdi[i])->m_nMaxDurability;
			ii.nMaxDura = gmgr.pItemManager->GetItemData(vdi[i])->m_nMaxDurability;
			ii.nSlotType = 1;
			ii.nSlotID = vwi.size() + i;
			ii.nItemID = vdi[i];
			ii.nStackAmt = 1;
			ii.nIUID = GFakeDBManager::NewIUID();

			m_Data.qItemInst.push_back(ii);
		}	
	}

	void ClearItemInstance(GDBT_ITEMINSTANCE& ii)
	{
		ii.nItemID = 0;
		ii.nStackAmt = 0;
		ii.nDura = 0;
		ii.nMaxDura = 0;
		ii.nDyedColor = 0;
		ii.nIUID = 0;
		ii.bClaimed = false;
		ii.nSoulCnt = 0;
		ii.nSlotType = 0;
		ii.nSlotID = 0;
		ii.nCharPtm = 0;
		ii.bUseUsagePeriod = false;
		ii.bStartCalculatingUsagePeriod = false;
		ii.nUsagePeriodSec = 0;
		ii.bUseExpiTm = false;
		ii.nExpiTm = 0;
		ii.strExpiDt.clear();
	}

	void SetCharacter()
	{
		GDBT_CHARINFO& c = m_Data.CharInfo;

		c.nCID = m_Data.nCID;
		c.nSex = SEX_MALE;
		c.strName = L"test";
		c.nHP = 100;		
		c.nEN = 100;
		c.nSTA = 100;
		c.nSharedFieldID = 3;
		c.fPosX = 62120.562500f;
		c.fPosY = 43085.992188f;
		c.fPosZ = 258.071350f;
		c.nFeatureFace = 1;
		c.nFeatureHair = 1;
		c.nFeatureHairColor = 1;
		c.nFeatureSkinColor = 1;
		c.nPlayTimeSec = 0;
		c.nPlayerGrade = 3;
		c.nLevel = 50;
		c.nXP = 0;
		c.nMoney = 100000000;
		c.nRemaindTP = 50;
		c.nWeaponSet = ITEMSWITCH_PRIMARY;

		
	}
};

bool GFakeDBManager::CharSerialize( const GDBT_CHAR_SERIALIZE& data )
{
	FakeCharSerialize t(data.m_uidPlayer);

	t.Input(data.m_nCID, data.m_nUIID, data.m_bReload);

	t.SetCharacter();
	t.SetItem();
	
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();
	return true;
}

IUID GFakeDBManager::NewIUID()
{
	return FAKE_IUID++;
}

bool GFakeDBManager::CharLevelUp( GDBT_CHAR_LEVEL_UP_DATA& data )
{
	GDBTaskCharLevelUp Task(data.m_uidPlayer);

	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return true;
}

bool GFakeDBManager::CharLevelUpSync( GDBT_CHAR_LEVEL_UP_DATA& data )
{
	GDBTaskCharLevelUp Task(data.m_uidPlayer);

	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return true;
}

bool GFakeDBManager::ItemEquip( GDBT_ITEM_EQUIP& data )
{
	GDBTaskItemEquip t(data.uidPlayer);

	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();	

	return true;
}

bool GFakeDBManager::ItemUnequip( GDBT_ITEM_UNEQUIP& data )
{
	GDBTaskItemUnequip t(data.uidPlayer);

	t.Input(data);
	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();	

	return true;
}

bool GFakeDBManager::ItemIncStackAmt( GDBT_ITEM_DATA& data )
{
	GDBTaskItemIncStackAmt Task(data.m_uidPlayer);
	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return true;
}

bool GFakeDBManager::ItemDecStackAmt( GDBT_ITEM_DEC_STACK_AMT_DATA& data )
{
	GDBTaskItemDecStackAmt Task(data.m_uidPlayer);
	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return true;
}

bool GFakeDBManager::ItemEnchant( const GDBT_ITEM_ENCH& data )
{
	GDBTaskItemEnchant t(data.m_uidPlayer);
	if (!t.Input(data))
		return false;

	if (!t.Input(data))
		return false;

	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::ItemDye( const GDBT_ITEM_DYE& data )
{
	GDBTaskItemDye Task(data.m_uidPlayer);
	if (!Task.Input(data))
		return false;

	Task.SetTaskSuccess();
	Task.OnPrePost();
	Task.OnCompleted();

	return true;
}

bool GFakeDBManager::QuestPVPERewardInven( GDBT_QPER_TOINVEN& data )
{
	GDBTaskQuestPvPRewardInven t(data.m_uidPlayer);

	if (!t.Input(data))
		return false;

	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}

bool GFakeDBManager::QuestPVPERewardMail( GDBT_QPER_TOMAIL& data )
{
	GDBTaskQuestPvPRewardMail t(data.m_uidPlayer);

	if (!t.Input(data))
		return false;

	t.SetTaskSuccess();
	t.OnPrePost();
	t.OnCompleted();

	return true;
}
