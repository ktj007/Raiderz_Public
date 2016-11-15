#include "stdafx.h"
#include "GDBTaskItem.h"
#include "GItem.h"
#include "CSChrInfo.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GItemManager.h"
#include "GItemHolder.h"
#include "GDB_CODE.h"

void GDBT_ITEM::Build(const int64 nCID, GItem* pItem )
{
	m_nCID			= nCID;
	m_nSlotType		= pItem->m_nSlotType;
	m_nSlotID		= pItem->m_nSlotID;
	m_nIUID			= pItem->m_nIUID;
	m_nItemID		= pItem->m_pItemData->m_nID;
	m_nStackAmt		= pItem->GetAmount();
	m_nSoulCnt		= pItem->m_nSoulCount;
	m_nDura			= pItem->m_nDurability;
	m_nMaxDura		= pItem->m_nDurability;
	m_nEnchCnt		= pItem->GetEnchantCount();
	m_nColor		= pItem->m_nDyedColor;
	m_bClaimed		= pItem->m_bBind;
	m_nCharPtm		= pItem->m_UsagePeriod.GetUsageStartCharPlayTimeSec();
	m_bPeriodItem	= pItem->m_UsagePeriod.IsPeriod() || pItem->m_ExpirationPeriod.IsPeriod();
	m_nUsagePeriod	= pItem->m_UsagePeriod.GetPeriodSec();

	pItem->GetExpiDtForDB(m_strExpiDt);
}

void GDBT_ITEM::Set( const int64 nCID, const uint8 nSlotType, const int16 nSlotID, const int nItemID, const int16 nStackAmt, const int nCharPtm 
					, const IUID nIUID, const uint8 nSoulCnt /*= 0*/, const uint8 nDura /*= 0*/, const uint8 nMaxDura /*= 0*/
					, const uint8 nEnchCnt /*= 0 */, const int nColor /*= 0*/, const bool bClaimed /*= false*/, const bool bPeriodItem /*= false*/
					, const int nUsagePeriod /*= 0 */, const wstring& strExpiDt /*= L"NULL"*/ )
{
	m_nCID			= nCID;
	m_nSlotType		= nSlotType;
	m_nSlotID		= nSlotID;
	m_nIUID			= nIUID;
	m_nItemID		= nItemID;
	m_nStackAmt		= nStackAmt;
	m_nSoulCnt		= nSoulCnt;
	m_nDura			= nDura;
	m_nMaxDura		= nMaxDura;
	m_nEnchCnt		= nEnchCnt;
	m_nColor		= nColor;
	m_bClaimed		= bClaimed;
	m_nCharPtm		= nCharPtm;
	m_bPeriodItem	= bPeriodItem;
	m_nUsagePeriod	= nUsagePeriod;
	m_strExpiDt		= strExpiDt;
}

