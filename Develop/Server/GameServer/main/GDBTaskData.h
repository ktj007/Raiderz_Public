#ifndef _GDBTASK_DATA_H
#define _GDBTASK_DATA_H


#include "CSChrInfo.h"
#include "CSPalette.h"


struct GDBTASKDATA_LOOTINCREASEMONEY
{
	int64	nAID;
	MUID	uidPlayer;	
	int		nModMoney;
	int		nMoney;	
	int		nCharPlayTime;
	int		nDeltaCharPtm;
	uint8	nLevel;
	int		nXP;

	int64	nCID;

	int		nNpcID;

	GDBTASKDATA_LOOTINCREASEMONEY(int64 nAID, MUID	uidPlayer, int nModMoney, int nMoney, int nCharPlayTime, int nDeltaCharPtm, uint8 nLevel, int nXP, int64 nCID, int nNpcID)
	: nAID(nAID), uidPlayer(uidPlayer), nModMoney(nModMoney), nMoney(nMoney), nCharPlayTime(nCharPlayTime), nDeltaCharPtm(nDeltaCharPtm), nLevel(nLevel), nXP(nXP), nCID(nCID), nNpcID(nNpcID)
	{

	}

	GDBTASKDATA_LOOTINCREASEMONEY& operator=(const GDBTASKDATA_LOOTINCREASEMONEY& data)
	{		
		nAID			= data.nAID;
		uidPlayer		= data.uidPlayer;
		nModMoney		= data.nModMoney;
		nMoney			= data.nMoney;
		nCharPlayTime	= data.nCharPlayTime;
		nDeltaCharPtm	= data.nDeltaCharPtm;
		nLevel			= data.nLevel;
		nXP				= data.nXP;

		nCID			= data.nCID;

		nNpcID			= data.nNpcID;

		return (*this);
	}
};

//////////////////////////////////////////////////////////////////////////
/// 우편
struct GDBT_MAIL_GET_MAILBOX_LIST
{
	MUID	m_uidPlayer;
	int64	m_nAID;
	int64	m_nCID;
	int64	m_nTopMUID;			///< 가져올 편지들 중 시작 편지의 MUID (가져올 편지에 포함됨)
	int64	m_nBottomMUID;		///< 가져올 편지들의 최소 MUID 바운더리 (가져올 편지에 포함되지 않음)
	int		m_nReqMailCount;	///< 얻어올 메일 개수

	GDBT_MAIL_GET_MAILBOX_LIST(MUID	uidPlayer, int64 nAID, int64 nCID, int64 nTopMUID, int64 nBottomMUID, int nReqMailCount)
	: m_uidPlayer(uidPlayer), m_nAID(nAID), m_nCID(nCID)
	, m_nTopMUID(nTopMUID), m_nBottomMUID(nBottomMUID), m_nReqMailCount(nReqMailCount)
	{

	}

	// Unittest에서 사용
	bool operator == (const GDBT_MAIL_GET_MAILBOX_LIST& other) const
	{
		if (m_uidPlayer != other.m_uidPlayer)	return false;
		if (m_nAID != other.m_nAID)	return false;
		if (m_nCID != other.m_nCID)	return false;
		if (m_nTopMUID != other.m_nTopMUID)	return false;
		if (m_nBottomMUID != other.m_nBottomMUID)	return false;
		if (m_nReqMailCount != other.m_nReqMailCount)	return false;

		return true;
	}
};

struct GDBT_MAIL_APPENDED_ITEM
{
	int		m_nSlotID;
	int		m_nItemID;
	IUID	m_nItemUID;

	int16	m_nStackAmount;
	int16	m_nDeltaStackAmount;
	int8	m_nDuration;
	int		m_nPeriod;				// 0: 무제한 아이템, 1: 기간제 아이템

	GDBT_MAIL_APPENDED_ITEM()
		: m_nSlotID(INVALID_ID)
		, m_nItemID(0)
		, m_nItemUID(0)
		, m_nStackAmount(0)
		, m_nDeltaStackAmount(0)
		, m_nDuration(0)
		, m_nPeriod(0)
	{
		// do nothing
	}

	GDBT_MAIL_APPENDED_ITEM(const GDBT_MAIL_APPENDED_ITEM& ref)
	{
		m_nSlotID = ref.m_nSlotID;
		m_nItemID = ref.m_nItemID;
		m_nItemUID = ref.m_nItemUID;

		m_nStackAmount = ref.m_nStackAmount;
		m_nDeltaStackAmount = ref.m_nDeltaStackAmount;
		m_nDuration = ref.m_nDuration;
		m_nPeriod = ref.m_nPeriod;
	}

