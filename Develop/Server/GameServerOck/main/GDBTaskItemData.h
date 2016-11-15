#ifndef _GDBTASK_ITEM_DATA_H
#define _GDBTASK_ITEM_DATA_H


#include "GDBTaskItem.h"


class GDBT_ITEM_DATA
{
public :
	GDBT_ITEM_DATA() : m_nAID(0), m_uidPlayer(0), m_nTier(0), m_nModStackAmt(0), m_nAddExpiMin(-1), m_nMoney(0), m_nNpcID(0) {}
	GDBT_ITEM_DATA(const int64 nAID, const MUID& uidPlayer, int nDeltaCharPtm, uint8 nLevel, int nMoney, const int16 nModStackAmt, int nAddExpiMin, const uint8 nTier, int nNpcID) 
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney), m_nTier(nTier), m_nModStackAmt(nModStackAmt), m_nAddExpiMin(nAddExpiMin), m_nNpcID(nNpcID) {}

	int64		m_nAID;
	MUID		m_uidPlayer;
	int			m_nDeltaCharPtm;
	uint8		m_nLevel;
	int			m_nMoney;
	GDBT_ITEM	m_Item;
	int16		m_nModStackAmt;
	int			m_nAddExpiMin;		
	uint8		m_nTier;
	int			m_nNpcID;
};

class GDBT_ITEM_DEC_STACK_AMT_DATA
{
public :
	GDBT_ITEM_DEC_STACK_AMT_DATA() : m_uidPlayer(0), m_nAID(0), m_nCID(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLevel(0), m_nMoney(0), m_nSlotType(0), m_nSlotID(0), m_nIUID(0), m_nItemID(0), m_nStackAmt(0), m_nModStackAmt(0), m_nTier(0), m_nNpcID(0) {}
	GDBT_ITEM_DEC_STACK_AMT_DATA(const MUID& uidPlayer, const int64 nAID, const int64 nCID, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID, const int nItemID
		, const int16 nStackAmt, const int16 nModStackAmt, const uint8 nTier, int nNpcID) 
		: m_uidPlayer(uidPlayer), m_nAID(nAID), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney), m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID), m_nItemID(nItemID)
		, m_nStackAmt(nStackAmt), m_nModStackAmt(nModStackAmt), m_nTier(nTier), m_nNpcID(nNpcID) {}

	MUID	m_uidPlayer;
	int64	m_nAID;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nMoney;
	uint8	m_nSlotType;
	int16	m_nSlotID;
	IUID	m_nIUID;
	int		m_nItemID;
	int16	m_nStackAmt;
	int16	m_nModStackAmt;	
	uint8	m_nTier;
	int		m_nNpcID;
};

class GDBT_ITEM_DEC_DURA_DATA
{
public :
	GDBT_ITEM_DEC_DURA_DATA() : m_uidPlayer(0), m_nCID(0), m_nSlotType(0), m_nSlotID(0)
		, m_nIUID(0), m_nDura(0), m_nModDura(0), m_nStackAmt(0) {}
	GDBT_ITEM_DEC_DURA_DATA(const MUID& uidPlayer, const int64 nCID, const uint8 nSlotType, const int16 nSlotID
		, const IUID nIUID, const uint8 nDura, const uint8 nModDura, const int16 nStackAmt) 
		: m_uidPlayer(uidPlayer), m_nCID(nCID), m_nSlotType(nSlotType), m_nSlotID(nSlotID)
		, m_nIUID(nIUID), m_nDura(nDura), m_nModDura(nModDura), m_nStackAmt(nStackAmt) {}

	MUID	m_uidPlayer;
	int64	m_nCID;
	uint8	m_nSlotType;
	int16	m_nSlotID;
	IUID	m_nIUID;
	uint8	m_nDura;
	uint8	m_nModDura;
	int16	m_nStackAmt;
};

class GDBT_ITEM_ENCH
{
public :
	GDBT_ITEM_ENCH() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLevel(0)
		, m_nEStoneItemSlotID(0), m_nEStoneIUID(0), m_nEStoneItemID(0)
		, m_nEAgentItemSlotID(0), m_nEAgentIUID(0)
		, m_nTargetItemSlotType(0), m_nTargetItemSlotID(0), m_nTargetIUID(0), m_nTargetItemID(0)
		, m_nTargetDura(0), m_nTargetMaxDura(0)
		, m_nTargetEnchSlotID(0), m_nTargetEnchItemID(0), m_bClaimed(false) {}

