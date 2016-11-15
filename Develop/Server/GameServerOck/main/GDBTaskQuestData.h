#ifndef _GDBTASK_QUEST_DATA_H
#define _GDBTASK_QUEST_DATA_H


#include "GITEM_STACK_AMT.h"
#include "CSQuestInfo.h"

class GDBT_QUEST_ADDITEM;
struct QUEST_REWARD;

class GDBT_QUEST_COMMON
{
public :
	GDBT_QUEST_COMMON() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nSlotID(0), m_nQuestID(0), m_nCharPtm(0), m_nLevel(0), m_nXP(0) {}
	GDBT_QUEST_COMMON(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nSlotID, const int nQuestID, const int nCharPtm, int nDeltaCharPtm, const uint8 nLevel, int nXP
		, const wstring& strAcceptDt, const int nFieldID) 
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nSlotID(nSlotID), m_nQuestID(nQuestID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm)
		, m_nLevel(nLevel), m_nXP(nXP), m_strAcceptDt(strAcceptDt), m_nFieldID(nFieldID) {}

	int64			m_nAID;
	MUID			m_uidPlayer;
	int64			m_nCID;
	uint8			m_nSlotID;
	int				m_nQuestID;
	int				m_nCharPtm;
	int				m_nDeltaCharPtm;
	uint8			m_nLevel;
	int				m_nXP;
	wstring			m_strAcceptDt;
	int				m_nFieldID;
};

class GDBT_QEUST_DONE
{
public :
	GDBT_QEUST_DONE() : m_nIncXP(0), m_nMoney(0), m_nIncMoney(0), m_nNPCUID(MUID::ZERO) {}
	GDBT_QEUST_DONE(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nLevel, const int nDeltaLevel, const uint8 nSlotID, const int nQID, QUEST_REWARD selRw
		, const int nXP, const int nIncXP, const int nMoney, const int nIncMoney, const wstring& strAcceptDt, const wstring& strExpiDt, const int nCharPtm, int nDeltaCharPtm, MUID nNPCUID, int nFieldID, const vec3& vPos) 
		: m_QuestComm(nAID, uidPlayer, nCID, nSlotID, nQID, nCharPtm, nDeltaCharPtm, nLevel, nXP, strAcceptDt, nFieldID)
		, m_SelectedReward(selRw), m_nDeltaLevel(nDeltaLevel), m_nIncXP(nIncXP), m_nMoney(nMoney), m_nIncMoney(nIncMoney), m_strExpiDt(strExpiDt), m_nNPCUID(nNPCUID), m_vPos(vPos) {}

	void Set(GDBT_QEUST_DONE& data)
	{
		m_QuestComm			= data.m_QuestComm;
		m_SelectedReward	= data.m_SelectedReward;
		m_nDeltaLevel		= data.m_nDeltaLevel;
		m_nIncXP			= data.m_nIncXP;
		m_nMoney			= data.m_nMoney;
		m_nIncMoney			= data.m_nIncMoney;
		m_strExpiDt			= data.m_strExpiDt;
		m_nNPCUID			= data.m_nNPCUID;
		m_vPos				= data.m_vPos;

		m_vecAddItem.swap(data.m_vecAddItem);
		m_vecRemoveItem.swap(data.m_vecRemoveItem);
	}

	GDBT_QUEST_COMMON	m_QuestComm;

	QUEST_REWARD		m_SelectedReward;
	uint8				m_nDeltaLevel;
	int					m_nIncXP;
	int					m_nMoney;
	int					m_nIncMoney;
	wstring				m_strExpiDt;
	MUID				m_nNPCUID;
	vec3				m_vPos;

	vector<GDBT_QUEST_ADDITEM>	m_vecAddItem;
	GITEM_STACK_AMT_VEC			m_vecRemoveItem;
	map<int, int64>				m_mapIUID;	
};

class GDBT_QUEST_GIVEUP
{
public :
	GDBT_QUEST_GIVEUP() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nLevel(0), m_nSlotID(0), m_nQuestID(0), m_nCharPtm(0), m_nFieldID(0) {}
	GDBT_QUEST_GIVEUP(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nLevel, const uint8 nSlotID, const int nQuestID, const int nCharPtm, int nDeltaCharPtm
		, const wstring& strAccpDate, const int nFieldID)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nLevel(nLevel), m_nSlotID(nSlotID), m_nQuestID(nQuestID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm)
		, m_strAcceptDt(strAccpDate), m_nFieldID(nFieldID) {}

	void Set(GDBT_QUEST_GIVEUP& data)
	{
		m_nAID			= data.m_nAID;
		m_uidPlayer		= data.m_uidPlayer;
		m_nCID			= data.m_nCID;
		m_nLevel		= data.m_nLevel;
		m_nSlotID		= data.m_nSlotID;
		m_nQuestID		= data.m_nQuestID;
		m_nCharPtm		= data.m_nCharPtm;
		m_nDeltaCharPtm	= data.m_nDeltaCharPtm;
		m_strAcceptDt	= data.m_strAcceptDt;
		m_nFieldID		= data.m_nFieldID;

		m_vecQItems.swap(data.m_vecQItems);
	}

