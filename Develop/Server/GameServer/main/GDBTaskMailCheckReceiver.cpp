#include "stdafx.h"
#include "GDBTaskMailCheckReceiver.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GMailSystem.h"
#include "GDBManager.h"
#include "GConst.h"
#include "GMsgCommandFacade.h"

GDBTaskMailCheckReceiver::GDBTaskMailCheckReceiver(const MUID& uidReqPlayer)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_CHECK_RCVR)
{
	m_Data.m_uidPlayer	= uidReqPlayer;
}

GDBTaskMailCheckReceiver::~GDBTaskMailCheckReceiver()
{
}

void GDBTaskMailCheckReceiver::Input(const GDBT_MAIL_WRITE& data)
{
	m_Data.m_mailWrite = data;
}

mdb::MDB_THRTASK_RESULT GDBTaskMailCheckReceiver::RouteResult(CCommandResultTable nResult)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);

	if (pPlayer != NULL)
	{
		gsys.pMailSystem->RouteMailPostRes(pPlayer, nResult);
	}

	return mdb::MDBTR_SUCESS;
}

void GDBTaskMailCheckReceiver::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(SQL_MAIL_CHECK_RCVR)))
		return;

	m_Result.m_nErrorNo = rs.Field("ERRNO").AsInt();	
}

mdb::MDB_THRTASK_RESULT GDBTaskMailCheckReceiver::_OnCompleted()
{
	switch(m_Result.m_nErrorNo)
	{
	case 0:	return MailWrite();
	case 1:	return RouteResult(CR_FAIL_MAIL_POST_NOT_FIND_USER);	// 없는 이름
	case 2:
		if (GConst::TEST_MAIL_POST_TO_ONESELF)
		{
			return MailWrite();			
		}
		else
		{
			return RouteResult(CR_FAIL_MAIL_POST_SELF);				// 같은 캐릭터
		}
		break;
	case 3:	return RouteResult(CR_FAIL_MAIL_POST_NOT_SEND_GM);		// 일반 유저가 GM에게 보냄
	case 4:	return RouteResult(CR_FAIL_MAIL_POST_FULL_BOX);			// 메일 공간부족
	default:
		return RouteResult(CR_FAIL);
	}

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskMailCheckReceiver::_OnFailed()
{
	// 클라이언트에 실패 통보
	return RouteResult(CR_FAIL_MAIL_POST_DB_CHECK_FAIL);
}

mdb::MDB_THRTASK_RESULT GDBTaskMailCheckReceiver::MailWrite(void)
{
	if (!gsys.pDBManager->MailWrite(m_Data.m_uidPlayer, m_Data.m_mailWrite))
	{
		return RouteResult(CR_FAIL_MAIL_POST_DB_ERROR);
	}	

	return mdb::MDBTR_SUCESS;
}
