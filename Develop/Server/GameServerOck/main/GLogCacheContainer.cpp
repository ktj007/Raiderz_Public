#include "stdafx.h"
#include "GLogCacheContainer.h"


void GLogCacheContainer::Update()
{
	if (m_ItemLootLogCache.IsHaveLog() && m_ItemLootLogCache.IsOverflowTime())
		m_ItemLootLogCache.PostLog();
	if (m_MoneyLogCache.IsHaveLog() && m_MoneyLogCache.IsOverflowTime())
		m_MoneyLogCache.PostLog();
	if (m_CraftLogCache.IsHaveLog() && m_CraftLogCache.IsOverflowTime())
		m_CraftLogCache.PostLog();
	if (m_QuestItemLogCache.IsHaveLog() && m_QuestItemLogCache.IsOverflowTime())
		m_QuestItemLogCache.PostLog();
	if (m_ItemMergeAndSplitLogCacahe.IsHaveLog() && m_ItemMergeAndSplitLogCacahe.IsOverflowTime())
		m_ItemMergeAndSplitLogCacahe.PostLog();
	if (m_NpcShopLogCache.IsHaveLog() && m_NpcShopLogCache.IsOverflowTime())
		m_NpcShopLogCache.PostLog();
	if (m_XPLogCache.IsHaveLog() && m_XPLogCache.IsOverflowTime())
		m_XPLogCache.PostLog();
	if (m_ItemDeleteLogCache.IsHaveLog() && m_ItemDeleteLogCache.IsOverflowTime())
		m_ItemDeleteLogCache.PostLog();
	if (m_PartyLogcache.IsHaveLog() && m_PartyLogcache.IsOverflowTime())
		m_PartyLogcache.PostLog();
	if (m_ItemMoveLogCache.IsHaveLog() && m_ItemMoveLogCache.IsOverflowTime())
		m_ItemMoveLogCache.PostLog();
}

void GLogCacheContainer::Release()
{
	m_ItemLootLogCache.PostLog();
	m_MoneyLogCache.PostLog();
	m_CraftLogCache.PostLog();
	m_QuestItemLogCache.PostLog();
	m_ItemMergeAndSplitLogCacahe.PostLog();
	m_NpcShopLogCache.PostLog();
	m_XPLogCache.PostLog();
	m_ItemDeleteLogCache.PostLog();
	m_PartyLogcache.PostLog();
	m_ItemMoveLogCache.PostLog();
}
