#ifndef _GDBTASK_NPCSHOP_DATA_H
#define _GDBTASK_NPCSHOP_DATA_H

#include "GITEM_STACK_AMT.h"
#include "STypes.h"
#include "GDBTaskItem.h"
#include "CSItemData.h"

class GDBT_NPC_SHOP_TRADE_DATA 
{
public :
	GDBT_NPC_SHOP_TRADE_DATA() : m_nAID(0), m_uidPlayer(0), m_nXP(0), m_nMoney(0), m_nModStackAmt(0), m_nAddExpiMin(-1) {}
	GDBT_NPC_SHOP_TRADE_DATA(const int64 nAID, const MUID& uidPlayer, int nDeltaCharPtm, uint8 nLevel, const int nxP, const int nMoney, const int nModMoney, const int16 nModStackAmt, const int nAddExpiMin) 
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nXP(nxP), m_nMoney(nMoney), m_nModMoney(nModMoney), m_nModStackAmt(nModStackAmt), m_nAddExpiMin(nAddExpiMin) {}

	int64		m_nAID;
	MUID		m_uidPlayer;
	int			m_nDeltaCharPtm;
	uint8		m_nLevel;
	int			m_nXP;	
	int			m_nMoney;
	int			m_nModMoney;
	int			m_nAddExpiMin;
	GDBT_ITEM	m_Item;
	int16		m_nModStackAmt;
};

class GDBT_SELL_ITEM_VERY_COMMON
{
public :
	GDBT_SELL_ITEM_VERY_COMMON() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nDeltaCharPtm(0), m_nLevel(0), m_nXP(0), m_nMoney(0), m_nModMoney(0), m_nCharPtm(0) {}
	GDBT_SELL_ITEM_VERY_COMMON(const int64 nAID, const MUID& uidPlayer, const int64 nCID, int nDeltaCharPtm, uint8 nLevel, const int nXP, const int nMoney, const int nModMoney, const int nCharPtm, GITEM_STACK_AMT_VEC& vVeryComm)
		: m_nAID(nAID),m_uidPlayer(uidPlayer), m_nCID(nCID), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nXP(nXP), m_nMoney(nMoney), m_nModMoney(nModMoney), m_nCharPtm(nCharPtm)
	{
		m_vVeryComm.swap(vVeryComm);
	}

	void SetData(GDBT_SELL_ITEM_VERY_COMMON& data)
	{
		m_nAID			= data.m_nAID;
		m_uidPlayer		= data.m_uidPlayer;
		m_nCID			= data.m_nCID;
		m_nDeltaCharPtm	= data.m_nDeltaCharPtm;
		m_nLevel		= data.m_nLevel;
		m_nXP			= data.m_nXP;
		m_nMoney		= data.m_nMoney;
		m_nModMoney		= data.m_nModMoney;
		m_nCharPtm		= data.m_nCharPtm;

		m_vVeryComm.swap(data.m_vVeryComm);
	}

	int64				m_nAID;
	MUID				m_uidPlayer;
	int64				m_nCID;
	int					m_nXP;
	int					m_nMoney;
	int					m_nModMoney;
	int					m_nCharPtm;
	int					m_nDeltaCharPtm;
	uint8				m_nLevel;
	GITEM_STACK_AMT_VEC m_vVeryComm;
};

class GDBT_ITEM_REPAIR_CHAR
{
public :
	GDBT_ITEM_REPAIR_CHAR() : m_nAID(0), m_nCID(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLevel(0), m_nXP(0), m_nMoney(0), m_nModMoney(0) {}
	GDBT_ITEM_REPAIR_CHAR(const int64 nAID, const int64 nCID, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, const int nXP, const int nMoney, const int nModMoney)
		: m_nAID(nAID), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nXP(nXP), m_nMoney(nMoney), m_nModMoney(nModMoney) {}

	int64		m_nAID;
	int64		m_nCID;
	int			m_nCharPtm;		
	int			m_nDeltaCharPtm;
	uint8		m_nLevel;
	int			m_nXP;	
	int			m_nMoney;
	int			m_nModMoney;
};

class GDBT_REPAIR_ITEM
{
public :
	GDBT_REPAIR_ITEM() : m_uidPlayer(0), m_nSlotType(0), m_nSlotID(0), m_nIUID(0) {}
	GDBT_REPAIR_ITEM(const int64 nAID, const MUID& uidPlayer, const int64 nCID
		, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID, const int nCharPtm, int nDeltacharPtm, uint8 nLevel
		, const int nXP, const int nMoney, const int nModMoney)
		: m_Char(nAID, nCID, nCharPtm, nDeltacharPtm, nLevel, nXP, nMoney, nModMoney), m_uidPlayer(uidPlayer), m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID) {}

	MUID					m_uidPlayer;
	GDBT_ITEM_REPAIR_CHAR	m_Char;
	uint8					m_nSlotType;
	int16					m_nSlotID;
	IUID					m_nIUID;
};

class GDBT_ITEM_REPAIR_ALL
{
public :
	GDBT_ITEM_REPAIR_ALL() : m_nSlotType(0), m_nSlotID(0), m_nIUID(0) {}
	GDBT_ITEM_REPAIR_ALL(const uint8 nSlotType, const int16 nSlotID, const IUID nIUID) 
		: m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID) {}

	uint8	m_nSlotType;
	int16	m_nSlotID;
	IUID	m_nIUID;
};

typedef vector<GDBT_ITEM_REPAIR_ALL> GDBT_ITEM_REPAIR_VEC;

class GDBT_REPAIR_ALL_ITEM 
{
public :
	GDBT_REPAIR_ALL_ITEM() : m_uidPlayer(0) {}
	GDBT_REPAIR_ALL_ITEM(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nMoney, const int nModMoney, const int nXP, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, GDBT_ITEM_REPAIR_VEC& vRepair)
		: m_Char(nAID, nCID, nCharPtm, nDeltaCharPtm, nLevel, nXP, nMoney, nModMoney), m_uidPlayer(uidPlayer) 
	{
		m_vRepair.swap(vRepair);
	}

	void SetData(GDBT_REPAIR_ALL_ITEM& data)
	{
		m_uidPlayer		= data.m_uidPlayer;
		m_Char			= data.m_Char;		

		m_vRepair.swap(data.m_vRepair);
	}

	MUID					m_uidPlayer;
	GDBT_ITEM_REPAIR_CHAR	m_Char;
	GDBT_ITEM_REPAIR_VEC	m_vRepair;
};

#endif