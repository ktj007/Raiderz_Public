#include "stdafx.h"
#include "GDBTaskItemLearnTalent.h"
#include "GDBTaskGlobal.h"
#include "GTrainingSystem.h"
#include "GItemRemover.h"
#include "GItemHolder.h"
#include "GDBManager.h"
#include "GDBTaskItemData.h"


GDBTaskItemLearnTalent::GDBTaskItemLearnTalent( const MUID& uidReqPlayer ) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMLEARNTALENT)
{
	
}

GDBTaskItemLearnTalent::~GDBTaskItemLearnTalent()
{

}

void GDBTaskItemLearnTalent::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_LEARN_TALENT));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemLearnTalent::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskItemLearnTalent::Input( GDBT_ITEM_LEARN_TALENT& data )
{
	if (!BeginItemCacheSync(data.m_nSlotType, data.m_nSlotID, data.m_nIUID))
		return false;

	m_Data.Set(data);

	return true;
}

bool GDBTaskItemLearnTalent::BeginItemCacheSync( const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* cc = gsys.pDBCacheSystem->FindItemCache(GetReqPlayer(), nSlotType, nSlotID, nIUID);
	if (NULL == cc)
		return false;

	if (!cc->BeginCacheSync())
		return false;

	return true;
}

void GDBTaskItemLearnTalent::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	GItemDBCache* cc = gsys.pDBCacheSystem->FindItemCache(m_Data.m_uidPlayer, m_Data.m_nSlotType, m_Data.m_nSlotID, m_Data.m_nIUID);
	if (NULL == cc)
		return;

	cc->EndCacheSync(IsSuccess());
}

void GDBTaskItemLearnTalent::Completer::Do()
{
	gsys.pDBManager->ItemDecStackAmtLog(GDBT_ITEM_DEC_STACK_AMT_DATA(m_Data.m_uidPlayer, m_Data.m_nAID, m_Data.m_nCID
		, m_Data.m_nCharPtm, m_Data.m_nDeltaCharPtm, m_Data.m_nLevel, m_Data.m_nMoney, m_Data.m_nSlotType, m_Data.m_nSlotID
		, m_Data.m_nIUID, m_Data.m_nItemID, 0, 1, 0, 0));
	gsys.pTrainingSystem->TrainByItemForDBTask(m_Data);
}
