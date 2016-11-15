#pragma once

class GDBT_QPER_ITEM
{
public:
	GDBT_QPER_ITEM() : m_nIUID(0), m_nItemID(0), m_nSlotID(0), m_nStackAmt(0), m_nModStackAmt(0), m_nMaxDura(0), m_nColor(0), m_bClaimed(0)
 {}
	GDBT_QPER_ITEM(IUID nIUID, int nItemID, int16 nSlotID, int16 nStackAmt, int16 nModStackAmt, uint8 nMaxDura, int nColor=0, bool bClaimed=false)
		:	m_nIUID(nIUID), m_nItemID(nItemID), m_nSlotID(nSlotID), m_nStackAmt(nStackAmt), m_nModStackAmt(nModStackAmt), 
		m_nMaxDura(nMaxDura), m_nColor(nColor), m_bClaimed(bClaimed) {}

	void Set(const GDBT_QPER_ITEM& qi)
	{
		m_nIUID				= qi.m_nIUID;
		m_nItemID			= qi.m_nItemID;
		m_nSlotID			= qi.m_nSlotID;
		m_nStackAmt			= qi.m_nStackAmt;
		m_nModStackAmt		= qi.m_nModStackAmt;
		m_nMaxDura			= qi.m_nMaxDura;
		m_nColor			= qi.m_nColor;
		m_bClaimed			= qi.m_bClaimed;
	}

	IUID	m_nIUID;
	int		m_nItemID;
	int16	m_nSlotID;
	int16	m_nStackAmt;
	int16	m_nModStackAmt;
	uint8	m_nMaxDura;
	int		m_nColor;
	bool	m_bClaimed;
};

class GDBT_QPER_TOINVEN
{
public:
	GDBT_QPER_TOINVEN() {}
	GDBT_QPER_TOINVEN(int64 nAID, MUID uidPlayer, int64 nCID, int nCharPtm, int nDeltaCharPtm, int nXP, int nDeltaXP, int nLevel, int nDeltaLevel, int nMoney, int nDeltaMoney, const vector<GDBT_QPER_ITEM>& vecItem, int nEventID, int nFieldID, vec3& vPos)
	: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nXP(nXP), m_nDeltaXP(nDeltaXP), m_nLevel(nLevel), m_nDeltaLevel(nDeltaLevel), m_nMoney(nMoney), m_nDeltaMoney(nDeltaMoney), m_vecItem(vecItem)
	, m_nEventID(nEventID), m_nFieldID(nFieldID), m_vPos(vPos) {}

	bool IsValid()
	{
		return !m_vecItem.empty();
	}

	void Set(GDBT_QPER_TOINVEN& data)
	{
		m_nAID			= data.m_nAID;
		m_uidPlayer		= data.m_uidPlayer;
		m_nCID			= data.m_nCID;
		m_nCharPtm		= data.m_nCharPtm;
		m_nDeltaCharPtm	= data.m_nDeltaCharPtm;
		m_nXP			= data.m_nXP;
		m_nDeltaXP		= data.m_nDeltaXP;
		m_nLevel		= data.m_nLevel;
		m_nDeltaLevel	= data.m_nDeltaLevel;
		m_nMoney		= data.m_nMoney;
		m_nDeltaMoney	= data.m_nDeltaMoney;
		m_nEventID		= data.m_nEventID;
		m_nFieldID		= data.m_nFieldID;
		m_vPos			= data.m_vPos;
		

		m_vecItem.swap(data.m_vecItem);
	}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	int		m_nXP;
	int		m_nDeltaXP;
	int		m_nLevel;
	int		m_nDeltaLevel;
	int		m_nMoney;
	int		m_nDeltaMoney;
	int		m_nEventID;
	int		m_nFieldID;
	vec3	m_vPos;

	vector<GDBT_QPER_ITEM>		m_vecItem;	
	vector<pair<int16, int64>>	m_vecRewardItemIUID;	// db에서 받아와서 저장함.
};


class GDBT_QPER_TOMAIL
{
public:
	GDBT_QPER_TOMAIL() {}
	GDBT_QPER_TOMAIL(int64 nAID, MUID uidPlayer, int64 nCID, int nCharPtm, int nDeltaCharPtm, int nXP, int nDeltaXP, int nLevel, int nDeltaLevel, int nMoney, int nDeltaMoney
		, wstring strSender, wstring strTitle, int nRemainDeleteSeconds, int nDefaultItemID, const vector<GDBT_QPER_ITEM>& vecItem
		, int nEventID, int nFieldID, const vec3& vPos)
	: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nXP(nXP), m_nDeltaXP(nDeltaXP), m_nLevel(nLevel), m_nDeltaLevel(nDeltaLevel), m_nMoney(nMoney), m_nDeltaMoney(nDeltaMoney), m_strSender(strSender), m_strTitle(strTitle), m_nRemainDeleteSeconds(nRemainDeleteSeconds), m_nDefaultItemID(nDefaultItemID), m_vecItem(vecItem),
	m_nMailUID(0), m_nEventID(nEventID), m_nFieldID(nFieldID), m_vPos(vPos) {}

	bool IsValid()
	{
		return !m_vecItem.empty();
	}

	void Set(GDBT_QPER_TOMAIL& data)
	{
		m_nAID					= data.m_nAID;
		m_uidPlayer				= data.m_uidPlayer;
		m_nCID					= data.m_nCID;
		m_nCharPtm				= data.m_nCharPtm;
		m_nDeltaCharPtm			= data.m_nDeltaCharPtm;
		m_nXP					= data.m_nXP;
		m_nDeltaXP				= data.m_nDeltaXP;
		m_nLevel				= data.m_nLevel;
		m_nDeltaLevel			= data.m_nDeltaLevel;
		m_nMoney				= data.m_nMoney;
		m_nDeltaMoney			= data.m_nDeltaMoney;
		m_nEventID				= data.m_nEventID;
		m_nFieldID				= data.m_nFieldID;
		m_vPos					= data.m_vPos;

		m_nRemainDeleteSeconds	= data.m_nRemainDeleteSeconds;
		m_nDefaultItemID		= data.m_nDefaultItemID;

		m_strTitle.swap(data.m_strTitle);
		m_strSender.swap(data.m_strSender);
	}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	int		m_nXP;
	int		m_nDeltaXP;
	int		m_nLevel;
	int		m_nDeltaLevel;
	int		m_nMoney;
	int		m_nDeltaMoney;
	int		m_nEventID;
	int		m_nFieldID;
	vec3	m_vPos;

	wstring	m_strSender;
	wstring	m_strTitle;
	int		m_nRemainDeleteSeconds;
	int		m_nDefaultItemID;

	vector<GDBT_QPER_ITEM>	m_vecItem;

	int64	m_nMailUID;
};
