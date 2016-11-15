#pragma once

#include "CCommandResultTable.h"
#include "CTransData.h"

class GEntityPlayer;

class GClientMailRouter : public MTestMemPool<GClientMailRouter>
{
public:
	GClientMailRouter();
	virtual ~GClientMailRouter();

	virtual void MailboxIsOpened(GEntityPlayer* pPlayer, const vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecTD_MAIL_MAILBOX_MAIL_INFO);
	virtual void MailboxIsClosed(GEntityPlayer* pPlayer);
	
	virtual void MailPostRes(GEntityPlayer* pPlayer, CCommandResultTable nResult);
	virtual void NotifyMail(GEntityPlayer* pPlayer, bool bIsNew, const vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecInfo);

	virtual void MailReadRes(GEntityPlayer* pPlayer, const TD_MAIL_CONTENT_INFO* pInfo, const wstring& strText, const vector<TD_ITEM>& vecAppendedItem);
	virtual void TakeAppendedItemRes(GEntityPlayer* pPlayer, MUID uidMail, uint8 nSlotID, int nNewThumbnailITemID);
	virtual void TakeAppendedMoneyRes(GEntityPlayer* pPlayer, MUID uidMail);

	virtual void MailDeleteRes(GEntityPlayer* pPlayer, MUID uidMail);

	virtual void PutUpItemRes(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem = NULL);
	virtual void PutDownItemRes(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem = NULL);
};
