#ifndef _GDBTASK_CRAFT_DATA_H
#define _GDBTASK_CRAFT_DATA_H


#include "GITEM_STACK_AMT.h"
#include "GDBTaskItem.h"


class GDBT_CRAFT_DATA
{
public :
	GDBT_CRAFT_DATA() : m_nAID(0), m_uidPlayer(0), m_nXP(0), m_nMoney(0), m_nModMoney(0), m_nModStackAmt(0), m_nNPCID(0), m_nAddExpiMin(-1) {}
	GDBT_CRAFT_DATA(const int64 nAID, const MUID& uidPlayer, int nDeltaCahrPtm, uint8 nLevel, const int nMoney, const int nModMoney, const int nXP, const int16 nModStackAmt
		, const int nAddExpiMin, const int nNPCID,  GITEM_STACK_AMT_VEC& vRecp)
		: m_nAID(nAID), m_uidPlayer(uidPlayer), m_nDeltaCharPtm(nDeltaCahrPtm), m_nLevel(nLevel), m_nMoney(nMoney), m_nModMoney(nModMoney), m_nXP(nXP), m_nModStackAmt(nModStackAmt)
		, m_nAddExpiMin(nAddExpiMin), m_nNPCID(nNPCID)
	{
		m_vRecp.swap(vRecp);
	}

	void SetData(GDBT_CRAFT_DATA& data)
	{
		m_Item			= data.m_Item;

		m_nAID			= data.m_nAID;
		m_uidPlayer		= data.m_uidPlayer;
		m_nDeltaCharPtm	= data.m_nDeltaCharPtm;
		m_nLevel		= data.m_nLevel;
		m_nMoney		= data.m_nMoney;
		m_nModMoney		= data.m_nModMoney;
		m_nXP			= data.m_nXP;
		m_nModStackAmt	= data.m_nModStackAmt;
		m_nNPCID		= data.m_nNPCID;
		m_nAddExpiMin	= data.m_nAddExpiMin;

		m_vRecp.swap(data.m_vRecp);
	}

	int64				m_nAID;
	MUID				m_uidPlayer;
	int					m_nDeltaCharPtm;
	uint8				m_nLevel;
	int					m_nXP;
	int					m_nMoney;
	int					m_nModMoney;
	int16				m_nModStackAmt;
	int					m_nAddExpiMin;
	int					m_nNPCID;

	GDBT_ITEM			m_Item;
	GITEM_STACK_AMT_VEC m_vRecp;	
};

#endif