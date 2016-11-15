#include "StdAfx.h"
#include "GNPCSetterForTest.h"
#include "GEntityNPC.h"
#include "GNPCRewarder.h"
#include "GNPCLoot.h"
#include "GDropList.h"

#define _SET(original, mock)	SAFE_DELETE(original); original = mock;

GNPCSetterForTest::GNPCSetterForTest(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
}

void GNPCSetterForTest::SetNPCRewarder(GNPCRewarder* pNPCRewarder)
{
	_SET(m_pOwner->m_pNPCRewarder, pNPCRewarder);
}

void GNPCSetterForTest::SetNPCLoot(GNPCLoot* pNPCLoot)
{
	_SET(m_pOwner->m_pNPCLoot, pNPCLoot);
}

void GNPCSetterForTest::SetDropList(GDropList* pDropList)
{
	_SET(m_pOwner->m_pNPCLoot->m_pDropList, pDropList);
}