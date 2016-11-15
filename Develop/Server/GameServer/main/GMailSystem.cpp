#include "StdAfx.h"
#include "GMailSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerMailBox.h"
#include "GClientMailRouter.h"
#include "GGlobal.h"
#include "GDBTaskData.h"
#include "GDBManager.h"
#include "GConfig.h"
#include "GStandAloneModeMailRouter.h"
#include "GAppServerMailRouter.h"
#include "GDBTaskData.h"
#include "GMail.h"
#include "GSystem.h"
#include "GMailItemPutUper.h"
#include "GMailItemPutDowner.h"
#include "GMailItemTaker.h"
#include "GMailMoneyTaker.h"
#include "GPlayerMailReserver.h"
#include "GConst.h"
#include "GMsgCommandFacade.h"
#include "CSStringFilter.h"

#define CHANGE_MODULE(pModule, pNewModule)	VALID(pNewModule != NULL);	\
											if (pModule != NULL)		\
												SAFE_DELETE(pModule);	\
											pModule = pNewModule;

GMailSystem::GMailSystem(void)
{
	m_pClientRouter = new GClientMailRouter();

	if (GConfig::IsStandAloneMode())
	{		
		m_pServerRouter = new GStandAloneModeMailRouter();
	}
	else
	{
		m_pServerRouter = new GAppServerMailRouter();
	}

	m_pItemPutUper = new GMailItemPutUper();
	m_pItemPutDowner = new GMailItemPutDowner();

	m_pItemTaker = new GMailItemTaker();
	m_pItemTaker->SetClientRouter(m_pClientRouter);

	m_pMoneyTaker = new GMailMoneyTaker();
}

GMailSystem::~GMailSystem(void)
{
	SAFE_DELETE(m_pClientRouter);
	SAFE_DELETE(m_pServerRouter);

	SAFE_DELETE(m_pItemPutUper);
	SAFE_DELETE(m_pItemPutDowner);

	SAFE_DELETE(m_pItemTaker);
	SAFE_DELETE(m_pMoneyTaker);
}

void GMailSystem::SetClientRouter(GClientMailRouter* pNewClientMailRouter)
{
	CHANGE_MODULE(m_pClientRouter, pNewClientMailRouter);
}
void GMailSystem::SetServerRouter(GServerMailRouter* pNewServerMailRouter)
{
	CHANGE_MODULE(m_pServerRouter, pNewServerMailRouter);
}
void GMailSystem::SetMailItemPutUper(GMailItemPutUper* pNewItemPutUper)
{
	CHANGE_MODULE(m_pItemPutUper, pNewItemPutUper);
}
void GMailSystem::SetMailItemPutDowner(GMailItemPutDowner* pNewItemPutDowner)
{
	CHANGE_MODULE(m_pItemPutDowner, pNewItemPutDowner);
}
void GMailSystem::SetMailItemTaker(GMailItemTaker* pNewItemTaker)
{
	CHANGE_MODULE(m_pItemTaker, pNewItemTaker);
}
void GMailSystem::SetMailMoneyTaker(GMailMoneyTaker* pNewMoneyTaker)
{
	CHANGE_MODULE(m_pMoneyTaker, pNewMoneyTaker);
}

void GMailSystem::RouteTakeAppendedMoneyRes(GEntityPlayer* pPlayer, MUID uidMail)
{
	m_pClientRouter->TakeAppendedMoneyRes(pPlayer, uidMail);
}

void GMailSystem::OpenMailbox(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	/// 유효기간 체크. 유효기간이 지났으면 파기.
	ClearExpiredMail(pPlayer);

	/// DB 에 편지가 남아있고 편지함에 여유가 있을 경우 DB 처리.
	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();

	if (refMailbox.IsRemainMailAtDB() &&
		refMailbox.GetRemainSize() > 0)
	{
		GetDBMailList(pPlayer, true);
		return;
	}

	/// DB 에 남아있는 편지가 없을 경우 바로 응답.
	RouteOpenMailbox(pPlayer);
}

void GMailSystem::ClearExpiredMail(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();
	GClientMailRouter router;

	vector<int64> vecExpiredMailUID;
	refMailbox.GetExpiredMailList(vecExpiredMailUID);
	for (size_t i = 0; i < vecExpiredMailUID.size(); ++i)
	{
		refMailbox.DelMail(vecExpiredMailUID[i]);
		router.MailDeleteRes(pPlayer, vecExpiredMailUID[i]);
	}
}

void GMailSystem::GetDBMailList(GEntityPlayer* pPlayer, bool bOpenMaibox/*=false*/)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();

	GDBT_MAIL_GET_MAILBOX_LIST dbParam(
		pPlayer->GetUID(),
		pPlayer->GetAID(),
		(int64)pPlayer->GetCID(),
		refMailbox.GetDBMailUID().GetCursor(),
		refMailbox.GetDBMailUID().GetBottom(),
		refMailbox.GetRemainSize());

	gsys.pDBManager->MailGetList(dbParam, bOpenMaibox);
}