bool GDBT_ITEMINSTANCEQ::GetFromDB(mdb::MDBRecordSet& rs )
{
	if (0 == rs.GetFetchedCount())
	{
		return true;
	}

	static const size_t nIUIDHash			= rs.MakeHashValueW(L"IUID");
	static const size_t nItemIDHash			= rs.MakeHashValueW(L"ITEM_ID");
	static const size_t nSlotTypeHash		= rs.MakeHashValueW(L"SLOT_TYPE");
	static const size_t nSlotIDHash			= rs.MakeHashValueW(L"SLOT_ID");
	static const size_t nStackAmtHash		= rs.MakeHashValueW(L"STACK_AMT");
	static const size_t nDuraHash			= rs.MakeHashValueW(L"DURA");
	static const size_t nClaimedHash		= rs.MakeHashValueW(L"CLAIMED");
	static const size_t nSoulCntHash		= rs.MakeHashValueW(L"SOUL_COUNT");
	static const size_t nColorHash			= rs.MakeHashValueW(L"COLOR");
	static const size_t nCharPtm			= rs.MakeHashValueW(L"CHAR_PTM");
	static const size_t bPeriodItem			= rs.MakeHashValueW(L"PERIOD");
	static const size_t nUsagePeriod		= rs.MakeHashValueW(L"USAGE_PERIOD");
	static const size_t nExpiDtHash			= rs.MakeHashValueW(L"EFF_END_DATE");
	static const size_t nExpirationTime		= rs.MakeHashValueW(L"EXPI_TIME");
	static const size_t nEnchItemID_1		= rs.MakeHashValueW(L"ENCH_ITEM_ID_1");
	static const size_t nEnchItemID_2		= rs.MakeHashValueW(L"ENCH_ITEM_ID_2");
	static const size_t nEnchItemID_3		= rs.MakeHashValueW(L"ENCH_ITEM_ID_3");
	static const size_t nEnchItemID_4		= rs.MakeHashValueW(L"ENCH_ITEM_ID_4");
	static const size_t nEnchItemID_5		= rs.MakeHashValueW(L"ENCH_ITEM_ID_5");
	static const size_t nEnchItemID_6		= rs.MakeHashValueW(L"ENCH_ITEM_ID_6");


	for (; !rs.IsEOF(); rs.MoveNext())
	{
		GDBT_ITEMINSTANCE ii;

		ii.nSlotType	= rs.FieldHash(nSlotTypeHash).AsByte();
		ii.nSlotID		= rs.FieldHash(nSlotIDHash).AsShort();

		if (SLOTTYPE_INVENTORY == ii.nSlotType && BASE_INVENTORY_SIZE <= ii.nSlotID)
		{
			continue;
		}

		ii.nIUID				= rs.FieldHash(nIUIDHash).AsInt64();
		ii.nItemID				= rs.FieldHash(nItemIDHash).AsInt();		
		ii.nStackAmt			= rs.FieldHash(nStackAmtHash).AsByte();
		ii.nDura				= rs.FieldHash(nDuraHash).AsByte();
		ii.bClaimed				= rs.FieldHash(nClaimedHash).AsBool();
		ii.nSoulCnt				= rs.FieldHash(nSoulCntHash).AsByte();
		ii.nCharPtm				= rs.FieldHash(nCharPtm).AsInt();
		ii.bUseUsagePeriod		= rs.FieldHash(bPeriodItem).AsBool();
		ii.nUsagePeriodSec		= rs.FieldHash(nUsagePeriod).AsInt();
		ii.nEnchItemID_1		= rs.FieldHash(nEnchItemID_1).AsInt();
		ii.nEnchItemID_2		= rs.FieldHash(nEnchItemID_2).AsInt();
		ii.nEnchItemID_3		= rs.FieldHash(nEnchItemID_3).AsInt();
		ii.nEnchItemID_4		= rs.FieldHash(nEnchItemID_4).AsInt();
		ii.nEnchItemID_5		= rs.FieldHash(nEnchItemID_5).AsInt();
		ii.nEnchItemID_6		= rs.FieldHash(nEnchItemID_6).AsInt();

		if (rs.FieldHash(nExpiDtHash).IsNull())
		{
			ii.strExpiDt = L"NULL";
		}
		else
		{
			ii.strExpiDt = rs.FieldHash(nExpiDtHash).AsWString();
		}

		if (rs.FieldHash(nCharPtm).IsNull())
			ii.bStartCalculatingUsagePeriod = false;
		else 
			ii.bStartCalculatingUsagePeriod = true;

		if (rs.FieldHash(nExpirationTime).IsNull())
		{
			ii.bUseExpiTm = false;
			ii.nExpiTm = 0;
		}
		else
		{
			ii.bUseExpiTm = true;
			ii.nExpiTm	= rs.FieldHash(nExpirationTime).AsInt();
		}


		if (false == rs.FieldHash(nColorHash).IsNull())
		{
			ii.nDyedColor = rs.FieldHash(nColorHash).AsInt();
		}

		push_back(ii);
	}

	rs.Close();

	return true;
}

