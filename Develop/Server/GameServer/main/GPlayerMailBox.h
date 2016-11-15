#pragma once

#include "CTransData.h"

class GEntityPlayer;
class GMail;
class GPlayerMailBox;

class GMailboxDBMailUID
{
	friend GPlayerMailBox;
public:
	GMailboxDBMailUID();
	~GMailboxDBMailUID() {}

	bool	Init(int64 nTop, int64 nBottom);

	int64	GetTop()				{ return m_nTop; }
	int64	GetBottom()				{ return m_nBottom; }
	int64	GetCursor()				{ return m_nCursor; }

private:
	int64	m_nTop;		///< DB 에서 읽어올 편지들의 UID 중 Top of Range.
	int64	m_nBottom;	///< DB 에서 읽어올 편지들의 UID 중 Bottom of Range.
	int64	m_nCursor;	///< DB 에서 읽어올 편지들의 UID 시작값. (CursorUID 값의 편지를 포함한다.)

	void	SetTop(int64 nUID)		{ m_nTop = nUID; }
	void	MoveCursor(int64 nUID);

	bool	IsEndCursor()			{ return (m_nCursor > m_nBottom ? false : true); }
	void	EndCursor()				{ m_nCursor = m_nBottom;}
};

class GPlayerMailBox : public MTestMemPool<GPlayerMailBox>
{
	typedef std::map<int64, GMail*> GMailboxItemMap;

public:
	GPlayerMailBox(GEntityPlayer* pOwner);
	~GPlayerMailBox();

	void				SetSummaryInfo(int nTotalMailCount, int nUnreadMailCount);
	bool				IsExistUnreadMail();
	bool				IsFullMailbox();

	bool				IsExist(int64 nMailUID);
	GMail*				GetMail(int64 nMailUID);

	GMail*				NewMail(int64 nMailUID);
	bool				AddMail(GMail* pMail);
	void				DelMail(int64 nMailUID);

	bool				IsReaded(int64 nMailUID);
	void				SetRead(int64 nMailUID);

	bool				IsRemainMailAtDB()		{ return (m_DBMailUID.IsEndCursor() ? false : true); }
	void				SetNoRemainMailAtDB()	{ m_DBMailUID.EndCursor(); }

	void				GetMailTo_TD(TD_MAIL_MAILBOX_MAIL_INFO& refInfo, int64 nMailUID);
	void				GetMailListTo_TD(std::vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecMailList);
	void				GetMailListTo_TD(std::vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecMailList, int64 nDBMailUIDTop, int64 nDBMailUIDBottom);
	void				GetExpiredMailList(std::vector<int64>& vecExpiredMailUID);

	GMailboxDBMailUID&	GetDBMailUID()		{ return m_DBMailUID; }
	
	int					GetRemainSize();
	
	int					GetSize()				{ return (int)m_mapMails.size(); }
	int					GetMaxSize();

private:
	GEntityPlayer*		m_pOwner;

	int					m_nTotalMailCount;
	int					m_nUnreadMailCount;

	GMailboxDBMailUID	m_DBMailUID;

	GMailboxItemMap		m_mapMails;

	void				Clear();

};
