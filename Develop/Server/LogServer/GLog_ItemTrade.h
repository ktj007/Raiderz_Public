#ifndef _G_LOG_ITEM_TRADE_H_
#define _G_LOG_ITEM_TRADE_H_

#include "GLog.h"

struct TDL_LOG_ITEM;

class GLog_ItemTrade : public GLog
{
private:
	int						m_nCID1;		///< 캐릭터 ID
	vector<TDL_LOG_ITEM>	m_vecTDItem1;	///< 아이템
	int						m_nGold1;		///< 골드

	int						m_nCID2;		///< 캐릭터 ID
	vector<TDL_LOG_ITEM>	m_vecTDItem2;	///< 아이템
	int						m_nGold2;		///< 골드

public:
	GLog_ItemTrade(const string& strDate,	int nCID1, const vector<TDL_LOG_ITEM>& vecTDItem1, int nGold1,
											int nCID2, const vector<TDL_LOG_ITEM>& vecTDItem2, int nGold2);
	~GLog_ItemTrade();

	virtual string MakeQuery();
};

#endif
