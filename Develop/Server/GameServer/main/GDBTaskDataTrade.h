#ifndef _GDBTASK_DATA_TRADE_H
#define _GDBTASK_DATA_TRADE_H

#include "GItemAdder.h"
#include "GItemRemover.h"
#include "GPlayerTrade.h"

enum CHAR_ORDER
{
	TRADE_CHAR_FIRSET = 0,
	TRADE_CHAR_SECOND = 1,
	TRADE_CHAR_SIZE = 2,
};

struct GDBT_TRADE_RESULT
{
	vector<ADD_ITEM>		vecAddItem;
	vector<REMOVE_ITEM>		vecRemoveItem;
};

struct GDBT_TRADE_ITEM
{
	int64	nCID;
	int16	nSlotID;
	int64	nToCID;
	int16	nToSlotID;
	int64	nIUID;
	int16	nStackAmt;
	uint8	nDura;
	int		nRemainUsagePeriod;
};

class GTRADE_ITEM_LOG
{
public :
	GTRADE_ITEM_LOG() : m_nItemID(0), m_nIUID(0), m_nToIUID(0), m_nStackAmt(0), m_nModStackAmt(0), m_nDura(0), m_nUsagePeriod(0) {}	
	GTRADE_ITEM_LOG(const CID nCID, const int16 nSlotID, const CID nToCID, const int16 nToSlotID
		, const int ITEM_ID, const int64 IUID, const int64 nToIUID, const int STACK_AMT, const int DELTA_STACK_AMT, const uint8 DURA, const int USAGE_PERIOD)
		: m_nItemID(ITEM_ID), m_nIUID(IUID), m_nToIUID(nToIUID), m_nStackAmt(STACK_AMT), m_nModStackAmt(DELTA_STACK_AMT), m_nDura(DURA), m_nUsagePeriod(USAGE_PERIOD) {}

	void Set(const int nItemID, const int64 nIUID, const int64 nToIUID, const int nStackAmt, const int nModStackAmt, const uint8 nDura, const int nUsagePeriod)
	{
		m_nItemID			= nItemID;
		m_nIUID				= nIUID;		
		m_nToIUID			= nToIUID;
		m_nStackAmt			= nStackAmt;
		m_nModStackAmt		= nModStackAmt;
		m_nDura				= nDura;
		m_nUsagePeriod		= nUsagePeriod;
	}

	int		m_nItemID;
	int64	m_nIUID;
	int64	m_nToIUID;
	int16	m_nStackAmt;
	int16	m_nModStackAmt;
	uint8	m_nDura;
	int		m_nUsagePeriod;
};

struct GDBT_TRADE_DATA
{
	GTRADE_CHAR_VEC			vTradeChar;
	vector<GDBT_TRADE_ITEM> vecDBInsertItemBySplit;
	GTRADE_ITEM_LOG			ItemLog[TRADE_CHAR_SIZE][MAX_TRADE_ITEM_SPACE];
};

#endif