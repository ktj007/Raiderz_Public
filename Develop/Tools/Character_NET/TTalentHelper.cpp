#include "stdafx.h"
#include "TTalentHelper.h"
#include "MainApp.h"

TTalentInfo* TTalentHelper::GetTalentInfo( int nID, int nTalentMode )
{
	return g_pMainApp->m_TTalentInfoMgr.Get(nID, nTalentMode, true);
}

CSTalentInfo* TTalentHelper::GetTalentInfo( string strTalentName )
{
	for( TTalentInfoMgr::iterator itor = g_pMainApp->m_TTalentInfoMgr.begin();
		itor != g_pMainApp->m_TTalentInfoMgr.end(); ++itor)
	{
		CSTalentInfo* pTalentInfo = (CSTalentInfo*)(*itor).second;

		string str;
		GetFormatString(str, "%s(%d)",
			pTalentInfo->m_szName.c_str(),
			pTalentInfo->m_nID);

		if(!stricmp(strTalentName.c_str(), str.c_str()))
		{
			return pTalentInfo;
		}
	}

	return NULL;
}