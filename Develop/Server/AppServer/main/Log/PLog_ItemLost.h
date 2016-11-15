#ifndef _G_LOG_ITEM_LOST_H_
#define _G_LOG_ITEM_LOST_H_

#include "PLog.h"
#include "MTypes.h"

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

class PLog_ItemLost : public PLog
{
private:
	int						m_nCID;
	LOG_ITEM_LOST_TYPE		m_nType;
	int64					m_nIUID;
	int						m_nQuantity;	
	int						m_nGold;

	static wstring			m_strQueryForm;

public:
	PLog_ItemLost(const wstring& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold);
	virtual ~PLog_ItemLost();

	virtual wstring MakeQuery();
};

#endif
