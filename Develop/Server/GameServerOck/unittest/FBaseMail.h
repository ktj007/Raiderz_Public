#pragma once

struct TD_MAIL_MAILBOX_MAIL_INFO;
struct TD_MAIL_RESERVE_ITEM;

class FBaseMail
{
public:
	FBaseMail();
	virtual ~FBaseMail();

	void InitMailInfo(TD_MAIL_MAILBOX_MAIL_INFO& info);
	void InitReserveItem(TD_MAIL_RESERVE_ITEM& item);
};
