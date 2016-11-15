#include "stdafx.h"
#include "FBaseMail.h"
#include "CTransData.h"

FBaseMail::FBaseMail()
{
	// do nothing
}

FBaseMail::~FBaseMail()
{
	// do nothing
}


void FBaseMail::InitMailInfo(TD_MAIL_MAILBOX_MAIL_INFO& info)
{
	info.uidMail = 1;
	info.nType = 1;
	wcsncpy_s(info.strSenderName, L"TestSenderName", _TRUNCATE);
	wcsncpy_s(info.strMailTitle, L"TestMailTitle", _TRUNCATE);
	info.nAppendedItemID = 3;
	info.bIsRead = false;
	info.nExpiringTimeSec = 3600;
	info.bHasText = true;
	info.nAppendedMoney = 100;
}

void FBaseMail::InitReserveItem(TD_MAIL_RESERVE_ITEM& item)
{
	item.nInventorySlotID = 0;
	item.nItemAmount = 5;
}
