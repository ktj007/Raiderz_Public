#include "StdAfx.h"
#include "GPlayerMailBox.h"
#include "CSDef.h"
#include "GMail.h"
#include "AStlUtil.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GSystem.h"


GMailboxDBMailUID::GMailboxDBMailUID()
: m_nTop(0)
, m_nBottom(0)
, m_nCursor(0)
{
}

bool GMailboxDBMailUID::Init(int64 nDBMailUIDTop, int64 nDBMailUIDBottom)
{
	VALID_RET(nDBMailUIDTop >= nDBMailUIDBottom, false);

	m_nTop = nDBMailUIDTop;
	m_nCursor = nDBMailUIDTop;
	m_nBottom = (nDBMailUIDBottom > 0 ? nDBMailUIDBottom - 1 : 0);

	return true;
}

void GMailboxDBMailUID::MoveCursor(int64 nUID)
{
	if (nUID <= m_nBottom)
		m_nCursor = m_nBottom;
	else
		m_nCursor = nUID - 1;

	if (m_nCursor < 0)
		m_nCursor = 0;
}

//////////////////////////////////////////////////////////////////////////

GPlayerMailBox::GPlayerMailBox(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_nTotalMailCount(0)
, m_nUnreadMailCount(0)
{
	VALID(m_pOwner != NULL);
}

GPlayerMailBox::~GPlayerMailBox()
{
	Clear();
}

void GPlayerMailBox::Clear()
{
	SAFE_DELETE_MAP(m_mapMails);
}

void GPlayerMailBox::SetSummaryInfo(int nTotalMailCount, int nUnreadMailCount)
{
	VALID((nTotalMailCount >= 0) && (nUnreadMailCount >= 0));
	VALID(nTotalMailCount >= nUnreadMailCount);

	m_nTotalMailCount = nTotalMailCount;
	m_nUnreadMailCount = nUnreadMailCount;
}

bool GPlayerMailBox::IsExistUnreadMail()
{
	if (m_nUnreadMailCount > 0) return true;
	return false;
}

bool GPlayerMailBox::IsFullMailbox()
{
	if (m_nTotalMailCount >= GetMaxSize()) return true;
	return false;
}

void GPlayerMailBox::GetExpiredMailList(std::vector<int64>& vecExpiredMailUID)
{
	if (m_mapMails.size() == 0) return;

	GMailboxItemMap::iterator itr = m_mapMails.begin();
	for (; itr != m_mapMails.end(); ++itr)
	{
		GMail* pMail = (GMail*)itr->second;

		/// Check Expired
		uint32 nServerNowTime = gsys.pSystem->GetNowTime();
		if (pMail->GetExpiRemainTimeSec(nServerNowTime) <= 0)
			vecExpiredMailUID.push_back(pMail->GetUID());
	}
}

bool GPlayerMailBox::IsExist(int64 nMailUID)
{
	GMailboxItemMap::iterator itr = m_mapMails.find(nMailUID);
	if (itr == m_mapMails.end())
		return false;

	return true;
}

GMail* GPlayerMailBox::GetMail(int64 nMailUID)
{
	GMailboxItemMap::iterator itr = m_mapMails.find(nMailUID);
	if (itr == m_mapMails.end())
		return NULL;

	return itr->second;
}

GMail* GPlayerMailBox::NewMail(int64 nMailUID)
{
	VALID_RET(nMailUID > 0, NULL);
	GMail* pNewMail = new GMail(nMailUID);
	return pNewMail;
}

bool GPlayerMailBox::AddMail(GMail* pMail)
{
	if (pMail == NULL)				return false;
	if (IsExist(pMail->GetUID()))	return false;

	/// BottomUID 보다 작은 UID 의 메일은 요청하지 않는다.
	if (pMail->GetUID() < m_DBMailUID.GetBottom())
		return false;

	/// TopUID 보다 큰 UID 의 편지는 온라인 중에 새로 받은 편지.
	if (pMail->GetUID() > m_DBMailUID.GetTop())
		m_DBMailUID.SetTop(pMail->GetUID());

	m_mapMails.insert(GMailboxItemMap::value_type(pMail->GetUID(), pMail));

	/// Cursor 를 갱신한다.
	if (pMail->GetUID() <= m_DBMailUID.GetCursor())
		m_DBMailUID.MoveCursor(pMail->GetUID());

	return true;
}

void GPlayerMailBox::DelMail(int64 nMailUID)
{
	GMailboxItemMap::iterator itr = m_mapMails.find(nMailUID);
	if (itr == m_mapMails.end())
		return;

	SAFE_DELETE(itr->second);
	m_mapMails.erase(itr);
}

bool GPlayerMailBox::IsReaded(int64 nMailUID)
{
	GMailboxItemMap::iterator itr = m_mapMails.find(nMailUID);
	if (itr == m_mapMails.end())
		return false;

	return itr->second->IsReaded();
}

void GPlayerMailBox::SetRead(int64 nMailUID)
{
	GMailboxItemMap::iterator itr = m_mapMails.find(nMailUID);
	if (itr == m_mapMails.end())
		return;

	itr->second->SetRead();
}

void GPlayerMailBox::GetMailTo_TD(TD_MAIL_MAILBOX_MAIL_INFO& refInfo, int64 nMailUID)
{
	GMailboxItemMap::iterator itr = m_mapMails.find(nMailUID);
	if (itr == m_mapMails.end()) return;

	GMail* pMail = (GMail*)itr->second;
	uint32 nServerNowTime = gsys.pSystem->GetNowTime();
	pMail->Export(refInfo, nServerNowTime);
}
void GPlayerMailBox::GetMailListTo_TD(std::vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecMailList)
{
	GetMailListTo_TD(vecMailList, m_DBMailUID.GetTop(), m_DBMailUID.GetBottom());
}
void GPlayerMailBox::GetMailListTo_TD(vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecMailList, int64 nDBMailUIDTop, int64 nDBMailUIDBottom)
{
	if (m_mapMails.size() == 0)	return;
	if (nDBMailUIDTop == 0) return;

	GMailboxItemMap::iterator itr = m_mapMails.begin();
	for (; itr != m_mapMails.end(); ++itr)
	{
		GMail* pMail = (GMail*)itr->second;

		if (nDBMailUIDBottom <= pMail->GetUID() && pMail->GetUID() <= nDBMailUIDTop)
		{
			TD_MAIL_MAILBOX_MAIL_INFO td;
			uint32 nServerNowTime = gsys.pSystem->GetNowTime();
			pMail->Export(td, nServerNowTime);
			vecMailList.push_back(td);
		}
	}
}

int GPlayerMailBox::GetMaxSize()
{
	return MAX_MAILBOX_MAIL_COUNT;
}

int GPlayerMailBox::GetRemainSize()
{
	int nRemainMailboxSlot = (GetMaxSize() - (int)m_mapMails.size());
	if (nRemainMailboxSlot < 0) return 0;
	return nRemainMailboxSlot;
}
