#include "stdafx.h"
#include "GEmblemInfoSQLMaker.h"
#include "GEmblemInfoMgr.h"
#include "CSEmblemInfo.h"

GEmblemInfoSQLMaker::GEmblemInfoSQLMaker( const std::wstring& strFileName ) : GSQLMaker(true, true), m_strFielName(strFileName)
{

}


bool GEmblemInfoSQLMaker::BuildSQL()
{
	if (!LoadXml())
		return false;

	for (EmblemMap::iterator it = m_XmlLoader.GetContainer().begin()
		; it != m_XmlLoader.GetContainer().end()
		; ++it)
	{
		CSEmblemInfo* pInfo = it->second;

		CString strSQL;
		strSQL.Format(L"MERGE INTO dbo.RZ_EMBLEM_INFO e USING (SELECT %d) src(EMBLEM_ID) ON e.EMBLEM_ID = src.EMBLEM_ID \
					  WHEN NOT MATCHED THEN INSERT(EMBLEM_ID, NAME) VALUES (src.EMBLEM_ID, N'%d');\n", pInfo->nID, pInfo->nID);

		m_vSQL.push_back(strSQL.GetBuffer());
	}

	return true;
}

bool GEmblemInfoSQLMaker::LoadXml()
{
	return m_XmlLoader.Load(m_strFielName.c_str());
}