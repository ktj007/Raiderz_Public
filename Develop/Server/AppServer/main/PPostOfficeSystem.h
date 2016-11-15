#pragma once

class SProxyPlayer;
struct TD_MAIL_MAILBOX_MAIL_INFO;
class PCmdRouter_Mail;
class PBounceMail;

#include <queue>

class PPostOfficeSystem
{
public:
	PPostOfficeSystem();
	virtual ~PPostOfficeSystem();

	void Update(void);

	virtual void RouteNewMailToReceiver(const SProxyPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo);
	virtual void RetryNewMailToReceiver(const SProxyPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo);

	virtual void SetNewRouter(PCmdRouter_Mail* pRouter);

protected:
	PCmdRouter_Mail* m_pRouter;
	queue<PBounceMail*>	m_queueBounceMail;
};
