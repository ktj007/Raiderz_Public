#include "StdAfx.h"
#include "GMailMoneyTaker.h"
#include "GClientMailRouter.h"
#include "GGlobal.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GNPCInteractionInfo.h"
#include "GPlayerMailBox.h"
#include "GEntityPlayer.h"
#include "GDBTaskData.h"
#include "GMail.h"
#include "GDBManager.h"
#include "GMsgCommandFacade.h"

GMailMoneyTaker::GMailMoneyTaker(void)
{
	m_pMailRouter = new GClientMailRouter();
}

GMailMoneyTaker::~GMailMoneyTaker(void)
{
	SAFE_DELETE(m_pMailRouter);
}

bool GMailMoneyTaker::TakeAppendedMoney(GEntityPlayer* pPlayer, MUID uidMail)
{
	VALID_RET(pPlayer != NULL, false);


	// 우편함 인터렉션 확인
	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_MAILBOX))
	{
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, CR_FAIL_MAIL_APPEND_ITEM_NOT_INTERATION);
		return false;
	}


	// 편지 확인
	GPlayerMailBox& mailBox = pPlayer->GetMailbox();
	GMail* pMail = mailBox.GetMail(uidMail.Value);

	if (pMail == NULL)
	{
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, CR_FAIL_MAIL_NOT_FIND_MAIL);
		return false;
	}


	// 첨부 확인
	int nAppendedMoney = pMail->GetAppendedMoney();
	if (nAppendedMoney <= 0)
	{
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, CR_FAIL_MAIL_NO_APPENDED_MONEY);
		return false;
	}

	
	// 최대 소지금 여유가 있는지 확인
	int nMoneyStorageMargin = MAX_MONEY - pPlayer->GetMoney();	
	if (nMoneyStorageMargin < nAppendedMoney)
	{
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, CR_FAIL_MAIL_NOT_ENOUGH_MONEY_STORAGE);
		return false;
	}


	// 돈 취득 요청
	MUID uidPlayer = pPlayer->GetUID();
	
	GDBT_MAIL_GET_MONEY money;
	money.m_nAID = pPlayer->GetAID();
	money.m_nCID = pPlayer->GetCID();
	money.m_nMailUID = pMail->GetUID();
	money.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;
	money.m_nDeltaCharPtm = pPlayer->GetPlayerInfo()->GetDeltaPlayTime();
	money.m_nLevel = pPlayer->GetLevel();
	money.m_nAppendedMoney = nAppendedMoney;
	
	int nTotalResultMoney = pPlayer->GetMoney() + nAppendedMoney;
	money.m_nResultMoney = nTotalResultMoney;

	if (!gsys.pDBManager->MailGetAppendedMoney(uidPlayer, money))
	{
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, CR_FAIL_MAIL_TAKE_DB_ERROR);
		return false;
	}


	return true;
}

void GMailMoneyTaker::SetClientRouter(GClientMailRouter* pNewRouter)
{
	VALID(pNewRouter != NULL);

	if (m_pMailRouter != NULL)
	{
		SAFE_DELETE(m_pMailRouter);
	}

	m_pMailRouter = pNewRouter;
}