	// Unittest에서 사용
	bool operator == (const GDBT_MAIL_APPENDED_ITEM& other) const
	{
		if (m_nSlotID != other.m_nSlotID)	return false;
		if (m_nItemID != other.m_nItemID)	return false;
		if (m_nItemUID != other.m_nItemUID)	return false;
		
		if (m_nStackAmount != other.m_nStackAmount)	return false;
		if (m_nDeltaStackAmount != other.m_nDeltaStackAmount)	return false;
		if (m_nDuration != other.m_nDuration)	return false;
		if (m_nPeriod != other.m_nPeriod)	return false;

		return true;
	}

	bool operator != (const GDBT_MAIL_APPENDED_ITEM& other) const
	{
		return !(*this == other);
	}
};

struct GDBT_MAIL_WRITE
{
	int64		m_nSenderAID;
	int64		m_nSenderCID;
	wstring		m_strSenderName;
	int			m_nSenderCharacterPlaySeconds;	
	int			m_nDeltaCharPtm;
	uint8		m_nLevel;
	int			m_nMoney;
	int			m_nDeltaMoney;
	int			m_nPostage;

	wstring		m_strReceiverName;
	
	MAIL_TYPE	m_nMailType;
	wstring		m_strTitle;
	bool		m_hasText;			// 0: 내용 없음, 1: 내용 있음
	wstring		m_strText;
	int			m_nRemainDeleteSeconds;
	int			m_nDefaultItemID;
	
	GDBT_MAIL_APPENDED_ITEM	m_appendedItemSlot[MAX_MAIL_APPENDED_ITEM_COUNT];
	int8					m_nAppendedItemCount;


	GDBT_MAIL_WRITE()	{}
	GDBT_MAIL_WRITE(const GDBT_MAIL_WRITE& ref)
	{
		m_nSenderAID = ref.m_nSenderAID;
		m_nSenderCID = ref.m_nSenderCID;
		m_strSenderName = ref.m_strSenderName;
		m_nSenderCharacterPlaySeconds = ref.m_nSenderCharacterPlaySeconds;
		m_nDeltaCharPtm = ref.m_nDeltaCharPtm;
		m_nLevel = ref.m_nLevel;
		m_nMoney = ref.m_nMoney;
		m_nDeltaMoney = ref.m_nDeltaMoney;
		m_nPostage = ref.m_nPostage;

		m_strReceiverName = ref.m_strReceiverName;

		m_nMailType = ref.m_nMailType;
		m_strTitle = ref.m_strTitle;
		m_hasText = ref.m_hasText;
		m_strText = ref.m_strText;
		m_nRemainDeleteSeconds = ref.m_nRemainDeleteSeconds;
		m_nDefaultItemID = ref.m_nDefaultItemID;

		for(int i = 0; i < MAX_MAIL_APPENDED_ITEM_COUNT; i++)
		{
			m_appendedItemSlot[i] = ref.m_appendedItemSlot[i];
		}
		
		m_nAppendedItemCount = ref.m_nAppendedItemCount;
	}


	// Unittest에서 사용
	bool operator == (const GDBT_MAIL_WRITE& other) const
	{
		if (m_nSenderAID != other.m_nSenderAID) return false;
		if (m_nSenderCID != other.m_nSenderCID)	return false;
		if (m_strSenderName != other.m_strSenderName)	return false;		
		if (m_nSenderCharacterPlaySeconds != other.m_nSenderCharacterPlaySeconds)	return false;
		if (m_nDeltaCharPtm != other.m_nDeltaCharPtm) return false;
		if (m_nLevel != other.m_nLevel) return false;
		if (m_nMoney != other.m_nMoney)	return false;
		if (m_nDeltaMoney != other.m_nDeltaMoney) return false;

		if (m_strReceiverName != other.m_strReceiverName)	return false;

		if (m_nMailType != other.m_nMailType)	return false;
		if (m_strTitle != other.m_strTitle)	return false;
		if (m_hasText != other.m_hasText)	return false;
		if (m_nRemainDeleteSeconds != other.m_nRemainDeleteSeconds)	return false;
		if (m_nDefaultItemID != other.m_nDefaultItemID)	return false;

		if (m_nAppendedItemCount != other.m_nAppendedItemCount)	return false;

		for(int i = 0; i < min(m_nAppendedItemCount, MAX_MAIL_APPENDED_ITEM_COUNT); i++)
		{
			if (m_appendedItemSlot[i] != other.m_appendedItemSlot[i])	return false;
		}

		return true;
	}
};

struct GDBT_MAIL_WRITE_RESULT	
{
	int64	m_nAID;
	int64	m_nCID;
	int64	m_nMailUID;
	
