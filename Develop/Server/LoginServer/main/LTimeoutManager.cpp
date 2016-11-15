#include "stdafx.h"
#include "LTimeoutManager.h"
#include "LTimeoutWatcher.h"
#include "LMoveServerTimeoutHandler.h"
#include "LConfig.h"

LTimeoutManager::LTimeoutManager()
{
	m_pSelMyCharWatcher = new LTimeoutWatcher;
	m_pMoveServerWatcher = new LTimeoutWatcher;
	m_pMoveServerTimeoutHandler = new LMoveServerTimeoutHandler;
}

LTimeoutManager::~LTimeoutManager()
{
	SAFE_DELETE(m_pMoveServerWatcher);
	SAFE_DELETE(m_pSelMyCharWatcher);
	SAFE_DELETE(m_pMoveServerTimeoutHandler);
}

void LTimeoutManager::InitWatcher(void)
{
	m_pSelMyCharWatcher->SetTimeoutPeriod(LConfig::m_nCommandTimeout);
	m_pMoveServerWatcher->SetTimeoutPeriod(LConfig::m_nMoveServerTimeout);
}

void LTimeoutManager::Update(void)
{
	m_pSelMyCharWatcher->UpdateTimeout();
	m_pMoveServerWatcher->UpdateTimeout();
}

void LTimeoutManager::StopAllWatching(MUID uidPlayer)
{
	m_pSelMyCharWatcher->RemovePlayer(uidPlayer);
	m_pMoveServerWatcher->RemovePlayer(uidPlayer);
}

void LTimeoutManager::StartWatchingSelMyCharTimeout(MUID uidPlayer, LTimeoutHandler* pHandler)
{
	m_pSelMyCharWatcher->AddPlayer(uidPlayer, pHandler);
}

void LTimeoutManager::StopWatchingSelMyCharTimeout(MUID uidPlayer)
{
	m_pSelMyCharWatcher->RemovePlayer(uidPlayer);
}

void LTimeoutManager::StartWatchingMoveServerTimeout(MUID uidPlayer)
{
	m_pMoveServerWatcher->AddPlayer(uidPlayer, m_pMoveServerTimeoutHandler);
}

void LTimeoutManager::StopWatchingMoveServerTimeout(MUID uidPlayer)
{
	m_pMoveServerWatcher->RemovePlayer(uidPlayer);
}