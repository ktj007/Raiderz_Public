#include "StdAfx.h"
#include "GLootSystem.h"
#include "GLootBeginner.h"
#include "GLootShower.h"
#include "GLootLooter.h"
#include "GLootMasterLooter.h"
#include "GLootEnder.h"
#include "GLootModifyHandler.h"
#include "GLootItemGetter.h"
#include "GLootSystemForMasterServer.h"
#include "GLootSystemSetterForTest.h"

#include "GItemRemover.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GGlobal.h"
#include "GInventory.h"
#include "GLootBPart.h"


GLootSystem::GLootSystem()
{	
	m_pBeginner = new GLootBeginner;
	m_pShower = new GLootShower;
	m_pLooter = new GLootLooter;
	m_pMasterLooter = new GLootMasterLooter;
	m_pEnder = new GLootEnder;
	m_pBPart = new GLootBPart;

	m_pModifyHandler = new GLootModifyHandler;
	m_pItemGetter = new GLootItemGetter;

	m_pForMasterServer = new GLootSystemForMasterServer;
	m_pSetterForTest = new GLootSystemSetterForTest(this);
}

GLootSystem::~GLootSystem()
{
	SAFE_DELETE(m_pBeginner);
	SAFE_DELETE(m_pShower);
	SAFE_DELETE(m_pLooter);
	SAFE_DELETE(m_pMasterLooter);
	SAFE_DELETE(m_pEnder);
	SAFE_DELETE(m_pBPart);

	SAFE_DELETE(m_pModifyHandler);
	SAFE_DELETE(m_pItemGetter);

	SAFE_DELETE(m_pForMasterServer);
	SAFE_DELETE(m_pSetterForTest);
}

GLootBeginner& GLootSystem::GetBeginner()
{
	return *m_pBeginner;
}

GLootShower& GLootSystem::GetShower()
{
	return *m_pShower;
}

GLootLooter& GLootSystem::GetLooter()
{
	return *m_pLooter;
}

GLootMasterLooter& GLootSystem::GetMasterLooter()
{
	return *m_pMasterLooter;
}

GLootEnder& GLootSystem::GetEnder()
{
	return *m_pEnder;
}

GLootBPart& GLootSystem::GetBPart()
{
	return *m_pBPart;
}

GLootModifyHandler& GLootSystem::GetModifyHandler()
{
	return *m_pModifyHandler;
}

GLootItemGetter& GLootSystem::GetItemGetter()
{
	return *m_pItemGetter;
}

GLootSystemForMasterServer& GLootSystem::ForMasterServer()
{
	return (*m_pForMasterServer);
}

GLootSystemSetterForTest& GLootSystem::GetSetterForTest()
{
	return (*m_pSetterForTest);
}

bool GLootSystem::PrepareGather(GEntityPlayer* pPlayer, GEntityNPC* pLootingNPC, int nLootID)
{
	if (true == pLootingNPC->IsDie()) return false;

	if (GATHER_RELIC == pLootingNPC->GetNPCInfo()->nGatherType)
	{
		GItem* pItem = pPlayer->GetItemHolder()->GetInventory().GetItemByGatherType(GATHER_RELIC, 0);
		if (NULL == pItem)	return false;
		if (false == gsys.pItemSystem->GetRemover().Remove(pPlayer, SLOTTYPE_INVENTORY, pItem->m_nSlotID, 1)) return false;
	}

	pLootingNPC->doDieByGather(pPlayer, nLootID);

	return true;
}

