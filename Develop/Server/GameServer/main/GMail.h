#pragma once

struct TD_MAIL_MAILBOX_MAIL_INFO;
class GItem;

class GMail : public MTestMemPool<GMail>
{
public:
	GMail(int64 nMailUID);
	virtual				~GMail(void);

	int64				GetUID()	{ return m_nMailUID; }
	bool				Init(CID nOwnerCID
							, MAIL_TYPE eType
							, bool bIsRead
							, wstring strSender
							, wstring strTitle
							, bool bHasText
							, int nAppendedMoney
							, int nAppendedItemID
							, int nExpiRemainTimeSec);

	bool				HasText()							{ return m_bHasText; }
	void				SetThumbnailItemID(int nNewItemID)	{ m_nThumbnailItemID = nNewItemID; }
	int					GetAppendedMoney()					{ return m_nAppendedMoney; }
	GItem*				GetAppendedItem(int nSlotID);
	void				DelAppendedItem(int nSlotID);
	int					GetNewThumbnailItemID(int nIgnoreSlotID);

	int					GetExpiDBRemainTimeSec()	{ return m_nExpiDBRemainTimeSec; }
	void				InitSystemTimeMSForExpiCheck(uint32 nExpiStartTimeMS)	{ m_nExpiCheckStartTimeMS = nExpiStartTimeMS; }
	int					GetExpiRemainTimeSec(uint32 nSystemNowTime);
	
	void				Export(TD_MAIL_MAILBOX_MAIL_INFO& refTD, uint32 nSystemNowTimeMS);

	bool				IsReaded()	{ return m_bIsRead;	}
	void				SetRead()	{ m_bIsRead = true;	}

	bool				IsFilledContent()			{ return m_bFilledContent; }
	void				SetFilledContentForDBTask()	{ m_bFilledContent = true; }

	void				ClearAppendedItems();
	bool				AddAppendedItemForDBTask(GItem* pItem);

	void				ClearAppendedMoney(void);

private:
	int64				m_nMailUID;
	CID					m_nOwnerCID;

	MAIL_TYPE			m_eType;
	bool				m_bIsRead;
	wstring				m_strSender;
	wstring				m_strTitle;
	bool				m_bHasText;
	int					m_nAppendedMoney;			///< 첨부된 돈.
	int					m_nThumbnailItemID;			///< 0 이면 첨부아이템 없음.
	int					m_nExpiDBRemainTimeSec;		///< DB 에서 얻어온 시점의 남은 유효시간. (초)
	uint32				m_nExpiCheckStartTimeMS;	///< 서버의 유효기간 체크 시작 시간. (Millisecond)

	bool				m_bFilledContent;
	std::vector<GItem*>	m_vecAppendedItem;
};
