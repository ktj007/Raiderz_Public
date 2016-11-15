#include "stdafx.h"
#include "GPartyManager.h"
#include "GParty.h"
#include "GEntityPlayer.h"

GPartyManager::~GPartyManager() 
{
	Clear();
}

GParty* GPartyManager::NewParty(MUID uidParty)
{ 
	GParty* pParty = new GParty(uidParty);
	return pParty; 
}

void GPartyManager::AddParty( GParty* pParty )
{
	_ASSERT(pParty);
	if (NULL == pParty)
	{
		return;
	}

	m_mapEntries.insert(PARTY_MAP::value_type(pParty->GetUID().Value, pParty));	
}

void GPartyManager::DeleteParty( const MUID& uidParty )
{
	PARTY_MAP::iterator it = m_mapEntries.find(uidParty.Value);
	if (it == m_mapEntries.end())	return;

	SAFE_DELETE(it->second);
	m_mapEntries.erase(uidParty.Value);
}

void GPartyManager::Clear()
{
	for (PARTY_MAP::iterator iter = m_mapEntries.begin();
		iter != m_mapEntries.end();
		++iter)
	{
		SAFE_DELETE(iter->second);
	}

	m_mapEntries.clear();
}

GParty* GPartyManager::FindParty( GEntityPlayer* pReqPlayer )
{
	VALID_RET(pReqPlayer, NULL);

	return FindParty(pReqPlayer->GetPartyUID());
}

GParty* GPartyManager::FindParty(MUID uidParty)
{
	if (uidParty.IsInvalid())			return NULL;

	PARTY_MAP::iterator iter = m_mapEntries.find(uidParty.Value);
	if (iter == m_mapEntries.end())		return NULL;

	return iter->second;
}
