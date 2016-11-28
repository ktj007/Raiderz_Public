#ifndef _GDBTASK_ITEM_MOVE_H
#define _GDBTASK_ITEM_MOVE_H


#include "STypes.h"
#include "MDBRecordSet.h"
#include <deque>
#include "CSItemData.h"


class GItem;
class GEntityPlayer;


class GDBT_ITEM
{
public :
	GDBT_ITEM() : m_nCID(0), m_nSlotType(0), m_nSlotID(-1), m_nIUID(0), m_nItemID(0), m_nStackAmt(0)
		, m_nSoulCnt(0), m_nDura(0), m_nMaxDura(0), m_nEnchCnt(0), m_nColor(0), m_bClaimed(false)
		, m_nCharPtm(0), m_bPeriodItem(false), m_nUsagePeriod(0), m_strExpiDt(L"NULL") {}

	void Build(const int64 nCID, GItem* pItem);
	void Set(const int64 nCID, const uint8 nSlotType, const int16 nSlotID, const int nItemID, const int16 nStackAmt, const int nCharPtm
		, const IUID nIUID = 0, const uint8 nSoulCnt = 0, const uint8 nDura = 0, const uint8 nMaxDura = 0, const uint8 nEnchCnt = 0
		, const int nColor = 0, const bool bClaimed = false, const bool bPeriodItem = false, const int nUsagePeriod = 0
		, const wstring& strExpiDt = L"NULL");

	int64		m_nCID;
	uint8		m_nSlotType;
	int16		m_nSlotID;
	IUID		m_nIUID;
	int			m_nItemID;
	int16		m_nStackAmt;
	uint8		m_nSoulCnt;
	uint8		m_nDura;
	uint8		m_nMaxDura;
	uint8		m_nEnchCnt;
	int			m_nColor;
	bool		m_bClaimed;
	int			m_nCharPtm;
	bool		m_bPeriodItem;
	int			m_nUsagePeriod;
	int			m_nExpiTm;
	wstring		m_strExpiDt;	
};

class GDBT_ITEM_MERGE_AND_SPLIT_SLOT
{
public: 
	GDBT_ITEM_MERGE_AND_SPLIT_SLOT() : m_nAID(0), m_nCIDorGID(0), m_nSlotType(0), m_nSlotID(0), m_nIUID(0), m_nStackAmt(0), m_nItemID(0) {}
	GDBT_ITEM_MERGE_AND_SPLIT_SLOT(int64	nAID, int64	nCIDorGID, uint8 nSlotType, int16 nSlotID, IUID nIUID, int16 nStackAmt, int nItemID)
	: m_nAID(nAID), m_nCIDorGID(nCIDorGID), m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID), m_nStackAmt(nStackAmt), m_nItemID(nItemID) {}

	int64	m_nAID;
	int64	m_nCIDorGID;
	uint8	m_nSlotType;
	int16	m_nSlotID;
	IUID	m_nIUID;
	int16	m_nStackAmt;
	int		m_nItemID;	
};

class GDBT_ITEM_MERGE_AND_SPLIT
{
public:
	enum TYPE
	{
		MERGE,
		SPLIT
	};

public :
	GDBT_ITEM_MERGE_AND_SPLIT() : m_uidPlayer(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLevel(0), m_nMoney(0), m_nType(MERGE), m_nCode(0) {}
	GDBT_ITEM_MERGE_AND_SPLIT(const MUID& uidPlayer, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney, const int16 nMergeAmt, TYPE nType)
	: m_uidPlayer(uidPlayer), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney), m_nMergeAndSplitAmt(nMergeAmt), m_nType(nType) {}
	void MakeDBCode();

	MUID	m_uidPlayer;	
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nMoney;
	int16	m_nMergeAndSplitAmt;
	int		m_nCode;
	TYPE	m_nType;

	GDBT_ITEM_MERGE_AND_SPLIT_SLOT m_FromSlot;
	GDBT_ITEM_MERGE_AND_SPLIT_SLOT m_ToSlot;	
};

class GDBT_ITEM_MOVE_SLOT
{
public :
	GDBT_ITEM_MOVE_SLOT() : m_nAID(0), m_nCIDorGID(0), m_nSlotType(SLOTTYPE_NONE), m_nSlotID(0), m_nIUID(0), m_nStackAmt(0) {}
	GDBT_ITEM_MOVE_SLOT(int64 nAID, int64 nCIDorGID, const SH_ITEM_SLOT_TYPE nSlotType, const int16 nSlotID, const int64 nIUID, int16 nStackAmt)
	: m_nAID(nAID), m_nCIDorGID(nCIDorGID), m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID), m_nStackAmt(nStackAmt) {}

	int64				m_nAID;
	int64				m_nCIDorGID;
	SH_ITEM_SLOT_TYPE	m_nSlotType;
	int16				m_nSlotID;
	int64				m_nIUID;
	int16				m_nStackAmt;
};

class GDBT_ITEM_MOVE
{
public :
	GDBT_ITEM_MOVE() : m_uidPlayer(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLev(0), m_nCurMoney(0), m_nCode(0) {}
	GDBT_ITEM_MOVE(const MUID& uidPlayer, int nCharPtm, int nDeltaCharPtm, uint8 nLev, int nCurMoney, int nItemID)
	: m_uidPlayer(uidPlayer), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLev(nLev), m_nCurMoney(nCurMoney), m_nItemID(nItemID) {}
	void MakeDBCode();

	MUID				m_uidPlayer;
	int					m_nCharPtm;
	int					m_nDeltaCharPtm;
	uint8				m_nLev;
	int					m_nCurMoney;
	int					m_nItemID;
	int					m_nCode;
	GDBT_ITEM_MOVE_SLOT	m_FromSlot;
	GDBT_ITEM_MOVE_SLOT	m_ToSlot;	
};

class GDBT_ITEMINSTANCE
{
public:
	int			nItemID;
	uint8		nStackAmt;
	uint8		nDura;
	uint8		nMaxDura;
	int			nDyedColor;
	int64		nIUID;		
	bool		bClaimed;
	uint8		nSoulCnt;		
	uint8		nSlotType;
	int16		nSlotID;
	int			nCharPtm;
	bool		bUseUsagePeriod;
	bool		bStartCalculatingUsagePeriod;
	int			nUsagePeriodSec;
	bool		bUseExpiTm;
	int			nExpiTm;
	wstring		strExpiDt;
	int			nEnchItemID_1;
	int			nEnchItemID_2;
	int			nEnchItemID_3;
	int			nEnchItemID_4;
	int			nEnchItemID_5;
	int			nEnchItemID_6;
};

class GDBT_ITEMINSTANCEQ : public deque<GDBT_ITEMINSTANCE>
{
public:
	bool GetFromDB(mdb::MDBRecordSet& rs);
	bool SetToServer(GEntityPlayer* pPlayer);
};






#endif