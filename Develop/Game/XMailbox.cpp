#include "StdAfx.h"
#include "XMailbox.h"
#include "XMail.h"
#include "XSystem.h"

XMailbox::XMailbox(void)
: m_bHadOpened(false)
{
}

XMailbox::~XMailbox(void)
{
	Clear();
}

void XMailbox::Clear()
{
	m_summary.bExistUnreadMail = false;
	m_summary.bIsFullMailbox = false;

	m_bHadOpened = false;
	SAFE_DELETE_MAP(m_mapMailboxItems);
}

void XMailbox::SetSummaryInfo(bool bExistUnreadMail, bool bIsFullMailbox)
{
	m_summary.bExistUnreadMail = bExistUnreadMail;
	m_summary.bIsFullMailbox = bIsFullMailbox;
}

bool XMailbox::IsExistUnreadMail()
{
	if (!HadOpened() && m_mapMailboxItems.size() == 0)
		return m_summary.bExistUnreadMail;

	XMailboxItemMap::iterator itr = m_mapMailboxItems.begin();
	for (; itr != m_mapMailboxItems.end(); ++itr)
	{
		XMail* pMail = (XMail*)itr->second;
		if (!pMail->IsRead())
			return true;
	}
	return false;
} 

bool XMailbox::IsFullMailbox()
{
	if (!HadOpened())
		return m_summary.bIsFullMailbox;

	if (MAX_MAILBOX_MAIL_COUNT <= (int)m_mapMailboxItems.size())
		return true;
	
	return false;
}

void XMailbox::AddNewMail(MUID uidMail, MAIL_TYPE eType, wstring strSenderName, wstring strMailTitle, uint32 nAppendedItemID, uint32 nAppendedMoney, bool bIsRead, uint32 nExpiServerTimeSec)
{
	XMail* pNewMail = new XMail(uidMail, eType, strSenderName, strMailTitle, nAppendedItemID, nAppendedMoney, bIsRead, nExpiServerTimeSec);
	pNewMail->InitSystemTimeMSForExpiCheck(XGetNowTime());

	m_mapMailboxItems.insert(XMailboxItemMap::value_type(pNewMail->GetUID(), pNewMail));
}

void XMailbox::DeleteMail(MUID uidMail)
{
	m_mapMailboxItems.erase(uidMail);
}


XMail* XMailbox::GetMail( MUID _uid)
{
	XMailboxItemMap::iterator itr = m_mapMailboxItems.find( _uid);
	if ( itr == m_mapMailboxItems.end())		return NULL;
	return (*itr).second;
}


XMail* XMailbox::GetMailIndex( int _index)
{
	XMailboxItemMap::iterator itr = m_mapMailboxItems.begin();
	for ( int i = 0;  i < _index;  i++, itr++) ;
	return (*itr).second;
}


void XMailbox::GetMailList(vector<const XMail*>& vecMailList)
{
	XMailboxItemMap::reverse_iterator ritr = m_mapMailboxItems.rbegin();
	for (; ritr != m_mapMailboxItems.rend(); ++ritr)
	{
		vecMailList.push_back(ritr->second);
	}
}


int XMailbox::GetMailCount() const
{
	return m_mapMailboxItems.size();
}

