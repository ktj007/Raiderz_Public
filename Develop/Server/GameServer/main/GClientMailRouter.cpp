#include "stdafx.h"
#include "GClientMailRouter.h"
#include "GGlobal.h"
#include "CTransData.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GCommandCenter.h"


GClientMailRouter::GClientMailRouter()
{
	// do nothing
}

GClientMailRouter::~GClientMailRouter()
{
	// do nothing
}

void GClientMailRouter::MailboxIsOpened(GEntityPlayer* pPlayer, const vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecTD_MAIL_MAILBOX_MAIL_INFO)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_MAILBOX_IS_OPENED,
		1,
		NEW_BLOB(vecTD_MAIL_MAILBOX_MAIL_INFO)
		);

	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::MailboxIsClosed(GEntityPlayer* pPlayer)
{
	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_MAILBOX_IS_CLOSED, 0, NULL);

	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::MailPostRes(GEntityPlayer* pPlayer, CCommandResultTable nResult)
{
	if (pPlayer == NULL)	return;

	MCommand* pNewCmd  = MakeNewCommand(MC_MAIL_POST_RES, 1, NEW_INT(nResult));
	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::NotifyMail(GEntityPlayer* pPlayer, bool bIsNew, const vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecInfo)
{
	if (pPlayer == NULL)	return;

	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_NOTIFY_MAIL, 2, NEW_BOOL(bIsNew), NEW_BLOB(vecInfo));
	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::MailReadRes(GEntityPlayer* pPlayer, const TD_MAIL_CONTENT_INFO* pInfo, const wstring& strText, const vector<TD_ITEM>& vecAppendedItem)
{
	VALID(pPlayer != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_READ_RES, 
										3, 
										NEW_SINGLE_BLOB(pInfo, sizeof(TD_MAIL_CONTENT_INFO)),
										NEW_WSTR(strText.c_str()),
										NEW_BLOB(vecAppendedItem));

	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::TakeAppendedItemRes(GEntityPlayer* pPlayer, MUID uidMail, uint8 nSlotID, int nNewThumbnailITemID)
{
	VALID(pPlayer != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_TAKE_APPENDED_ITEM_RES, 
		3,
		NEW_UID(uidMail),
		NEW_UCHAR(nSlotID),
		NEW_INT(nNewThumbnailITemID));

	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::TakeAppendedMoneyRes(GEntityPlayer* pPlayer, MUID uidMail)
{
	VALID(pPlayer != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_TAKE_APPENDED_MONEY_RES, 
		1,
		NEW_UID(uidMail));

	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::MailDeleteRes(GEntityPlayer* pPlayer, MUID uidMail)
{
	VALID(pPlayer != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_DELETE_RES, 
		1, 
		NEW_UID(uidMail));

	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::PutUpItemRes(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem /*= NULL*/)
{
	VALID(pPlayer != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_APPEND_PUTUP_ITEM_RES,
										1,
										NEW_SINGLE_BLOB(pItem, sizeof(TD_MAIL_RESERVE_ITEM))										
										);

	pPlayer->RouteToMe(pNewCmd);
}

void GClientMailRouter::PutDownItemRes(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem /*= NULL*/)
{
	VALID(pPlayer != NULL);

	MCommand* pNewCmd = MakeNewCommand(MC_MAIL_APPEND_PUTDOWN_ITEM_RES,
										1, 
										NEW_SINGLE_BLOB(pItem, sizeof(TD_MAIL_RESERVE_ITEM))
										);

	pPlayer->RouteToMe(pNewCmd);
}
