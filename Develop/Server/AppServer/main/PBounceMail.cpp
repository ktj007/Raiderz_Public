#include "stdafx.h"
#include "PBounceMail.h"
#include "SDef.h"
#include "CTransData.h"


#define TRANS_MAIL_DELEY_TICK				30000
#define CREATE_TIME_LIMIT					((UINT_MAX) - (TRANS_MAIL_DELEY_TICK))

PBounceMail::PBounceMail(uint32 nNowTime, CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{	
	// 생성 시간, 전달 시간 기록
	m_nCreateTime = nNowTime;
	
	if (m_nCreateTime <= CREATE_TIME_LIMIT)
	{
		m_nReadyTime = m_nCreateTime + TRANS_MAIL_DELEY_TICK;
	}
	else
	{
		// 오버플로우
		m_nReadyTime = TRANS_MAIL_DELEY_TICK - (UINT_MAX - m_nCreateTime) - 1;		// 0 포함.
	}


	// 반송 우편정보 기록
	m_nCID = nCID;

	VALID(pInfo != NULL);

	m_uidMail = pInfo->uidMail;
	m_nType = pInfo->nType;
	m_strSenderName = pInfo->strSenderName;
	m_strMailTitle = pInfo->strMailTitle;
	m_nAppendedItemID = pInfo->nAppendedItemID;
	m_bIsRead = pInfo->bIsRead;
	m_nExpiringTimeSec = pInfo->nExpiringTimeSec;
	m_bHasText = pInfo->bHasText;
	m_nAppendedMoney = pInfo->nAppendedMoney;
}

PBounceMail::~PBounceMail()
{
	// do nothing
}

bool PBounceMail::IsReady(uint32 nNowTime)
{
	if (m_nCreateTime <= m_nReadyTime)
	{
		// 일반 설정 시
		if (nNowTime < m_nCreateTime || m_nReadyTime <= nNowTime)
		{
			return true;		
		}
		else
		{
			return false;
		}
	}
	else
	{
		// 오버플로우 설정 시
		if (m_nReadyTime <= nNowTime && nNowTime < m_nCreateTime)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

CID	PBounceMail::GetCID(void)
{
	return m_nCID;
}

void PBounceMail::Export(TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{
	VALID(pInfo != NULL);
	
	pInfo->uidMail = m_uidMail;
	pInfo->nType = m_nType;
	wcsncpy_s(pInfo->strSenderName, m_strSenderName.c_str(), _TRUNCATE);
	wcsncpy_s(pInfo->strMailTitle, m_strMailTitle.c_str(), _TRUNCATE);
	pInfo->nAppendedItemID = m_nAppendedItemID;
	pInfo->bIsRead = m_bIsRead;
	pInfo->nExpiringTimeSec = m_nExpiringTimeSec;
	pInfo->bHasText = m_bHasText;
	pInfo->nAppendedMoney = m_nAppendedMoney;
}
