#include "StdAfx.h"
#include "LServerStatusUpdater.h"
#include "LDef.h"
#include "LConfig.h"
#include "LDBManager.h"
#include "LServer.h"
#include "CAppHelper.h"
#include "STypes.h"
#include "LPlayerObjectManager.h"
#include "LServableChecker.h"
#include "MLocale.h"

LServerStatusUpdater::LServerStatusUpdater(LDBManager* pDBManager)
: m_prefDBManager(pDBManager)
{
}

LServerStatusUpdater::~LServerStatusUpdater()
{
}

void LServerStatusUpdater::Start(const std::wstring& strServerVersion)
{
	wstring strIP = MLocale::ConvAnsiToUTF16(MiNetHelper::GetHostIPString().c_str()).c_str();


	if (!m_prefDBManager->ServerStatusStart(LConfig::m_nWorldID, LConfig::m_nServerID, LConfig::m_strServerName.c_str(), strServerVersion, strIP.c_str(), LConfig::m_nPort, DEFAULT_MAX_PLAYER, SERVER_LOGIN, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC, MAX_UPDATE_SERVERSTATUS_ELAPSEDTIME_SEC))
	{
		MLog3("Start ServerStatus failed.\n");
	}
}

void LServerStatusUpdater::OnUpdate(const float fDelta)
{
	m_prefDBManager->ServerStatusUpdate(LConfig::m_nWorldID, LConfig::m_nServerID, gmgr.pPlayerObjectManager->GetClientsCount(), gsys.pServer->GetServableChecker().IsServable());
}
