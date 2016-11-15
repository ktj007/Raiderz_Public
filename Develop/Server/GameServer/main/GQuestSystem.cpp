#include "stdafx.h"
#include "GQuestSystem.h"

#include "GQuestUIShower.h"
#include "GQuestGiver.h"
#include "GQuestCompleter.h"
#include "GQuestRewarder.h"
#include "GQuestRewarderForDBTask.h"

#include "GQuestSharer.h"
#include "GQuestCanceler.h"
#include "GQuestFailer.h"
#include "GQuestAddItemCollector.h"
#include "GQuestRemoveItemCollector.h"

#include "GQuestVarUpdater.h"
#include "GQObjUpdater.h"
#include "GQObjUpdaterForDBTask.h"
#include "GQObjInteractor.h"

#include "GQuestReseter.h"

#include "GQuestSystemSetterForTest.h"


GQuestSystem::GQuestSystem()
{
	m_pUIShower				= new GQuestUIShower;
	m_pGiver				= new GQuestGiver;
	m_pCompleter			= new GQuestCompleter;
	m_pRewarder				= new GQuestRewarder;
	m_pRewardForDBTask		= new GQuestRewarderForDBTask;	

	m_pSharer				= new GQuestSharer;
	m_pCanceler				= new GQuestCanceler;
	m_pFailer				= new GQuestFailer;
	m_pAddItemCollector		= new GQuestAddItemCollector;
	m_pRemoveItemCollector	= new GQuestRemoveItemCollector;

	m_pQuestVarUpdater		= new GQuestVarUpdater;
	m_pQObjUpdater			= new GQObjUpdater;
	m_pQObjUpdaterForDBTask	= new GQObjUpdaterForDBTask;
	m_pQObjInteractor		= new GQObjInteractor;

	m_pQuestReseter			= new GQuestReseter;

	m_pSetterForTest		= new GQuestSystemSetterForTest(this);
}

GQuestSystem::~GQuestSystem()
{
	SAFE_DELETE(m_pUIShower);
	SAFE_DELETE(m_pGiver);
	SAFE_DELETE(m_pCompleter);
	SAFE_DELETE(m_pRewarder);
	SAFE_DELETE(m_pRewardForDBTask);

	SAFE_DELETE(m_pSharer);
	SAFE_DELETE(m_pCanceler);
	SAFE_DELETE(m_pFailer);
	SAFE_DELETE(m_pAddItemCollector);
	SAFE_DELETE(m_pRemoveItemCollector);

	SAFE_DELETE(m_pQuestVarUpdater);
	SAFE_DELETE(m_pQObjUpdater);
	SAFE_DELETE(m_pQObjUpdaterForDBTask);
	SAFE_DELETE(m_pQObjInteractor);

	SAFE_DELETE(m_pQuestReseter);

	SAFE_DELETE(m_pSetterForTest);
}

GQuestUIShower& GQuestSystem::GetUIShower()
{
	return *m_pUIShower;
}

GQuestGiver& GQuestSystem::GetGiver()
{
	return *m_pGiver;
}

GQuestCompleter& GQuestSystem::GetCompleter()
{
	return *m_pCompleter;
}

GQuestRewarder& GQuestSystem::GetRewarder()
{
	return *m_pRewarder;
}

GQuestRewarderForDBTask& GQuestSystem::GetRewarderForDBTask()
{
	return *m_pRewardForDBTask;
}

GQuestSharer& GQuestSystem::GetSharer()
{
	return *m_pSharer;
}

GQuestCanceler& GQuestSystem::GetCanceler()
{
	return *m_pCanceler;
}

GQuestFailer& GQuestSystem::GetFailer()
{
	return *m_pFailer;
}

GQuestAddItemCollector& GQuestSystem::GetAddItemCollector()
{
	return *m_pAddItemCollector;
}

GQuestRemoveItemCollector& GQuestSystem::GetRemoveItemCollector()
{
	return *m_pRemoveItemCollector;
}

GQuestVarUpdater& GQuestSystem::GetQuestVarUpdater()
{
	return *m_pQuestVarUpdater;
}

GQObjUpdater& GQuestSystem::GetQObjUpdater()
{
	return *m_pQObjUpdater;
}

GQObjUpdaterForDBTask& GQuestSystem::GetQObjUpdaterForDBTask()
{
	return *m_pQObjUpdaterForDBTask;
}

GQObjInteractor& GQuestSystem::GetQObjInteractor()
{
	return *m_pQObjInteractor;
}

GQuestReseter& GQuestSystem::GetReseter()
{
	return *m_pQuestReseter;
}

GQuestSystemSetterForTest& GQuestSystem::GetSetterForTest()
{
	return *m_pSetterForTest;
}