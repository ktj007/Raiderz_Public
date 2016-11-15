#include "StdAfx.h"
#include "GNPCQuest.h"
#include "GNPCEscortQuests.h"

GNPCQuest::GNPCQuest(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
	m_pNPCEscortQuests = new GNPCEscortQuests(pOwner);
}

GNPCQuest::~GNPCQuest()
{
	SAFE_DELETE(m_pNPCEscortQuests);
}

void GNPCQuest::Clear()
{
	m_pNPCEscortQuests->Clear();
}