	int64				m_nAID;
	MUID				m_uidPlayer;
	int64				m_nCID;
	int					m_nLevel;	
	uint8				m_nSlotID;
	int					m_nQuestID;
	int					m_nCharPtm;
	int					m_nDeltaCharPtm;	
	wstring				m_strAcceptDt;
	int					m_nFieldID;
	GITEM_STACK_AMT_VEC m_vecQItems;
};

class GDBT_QUEST_ACCEPT
{
public :
	GDBT_QUEST_ACCEPT() : m_bChallange(false), m_nMoney(0), m_bComplete(false), m_uidNPC(MUID::ZERO) {}
	GDBT_QUEST_ACCEPT(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nSlotID, const int nQuestID, const bool bChallange, const int nCharPtm, int nDeltaCharPtm
		, const int nXP, const uint8 nLevel, const int nMoney, const bool bComplete, const wstring& strAcceptDt, const wstring& strExpiDt, const int nFieldID, const MUID& uidNPC)
		: m_QuestComm(nAID, uidPlayer, nCID, nSlotID, nQuestID, nCharPtm, nDeltaCharPtm, nLevel, nXP, strAcceptDt, nFieldID)
		, m_bChallange(bChallange), m_bComplete(bComplete), m_strExpiDt(strExpiDt), m_uidNPC(uidNPC) {}

	GDBT_QUEST_ACCEPT& operator==(GDBT_QUEST_ACCEPT& data)
	{
		m_QuestComm = data.m_QuestComm;
		m_bChallange = data.m_bChallange;
		m_nMoney = data.m_nMoney;
		m_bComplete = data.m_bComplete;
		m_strExpiDt = data.m_strExpiDt;		

		m_vecAddItem.swap(data.m_vecAddItem);
		m_vecRemoveItem.swap(data.m_vecRemoveItem);
	}


	GDBT_QUEST_COMMON			m_QuestComm;
	bool						m_bChallange;
	int							m_nMoney;
	bool						m_bComplete;
	wstring						m_strExpiDt;
	vector<GDBT_QUEST_ADDITEM>	m_vecAddItem;
	GITEM_STACK_AMT_VEC			m_vecRemoveItem;
	map<int, int64>				m_mapIUID;
	MUID						m_uidNPC;
};

class GDBT_QUEST_OBJECT
{
public :
	GDBT_QUEST_OBJECT() : m_nObjID(0), m_nProgress(0), m_bComplete(false), m_uidNPC(MUID::ZERO) {}
	GDBT_QUEST_OBJECT(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nSlotID, const int nQuestID, const int nCharPtm, int nDeltaCharPtm
		, const uint8 nLevel, const int nXP, const int nObjNum, const int16 nProgress, const bool bComplete, bool bTrigger, MUID uidNPC, const wstring& strAcceptDt, const int nFieldID) 
		: m_QuestComm(nAID, uidPlayer, nCID, nSlotID, nQuestID, nCharPtm, nDeltaCharPtm, nLevel, nXP, strAcceptDt, nFieldID)
		, m_nObjID(nObjNum), m_nProgress(nProgress), m_bComplete(bComplete), m_bTrigger(bTrigger), m_uidNPC(uidNPC) {}

	GDBT_QUEST_COMMON	m_QuestComm;
	int					m_nObjID;
	int16				m_nProgress;
	bool				m_bComplete;
	bool				m_bTrigger;
	MUID				m_uidNPC;
};

class GDBT_QUEST_VAR
{
public :
	GDBT_QUEST_VAR() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nSlotID(0), m_nQuestID(0), m_nVar(0) {}
	GDBT_QUEST_VAR(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const uint8 nSlotID, const int nQuestID, const int nVar)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nSlotID(nSlotID), m_nQuestID(nQuestID), m_nVar(nVar) {}

	const int64 m_nAID;
	const MUID	m_uidPlayer;
	const int64	m_nCID;
	const uint8 m_nSlotID;
	const int	m_nQuestID;
	const int	m_nVar;
};

class GDBT_QUEST_ADDITEM
{
public:
	GDBT_QUEST_ADDITEM()
	: m_nSlotID(0), m_nIUID(0), m_nItemID(0), m_nStackAmt(0), m_nModStackAmt(0), m_nMaxDura(0), m_bClaimed(false), m_bPeriod(false) {}

	GDBT_QUEST_ADDITEM(int16 nSlotID, IUID nIUID, int nItemID, int16 nStackAmt, int16 nModStackAmt, uint8 nMaxDura, bool bClaimed, 	bool bPeriod)
	: m_nSlotID(nSlotID), m_nIUID(nIUID), m_nItemID(nItemID), m_nStackAmt(nStackAmt), m_nModStackAmt(nModStackAmt), m_nMaxDura(nMaxDura), m_bClaimed(bClaimed), m_bPeriod(bPeriod) {}

	int16	m_nSlotID;
	IUID	m_nIUID;
	int		m_nItemID;
	int16	m_nStackAmt;
	int16	m_nModStackAmt;
	uint8	m_nMaxDura;
	bool	m_bClaimed;
	bool	m_bPeriod;
};

#endif