#include "stdafx.h"
#include "GDBTaskTrade.h"
#include "GDBTaskGlobal.h"
#include "GTradeSystem.h"
#include "GTradeExchangerForDBTask.h"
#include "GItemHolder.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"


GDBTaskTrade::GDBTaskTrade() 
: GDBAsyncTask(0, SDBT_DBTYPE_GAMEDB, SDBTID_TRADE)
{

}

GDBTaskTrade::~GDBTaskTrade()
{

}

bool GDBTaskTrade::Input(GTRADE_CHAR_VEC& vTradeChar)
{
	if (!BeginCacheSync(vTradeChar))
		return false;

	m_Data.vTradeChar.swap(vTradeChar);

	return true;
}

void GDBTaskTrade::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(TRADE)))
		return;

	GDBT_TRADE_ITEM ti;

	int i = 0;
	int j = 0;

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		if (rs.FieldW(L"CHAR_ID").AsInt() == rs.FieldW(L"TO_CHAR_ID").AsInt())
		{
			// 아이템 로그 저장을 위한 자료구조.
			if (m_Data.vTradeChar[TRADE_CHAR_FIRSET].m_nCID == rs.FieldW(L"TO_CHAR_ID").AsInt())
			{
				m_Data.ItemLog[TRADE_CHAR_FIRSET][i].Set(rs.FieldW(L"ITEM_ID").AsInt()
					, rs.FieldW(L"IUID").AsInt64()
					, rs.FieldW(L"TO_IUID").AsInt64()
					, rs.FieldW(L"STACK_AMT").AsShort()
					, rs.FieldW(L"DELTA_STACK_AMT").AsShort()
					, rs.FieldW(L"DURA").AsByte()
					, rs.FieldW(L"REMAIN_USAGE_PERIOD").AsInt());
				++i;
			}
			else
			{
				m_Data.ItemLog[TRADE_CHAR_SECOND][j].Set(rs.FieldW(L"ITEM_ID").AsInt()
					, rs.FieldW(L"IUID").AsInt64()
					, rs.FieldW(L"TO_IUID").AsInt64()
					, rs.FieldW(L"STACK_AMT").AsShort()
					, rs.FieldW(L"DELTA_STACK_AMT").AsShort()
					, rs.FieldW(L"DURA").AsByte()
					, rs.FieldW(L"REMAIN_USAGE_PERIOD").AsInt());			
				++j;
			}

			continue;
		}

		if (rs.FieldW(L"STACK_AMT").AsShort() <= 0)
			continue;

		// 아이템이 분리되서 새로 insert된것.
		if (1 == rs.FieldW(L"TR_TYPE").AsByte())
		{		
			ti.nCID					= rs.FieldW(L"CHAR_ID").AsInt64();
			ti.nToCID				= rs.FieldW(L"TO_CHAR_ID").AsInt64();
			ti.nStackAmt			= rs.FieldW(L"STACK_AMT").AsShort();
			ti.nSlotID				= rs.FieldW(L"SLOT_ID").AsByte();
			ti.nToSlotID			= rs.FieldW(L"TO_SLOT_ID").AsShort();
			ti.nIUID				= rs.FieldW(L"IUID").AsInt64();
			ti.nDura				= rs.FieldW(L"DURA").AsByte();
			ti.nRemainUsagePeriod	= rs.FieldW(L"REMAIN_USAGE_PERIOD").AsInt();

			m_Data.vecDBInsertItemBySplit.push_back(ti);
		}		
	}
}

mdb::MDB_THRTASK_RESULT GDBTaskTrade::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();
	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskTrade::_OnFailed()
{
	GEntityPlayer* pPlayer1 = gmgr.pPlayerObjectManager->GetEntity(m_Data.vTradeChar[TRADE_CHAR_FIRSET].m_uidPlayer);
	if (NULL != pPlayer1)
	{
		GItemHolder* pItemHolder = pPlayer1->GetItemHolder();
		for(int i = 0; i < INVALID_ID; i++)
		{
			pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, m_Data.vTradeChar[TRADE_CHAR_SECOND].m_vItem[i].m_nSlotIDTo);
		}
	}

	GEntityPlayer* pPlayer2 = gmgr.pPlayerObjectManager->GetEntity(m_Data.vTradeChar[TRADE_CHAR_SECOND].m_uidPlayer);
	if (NULL != pPlayer2)
	{
		GItemHolder* pItemHolder = pPlayer2->GetItemHolder();
		for(int i = 0; i < INVALID_ID; i++)
		{
			pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, m_Data.vTradeChar[TRADE_CHAR_FIRSET].m_vItem[i].m_nSlotIDTo);
		}
	}

	return mdb::MDBTR_SUCESS;
}