	IUID	m_nItemUID[MAX_MAIL_APPENDED_ITEM_COUNT];
	IUID	m_nNewItemUID[MAX_MAIL_APPENDED_ITEM_COUNT];
	int8	m_nResultItemCount;

	IUID GetNewIUID(int64 nIUID) const
	{
		if (0 == nIUID)
			return 0;

		for (int i = 0; i < MAX_MAIL_APPENDED_ITEM_COUNT; i++)
		{
			if (m_nItemUID[i] == nIUID)
				return m_nNewItemUID[i];
		}

		return 0;
	}
};

struct GDBT_MAIL_CHECK_RECEIVER
{
	int64			m_nSenderCID;
	GPlayerGrade	m_nSenderPlayerGrade;
	wstring			m_strReceiverName;
	int				m_nMaxMailCount;
};

struct GDBT_MAIL_DELETE
{
	int64	m_nAID;
	int64	m_nCID;
	int		m_nCharPtm;
	int64	m_nMailUID;

	bool IsValid()
	{
		if (m_nAID <= 0 ||
			m_nCID <= 0 ||
			m_nCharPtm <= 0 ||
			m_nMailUID <= 0)
			return false;
		return true;
	}

	bool operator == (const GDBT_MAIL_DELETE& other) const
	{
		if (m_nAID != other.m_nAID) return false;
		if (m_nCID != other.m_nCID)	return false;
		if (m_nCharPtm != other.m_nCharPtm) return false;
		if (m_nMailUID != other.m_nMailUID)	return false;

		return true;
	}
};

struct GDBT_MAIL_GET_ITEM
{
	MUID	m_uidPlayer;
	int64	m_nAID;
	int64	m_nCID;
	int64	m_nMailUID;
	int		m_nMailSlotID;
	int64	m_nIUID;
	int		m_nItemID;
	int		m_InvenSlotID;
	int		m_nNewMailDefaultItemID;
	int		m_nCharPlayTime;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nModStackAmt;
	int		m_nMoney;
	
	bool operator == (const GDBT_MAIL_GET_ITEM& other) const
	{
		if (m_uidPlayer != other.m_uidPlayer) return false;
		if (m_nAID != other.m_nAID) return false;
		if (m_nCID != other.m_nCID)	return false;
		if (m_nMailUID != other.m_nMailUID)	return false;
		if (m_nMailSlotID != other.m_nMailSlotID)	return false;
		if (m_nIUID != other.m_nIUID)	return false;
		if (m_nItemID != other.m_nItemID) return false;
		if (m_InvenSlotID != other.m_InvenSlotID)	return false;
		if (m_nNewMailDefaultItemID != other.m_nNewMailDefaultItemID)	return false;
		if (m_nCharPlayTime != other.m_nCharPlayTime)	return false;
		if (m_nDeltaCharPtm != other.m_nDeltaCharPtm) return false;
		if (m_nLevel != other.m_nLevel) return false;
		if (m_nModStackAmt != other.m_nModStackAmt) return false;		
		if (m_nMoney != other.m_nMoney) return false;

		return true;
	}
};

struct GDBT_MAIL_GET_MONEY
{
	int64	m_nAID;
	int64	m_nCID;
	int64	m_nMailUID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nAppendedMoney;
	int		m_nResultMoney;

	GDBT_MAIL_GET_MONEY()	{}

	GDBT_MAIL_GET_MONEY(const GDBT_MAIL_GET_MONEY& ref)
	{
		m_nAID = ref.m_nAID;
		m_nCID = ref.m_nCID;
		m_nMailUID = ref.m_nMailUID;
		m_nCharPtm = ref.m_nCharPtm;
		m_nDeltaCharPtm = ref.m_nDeltaCharPtm;
		m_nLevel = ref.m_nLevel;
		m_nAppendedMoney = ref.m_nAppendedMoney;
		m_nResultMoney = ref.m_nResultMoney;
	}

	bool operator == (const GDBT_MAIL_GET_MONEY& other) const
	{
		if (m_nAID != other.m_nAID)						return false;
		if (m_nCID != other.m_nCID)						return false;
		if (m_nMailUID != other.m_nMailUID)				return false;
		if (m_nCharPtm != other.m_nCharPtm)				return false;
		if (m_nDeltaCharPtm != other.m_nDeltaCharPtm)	return false;
		if (m_nLevel != other.m_nLevel)					return false;
		if (m_nAppendedMoney != other.m_nAppendedMoney)	return false;
		if (m_nResultMoney != other.m_nResultMoney)		return false;

		return true;
	}
};

#endif
