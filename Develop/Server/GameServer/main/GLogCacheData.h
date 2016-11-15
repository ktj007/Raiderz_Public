#ifndef _GLOG_CACHE_DATA_H
#define _GLOG_CACHE_DATA_H

class GLOG_DATA_ITEM
{
public :
	GLOG_DATA_ITEM() { Reset(); }
	GLOG_DATA_ITEM(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int64 nIUID, int nItemID, int16 nStackAmt, int nModStackAmt, const wstring& strRegDate, int nNpcID)
		: m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nCurMoney(nCurMoney), m_nIUID(nIUID), m_nItemID(nItemID), m_nCurStackAmt(nStackAmt), m_nDeltaStackAmt(nModStackAmt)
		, m_strRegDate(strRegDate) 
	{
		if (0 < nNpcID)
			m_strNpcID.Format(L"%d", nNpcID);
		else
			m_strNpcID = L"NULL";
	}

	void Reset()
	{
		m_nAID				= 0;
		m_nCID				= 0;
		m_nCode				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLevel			= 0;
		m_nCurMoney			= 0;
		m_nIUID				= 0;
		m_nItemID			= 0;
		m_nCurStackAmt		= 0;
		m_nDeltaStackAmt	= 0;
		m_strRegDate		= L"NULL";
		m_strNpcID			= L"NULL";
	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nCurMoney;
	IUID	m_nIUID;
	int		m_nItemID;
	int16	m_nCurStackAmt;
	int16	m_nDeltaStackAmt;
	wstring	m_strRegDate;
	CString m_strNpcID;
};

class GLOG_DATA_MONEY
{
public :
	GLOG_DATA_MONEY() { Reset(); }
	GLOG_DATA_MONEY(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLev, int nCurMoney, int nDeltaMoney, const wstring strRegDate, int nNpcID)
		: m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLev(nLev), m_nCurMoney(nCurMoney), m_nDeltaMoney(nDeltaMoney), m_strRegDate(strRegDate)
	{
		if(0 < nNpcID)
			m_strNpcID.Format(L"%d", nNpcID);
		else
			m_strNpcID = L"NULL";
	}

	void Reset()
	{
		m_nAID			= 0;
		m_nCID			= 0;
		m_nCode			= 0;
		m_nCharPtm		= 0;
		m_nDeltaCharPtm	= 0;
		m_nLev			= 0;
		m_nCurMoney		= 0;
		m_nDeltaMoney	= 0;
		m_strRegDate	= L"NULL";
		m_strNpcID		= L"NULL";
	}
	
	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	int		m_nLev;
	int		m_nCurMoney;
	int		m_nDeltaMoney;
	wstring	m_strRegDate;
	CString	m_strNpcID;
};

class GLOG_DATA_CRAFT
{
public :
	GLOG_DATA_CRAFT() { Reset(); }
	GLOG_DATA_CRAFT(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, int nLevel, int nCurMoney, int nDeltaMoney, int64 nIUID, int nItemID, int16 nCurStackAmt, int16 nDeltaStackAmt, const wstring& strRegDate, int nRefNpcID, int64 nRefIUID)
		: m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nCurMoney(nCurMoney), m_nDeltaMoney(nDeltaMoney), m_nIUID(nIUID), m_nItemID(nItemID), m_nCurStackAmt(nCurStackAmt), m_nDeltaStackAmt(nDeltaStackAmt)
		, m_strRegDate(strRegDate)
	{
		if (0 == nRefNpcID)
			m_strRefNpcID = L"NULL";
		else
			m_strRefNpcID.Format(L"%d", nRefNpcID);

		if (0 == nRefIUID)
			m_strRefIUID = L"NULL";
		else
			m_strRefIUID.Format(L"%I64d", nRefIUID);
	}

	virtual ~GLOG_DATA_CRAFT() {}

