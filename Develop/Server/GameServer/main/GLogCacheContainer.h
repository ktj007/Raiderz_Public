#ifndef _GLOG_CACHE_CONTAINER_H
#define _GLOG_CACHE_CONTAINER_H

#include "GItemLogCache.h"
#include "GMoneyLogCache.h"
#include "GCraftLogCache.h"
#include "GQuestItemLogCache.h"
#include "GItemMergeLogCache.h"
#include "GNpcShopLogCache.h"
#include "GXPLogCache.h"
#include "GItemDeleteLogCache.h"
#include "GNpcKillerData.h"
#include "GPartyLogCache.h"
#include "GItemMoveLogCache.h"

class GLogCacheContainer : public MTestMemPool<GLogCacheContainer>
{
public :
	void Update();
	void Release();

	GItemLogCache				m_ItemLootLogCache;
	GMoneyLogCache				m_MoneyLogCache;
	GCraftLogCache				m_CraftLogCache;
	GQuestItemLogCache			m_QuestItemLogCache;
	GItemMergeAndSplitLogCache	m_ItemMergeAndSplitLogCacahe;
	GNpcShopLogCache			m_NpcShopLogCache;
	GXPLogCache					m_XPLogCache;
	GItemDeleteLogCache			m_ItemDeleteLogCache;
	GPartyLogCache				m_PartyLogcache;
	GItemMoveLogCache			m_ItemMoveLogCache;
};


#endif