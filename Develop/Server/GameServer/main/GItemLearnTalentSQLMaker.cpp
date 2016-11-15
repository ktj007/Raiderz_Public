#include "stdafx.h"
#include "GItemLearnTalentSQLMaker.h"
#include "GItemManager.h"



GItemLearnTalentMaker::GItemLearnTalentMaker( GItemManager* pMgr ) : GSQLMaker(false, true), m_pMgr(pMgr)
{

}

GItemLearnTalentMaker::~GItemLearnTalentMaker()
{

}

bool GItemLearnTalentMaker::BuildSQL()
{
	if (NULL == m_pMgr)
		return false;

	m_vSQL.push_back(L"DELETE dbo.RZ_ITEM_LEARN_TALENT WITH (TABLOCK);\n");

	wchar_t szBuf[1024];

	for (GItemManager::GItemDataMap::iterator it = m_pMgr->Begin()
		; it != m_pMgr->End()
		; ++it)
	{
		GItemData* pItem = it->second;

		if (USABLE_TALENT_TRAIN != pItem->m_nUsableType)
			continue;

		for (size_t i = 0
			; i < pItem->m_vecUsableParam.size()
			; ++i)
		{
			_snwprintf_s(szBuf, 1024, 1021, L"INSERT INTO dbo.RZ_ITEM_LEARN_TALENT(ITEM_ID, TALENT_ID) VALUES (%d, %d);\n"
				, pItem->m_nID, pItem->m_vecUsableParam[i]);

			m_vSQL.push_back(szBuf);
		}
	}

	return true;
}