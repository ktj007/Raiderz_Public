#include "stdafx.h"
#include "GItemSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GItemManager.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "GDBManager.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GConditionInfo.h"
#include "GCalculator.h"
#include "GCommandCenter.h"
#include "GItemUnequiper.h"
#include "GItemRemover.h"
#include "GEntityNPC.h"
#include "GItemDurabilityUpdater.h"
#include "CSItemHelper.h"
#include "GQuestSystem.h"
#include "GItemUser.h"
#include "GInteractionSystem.h"
#include "GItemEquiper.h"
#include "GItemAdder.h"
#include "GItemActableChecker.h"
#include "GItemHolder.h"
#include "GItemMover.h"
#include "GItemSorter.h"
#include "GItemEnchant.h"
#include "GItemDyer.h"

#include "GItemMoverForDBTask.h"
#include "GItemEquiperForDBTask.h"
#include "GItemAddAndRemoverForInfo.h"
#include "GItemSystemSetterForTest.h"



GItemSystem::GItemSystem()
{
	m_pEquiper = new GItemEquiper;
	m_pUnequiper = new GItemUnequiper;
	m_pAdder = new GItemAdder;
	m_pRemover = new GItemRemover;	
	m_pUser = new GItemUser;
	m_pMover = new GItemMover;
	m_pDurabilityUpdater = new GItemDurabilityUpdater;
	m_pActableChecker = new GItemActableChecker;
	m_pAddAndRemoverForInfo = new GItemAddAndRemoverForInfo;
	m_pSorter = new GItemSorter;
	m_pEnchant = new GItemEnchant;
	m_pDyer = new GItemDyer;

	m_pMoverForDBTask = new GItemMoverForDBTask;	
	m_pEquiperForDBTask = new GItemEquiperForDBTask;

	m_pSetterForTest = new GItemSystemSetterForTest(this);	
}

GItemSystem::~GItemSystem()
{
	SAFE_DELETE(m_pEquiper);
	SAFE_DELETE(m_pUnequiper);
	SAFE_DELETE(m_pAdder);
	SAFE_DELETE(m_pRemover);	
	SAFE_DELETE(m_pUser);
	SAFE_DELETE(m_pMover);
	SAFE_DELETE(m_pDurabilityUpdater);
	SAFE_DELETE(m_pActableChecker);
	SAFE_DELETE(m_pAddAndRemoverForInfo);
	SAFE_DELETE(m_pSorter);
	SAFE_DELETE(m_pEnchant);
	SAFE_DELETE(m_pDyer);

	SAFE_DELETE(m_pMoverForDBTask);
	SAFE_DELETE(m_pEquiperForDBTask);

	SAFE_DELETE(m_pSetterForTest);
}

GItemEquiper& GItemSystem::GetEquiper()
{
	return *m_pEquiper;
}

GItemUnequiper& GItemSystem::GetUnequiper()
{
	return *m_pUnequiper;
}

GItemAdder& GItemSystem::GetAdder()
{
	return *m_pAdder;
}

GItemRemover& GItemSystem::GetRemover()
{
	return *m_pRemover;
}

GItemUser& GItemSystem::GetUser()
{
	return *m_pUser;
}

GItemDurabilityUpdater& GItemSystem::GetDurabilityUpdater()
{
	return *m_pDurabilityUpdater;
}

GItemMover& GItemSystem::GetMover()
{
	return *m_pMover;
}

GItemActableChecker& GItemSystem::GetActableChecker()
{
	return *m_pActableChecker;
}

GItemMoverForDBTask& GItemSystem::GetMoverForDBTask()
{
	return *m_pMoverForDBTask;
}

GItemEquiperForDBTask& GItemSystem::GetEquiperForDBTask()
{
	return *m_pEquiperForDBTask;
}

GItemAddAndRemoverForInfo& GItemSystem::GetAddAndRemoverForInfo()
{
	return *m_pAddAndRemoverForInfo;
}

GItemSorter& GItemSystem::GetSorter()
{
	return *m_pSorter;
}

GItemEnchant& GItemSystem::GetEnchant()
{
	return *m_pEnchant;
}

GItemDyer& GItemSystem::GetDyer()
{
	return *m_pDyer;
}

GItemSystemSetterForTest& GItemSystem::GetSetterForTest()
{
	return *m_pSetterForTest;
}