bool GDBT_ITEMINSTANCEQ::SetToServer(GEntityPlayer* pPlayer)
{
	PLAYER_INFO*	pPlayerInfo = pPlayer->GetPlayerInfo();
	GItemHolder*	pItemHolder = pPlayer->GetItemHolder();

	bool	bPeriod = false;

	for (deque<GDBT_ITEMINSTANCE>::iterator it = begin(); it != end(); ++it)
	{
		int nItemID = (*it).nItemID;
		SH_ITEM_SLOT_TYPE nSlotType = (SH_ITEM_SLOT_TYPE)(*it).nSlotType;
		int nSlotID = (*it).nSlotID;

		if (false == gmgr.pItemManager->IsExist(nItemID))
		{
			pPlayer->GetItemHolder()->ReserveDummySlot(static_cast<SH_ITEM_SLOT_TYPE>((*it).nSlotType), (*it).nSlotID);

			if (false == gmgr.pItemManager->IsNotExistItemIDInXML(nItemID))
			{
				gmgr.pItemManager->IsNotExistItemIDInXML(nItemID);
				mlog3("Not exist ItemID(%d) in xitem.xml\n", nItemID);
			}			

			continue;
		}

		GItem*	pItem = gmgr.pItemManager->MakeNewItem(nItemID, (*it).nStackAmt, (*it).bClaimed);
		if (NULL == pItem) 
		{				
			continue;
		}

		pItem->m_nIUID			= (*it).nIUID;
		pItem->m_nDurability	= (*it).nDura;
		pItem->m_nSoulCount		= (*it).nSoulCnt;
		pItem->m_nDyedColor		= (*it).nDyedColor;
		pItem->m_nSlotID		= (*it).nSlotID;
		pItem->m_strExpiDt		= (*it).strExpiDt;
		pItem->m_nEnchants[0]	= (*it).nEnchItemID_1;
		pItem->m_nEnchants[1]	= (*it).nEnchItemID_2;
		pItem->m_nEnchants[2]	= (*it).nEnchItemID_3;
		pItem->m_nEnchants[3]	= (*it).nEnchItemID_4;
		pItem->m_nEnchants[4]	= (*it).nEnchItemID_5;
		pItem->m_nEnchants[5]	= (*it).nEnchItemID_6;

		pItem->m_UsagePeriod.Set((*it).bUseUsagePeriod
			, (*it).bStartCalculatingUsagePeriod
			, (*it).nUsagePeriodSec
			, (*it).nCharPtm);

		pItem->m_ExpirationPeriod.Set((*it).bUseExpiTm
			, true/*특정 날짜가 만료 아이템은 무조건 사용으로 설정된다.*/
			, (*it).nExpiTm
			, pPlayerInfo->nPlayTimeSec);

		pItemHolder->PushItem(nSlotType, nSlotID, pItem);
	}

	return true;
}

void GDBT_ITEM_MERGE_AND_SPLIT::MakeDBCode()
{
	if (MERGE == m_nType)
	{
		if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_MERGE_INVEN_TO_INVEN;
		}
		else if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_STORAGE == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_MERGE_INVEN_TO_CHAR_STORAGE;
		}
		else if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_GUILDSTORAGE == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_MERGE_INVEN_TO_GUILD_STORAGE;
		}
		else if (SLOTTYPE_STORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_MERGE_CHAR_STORAGE_TO_INVEN;
		}
		else if (SLOTTYPE_GUILDSTORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_MERGE_GUILD_STORAGE_TO_INVEN;
		}
	}
	else if (SPLIT == m_nType)
	{
		if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_SPLIT_INVEN_TO_INVEN;
		}
		else if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_STORAGE == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE;
		}
		else if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_GUILDSTORAGE == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_SPLIT_INVEN_TO_GUILD_STORAGE;		
		}
		else if (SLOTTYPE_STORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_STORAGE == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_SPLIT_CHAR_STORAGE_TO_CHAR_STORAGE;
		}
		else if (SLOTTYPE_STORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_SPLIT_CHAR_STORAGE_TO_INVEN;
		}
		else if (SLOTTYPE_GUILDSTORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_GUILDSTORAGE == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_SPLIT_GUILD_STORAGE_TO_GUILD_STORAGE;
		}
		else if (SLOTTYPE_GUILDSTORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
		{
			m_nCode = GDB_CODE::CD_L_ITEM_SPLIT_GUILD_STORAGE_TO_INVEN;
		}
	}
}

void GDBT_ITEM_MOVE::MakeDBCode()
{
	if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
	{
		m_nCode = GDB_CODE::CD_L_ITEM_MOVE_INVEN_TO_INVEN;
	}
	else if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_STORAGE == m_ToSlot.m_nSlotType)
	{
		m_nCode = GDB_CODE::CD_L_ITEM_MOVE_INVEN_TO_CHAR_STORAGE;
	}
	else if (SLOTTYPE_INVENTORY == m_FromSlot.m_nSlotType && SLOTTYPE_GUILDSTORAGE == m_ToSlot.m_nSlotType)
	{
		m_nCode = GDB_CODE::CD_L_ITEM_MOVE_INVEN_TO_GUILD_STORAGE;
	}
	else if (SLOTTYPE_STORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
	{
		m_nCode = GDB_CODE::CD_L_ITEM_MOVE_CHAR_STORAGE_TO_INVEN;
	}
	else if (SLOTTYPE_GUILDSTORAGE == m_FromSlot.m_nSlotType && SLOTTYPE_INVENTORY == m_ToSlot.m_nSlotType)
	{
		m_nCode = GDB_CODE::CD_L_ITEM_MOVE_GUILD_STORAGE_TO_INVEN;
	}
}