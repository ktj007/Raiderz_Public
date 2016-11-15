#pragma once


struct TD_ITEM;

struct XMailContent
{
	/// 처음 내용을 받아올 때 true 로 변경하고 다음 편지를 읽을 때는 서버에 요청하지 않고 로컬 데이터를 읽습니다.
	bool		bFill;

	wstring				strText;
	int					nAppendedMoney;
	vector< TD_ITEM>	vAppendedItems;

	void DelAppendedItem(int nMailSlotID)
	{
		vector< TD_ITEM>::iterator itr = vAppendedItems.begin();
		for (; itr < vAppendedItems.end(); ++itr)
		{
			if ((*itr).m_nSlotID == nMailSlotID)
			{
				vAppendedItems.erase(itr);
				break;
			}
		}
	}

	/// TODO - mailbox : 첨부 아이템 목록을 들고 있어야 합니다.

	XMailContent()
	: bFill(false)
	, nAppendedMoney(0) {}
};

class XMail
{
public:
	XMail(MUID uidMail
		, MAIL_TYPE eType
		, wstring strSenderName
		, wstring strMailTitle
		, int nAppendedItemID
		, int nAppendedIMoney
		, bool bIsRead
		, int nExpiServerTimeSec);
	~XMail();

	MUID			GetUID()				{ return m_nMailUID; }
	void			SetRead()				{ m_bIsRead = true; }
	bool			IsRead() const			{ return m_bIsRead; }
	wstring			GetSender()				{ return m_strSender; }
	wstring			GetTitle()				{ return m_strTitle; }
	XMailContent&	GetContent()			{ return m_content; }

	int				GetThumbnailItemID()	{ return m_nThumbnailItemID; }
	void			SetThumbnailItemID(int nNewItemID)	{ m_nThumbnailItemID = nNewItemID; }

	void			InitSystemTimeMSForExpiCheck(uint32 nExpiStartTimeMS)	{ m_nExpiCheckStartTimeMS = nExpiStartTimeMS; }
	int				GetMailRemaindDay();	///< 남은 날짜.

private:
	MUID			m_nMailUID;
	MAIL_TYPE		m_eType;

	wstring			m_strSender;
	wstring			m_strTitle;
	int				m_nExpiServerTimeSec;		///< Server 에서 얻어온 시점의 남은 유효기간. (초)
	uint32			m_nExpiCheckStartTimeMS;	///< 클라이언트의 유효기간 체크 시작시간. (Millisecond)

	bool			m_bIsRead;
	int				m_nThumbnailItemID;
	XMailContent	m_content;

};
