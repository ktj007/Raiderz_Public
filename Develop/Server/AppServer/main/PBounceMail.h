#pragma once

#include "MTime.h"
#include "STypes.h"

struct TD_MAIL_MAILBOX_MAIL_INFO;

class PBounceMail
{
public:
	PBounceMail(uint32 nNowTime, CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo);
	virtual ~PBounceMail();
	
	bool IsReady(uint32 nNowTime);
	CID	GetCID(void);
	void Export(TD_MAIL_MAILBOX_MAIL_INFO* pInfo);

private:
	uint32		m_nCreateTime;
	uint32		m_nReadyTime;

	CID			m_nCID;
	int64		m_uidMail;
	uint8		m_nType;
	wstring		m_strSenderName;
	wstring 	m_strMailTitle;
	uint32		m_nAppendedItemID;
	bool		m_bIsRead;
	uint32		m_nExpiringTimeSec;
	bool		m_bHasText;
	int			m_nAppendedMoney;
};
