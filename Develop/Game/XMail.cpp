#include "StdAfx.h"
#include "XMail.h"
#include "XSystem.h"

XMail::XMail(MUID nMailUID
			, MAIL_TYPE eType
			, wstring strSenderName
			, wstring strMailTitle
			, int nAppendedItemID
			, int nAppendedIMoney
			, bool bIsRead
			, int nExpiServerTimeSec)
: m_nMailUID(nMailUID)
, m_eType(eType)
, m_strSender(strSenderName)
, m_strTitle(strMailTitle)
, m_nThumbnailItemID(nAppendedItemID)
, m_bIsRead(bIsRead)
, m_nExpiServerTimeSec(nExpiServerTimeSec)
{
	m_content.nAppendedMoney = nAppendedIMoney;
}

XMail::~XMail()
{
}

int XMail::GetMailRemaindDay()
{
	uint32 nSystemNowTime = XGetNowTime();
	uint32 nDelta = nSystemNowTime - m_nExpiCheckStartTimeMS;
	if (nDelta < 0)
		return 0;

	int nExpiRemainTimeSec = m_nExpiServerTimeSec - static_cast<int>((float)nDelta * 0.001f);
	if (nExpiRemainTimeSec >= MAX_MAIL_DELETE_SECOND)
		nExpiRemainTimeSec = MAX_MAIL_DELETE_SECOND - 1;

	const int MIN_PER_SEC = 60;
	const int HOUR_PER_MIN = 60;
	const int DAY_PER_HOUR = 24;

	int nRemainDay = nExpiRemainTimeSec / (MIN_PER_SEC * HOUR_PER_MIN * DAY_PER_HOUR);
	return nRemainDay;
}