void GMailSystem::RouteOpenMailbox(GEntityPlayer* pPlayer, int64 nReqMailUIDTop/*=0*/, int64 nReqMailUIDBottom/*=0*/)
{
	vector<TD_MAIL_MAILBOX_MAIL_INFO> vecTD_MAIL_MAILBOX_MAIL_INFO;
	pPlayer->GetMailbox().GetMailListTo_TD(vecTD_MAIL_MAILBOX_MAIL_INFO, nReqMailUIDTop, nReqMailUIDBottom);

	m_pClientRouter->MailboxIsOpened(pPlayer, vecTD_MAIL_MAILBOX_MAIL_INFO);
}


bool GMailSystem::PostMail(GEntityPlayer* pPlayer, const TD_MAIL_POST_REQ_INFO* pInfo, wstring strMailText)
{	
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pInfo != NULL, false);


	// 자신에겐 보낼 수 없다.
	if (!GConst::TEST_MAIL_POST_TO_ONESELF)
	{
		if (pPlayer->GetName() == pInfo->strReceiverName)
		{
			m_pClientRouter->MailPostRes(pPlayer, CR_FAIL_MAIL_POST_SELF);
			return false;
		}
	}

	// 제목 길이 검사
	if (MAX_MAIL_TITLE_LEN < wcslen(pInfo->strMailTitle))
	{
		m_pClientRouter->MailPostRes(pPlayer, CR_FAIL_MAIL_POST_LIMIT_TITLE);
		return false;
	}

	// 본문 길이 검사
	if (MAX_MAIL_TEXT_LEN <  strMailText.length())
	{
		m_pClientRouter->MailPostRes(pPlayer, CR_FAIL_MAIL_POST_LIMIT_TEXT);
		return false;
	}

	
	// 첨부 게임머니 검사
	int nTotalMoney = pPlayer->GetPlayerInfo()->GetMoney();

	if (nTotalMoney < pInfo->nAppendedMoney)
	{
		m_pClientRouter->MailPostRes(pPlayer, CR_FAIL_MAIL_POST_NOT_ENOUGH_MONEY);
		return false;
	}

	
	// 우편요금 검사 
	GPlayerMailReserver& reserver = pPlayer->GetMailReserver();
	int nAppendedSlotCount = reserver.GetAppendedSlotCount();

	int nPostage = CalcPostage(nAppendedSlotCount);

	if (nTotalMoney < pInfo->nAppendedMoney + nPostage)
	{
		m_pClientRouter->MailPostRes(pPlayer, CR_FAIL_MAIL_POST_NOT_ENOUGH_MONEY);
		return false;
	}



	// 보내느 편지 정보 설정
	GDBT_MAIL_WRITE mail;

	mail.m_nSenderAID = pPlayer->GetAID();
	mail.m_nSenderCID = pPlayer->GetCID();
	mail.m_strSenderName = pPlayer->GetName();
	mail.m_nSenderCharacterPlaySeconds = pPlayer->GetPlayerInfo()->nPlayTimeSec;
	mail.m_nDeltaCharPtm = pPlayer->GetPlayerInfo()->GetDeltaPlayTime();
	mail.m_nLevel = pPlayer->GetLevel();

	mail.m_nMoney = max(0, nTotalMoney - pInfo->nAppendedMoney - nPostage);		// 우편요금 + 첨부 게임머니 제거 후 금액
	mail.m_nDeltaMoney = pInfo->nAppendedMoney;
	mail.m_nPostage = nPostage;

	mail.m_strReceiverName = pInfo->strReceiverName;

	mail.m_nMailType = MT_PLAYER;
	mail.m_strTitle = CSStringFilter::RemoveGameSystemCharacter(pInfo->strMailTitle);
	mail.m_hasText = !strMailText.empty();
	mail.m_strText = CSStringFilter::RemoveGameSystemCharacter(strMailText);
	mail.m_nRemainDeleteSeconds = MAX_MAIL_DELETE_SECOND;


	// 아이템 첨부	
	mail.m_nDefaultItemID = reserver.GetDefaultItemID();
	mail.m_nAppendedItemCount = nAppendedSlotCount;

	int nAppendSuccessCount = reserver.MakeDBT(mail.m_appendedItemSlot);

	if (mail.m_nAppendedItemCount != nAppendSuccessCount)
	{
		m_pClientRouter->MailPostRes(pPlayer, CR_FAIL_MAIL_POST_WRONG_APPENDED_ITEM);
		return false;
	}


	// DB 요청
	MUID uidPlayer = pPlayer->GetUID();

	if (!gsys.pDBManager->MailCheckReceiver(pPlayer, mail))
	{		
		m_pClientRouter->MailPostRes(pPlayer, CR_FAIL_MAIL_POST_DB_ERROR);
		return false;
	}


	return true;
}

