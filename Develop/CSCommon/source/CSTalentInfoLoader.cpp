#include "stdafx.h"
#include "CSTalentInfoLoader.h"
#include "CSTalentInfoMgr.h"
#include "CSHitInfoLoader.h"
#include "MDebug.h"
#include "MLocale.h"

bool CSTalentInfoLoader::LoadHitInfo( CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName)
{
	CSHitInfoLoader hitInfoLoader;
	if (hitInfoLoader.LoadHitInfo(pTalentInfoMgr, szFileName) == false)
	{
		mlog("Failed To Load %s\n", MLocale::ConvTCHARToAnsi(szFileName).c_str());
		return false;
	}

	return true;
}