#include "StdAfx.h"
#include "PServerStatusUpdater.h"
#include "PConfig.h"
#include "PDBManager.h"
#include "MiNetHelper.h"
#include "PServer.h"
#include "PServableChecker.h"
#include "MLocale.h"

PServerStatusUpdater::PServerStatusUpdater(PDBManager* pDBManager)
: m_prefDBManager(pDBManager)
{
}

PServerStatusUpdater::~PServerStatusUpdater()
{
}

void PServerStatusUpdater::Start(const std::wstring& strServerVersion)
{
	wstring strIP;
	strIP = MLocale::ConvAnsiToUCS2(MiNetHelper::GetHostIPString().c_str()).c_str();

	if (!m_prefDBManager->ServerStatusStart(PConfig::m_nMyWorldID, PConfig::m_nMyServerID, PConfig::m_strServerName
		, strServerVersion, strIP.c_str(), PConfig::m_nPort, 0/*PConfig::m_nMaxUser*/, PConfig::m_nServerType
		, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC))
	{
		MLog3("Start ServerStatus failed.\n");
	}
}

void PServerStatusUpdater::OnUpdate(const float fDelta)
{
	m_prefDBManager->ServerStatusUpdate(PConfig::m_nMyWorldID, PConfig::m_nMyServerID, 0/*No Player Session*/, gsys.pServer->GetServableChecker().IsServable());
}
