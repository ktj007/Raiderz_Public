#include "stdafx.h"
#include "GQuestInfoSQLMaker.h"
#include "GQuestInfoMgr.h"
#include "GQuestInfo.h"
#include "GGlobal.h"
#include "GExportDBStringTable.h"

GQuestInfoSQLMaker::GQuestInfoSQLMaker( GQuestInfoMgr* pMgr ) : GSQLMaker(true, true), m_pMgr(pMgr)
{

}


bool GQuestInfoSQLMaker::BuildSQL()
{
	if (NULL == m_pMgr)
		return false;

	const static uint8 NORMAL_QUEST		= 1;
	const static uint8 CHALLENGE_QUEST	= 2;

	wchar_t szBuf[1024];

	vector<GQuestInfo*> vecQuestInfo = m_pMgr->GetAllQuestInfo();
	for each (GQuestInfo* pQuestInfo in vecQuestInfo)
	{
		memset(szBuf, 0, sizeof(szBuf));

		uint8 nType = NORMAL_QUEST;
		if (pQuestInfo->bChallengerQuest)
		{
			nType = CHALLENGE_QUEST;
		}
	
		

		_snwprintf(szBuf, 1024, L"MERGE INTO dbo.RZ_QUEST_INFO q \
							   USING (SELECT %d, N'%s', %d)s(QUEST_ID, NAME, [TYPE]) \
							   ON q.QUEST_ID = s.QUEST_ID \
							   WHEN MATCHED THEN UPDATE SET q.NAME = s.NAME \
							   WHEN NOT MATCHED THEN INSERT(QUEST_ID, NAME, [TYPE]) VALUES (s.QUEST_ID, s.NAME, s.[TYPE]);\n\n"
							   , pQuestInfo->nID
							   , gmgr.pExportDBStringTable->GetString(pQuestInfo->strTitle).c_str()
							   , nType);
		
		m_vSQL.push_back(szBuf);
	}
	
	return true;
}

GQuestInfoSQLMaker::~GQuestInfoSQLMaker()
{

}
