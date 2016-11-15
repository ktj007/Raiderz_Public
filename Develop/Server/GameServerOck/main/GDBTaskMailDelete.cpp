#include "StdAfx.h"
#include "GDBTaskMailDelete.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GPlayerMailBox.h"
#include "GClientMailRouter.h"
#include "GMailSystem.h"
#include "GDBManager.h"


GDBTaskMailDelete::GDBTaskMailDelete(const MUID& uidReqPlayer, int64 nMailUID)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_DELETE)
{
	m_Data.uidPlayer = uidReqPlayer;
	m_Data.nMailUID = nMailUID;
}

GDBTaskMailDelete::~GDBTaskMailDelete(void)
{
}

void GDBTaskMailDelete::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(SQL_MAIL_DELETE));
}
mdb::MDB_THRTASK_RESULT GDBTaskMailDelete::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}
mdb::MDB_THRTASK_RESULT GDBTaskMailDelete::_OnFailed()
{
	return mdb::MDBTR_SUCESS;
}

void GDBTaskMailDelete::Input( const GDBT_MAIL_DELETE& data )
{
	m_Data.MailData = data;
}

GDBTaskMailDelete::Completer::Completer(_DATA& Data) : m_Data(Data)
{
}

void GDBTaskMailDelete::Completer::Do()
{
	gsys.pDBManager->MailDeleteLog(m_Data.MailData);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.uidPlayer);
	if (pPlayer == NULL) return;

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();
	refMailbox.DelMail(m_Data.nMailUID);

	GClientMailRouter router;
	router.MailDeleteRes(pPlayer, m_Data.nMailUID);

	// DB 에 아직 읽어오지 않은 메일이 남아있을 경우.
	if (refMailbox.IsRemainMailAtDB())
		gsys.pMailSystem->GetDBMailList(pPlayer);
}
