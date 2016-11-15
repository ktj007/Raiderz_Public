#include "StdAfx.h"
#include "GNPCLoot.h"
#include "GDropList.h"

GNPCLoot::GNPCLoot(GEntityNPC* pOwner)
: m_nBPartLootID(INVALID_ID)
{
	m_pDropList = new GDropList(pOwner);
}

GNPCLoot::~GNPCLoot()
{
	SAFE_DELETE(m_pDropList);
}

void GNPCLoot::Begin(int nCID)
{
	m_setLootingPlayerCID.insert(nCID);
}

void GNPCLoot::End(int nCID)
{
	m_setLootingPlayerCID.erase(nCID);
}

set<int> GNPCLoot::GetLootingPlayerCID()
{
	return m_setLootingPlayerCID;
}

bool GNPCLoot::IsLootingPlayer(int nCID)
{
	if (m_setLootingPlayerCID.end() == m_setLootingPlayerCID.find(nCID)) return false;

	return true;
}

bool GNPCLoot::IsLootingPlayerEmpty()
{
	return m_setLootingPlayerCID.empty();
}

int GNPCLoot::GetBPartLootID() const
{
	return m_nBPartLootID;
}

void GNPCLoot::SetBPartLootID( int nID )
{
	m_nBPartLootID = nID;
}
