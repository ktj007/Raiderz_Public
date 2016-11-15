#include "stdafx.h"
#include "ZBaseApplication.h"
#include "SCommandTable_App.h"
#include "ZServer.h"
#include "MStatisticsProfiler.h"
#include "ZConfig.h"
#include "ZConst.h"
#include "ZGlobal.h"
#include "ZPMSSystem.h"
#include "PMServerInitLogger.h"
#include "PMSCodes.h"

ZBaseApplication::ZBaseApplication()
{

}

ZBaseApplication::~ZBaseApplication()
{

}

bool ZBaseApplication::OnCreate()
{
	USES_CONVERSION_EX;

	wstring strCmdLine = A2W_EX(GetCommandLine().GetCommandLineString().c_str(), 0);
	mlog("application_commandline : %s\n", W2A_EX(strCmdLine.c_str(), 0));
	
	AddAppCommandTable();

	m_Systems.Create();
	m_Managers.Create();

	m_Systems.pServer->Create();

#ifndef _PUBLISH
	StartServerMonitorEventListener(ZConfig::m_strServerName.c_str());
#endif

	return true;
}

bool ZBaseApplication::OnUpdate( float fDelta )
{
	m_Systems.pServer->Update(fDelta);
	return true;
}

void ZBaseApplication::OnDestroy()
{
	if (m_Systems.pServer)
	{
		m_Systems.pServer->Destroy();
	}

	m_Managers.Destroy();
	m_Systems.Destroy();
}

void ZBaseApplication::StartProfiling()
{
#ifdef _PUBLISH
	return;
#endif

	MInitStatisticsProfile(true);
	MResetStatisticsProfile();
}

void ZBaseApplication::AnalysisProfiling()
{
	USES_CONVERSION_EX;
	MSaveStatisticsProfile(W2A_EX(FILENAME_PROFILE, 0), false);
}