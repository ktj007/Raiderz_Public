#include "StdAfx.h"
#include "GMail.h"
#include "CTransData.h"
#include "GItem.h"
#include "AStlUtil.h"


GMail::GMail(int64 nMailUID)
: m_nMailUID(nMailUID)
, m_nOwnerCID(0)
, m_eType(MT_PLAYER)
, m_bIsRead(false)
, m_bHasText(false)
, m_nAppendedMoney(0)
, m_nThumbnailItemID(0)
, m_nExpiDBRemainTimeSec(0)
, m_nExpiCheckStartTimeMS(0)
, m_vecAppendedItem(MAX_MAIL_APPENDED_ITEM_COUNT)
, m_bFilledContent(false)
{
}

GMail::~GMail(void)
{
	SAFE_DELETE_VECTOR(m_vecAppendedItem);
}

bool GMail::Init(CID nOwnerCID
				 , MAIL_TYPE eType
				 , bool bIsRead
				 , wstring strSender
				 , wstring strTitle
				 , bool bHasText
				 , int nAppendedMoney
				 , int nAppendedItemID
				 , int nExpiDBRemainTimeSec)
{
	VALID_RET(nOwnerCID > 0, false);
	VALID_RET(nAppendedMoney >= 0, false);
	VALID_RET(nExpiDBRemainTimeSec > 0, false);

	m_nOwnerCID = nOwnerCID;
	m_eType = eType;
	m_bIsRead = bIsRead;
	m_strSender = strSender;
	m_strTitle = strTitle;
	m_bHasText = bHasText;
	m_nExpiDBRemainTimeSec = nExpiDBRemainTimeSec;
	m_nAppendedMoney = nAppendedMoney;
	m_nThumbnailItemID = nAppendedItemID;
	return true;
}

GItem* GMail::GetAppendedItem(int nSlotID)
{
	VALID_RET(nSlotID >= 0, NULL);
	VALID_RET(nSlotID < MAX_MAIL_APPENDED_ITEM_COUNT, NULL);
	VALID_RET(m_vecAppendedItem.size() == MAX_MAIL_APPENDED_ITEM_COUNT, NULL);

	return m_vecAppendedItem[nSlotID];
}

void GMail::DelAppendedItem(int nSlotID)
{
	VALID(nSlotID >= 0);
	VALID(nSlotID < MAX_MAIL_APPENDED_ITEM_COUNT);
	VALID(m_vecAppendedItem.size() == MAX_MAIL_APPENDED_ITEM_COUNT);

	SAFE_DELETE(m_vecAppendedItem[nSlotID]);
}

int GMail::GetNewThumbnailItemID(int nIgnoreSlotID)
{
	for (size_t i = 0; i < m_vecAppendedItem.size(); ++i)
	{
		GItem* pItem = m_vecAppendedItem[i];
		if (pItem == NULL)
			continue;

		if (pItem->m_nSlotID != nIgnoreSlotID)
			return pItem->m_pItemData->m_nID;
	}
	return 0;
}

int GMail::GetExpiRemainTimeSec(uint32 nSystemNowTime)
{
	uint32 nDelta = nSystemNowTime - m_nExpiCheckStartTimeMS;
	if (nDelta < 0)
		return 0;

	int nExpiRemainTimeSec = m_nExpiDBRemainTimeSec - static_cast<int>((float)nDelta * 0.001f);
	return nExpiRemainTimeSec;
}

void GMail::Export(TD_MAIL_MAILBOX_MAIL_INFO& refTD, uint32 nSystemNowTimeMS)
{
	refTD.uidMail = m_nMailUID;
	refTD.nType = m_eType;
	refTD.bIsRead = m_bIsRead;
	wcsncpy_s(refTD.strSenderName , m_strSender.c_str(), _TRUNCATE);
	wcsncpy_s(refTD.strMailTitle, m_strTitle.c_str(), _TRUNCATE);
	refTD.nAppendedItemID = m_nThumbnailItemID;
	refTD.bHasText = m_bHasText;
	refTD.nAppendedMoney = m_nAppendedMoney;

	int nExpiRemainTimeSec = GetExpiRemainTimeSec(nSystemNowTimeMS);
	refTD.nExpiringTimeSec = (nExpiRemainTimeSec < 0 ? 0 : nExpiRemainTimeSec);
}

void GMail::ClearAppendedItems()
{
	for (size_t i = 0; i < m_vecAppendedItem.size(); ++i)
		SAFE_DELETE(m_vecAppendedItem[i]);
}

bool GMail::AddAppendedItemForDBTask(GItem* pItem)
{
	VALID_RET(pItem != NULL, false);
	VALID_RET(m_vecAppendedItem.size() == MAX_MAIL_APPENDED_ITEM_COUNT, false);
	VALID_RET(pItem->m_nSlotID >= 0, false);
	VALID_RET(pItem->m_nSlotID < MAX_MAIL_APPENDED_ITEM_COUNT, false);
	VALID_RET(pItem->m_nSlotType == SLOTTYPE_MAIL , false);

	if (m_vecAppendedItem[pItem->m_nSlotID] != NULL)
	{
		mlog("Error! GMail::AddAppendedItemForDBTask(Item), Slot is Not NULL! (MailUID=%I64d, SlotID=%d)\n", m_nMailUID, pItem->m_nSlotID);
		return false;
	}

	m_vecAppendedItem[pItem->m_nSlotID] = pItem;

	return true;
}

void GMail::ClearAppendedMoney(void)
{
	m_nAppendedMoney = 0;
}
