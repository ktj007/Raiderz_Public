#include "stdafx.h"
#include "PBaseApplication.h"
#include "SCommandTable_App.h"
#include "PServer.h"
#include "MStatisticsProfiler.h"
#include "PConfig.h"
#include "MStatisticsProfiler.h"
#include "PPMSSystem.h"
#include "PMServerInitLogger.h"
#include "PMSCodes.h"
#include "MFileSystem.h"
#include "MLocale.h"

PBaseApplication::PBaseApplication() : SServerApplication()
{

}

PBaseApplication::~PBaseApplication()
{

}

bool PBaseApplication::OnCreate()
{
	wstring strCmdLine = MLocale::ConvAnsiToUCS2(GetCommandLine().GetCommandLineString().c_str());
	mlog("application_commandline : %s\n", MLocale::ConvUTF16ToAnsi(strCmdLine.c_str()).c_str());

	AddAppCommandTable();

	m_Systems.Create();
	m_Managers.Create();

	m_Systems.pServer->Create();

#ifndef _PUBLISH
	StartServerMonitorEventListener(PConfig::m_strServerName.c_str());
#endif

	return true;
}

void PBaseApplication::OnDestroy()
{
	if (m_Systems.pServer)
	{
		m_Systems.pServer->Destroy();
	}

	m_Managers.Destroy();
	m_Systems.Destroy();

}

bool PBaseApplication::OnUpdate(float fDelta)
{
	if (!m_Systems.pServer->Update(fDelta))
		return false;

	return true;
}

void PBaseApplication::AnalysisProfiling()
{
	MSaveStatisticsProfile(MLocale::ConvUTF16ToAnsi(FILENAME_PROFILE).c_str(), false);
}

void PBaseApplication::StartProfiling()
{
#ifdef _PUBLISH
	return;
#endif

	MInitStatisticsProfile(true);
	MResetStatisticsProfile();
}