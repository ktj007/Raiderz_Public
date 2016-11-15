#ifndef _G_LOG_ITEM_LOST_H_
#define _G_LOG_ITEM_LOST_H_

#include "GLog.h"

enum LOG_ITEM_LOST_TYPE
{
	LILT_NONE = 0,

	LILT_BUY,
	LILT_SELL,
	LILT_CRAFT,
	LILT_QUEST,
	LILT_MAIL,
	LILT_DESTROY,

	LILT_MAX
};

class GLog_ItemLost : public GLog
{
private:
	int						m_nCID;
	LOG_ITEM_LOST_TYPE		m_nType;
	int64					m_nIUID;
	int						m_nQuantity;	
	int						m_nGold;

	static string			m_strQueryForm;

public:
	GLog_ItemLost(const string& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	virtual ~GLog_ItemLost();

	virtual string MakeQuery();
};

#endif
