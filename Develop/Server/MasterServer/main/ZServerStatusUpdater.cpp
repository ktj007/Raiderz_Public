#include "StdAfx.h"
#include "ZServerStatusUpdater.h"
#include "ZConfig.h"
#include "ZServerSystems.h"
#include "ZDBManager.h"
#include "ZPlayerManager.h"
#include "MiNetHelper.h"
#include "ZServer.h"
#include "ZServableChecker.h"
#include "MLocale.h"


ZServerStatusUpdater::ZServerStatusUpdater(ZDBManager* pDBManager)
: m_prefDBManager(pDBManager)
{
}

ZServerStatusUpdater::~ZServerStatusUpdater()
{
}

void ZServerStatusUpdater::Start(const std::wstring& strServerVersion)
{
	wstring strIP = MLocale::ConvAnsiToUTF16(MiNetHelper::GetHostIPString().c_str());

	if (!m_prefDBManager->ServerStatusStart(ZConfig::m_nMyWorldID, ZConfig::m_nMyServerID, ZConfig::m_strServerName
		, strServerVersion, strIP.c_str(), ZConfig::m_nPort, ZConfig::m_nMaxUser, ZConfig::m_nServerType
		, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC))
	{
		MLog3("Start ServerStatus failed.\n");
	}
}

void ZServerStatusUpdater::OnUpdate(const float fDelta)
{
	// 서버 상태 갱신
	m_prefDBManager->ServerStatusUpdate(ZConfig::m_nMyWorldID, ZConfig::m_nMyServerID, gmgr.pPlayerManager->GetObjectCount(), gsys.pServer->GetServableChecker().IsServable());
}