	void Reset()
	{
		m_nAID				= 0;
		m_nCID				= 0;
		m_nCode				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLevel			= 0;
		m_nCurMoney			= 0;
		m_nDeltaMoney		= 0;
		m_nIUID				= 0;
		m_nItemID			= 0;
		m_nCurStackAmt		= 0;
		m_nDeltaStackAmt	= 0;
		m_strRegDate		= L"NULL";
		m_strRefNpcID		= L"NULL";
		m_strRefIUID		= L"NULL";
	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nCurMoney;
	int		m_nDeltaMoney;
	int64	m_nIUID;
	int		m_nItemID;
	int16	m_nCurStackAmt;
	int16	m_nDeltaStackAmt;
	wstring	m_strRegDate;
	CString	m_strRefNpcID;
	CString m_strRefIUID;
};

class GLOG_DATA_CRAFT_ITEM
{
public :
	GLOG_DATA_CRAFT_ITEM() { Reset(); }
	GLOG_DATA_CRAFT_ITEM(int64 nAID, int64 nCID, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int64 nIUID, int nItemID, int16 nCurStackAmt, int16 nDeltaStackAmt, const wstring& strRegDate, int64 nRefIUID)
		: m_nAID(nAID), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nIUID(nIUID), m_nItemID(nItemID), m_nCurStackAmt(nCurStackAmt), m_nDeltaStackAmt(nDeltaStackAmt)
		, m_strRegDate(strRegDate), m_nRefIUID(nRefIUID)
	{

	}
	virtual ~GLOG_DATA_CRAFT_ITEM() {}

	void Reset()
	{
		m_nAID				= 0;
		m_nCID				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLevel			= 0;
		m_nIUID				= 0;
		m_nItemID			= 0;
		m_nCurStackAmt		= 0;
		m_nDeltaStackAmt	= 0;
		m_strRegDate		= L"NULL";
		m_nRefIUID			= 0;
	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int64	m_nIUID;
	int		m_nItemID;
	int16	m_nCurStackAmt;
	int16	m_nDeltaStackAmt;
	wstring	m_strRegDate;
	int64	m_nRefIUID;
};

class GLOG_DATA_QUEST_ITEM
{
public :
	GLOG_DATA_QUEST_ITEM() { Reset(); }
	GLOG_DATA_QUEST_ITEM(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int nDeltaMoney, int64 nIUID, int nItemID, int16 nCurStackAmt, int16 nDeltaStackAmt
		, const wstring& strRegDate, int64 nRefMailUID, int nQuestID, int nEventID)
		: m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_strRegDate(strRegDate), m_nQuestID(nQuestID), m_nEventID(nEventID)
	{
		m_strCurMoney.Format(L"%d", nCurMoney);
		m_strDeltaMoney.Format(L"%d", nDeltaMoney);

		if (0 < nIUID)
			m_strIUID.Format(L"%I64d", nIUID);
		else
			m_strIUID = L"NULL";

		if (0 < nItemID && 0 != nDeltaStackAmt)
		{
			m_strItemID.Format(L"%d", nItemID);
			m_strCurStackAmt.Format(L"%d", nCurStackAmt);
			m_strDeltaStackAmt.Format(L"%d", nDeltaStackAmt);
		}
		else 
		{
			m_strItemID			= L"NULL";
			m_strCurStackAmt	= L"NULL";
			m_strDeltaStackAmt	= L"NULL";
		}
		
		if (0 < nRefMailUID)
			m_strRefMailUID.Format(L"%I64d", nRefMailUID);
		else 
			m_strRefMailUID = L"NULL";
	}

	virtual ~GLOG_DATA_QUEST_ITEM() {}

	void Reset()
	{
		m_nAID				= 0;
		m_nCID				= 0;
		m_nCode				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLevel			= 0;
		m_strCurMoney		= L"NULL";
		m_strDeltaMoney		= L"NULL";
		m_strIUID			= L"NULL";
		m_strItemID			= L"NULL";
		m_strCurStackAmt	= L"NULL";
		m_strDeltaStackAmt	= L"NULL";
		m_strRegDate		= L"NULL";
		m_strRefMailUID		= L"NULL";
		m_nQuestID			= 0;
		m_nEventID			= 0;
	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	CString	m_strCurMoney;
	CString	m_strDeltaMoney;
	CString	m_strIUID;
	CString	m_strItemID;
	CString	m_strCurStackAmt;
	CString	m_strDeltaStackAmt;
	wstring	m_strRegDate;
	CString	m_strRefMailUID;
	int		m_nQuestID;
	int		m_nEventID;
};

class GLOG_DATA_ITEM_MERGE_AND_SPLIT
{
public :
	GLOG_DATA_ITEM_MERGE_AND_SPLIT() { Reset(); }
	GLOG_DATA_ITEM_MERGE_AND_SPLIT(int64 nAID, int64 nOwnerID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney, int64 nIUID, int nItemID, int16 nStackAmt, int nModStackAmt, int64 nRefAID, int64 nRefOwnerID, int64 nRefIUID, int16 nRefCurStackAmt)
		: m_nAID(nAID), m_nOwnerID(nOwnerID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney), m_nIUID(nIUID), m_nItemID(nItemID), m_nCurStackAmt(nStackAmt), m_nDeltaStackAmt(nModStackAmt), m_nRefCurStackAmt(nRefCurStackAmt)
		, m_nRefAID(nRefAID), m_nRefOwnerID(nRefOwnerID), m_nRefIUID(nRefIUID)		
	{
		
	}

