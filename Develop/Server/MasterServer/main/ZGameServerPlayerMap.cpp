#include "stdafx.h"
#include "ZGameServerPlayerMap.h"

ZGameServerPlayerMap::~ZGameServerPlayerMap()
{
	Clear();
}

void ZGameServerPlayerMap::Add( ZPlayer* pPlayer )
{	
	pPlayer->AddRef();
	m_PlayerCIDMap.insert(ZPlayerCIDMap::value_type(pPlayer->GetCID(), pPlayer));
	
	ZBasePlayerManager::Add(pPlayer);
}

void ZGameServerPlayerMap::Clear()
{	
	m_PlayerUIDMap.clear();
	ZBasePlayerManager::Clear();
}
