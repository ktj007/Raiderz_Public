#ifndef _GDBTASK_DATA_TALENT_H
#define _GDBTASK_DATA_TALENT_H

#include "CSPalette.h"

#include <vector>
#include <utility>


using std::pair;


class GDBT_TALENT
{
public :
	GDBT_TALENT() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nCharPtm(0), m_nDeltaCharPtm(0), m_nLevel(0), m_nMoney(0), m_nTalentID(0), m_nLowRankTalentID(0), m_nRemainTP(0), m_bOnPalette(false), m_nResetAllTalentCountOnPalette(0)
	, m_nSlotID(0), m_nIUID(0), m_nItemID(0) {}
	GDBT_TALENT(const int64 nAID, const MUID& uidPlayer, const int64 nCID, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney, const int nTalentID, const int nLowRankTalentID, const short nRemainTP
		, const bool bOnPalette, const int nSlotID = 0, const int64 nIUID = 0, const int nItemID = 0)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney), m_nTalentID(nTalentID), m_nLowRankTalentID(nLowRankTalentID), m_nRemainTP(nRemainTP)
		, m_bOnPalette(bOnPalette), m_nResetAllTalentCountOnPalette(0), m_nSlotID(nSlotID), m_nIUID(nIUID), m_nItemID(nItemID) {}

	typedef vector<pair<PALETTE_NUM, PALETTE_SLOT>> PALETTE_VEC;

	void SetPalette(PALETTE_VEC& vPalette) { m_vNumAndSlot.swap(vPalette); }

	void Set(GDBT_TALENT& data)
	{
		m_nAID							= data.m_nAID;
		m_uidPlayer						= data.m_uidPlayer;
		m_nCID							= data.m_nCID;
		m_nCharPtm						= data.m_nCharPtm;
		m_nDeltaCharPtm					= data.m_nDeltaCharPtm;
		m_nLevel						= data.m_nLevel;
		m_nMoney						= data.m_nMoney;
		m_nTalentID						= data.m_nTalentID;
		m_nLowRankTalentID				= data.m_nLowRankTalentID;
		m_nRemainTP						= data.m_nRemainTP;
		m_bOnPalette					= data.m_bOnPalette;
		m_nResetAllTalentCountOnPalette	= data.m_nResetAllTalentCountOnPalette;
		m_nSlotID						= data.m_nSlotID;
		m_nIUID							= data.m_nIUID;
		m_nItemID						= data.m_nItemID;

		m_vNumAndSlot.swap(data.m_vNumAndSlot);
	}

	void SetRestAllTalentCountOnPalette(const int nTalentCountOnPalette)
	{
		m_nResetAllTalentCountOnPalette = nTalentCountOnPalette;
	}

	int64			m_nAID;
	MUID			m_uidPlayer;
	int64			m_nCID;
	int				m_nCharPtm;
	int				m_nDeltaCharPtm;
	uint8			m_nLevel;
	int				m_nMoney;
	int				m_nTalentID;
	short			m_nLowRankTalentID;
	short			m_nRemainTP;
	bool			m_bOnPalette;
	PALETTE_VEC		m_vNumAndSlot;
	int				m_nResetAllTalentCountOnPalette; // used to reset all talent.
	int				m_nSlotID;
	int64			m_nIUID;
	int				m_nItemID;
};

class GDBT_ITEM_LEARN_TALENT
{
public :
	GDBT_ITEM_LEARN_TALENT() : m_nAID(0), m_uidPlayer(0), m_nCID(0), m_nSlotType(0), m_nSlotID(0), m_nItemID(0), m_nIUID(0), m_nRemainTP(0), m_nCharPtm(0)
	{
		SetZeroTalentID();
	}

	GDBT_ITEM_LEARN_TALENT(const int64 nAID, const MUID uidPlayer, const int64 nCID, const uint8 nSlotType, const int16 nSlotID, const int nItemID
		, const IUID nIUID, const int16 nRemainTP, const int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nCID(nCID), m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nItemID(nItemID)
		, m_nIUID(nIUID), m_nRemainTP(nRemainTP), m_nCharPtm(nCharPtm), m_nDeltaCharPtm(nDeltaCharPtm), m_nLevel(nLevel), m_nMoney(nMoney)
	{
		SetZeroTalentID();
	}

	void SetZeroTalentID()
	{
		m_nTalentID[0] = 0;
		m_nTalentID[1] = 0;
		m_nTalentID[2] = 0;
		m_nTalentID[3] = 0;
		m_nTalentID[4] = 0;
	}

	void SetTalent(const size_t nIdx, const int nTalentID)
	{
		if (MAX_COUNT <= nIdx)
			return;

		m_nTalentID[nIdx] = nTalentID;
	}

	void Set(GDBT_ITEM_LEARN_TALENT& data)
	{
		m_nAID			= data.m_nAID;
		m_uidPlayer		= data.m_uidPlayer;
		m_nCID			= data.m_nCID;
		m_nSlotType		= data.m_nSlotType;
		m_nSlotID		= data.m_nSlotID;
		m_nItemID		= data.m_nItemID;
		m_nIUID			= data.m_nIUID;
		m_nRemainTP		= data.m_nRemainTP;
		m_nCharPtm		= data.m_nCharPtm;
		m_nDeltaCharPtm = data.m_nDeltaCharPtm;
		m_nLevel		= data.m_nLevel;
		m_nMoney		= data.m_nMoney;

		SetTalent(0, data.m_nTalentID[0]);
		SetTalent(1, data.m_nTalentID[1]);
		SetTalent(2, data.m_nTalentID[2]);
		SetTalent(3, data.m_nTalentID[3]);
		SetTalent(4, data.m_nTalentID[4]);
	}

	enum TALENT_COUNT
	{
		MAX_COUNT = 5, // 이 수치는 추후 확장이 필요하면 그때 수정을 하자.
	};

	int64	m_nAID;
	MUID	m_uidPlayer;
	int64	m_nCID;
	uint8	m_nSlotType;
	int16	m_nSlotID;
	int		m_nItemID;
	IUID	m_nIUID;
	int16	m_nRemainTP;
	int		m_nCharPtm;
	int		m_nDeltaCharPtm;
	uint8	m_nLevel;
	int		m_nMoney;
	int		m_nTalentID[MAX_COUNT];
};

#endif
