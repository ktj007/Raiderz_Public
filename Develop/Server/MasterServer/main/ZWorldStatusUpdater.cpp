#include "stdafx.h"
#include "ZWorldStatusUpdater.h"
#include "ZDef.h"
#include "ZConfig.h"
#include "MLocale.h"
#include "ZDBManager.h"
#include "ZPlayerManager.h"
#include "ZServer.h"
#include "ZServableChecker.h"
#include "ZConst.h"


static const float WORLD_UPDATE_TICK_SECOND = 30.0f;


ZWorldStatusUpdater::ZWorldStatusUpdater(ZDBManager* pDBManager)
: m_pDBManager(pDBManager)
{
	m_updateRegulator.SetTickTime(WORLD_UPDATE_TICK_SECOND);
	m_updateRegulator.Stop();
}

ZWorldStatusUpdater::~ZWorldStatusUpdater()
{
	m_pDBManager = NULL;
}

void ZWorldStatusUpdater::Start(void)
{	
	m_updateRegulator.Start();

	OnStart();
}

void ZWorldStatusUpdater::Update(float fDeltaSecond)
{
	if (m_updateRegulator.IsReady(fDeltaSecond))
	{
		OnUpdate();
	}
}

void ZWorldStatusUpdater::OnStart(void)
{
	// 월드정보 추가 설정이 있을때만 추가 - 내부 테스트용
	if (!ZConst::INSERT_WORLD_INFO)		return;


	// 월드정보 추가
	ZWorldContext wc;

	wstringstream ss;
	ss << "World#" << ZConfig::m_nMyWorldID;
	wc.strName = ss.str();

	wc.nID = ZConfig::m_nMyWorldID;
	wc.strIP = MLocale::ConvAnsiToUTF16(MiNetHelper::GetHostIPString().c_str());
	wc.nType = 0;
	wc.nMaxPlayerCount = WORLD_PLAYER_LIMIT;
	wc.nServable = false;
	wc.nState = WORLD_STATE_OPEN;
	wc.nMaxElapsedTime = (int)WORLD_UPDATE_TICK_SECOND * 2;

	m_pDBManager->WorldInsert(wc);
}

void ZWorldStatusUpdater::OnUpdate(void)
{
	int nID = ZConfig::m_nMyWorldID;
	int nCount = (int)gmgr.pPlayerManager->GetObjectCount();
	bool isServable = gsys.pServer->GetServableChecker().IsServable();	// 로그인 서버의 상태와 시차가 있다.

	m_pDBManager->WorldUpdate(nID, nCount, isServable);
}
