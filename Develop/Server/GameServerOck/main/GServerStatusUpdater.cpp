#include "StdAfx.h"
#include "GServerStatusUpdater.h"
#include "MTime.h"
#include "GDef.h"
#include "GConfig.h"
#include "GServerSystems.h"
#include "GDBManager.h"
#include "GServer.h"
#include "MiNetHelper.h"
#include "GServableChecker.h"
#include "GConst.h"

GServerStatusUpdater::GServerStatusUpdater(GDBManager* pDBManager)
: m_prefDBManager(pDBManager)
{
}

GServerStatusUpdater::~GServerStatusUpdater()
{
}

void GServerStatusUpdater::Start(const std::wstring& strServerVersion)
{
	wstring strIP;
	strIP = MLocale::ConvAnsiToUCS2(MiNetHelper::GetHostIPString().c_str());

	if (!m_prefDBManager->ServerStatusStart(GConfig::m_nMyWorldID, GConfig::m_nMyServerID, GConfig::m_strServerName
		, strServerVersion, strIP.c_str(), GConfig::m_nPort, GConfig::m_nMaxUser, GConfig::m_nServerType
		, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC))
	{
		MLog3(L"Start ServerStatus failed.\n");
	}
}

void GServerStatusUpdater::OnUpdate(const float fDelta)
{
	m_prefDBManager->ServerStatusUpdate(GConfig::m_nMyWorldID, GConfig::m_nMyServerID, gsys.pServer->GetServerInfo().nNowPlayer, gsys.pServer->GetServableChecker().IsServable());
}
