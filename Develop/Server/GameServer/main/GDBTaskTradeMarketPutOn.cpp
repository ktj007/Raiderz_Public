#include "stdafx.h"
#include "GDBTaskTradeMarketPutOn.h"
#include "GGlobal.h"
#include "GPlayerObject.h"
#include "GEntityPlayer.h"
// #include "GItemSystem.h"
#include "GItemRemover.h"
#include "GPlayerObjectManager.h"
#include "GItemHolder.h"
// #include "GDBManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "CTradeMarketConstant.h"


GDBTaskTradeMarketPutOn::GDBTaskTradeMarketPutOn( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_TRADEMARKET_PUTON), m_Syncer(uidReqPlayer)
{
}

GDBTaskTradeMarketPutOn::~GDBTaskTradeMarketPutOn()
{

}

void GDBTaskTradeMarketPutOn::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_Syncer.EndCacheSync(IsSuccess());
}

bool GDBTaskTradeMarketPutOn::Input( const GDBT_TRADEMARKET_PUT_ON& data )
{
	if (false == m_Syncer.BeginCacheSync(SLOTTYPE_INVENTORY, data.slotFromInventory, data.itemIUID))
	{
		m_Syncer.EndCacheSync(false);
		return false;
	}

	m_Data = data;

	return true;
}

void GDBTaskTradeMarketPutOn::OnExecute( mdb::MDatabase& rfDB )
{
// 	mdb::MDatabaseQuery q(&rfDB);
// 	ExecuteW(q, GetSQLW(ITEM_DELETE));
}

mdb::MDB_THRTASK_RESULT GDBTaskTradeMarketPutOn::_OnCompleted()
{
// 	VALID(NULL!=gsys.pDBManager);
// 	gsys.pDBManager->ItemDeleteLog(m_Data);

	VALID_RET(NULL!=gmgr.pPlayerObjectManager, mdb::MDBTR_SUCESS);
	GPlayerObject* const pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(m_Data.playerUID);
	if (NULL == pPlayerObj)
		return mdb::MDBTR_SUCESS;

	GEntityPlayer* const pPlayer = pPlayerObj->GetEntity();
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	VALID_RET(NULL!=pPlayer->GetItemHolder(), mdb::MDBTR_SUCESS);
	GItem* const pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, m_Data.slotFromInventory);
	if (NULL == pItem) 
		return mdb::MDBTR_SUCESS;

	if (m_Data.itemIUID != pItem->m_nIUID)
		return mdb::MDBTR_SUCESS;

	//todo m_Data.expectItemCount - 디비에서 리턴받은 아이템결과값을 세팅해서 보내줘야 한다(ui에 내가 올린 아이템 리스트를 뿌려줘야 하기 때문) 
	REMOVE_ITEM remove(SLOTTYPE_INVENTORY, m_Data.slotFromInventory, m_Data.expectItemCount, m_Data.quantity);
	VALID_RET(NULL!=gsys.pItemSystem, mdb::MDBTR_SUCESS);
	gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), remove, GItemRemover::DONT_ROUTE);

	//todo 디비에서 리턴받은 시리얼번호를 세팅해서 보내줘야 한다(ui에 내가 올린 아이템 리스트를 뿌려줘야 하기 때문) 
	const int64 tempDBResult = 0;
	MCommand* const cmd = MakeNewCommand(MC_SC_TRADEMARKET_PUTTED_ON, 3
		, NEW_INT(m_Data.slotFromInventory)
		, NEW_INT(m_Data.quantity)
		, NEW_INT64(tempDBResult)
	);
	VALID_RET(NULL!=cmd, mdb::MDBTR_SUCESS);
	pPlayer->RouteToMe(cmd);

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskTradeMarketPutOn::_OnFailed()
{
	VALID_RET(NULL!=gmgr.pPlayerObjectManager, mdb::MDBTR_SUCESS);
	GPlayerObject* const pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(m_Data.playerUID);
	if (NULL == pPlayerObj)
		return mdb::MDBTR_SUCESS;

	GEntityPlayer* const pPlayer = pPlayerObj->GetEntity();
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	MCommand* const cmd = MakeNewCommand(MC_SC_TRADEMARKET_EXCEPTION, 1
		, NEW_UCHAR(TRADEMARKET_EXCEPTION_PUTON_DB_ERROR)
		);
	VALID_RET(NULL!=cmd, mdb::MDBTR_SUCESS);
	pPlayer->RouteToMe(cmd);

	return mdb::MDBTR_SUCESS;
}
