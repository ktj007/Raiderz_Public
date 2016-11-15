#ifndef _G_LOG_ITEM_ACQUIRE_H_
#define _G_LOG_ITEM_ACQUIRE_H_

#include "PLog.h"
#include "MTypes.h"

struct TD_ITEM;

enum LOG_ITEM_ACQUIRE_TYPE
{
	LIAT_NONE = 0,

	LIAT_BUY,
	LIAT_SELL,
	LIAT_CRAFT,
	LIAT_QUEST,
	LIAT_MAIL,
	LIAT_LOOT,
	
	LIAT_MAX
};

class PLog_ItemAcquire : public PLog
{
private:
	int						m_nCID;
	LOG_ITEM_ACQUIRE_TYPE	m_nType;
	int64					m_nIUID;
	int						m_nQuantity;	
	int						m_nGold;

	static wstring			m_strQueryForm;

public:
	PLog_ItemAcquire(const wstring& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	virtual ~PLog_ItemAcquire();

	virtual wstring MakeQuery();
};


#endif