#include "StdAfx.h"
#include "GGameGuard.h"
#include "ggsrv26.h"
#include "GConfig.h"

//////////////////////////////////////////////////////////////////////////
//
// Callbacks
//
//////////////////////////////////////////////////////////////////////////

GGAUTHS_API void NpLog(int mode, char* msg)
{
	mlog("nplog: %s\n", msg);
}

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
	VALID(report);
	mlog3("np version updated [%s] : [%ld] -> [%ld]\n",
		report->nType==1?"GameGuard Ver":"Protocol Num",
		report->dwBefore, report->dwNext);
}


//////////////////////////////////////////////////////////////////////////
//
// GGameGuard
//
//////////////////////////////////////////////////////////////////////////

GGameGuard::GGameGuard(void)
: m_bInit(false)
, m_bEnable(true)
{
}

GGameGuard::~GGameGuard(void)
{
	Fini();
}

bool GGameGuard::Init()
{
	if (m_bInit)
		return false;;

	int nLogFlag = NPLOG_ERROR;
	if (GConfig::m_bGGShowDebugLog)
	{
		nLogFlag |= NPLOG_DEBUG;
	}

	DWORD dwRet = InitGameguardAuth("./nProtect/", GConfig::m_nGGInitNumActive, true, nLogFlag);
	if (dwRet != ERROR_SUCCESS)
	{
		mlog3("GameGuard Init failed(InitGameguardAuth): %d\n", dwRet);
		return false;	
	}

	char szVer[512];
	int nRet = ModuleInfo(szVer, 512);
	if (nRet != NPGG_INFO_SUCCESS)
	{
		mlog3("GameGuard Init failed(ModuleInfo): %d\n", nRet);
		return false;
	}

	dwRet = SetUpdateCondition(GConfig::m_nGGTimeLimit, GConfig::m_nGGCondition);
	if (dwRet != ERROR_SUCCESS)
	{
		mlog3("GameGuard Init failed(SetUpdateCondition): %d\n", dwRet);
		return false;	
	}

	m_bInit = true;
	mlog("GameGuard Init success. (ver: %s)\n", szVer);
	return true;
}

void GGameGuard::Fini()
{
	if (!m_bInit)
		return;

	m_bInit = false;
	mlog("GameGuard finalized success.\n");

	CleanupGameguardAuth();
}

bool GGameGuard::IsEnable() const
{
	return m_bEnable;
}

void GGameGuard::Enable( bool bEnable )
{
	m_bEnable = bEnable;
}