#pragma once

#include "STypes.h"
#include "CCommandResultTable.h"

struct TD_MAIL_MAILBOX_MAIL_INFO;

class GServerMailRouter
{
public:
	// gmock이 인터페이스 클래스에도 소멸자를 요구한다.
	virtual ~GServerMailRouter()	{}

	virtual void RouteNewMailToReceiver(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo) = 0;
	virtual void RetryTransNewMail(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo) = 0;
};
