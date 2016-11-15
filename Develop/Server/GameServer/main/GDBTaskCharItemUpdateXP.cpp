#include "stdafx.h"
#include "GDBTaskCharItemUpdateXP.h"
#include "GDBTaskGlobal.h"
#include "GExpSystem.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"


GDBTaskCharItemUpdateXP::GDBTaskCharItemUpdateXP( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_CHAR_ITEM_UPDATE_XP), m_SyncingList(uidReqPlayer)
{

}

bool GDBTaskCharItemUpdateXP::Input( GDBT_ITEM_XP_DATA& data )
{
	/*if (!UpdateCache(data))
		return false;*/

	m_Data = data;

	return true;
}

void GDBTaskCharItemUpdateXP::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(UPDATE_XP));
}

mdb::MDB_THRTASK_RESULT GDBTaskCharItemUpdateXP::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

/*bool GDBTaskCharItemUpdateXP::UpdateCache(const GDBT_CHAR_XP_DATA& data)
{
	GCharacterDBCache* pch = gsys.pDBCacheSystem->FindCharCache(data.m_uidPlayer);
	if (NULL == pch)
		return true;

	if (!pch->IncXP(data.m_nModXP))
		return false;
		
	if (!pch->IsOverflow())
	{
		SetSync(false);
		pch->EndCacheSync(false);
	}
	else
	{
		if (NULL == m_SyncingList.PushChar())
		{
			pch->EndCacheSync(false);
			return false;
		}
	}

	return true;
}*/

void GDBTaskCharItemUpdateXP::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	//m_SyncingList.EndCacheSync(IsSuccess());
}

void GDBTaskCharItemUpdateXP::Completer::Do()
{
	//gsys.pDBManager->CharAddXPLog(m_Data);	

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	gsys.pExpSystem->AddItemExpForDB(pPlayer, m_Data.m_nModXP, m_Data.m_nNextLvlXP);
}