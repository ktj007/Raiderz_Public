#include "stdafx.h"
#include "GItemInfoSQLMaker.h"
#include "GItemManager.h"
#include "GExportDBStringTable.h"
#include "GGlobal.h"

GItemInfoSQLMaker::GItemInfoSQLMaker( GItemManager* pMgr ) : GSQLMaker(true, true), m_pMgr(pMgr)
{

}

GItemInfoSQLMaker::~GItemInfoSQLMaker()
{

}

bool GItemInfoSQLMaker::BuildSQL()
{
	if (NULL == m_pMgr)
		return false;

	wchar_t szBuf[1024];

	for (GItemManager::GItemDataMap::iterator it = m_pMgr->Begin()
		; it != m_pMgr->End()
		; ++it)
	{
		memset(szBuf, 0, sizeof(szBuf));

		_snwprintf(szBuf, 1024, L"MERGE INTO dbo.RZ_ITEM_INFO i\
							   USING (SELECT %d, '%s', %d, %d, %d, %d, %d) s(ITEM_ID, NAME, [TYPE], SLOT, TIER, MAX_DURA, COLOR)\
							   ON i.ITEM_ID = s.ITEM_ID\
							   WHEN MATCHED THEN UPDATE SET i.NAME = s.NAME, i.[TYPE] = s.[TYPE], i.SLOT = s.SLOT, i.TIER = s.TIER, i.MAX_DURA = s.MAX_DURA, i.COLOR = s.COLOR\
							   WHEN NOT MATCHED THEN INSERT (ITEM_ID, NAME, [TYPE], SLOT, TIER, MAX_DURA, COLOR) \
							   VALUES (s.ITEM_ID, s.NAME, s.[TYPE], s.SLOT, s.TIER, s.MAX_DURA, s.COLOR);\n\n\0"
							   , it->second->m_nID
							   , gmgr.pExportDBStringTable->GetString(it->second->m_strName).c_str()
							   , it->second->m_ItemType
							   , it->second->m_nItemSlot
							   , it->second->m_nTier
							   , it->second->m_nMaxDurability
							   , it->second->m_nTexColor);

		m_vSQL.push_back(szBuf);
	}

	return true;
}