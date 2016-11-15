#include "StdAfx.h"
#include "GDBTaskMailGetMoney.h"
#include "GGlobal.h"
#include "GDBCacheSystem.h"
#include "GCharacterDBCache.h"
#include "GPlayerObjectManager.h"
#include "GMailSystem.h"
#include "GMoneySystem.h"
#include "GPlayerMailBox.h"
#include "GEntityPlayer.h"
#include "GMail.h"
#include "GMsgCommandFacade.h"
#include "GDBManager.h"

GDBTaskMailGetMoney::GDBTaskMailGetMoney(const MUID& uidPlayer)
: GDBAsyncTask(uidPlayer,SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_GET_MONEY)
, m_SyncingList(uidPlayer)
{
}

GDBTaskMailGetMoney::~GDBTaskMailGetMoney(void)
{
}

bool GDBTaskMailGetMoney::Input(const GDBT_MAIL_GET_MONEY& data)
{
	// 입력값 저장
	m_Data.m_mailMoney = data;

	// DB 캐시
	if (!UpdateCache())
	{
		return false;
	}

	return true;
}

void GDBTaskMailGetMoney::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(SQL_MAIL_GET_MONEY));
}

void GDBTaskMailGetMoney::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	
	m_SyncingList.EndCacheSync(IsSuccess());
}

mdb::MDB_THRTASK_RESULT GDBTaskMailGetMoney::_OnCompleted()
{
	gsys.pDBManager->MailGetAppendedMoneyLog(m_Data.m_mailMoney);
	
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidReqPlayer);

	if (pPlayer != NULL)
	{	
		// 우편에서 게임머니를 제거합니다.
		GPlayerMailBox& mailBox = pPlayer->GetMailbox();
		GMail* pMail = mailBox.GetMail(m_Data.m_mailMoney.m_nMailUID);

		if (pMail != NULL)
		{
			pMail->ClearAppendedMoney();
		}


		// 게임머니를 증가
		gsys.pMoneySystem->IncreaseForDBTask(pPlayer, m_Data.m_mailMoney.m_nAppendedMoney);

		// 결과 통보
		gsys.pMailSystem->RouteTakeAppendedMoneyRes(pPlayer, m_Data.m_mailMoney.m_nMailUID);				
	}	

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskMailGetMoney::_OnFailed() 
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_uidReqPlayer);

	if (pPlayer != NULL)
	{
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, CR_FAIL_MAIL_TAKE_DB_ERROR);
	}

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskMailGetMoney::UpdateCache(void)
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(GetReqPlayer());
	if (NULL == cc)
		return true;

	if (!cc->IncMoney(m_Data.m_mailMoney.m_nAppendedMoney))
		return false;

	if (!cc->IsOverflow())
	{
		SetSync(false);	
		cc->EndCacheSync(false);
	}
	else
	{
		if (NULL == m_SyncingList.PushChar())
		{
			cc->EndCacheSync(false);
			return false;
		}
	}

	return true;
}
