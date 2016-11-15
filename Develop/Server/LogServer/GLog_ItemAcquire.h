#ifndef _G_LOG_ITEM_ACQUIRE_H_
#define _G_LOG_ITEM_ACQUIRE_H_

#include "GLog.h"

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

class GLog_ItemAcquire : public GLog
{
private:
	int						m_nCID;
	LOG_ITEM_ACQUIRE_TYPE	m_nType;
	int64					m_nIUID;
	int						m_nQuantity;	
	int						m_nGold;

	static string			m_strQueryForm;

public:
	GLog_ItemAcquire(const string& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	virtual ~GLog_ItemAcquire();

	virtual string MakeQuery();
};


#endif