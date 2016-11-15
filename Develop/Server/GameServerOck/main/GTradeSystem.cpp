#include "StdAfx.h"
#include "GTradeSystem.h"
#include "GTradeRequester.h"
#include "GTradeAcceptor.h"
#include "GTradeItemPutUper.h"
#include "GTradeItemPutDowner.h"
#include "GTradeSilverPutUper.h"
#include "GTradeConfirmer.h"
#include "GTradeCanceler.h"
#include "GTradeExchanger.h"
#include "GTradeChecker.h"

#include "GTradeExchangerForDBTask.h"


GTradeSystem::GTradeSystem()
{
	m_pTradeRequester = new GTradeRequester;
	m_pTradeAcceptor = new GTradeAcceptor;	
	m_pTradeItemPutUper = new GTradeItemPutUper;
	m_pTradeItemPutDowner = new GTradeItemPutDowner;
	m_pTradeMoneyPutUper = new GTradeMoneyPutUper;
	m_pTradeCanceler = new GTradeCanceler;
	m_pTradeConfirmer = new GTradeConfirmer;
	m_pTradeExchanger = new GTradeExchanger;
	m_pTradeChecker = new GTradeChecker;

	m_pTradeExchangerForDBTask = new GTradeExchangerForDBTask;
}

GTradeSystem::~GTradeSystem()
{
	SAFE_DELETE(m_pTradeRequester);
	SAFE_DELETE(m_pTradeAcceptor);
	SAFE_DELETE(m_pTradeItemPutUper);
	SAFE_DELETE(m_pTradeItemPutDowner);
	SAFE_DELETE(m_pTradeMoneyPutUper);
	SAFE_DELETE(m_pTradeCanceler);
	SAFE_DELETE(m_pTradeConfirmer);
	SAFE_DELETE(m_pTradeExchanger);
	SAFE_DELETE(m_pTradeChecker);

	SAFE_DELETE(m_pTradeExchangerForDBTask);
}

GTradeRequester& GTradeSystem::GetRequester()
{
	return *m_pTradeRequester;
}

GTradeAcceptor& GTradeSystem::GetAcceptor()
{
	return *m_pTradeAcceptor;
}

GTradeItemPutUper& GTradeSystem::GetItemPutUper()
{
	return *m_pTradeItemPutUper;
}

GTradeItemPutDowner& GTradeSystem::GetItemPutDowner()
{
	return *m_pTradeItemPutDowner;
}

GTradeMoneyPutUper& GTradeSystem::GetMoneyPutUper()
{
	return *m_pTradeMoneyPutUper;
}

GTradeCanceler& GTradeSystem::GetCanceler()
{
	return *m_pTradeCanceler;
}

GTradeConfirmer& GTradeSystem::GetConfirmer()
{
	return *m_pTradeConfirmer;
}

GTradeExchanger& GTradeSystem::GetExchanger()
{
	return *m_pTradeExchanger;
}

GTradeChecker& GTradeSystem::GetChecker()
{
	return *m_pTradeChecker;
}

GTradeExchangerForDBTask& GTradeSystem::GetExchangerForDBTask()
{
	return *m_pTradeExchangerForDBTask;
}