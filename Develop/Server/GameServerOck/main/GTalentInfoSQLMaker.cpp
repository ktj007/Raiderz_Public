
#include "stdafx.h"
#include "GTalentInfoSQLMaker.h"
#include "GTalentInfoMgr.h"
#include "GExportDBStringTable.h"
#include "GGlobal.h"


GTalentInfoSQLMaker::GTalentInfoSQLMaker(GTalentInfoMgr* pMgr) : GSQLMaker(true, true), m_pMgr(pMgr)
{
	
}

GTalentInfoSQLMaker::~GTalentInfoSQLMaker()
{

}

bool GTalentInfoSQLMaker::BuildSQL()
{
	if (NULL == m_pMgr)
		return false;

	wchar_t szSQL[512];
	wchar_t strBadWord = L'%';
	
	for (GTalentInfoMgr::iterator it = m_pMgr->begin(); it != m_pMgr->end(); ++it)
	{
		memset(szSQL, 0, sizeof(szSQL));
		wchar_t szTalentName[64] = {0,};

		wstring strName = gmgr.pExportDBStringTable->GetString(it->second->m_szName);

		for (size_t i = 0; i < strName.length(); ++i)
		{
			if (strBadWord == strName[i])
			{
				strName[i] = L' ';
			}
		}

		_snwprintf_s(szTalentName, 64, 63, L"%s", strName.c_str());

		_snwprintf(szSQL, 512, L"MERGE INTO dbo.RZ_TALENT_INFO t\
								 USING (SELECT %d, N'%s', %d, %d, %d) s(TALENT_ID, NAME, CONSUME_TP, UNTRAINABLE, STYLE)\
								 ON t.TALENT_ID = s.TALENT_ID\
								 WHEN MATCHED THEN UPDATE SET t.NAME = s.NAME, t.CONSUME_TP = s.CONSUME_TP, t.UNTRAINABLE = s.UNTRAINABLE, t.STYLE = s.STYLE\
								 WHEN NOT MATCHED THEN INSERT (TALENT_ID, NAME, CONSUME_TP, UNTRAINABLE, STYLE) VALUES (s.TALENT_ID, s.NAME, s.CONSUME_TP, s.UNTRAINABLE, s.STYLE);\n\n"
								 , it->second->m_nID, szTalentName, CONSUME_TP_FOR_LEARN_TALENT, it->second->m_bUntrainable, it->second->m_nStyle);

		m_vSQL.push_back(szSQL);		
	}

	return true;
}