GDBT_TRADE_DATA& GDBTaskTrade::GetData()
{
	return m_Data;
}

bool GDBTaskTrade::SyncCharacter( const MUID& uidPlayer, CHAR_ORDER nOrdNum )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(uidPlayer);
	if (NULL == cc)
		return true;

	if (!cc->BeginCacheSync())
		return false;

	m_SyncingList[nOrdNum].SetPlayer(uidPlayer);

	if (NULL == m_SyncingList[nOrdNum].PushChar())
	{
		cc->EndCacheSync(false);
		return false;
	}

	return true;
}

bool GDBTaskTrade::SyncItem( const MUID& uidPlayer, GTRADE_ITEM_VEC& vItem, const CHAR_ORDER nOrdNum )
{
	GItemDBCache* pi= NULL;
	for each(const GTRADE_ITEM& ti in vItem)
	{
		if (0 >= ti.m_nIUID)
			continue;

		pi = gsys.pDBCacheSystem->FindItemCache(uidPlayer, (uint8)SLOTTYPE_INVENTORY, ti.m_nSlotID, ti.m_nIUID);
		if (NULL == pi)
			return false;

		if (!pi->BeginCacheSync())
			return false;

		if (NULL == m_SyncingList[nOrdNum].PushItem(ti.m_nIUID))
		{
			pi->EndCacheSync(false);
			return false;
		}
	}

	return true;
}

void GDBTaskTrade::_OnPreCompleted()
{
	EndSync(m_Data.vTradeChar, TRADE_CHAR_FIRSET, IsSuccess());
	EndSync(m_Data.vTradeChar, TRADE_CHAR_SECOND, IsSuccess());
}

void GDBTaskTrade::EndSync( GTRADE_CHAR_VEC& vTradeChar, const CHAR_ORDER nOrdNum, const bool bZero )
{
	DecreaseRefCount(vTradeChar[nOrdNum].m_uidPlayer);

	m_SyncingList[nOrdNum].EndCacheSync(bZero);	
}

bool GDBTaskTrade::BeginCacheSync(GTRADE_CHAR_VEC& vTradeChar)
{
	IncreaseRefCount(vTradeChar[TRADE_CHAR_FIRSET].m_uidPlayer);
	IncreaseRefCount(vTradeChar[TRADE_CHAR_SECOND].m_uidPlayer);

	if (!SyncCharacter(vTradeChar[TRADE_CHAR_FIRSET].m_uidPlayer, TRADE_CHAR_FIRSET) ||
		!SyncItem(vTradeChar[TRADE_CHAR_FIRSET].m_uidPlayer, vTradeChar[TRADE_CHAR_FIRSET].m_vItem, TRADE_CHAR_FIRSET))
	{
		EndSync(vTradeChar, TRADE_CHAR_FIRSET, false);
		EndSync(vTradeChar, TRADE_CHAR_SECOND, false);
		return false;
	}

	if (!SyncCharacter(vTradeChar[TRADE_CHAR_SECOND].m_uidPlayer, TRADE_CHAR_SECOND) || 
		!SyncItem(vTradeChar[TRADE_CHAR_SECOND].m_uidPlayer, vTradeChar[TRADE_CHAR_SECOND].m_vItem, TRADE_CHAR_SECOND))
	{
		EndSync(vTradeChar, TRADE_CHAR_FIRSET, false);
		EndSync(vTradeChar, TRADE_CHAR_SECOND, false);
		return false;
	}

	return true;
}

void GDBTaskTrade::Completer::Do()
{
	gsys.pDBManager->TradeLog(m_Data);

	gsys.pTradeSystem->GetExchangerForDBTask().Exchange(m_Data);	
}