void GMailSystem::UpdateMailBox(GEntityPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{
	VALID(pPlayer != NULL);
	VALID(pInfo != NULL);


	// 메일 추가
	GPlayerMailBox& mailBox = pPlayer->GetMailbox();

	GMail* pMail = mailBox.NewMail(pInfo->uidMail);
	
	pMail->Init(	pPlayer->GetCID(),
					(MAIL_TYPE)pInfo->nType,
					pInfo->bIsRead,
					pInfo->strSenderName,
					pInfo->strMailTitle,
					pInfo->bHasText,
					pInfo->nAppendedMoney,
					pInfo->nAppendedItemID,
					pInfo->nExpiringTimeSec
				);

	pMail->InitSystemTimeMSForExpiCheck(gsys.pSystem->GetNowTime());

	mailBox.AddMail(pMail);


	// 새 메일 통보
	bool bIsNew = true;

	vector<int64> vecMailUID;
	vecMailUID.push_back(pInfo->uidMail);
	RouteNotifyMail(pPlayer, bIsNew, vecMailUID);
}

void GMailSystem::RouteNotifyMail(GEntityPlayer* pPlayer, bool bIsNew, const vector<int64>& vecMailUID)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();
	vector<TD_MAIL_MAILBOX_MAIL_INFO> vecInfo;

	for (size_t i = 0; i < vecMailUID.size(); ++i)
	{
		TD_MAIL_MAILBOX_MAIL_INFO td;
		refMailbox.GetMailTo_TD(td, vecMailUID[i]);
		vecInfo.push_back(td);
	}

	// 통보
	m_pClientRouter->NotifyMail(pPlayer, bIsNew, vecInfo);
}

void GMailSystem::ReadMail(GEntityPlayer* pPlayer, MUID uidMail)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();

	/// 편지 확인.
	GMail* pMail = refMailbox.GetMail(uidMail.Value);
	if (pMail == NULL)
		return;

	/// 편지 내용이 채워져 있으면 처리하지 않음. - 이미 편지 내용을 보낸적이 있음.
	if (pMail->IsFilledContent())
		return;

	/// 편재 내용이 채워져있지 않으면 DB 처리.
	MUID uidPlayer = pPlayer->GetUID();
	gsys.pDBManager->MailGetContent(uidPlayer, uidMail.Value, pMail->HasText());
}

void GMailSystem::RetryTransNewMail(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{
	VALID(pInfo != NULL);
	m_pServerRouter->RetryTransNewMail(nCID, pInfo);	
}

void GMailSystem::RouteNewMailToReceiver(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{
	VALID(pInfo != NULL);
	m_pServerRouter->RouteNewMailToReceiver(nCID, pInfo);	
}

void GMailSystem::RouteMailPostRes(GEntityPlayer* pPlayer, CCommandResultTable nResult)
{
	VALID(pPlayer != NULL);
	m_pClientRouter->MailPostRes(pPlayer, nResult);
}

void GMailSystem::PutUpItem(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem)
{
	VALID(pPlayer != NULL);
	VALID(pItem != NULL);

	// 첨부 아이템 올리기	
	if (!m_pItemPutUper->Do(pPlayer, pItem))
	{
		CCommandResultTable nResult = m_pItemPutUper->GetLastFailure();
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, nResult);

		return;
	}

	// 첨부 아이템 올리기 결과 통보
	m_pClientRouter->PutUpItemRes(pPlayer, pItem);
}

void GMailSystem::PutDownItem(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem)
{
	VALID(pPlayer != NULL);
	VALID(pItem != NULL);

	// 첨부 아이템 내리기
	if (!m_pItemPutDowner->Do(pPlayer, pItem))
	{
		CCommandResultTable nResult = m_pItemPutDowner->GetLastFailure();
		gsys.pMsgCommandFacade->RouteAdviceMsg(pPlayer, nResult);

		return;
	}


	// 첨부 아이템 내리기 결과 통보
	m_pClientRouter->PutDownItemRes(pPlayer, pItem);
}

void GMailSystem::DeleteMail(GEntityPlayer* pPlayer, MUID uidMail)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();
	if (false == refMailbox.IsExist(uidMail.Value))
	{
		mlog3("Failed! GMailSystem::DeleteMail(), Not Exist Mail(MailUID=%I64u)\n", uidMail.Value);
		return;
	}

	GDBT_MAIL_DELETE dbParam;
	dbParam.m_nAID = pPlayer->GetAID();
	dbParam.m_nCID = (int64)pPlayer->GetCID();
	dbParam.m_nCharPtm = pPlayer->GetPlayerInfo()->nPlayTimeSec;
	dbParam.m_nMailUID = uidMail.Value;
	gsys.pDBManager->MailDelete(pPlayer->GetUID(), dbParam);
}

int GMailSystem::CalcPostage(int nSlotCount)
{
	int nPostage = MAIL_BASE_POSTAGE + (nSlotCount * MAIL_EXTRA_POSTAGE_PER_SLOT);
	return nPostage;
}

MAIL_TYPE GMailSystem::GetMailType(const GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer != NULL, MT_PLAYER);

	if (PLAYER_GRADE_GM == pPlayer->GetPlayerGrade())
	{
		return MT_GM;
	}
	else
	{
		return MT_PLAYER;
	}
}