	void Reset()
	{
		m_nAID				= 0;
		m_nOwnerID			= 0;
		m_nCode				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLevel			= 0;
		m_nMoney			= 0;
		m_nIUID				= 0;
		m_nItemID			= 0;
		m_nCurStackAmt		= 0;
		m_nDeltaStackAmt	= 0;
		m_nRefAID			= 0;
		m_nRefOwnerID		= 0;
		m_nRefIUID			= 0;
		m_nRefCurStackAmt	= 0;
	}

	int64	m_nAID;
	int64	m_nOwnerID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nMoney;
	IUID	m_nIUID;
	int		m_nItemID;
	int16	m_nCurStackAmt;
	int16	m_nDeltaStackAmt;
	int64	m_nRefAID;
	int64	m_nRefOwnerID;
	int64	m_nRefIUID;
	int16	m_nRefCurStackAmt;
};

class GLOG_DATA_ITEM_MOVE
{
public :
	GLOG_DATA_ITEM_MOVE() { Reset(); }
	GLOG_DATA_ITEM_MOVE(int64 nAID, int64 nOwnerID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLev, int nCurMoney, int64 nIUID, int nItemID, int16 nCurStackAmt
		, int64 nRefAID, int64 nRefOwnerID)
		: m_nAID(nAID), m_nOwnerID(nOwnerID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLev(nLev), m_nCurMoney(nCurMoney)
		, m_nIUID(nIUID), m_nItemID(nItemID), m_nCurStackAmt(nCurStackAmt), m_nRefAID(nRefAID), m_nRefOwnerID(nRefOwnerID) {}

	void Reset()
	{
		m_nAID				= 0;
		m_nOwnerID			= 0;
		m_nCode				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLev				= 0;
		m_nCurMoney			= 0;
		m_nIUID				= 0;
		m_nItemID			= 0;
		m_nCurStackAmt		= 0;
		m_nRefAID			= 0;
		m_nRefOwnerID		= 0;
	}

	int64	m_nAID;
	int64	m_nOwnerID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLev;
	int		m_nCurMoney;
	int64	m_nIUID;
	int		m_nItemID;
	int16	m_nCurStackAmt;
	int64	m_nRefAID;
	int64	m_nRefOwnerID;
};

class GLOG_DATA_NPC_SHOP
{
public :
	GLOG_DATA_NPC_SHOP() { Reset(); }
	GLOG_DATA_NPC_SHOP(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int nDeltaMoney, int64 nIUID, int nItemID, int16 nCurStackAmt, int16 nDeltaStackAmt, const wstring& strRegDate)
		: m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_strRegDate(strRegDate)
	{
		m_strCurMoney.Format(L"%d", nCurMoney);
		m_strDeltaMoney.Format(L"%d", nDeltaMoney);

		if (0 < nIUID)
			m_strIUID.Format(L"%I64d", nIUID);
		else
			m_strIUID = L"NULL";

		if (0 < nItemID && 0 != nDeltaStackAmt)
		{
			m_strItemID.Format(L"%d", nItemID);
			m_strCurStackAmt.Format(L"%d", nCurStackAmt);
			m_strDeltaStackAmt.Format(L"%d", nDeltaStackAmt);
		}
		else 
		{
			m_strItemID			= L"NULL";
			m_strCurStackAmt	= L"NULL";
			m_strDeltaStackAmt	= L"NULL";
		}		
	}

	virtual ~GLOG_DATA_NPC_SHOP() {}