	GDBT_ITEM_ENCH(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney
		, const int16 nEStoneItemSlotID, const int64 nEStoneIUID, const int nEStoneItemID
		, const int16 nEAgentItemSlotID, const int64 nEAgentIUID, const int nEAgnetItemID, int16 nEAgentStackAmt, int16 nEAgentDeltaStackAmt
		, const uint8 nTargetItemSlotType, const int16 nTargetItemSlotID, const int64 nTargetIUID, const int nTargetItemID
		, const uint8 nTargetDura, const uint8 nTargetMaxDura
		, const int nTargetEnchSlotID, const int nTargetEnchItemID, const bool bClaimed)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney)
		, m_nEStoneItemSlotID(nEStoneItemSlotID), m_nEStoneIUID(nEStoneIUID), m_nEStoneItemID(nEStoneItemID)
		, m_nEAgentItemSlotID(nEAgentItemSlotID), m_nEAgentIUID(nEAgentIUID), m_nEAgentItemID(nEAgnetItemID), m_nEAgentStackAmt(nEAgentStackAmt), m_nEAgentDeltaStackAmt(nEAgentDeltaStackAmt)
		, m_nTargetItemSlotType(nTargetItemSlotType), m_nTargetItemSlotID(nTargetItemSlotID), m_nTargetIUID(nTargetIUID), m_nTargetItemID(nTargetItemID)
		, m_nTargetDura(nTargetDura), m_nTargetMaxDura(nTargetMaxDura)
		, m_nTargetEnchSlotID(nTargetEnchSlotID), m_nTargetEnchItemID(nTargetEnchItemID), m_bClaimed(bClaimed) {}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nMoney;

	int16	m_nEStoneItemSlotID;
	int64	m_nEStoneIUID;
	int		m_nEStoneItemID;		// log에 사용됨.

	int16	m_nEAgentItemSlotID;	// enchent recipe
	int64	m_nEAgentIUID;
	int		m_nEAgentItemID;
	int16	m_nEAgentStackAmt;
	int16	m_nEAgentDeltaStackAmt;

	uint8	m_nTargetItemSlotType;
	int16	m_nTargetItemSlotID;		// enchent
	int64	m_nTargetIUID;
	int		m_nTargetItemID;
	uint8	m_nTargetDura;
	uint8	m_nTargetMaxDura;
	int		m_nTargetEnchSlotID;			// 1 ~ 5.
	int		m_nTargetEnchItemID;		// 실패했으면 -1로 설정. 이 값과 m_nEStoneItemID와 같다. 
										// 다른 점은 실패 했을 경우 이 값은 -1로 설정 되지만 m_nEStoneItemID는 item id값을 가지고 있는다.
	bool	m_bClaimed;
};

class GDBT_ITEM_DYE
{
public :
	GDBT_ITEM_DYE() {}
	GDBT_ITEM_DYE(const int64 nAID, const MUID& uidPlayer, const int nCID, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney
		, const int16 nDyeItemSlotID, const int64 nDyeIUID, const int nDyeItemID
		, const uint8 nTargetItemSlotType, const int16 nTargetItemSlotID, const int64 nTargetIUID, const int nTargetItemID
		, const int nColor)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney)
		, m_nDyeItemSlotID(nDyeItemSlotID), m_nDyeIUID(nDyeIUID), m_nDyeItemID(nDyeItemID)
		, m_nTargetItemSlotType(nTargetItemSlotType), m_nTargetItemSlotID(nTargetItemSlotID), m_nTargetIUID(nTargetIUID), m_nTargetItemID(nTargetItemID)
		, m_nColor(nColor)
	{

	}

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nMoney;

	int16	m_nDyeItemSlotID;
	int64	m_nDyeIUID;
	int		m_nDyeItemID;

	uint8	m_nTargetItemSlotType;
	int16	m_nTargetItemSlotID;
	int64	m_nTargetIUID;
	int		m_nTargetItemID;

	int		m_nColor;
};


#endif