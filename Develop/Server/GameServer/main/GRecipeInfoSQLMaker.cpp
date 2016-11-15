#include "stdafx.h"
#include "GRecipeInfoSQLMaker.h"
#include "CSRecipeInfoMgr.h" 
#include "GRecipeInfoMgr.h"
#include "CSRecipeInfo.h"


bool GRecipeInfoSQLMaker::BuildSQL()
{
	if (NULL == m_pMgr)
		return false;

	wchar_t szBuf[1024];

	MAP_CSRECIPEINFO& rm = m_pMgr->GetContainer();
	for (MAP_CSRECIPEINFO::iterator it = rm.begin()
		; it != rm.end()
		; ++it)
	{
		memset(szBuf, 0, sizeof(szBuf));
		
		_snwprintf(szBuf, 1024, L"MERGE INTO dbo.RZ_RECIPE_INFO r \
								USING (SELECT %d, %d, %d) AS ri(RECP_ID, PROD_ITEM_ID, AMT) \
								ON r.RECP_ID = ri.RECP_ID \
								WHEN MATCHED THEN \
									UPDATE SET r.PROD_ITEM_ID = ri.PROD_ITEM_ID, r.AMT = ri.AMT \
								WHEN NOT MATCHED THEN \
									INSERT (RECP_ID, PROD_ITEM_ID, AMT) \
									VALUES (ri.RECP_ID, ri.PROD_ITEM_ID, ri.AMT); \n\n"
									, it->second->m_nID
									, it->second->m_nProductItemID
									, it->second->m_nAmount);
		m_vSQL.push_back(szBuf);
	}

	return true;
}