	void Reset()
	{
		m_nAID				= 0;
		m_nCID				= 0;
		m_nCode				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLevel			= 0;
		m_strCurMoney		= L"NULL";
		m_strDeltaMoney		= L"NULL";
		m_strIUID			= L"NULL";
		m_strItemID			= L"NULL";
		m_strCurStackAmt	= L"NULL";
		m_strDeltaStackAmt	= L"NULL";
		m_strRegDate		= L"NULL";
	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	CString	m_strCurMoney;
	CString	m_strDeltaMoney;
	CString	m_strIUID;
	CString	m_strItemID;
	CString	m_strCurStackAmt;
	CString	m_strDeltaStackAmt;
	wstring	m_strRegDate;
};

class GLOG_DATA_XP
{
public :
	GLOG_DATA_XP() { Reset(); }
	GLOG_DATA_XP(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurXP, int nDeltaXP, const wstring& strRegDate, int nNpcID)
		: m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nCurXP(nCurXP), m_nDeltaXP(nDeltaXP), m_strRegDate(strRegDate)
	{
		if (0 < nNpcID)
			m_strNpcID.Format(L"%d", nNpcID);
		else 
			m_strNpcID = L"NULL";
	}
	virtual ~GLOG_DATA_XP() {}

	void Reset()
	{
		m_nAID			= 0;
		m_nCID			= 0;
		m_nCode			= 0;
		m_nCharPtm		= 0;
		m_nDeltaCharPtm	= 0;
		m_nLevel		= 0;
		m_nCurXP		= 0;
		m_nDeltaXP		= 0;
		m_strRegDate	= L"NULL";
		m_strNpcID		= L"NULL";

	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nCurXP;
	int		m_nDeltaXP;
	wstring	m_strRegDate;
	CString	m_strNpcID;
};

class GLOG_DATA_ITEM_DELETE
{
public :
	GLOG_DATA_ITEM_DELETE() { Reset(); }
	GLOG_DATA_ITEM_DELETE(int64 nAID, int64 nCID, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int64 nIUID, int nItemID, int16 nDeltaStackAmt, uint8 nMaxDura, int nUsagePeriod, const wstring& strEffEndDate, const wstring& strRegDate)
		: m_nAID(nAID), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nCurMoney(nCurMoney), m_nIUID(nIUID), m_nItemID(nItemID), m_nDeltaStackAmt(nDeltaStackAmt), m_nMaxDura(nMaxDura)
		, m_strEffEndDate(strEffEndDate), m_strRegDate(strRegDate) 
	{
		if (0 >= nUsagePeriod)
			m_strUsagePeriod = L"NULL";
		else 
			m_strUsagePeriod.Format(L"%d", nUsagePeriod);		
	}

	void Reset()
	{
		m_nAID				= 0;
		m_nCID				= 0;
		m_nCharPtm			= 0;
		m_nDeltaCharPtm		= 0;
		m_nLevel			= 0;
		m_nCurMoney			= 0;
		m_nIUID				= 0;
		m_nItemID			= 0;
		m_nDeltaStackAmt	= 0;
		m_nMaxDura			= 0;
		m_strUsagePeriod	= L"NULL";
		m_strEffEndDate		= L"NULL";
		m_strRegDate		= L"NULL";
		
	}

	int64	m_nAID;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nCurMoney;
	int64	m_nIUID;
	int		m_nItemID;
	int16	m_nDeltaStackAmt;
	uint8	m_nMaxDura;
	CString	m_strUsagePeriod;
	wstring m_strEffEndDate;
	wstring m_strRegDate;
};

class GLOG_DATA_PARTY
{
public :
	GLOG_DATA_PARTY() { Reset(); }
	GLOG_DATA_PARTY(const MUID& uidParty, int64 nAID, int64 nCID, int nCode, const wstring& strRegDate, uint8 nType)
		: m_uidParty(uidParty), m_nAID(nAID), m_nCID(nCID), m_nCode(nCode), m_strRegDate(strRegDate), m_nType(nType)
	{

	}

	void Reset()
	{
		m_uidParty		= 0;
		m_nAID			= 0;
		m_nCID			= 0;
		m_nCode			= 0;
		m_strRegDate	= L"NULL";
		m_nType			= 0;
	}

	MUID	m_uidParty;
	int64	m_nAID;
	int64	m_nCID;
	int		m_nCode;
	wstring m_strRegDate;
	uint8	m_nType;
};

#endif