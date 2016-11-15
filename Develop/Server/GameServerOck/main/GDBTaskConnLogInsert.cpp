#include "stdafx.h"
#include "GDBTaskConnLogInsert.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GDBManager.h"


//GDBTaskConnLogInsert::GDBTaskConnLogInsert(const MUID& uidReqPlayer) : GDBAsyncTask(uidReqPlayer)
//{
//	m_Data.uidPlayer	= uidReqPlayer;
//}
//
//
//GDBTaskConnLogInsert::~GDBTaskConnLogInsert()
//{
//
//}
//
//void GDBTaskConnLogInsert::Input( const int nAID )
//{
//	m_Data.nAID = nAID;
//}
//
//void GDBTaskConnLogInsert::OnExecute(mdb::MDatabase& rfDB)
//{
//	mdb::MDatabaseQuery dbq(&rfDB);
//	Execute(dbq, GetSQLW(CONNLOG_INSERT));
//}
//
//
//mdb::MDB_THRTASK_RESULT GDBTaskConnLogInsert::_OnCompleted()
//{
//	Completer completer(m_Data);
//	completer.Do();
//
//	return mdb::MDBTR_SUCESS;
//}
//
//SDBTASK_ID GDBTaskConnLogInsert::GetID()
//{
//	return SDBTID_CONNLOGINSERT;
//}
//
//GDBTaskConnLogInsert::Completer::Completer( GDBTaskConnLogInsert::_DATA& Data ) : m_Data(Data)
//{
//
//}
//
//void GDBTaskConnLogInsert::Completer::Do()
//{
//	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
//	if (NULL == pPlayer)
//	{
//		ImmediatelyUpdateDisconnTime();
//		return;
//	}
//}
//
//void GDBTaskConnLogInsert::Completer::ImmediatelyUpdateDisconnTime()
//{
//	gsys.pDBManager->DisconnTimeUpdate(m_Data.nAID);
//}