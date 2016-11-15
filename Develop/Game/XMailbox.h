#pragma once

class XMail;

class XMailbox
{
	typedef std::map<MUID, XMail* > XMailboxItemMap;

public:
	XMailbox();
	~XMailbox();

	void	SetSummaryInfo(bool bExistUnreadMail, bool bIsFullMailbox);
	bool	IsExistUnreadMail();
	bool	IsFullMailbox();

	bool	HadOpened()	{ return m_bHadOpened; }
	void	SetHasOpened() { m_bHadOpened = true; }

	void	Clear();
	void	AddNewMail(MUID uidMail, MAIL_TYPE eType, wstring strSenderName, wstring strMailTitle, uint32 nAppendedItemID, uint32 nAppendedMoney, bool bIsRead, uint32 nExpiringTimeSec);
	void	DeleteMail(MUID uidMail);

	XMail*	GetMail( MUID _uid);
	XMail*	GetMailIndex( int _index);
	void	GetMailList(vector<const XMail*>& vecMailList);

	int		GetMailCount() const;


private:
	struct SUMMARY_INFO
	{
		bool bExistUnreadMail;
		bool bIsFullMailbox;
		SUMMARY_INFO() : bExistUnreadMail(false), bIsFullMailbox(false) {}
	};
	SUMMARY_INFO	m_summary;	///< 편지함 목록을 받기 전까지만 사용합니다.

	bool			m_bHadOpened;
	XMailboxItemMap	m_mapMailboxItems;

};
