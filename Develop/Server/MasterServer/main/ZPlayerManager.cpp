#include "stdafx.h"
#include "ZPlayerManager.h"
#include "ZPlayer.h"
#include "ZGlobal.h"

ZBasePlayerManager::~ZBasePlayerManager()
{
	Clear();
}

void ZBasePlayerManager::Add( ZPlayer* pPlayer )
{
	m_PlayerAIDMap.insert(ZPlayerAIDMap::value_type(pPlayer->GetAID(), pPlayer));
	m_PlayerUIDMap.insert(ZPlayerUIDMap::value_type(pPlayer->GetUID().Value, pPlayer));
}

void ZBasePlayerManager::Delete(AID nAID)
{
	ZPlayerAIDMap::iterator itor = m_PlayerAIDMap.find(nAID);
	if (itor != m_PlayerAIDMap.end())
	{
		ZPlayer* pPlayer = (*itor).second;
		
		m_PlayerUIDMap.erase(pPlayer->GetUID().Value);
		m_PlayerCIDMap.erase(pPlayer->GetCID());

		if (pPlayer != NULL)
			pPlayer->Drop();

		m_PlayerAIDMap.erase(itor);
	}
}

void ZBasePlayerManager::Clear()
{
	for (ZPlayerAIDMap::iterator itor = m_PlayerAIDMap.begin(); itor != m_PlayerAIDMap.end(); ++itor)
	{
		ZPlayer* pPlayer = (*itor).second;
		
		if (pPlayer != NULL)
		{
			pPlayer->Drop();
		}
	}

	m_PlayerAIDMap.clear();
	m_PlayerCIDMap.clear();
}

ZPlayer* ZBasePlayerManager::Find(AID nAID)
{
	ZPlayerAIDMap::iterator itor = m_PlayerAIDMap.find(nAID);
	if (itor == m_PlayerAIDMap.end()) return NULL;

	return (*itor).second;
}

ZPlayer* ZBasePlayerManager::FindByCID(CID nCID)
{
	ZPlayerCIDMap::iterator itor = m_PlayerCIDMap.find(nCID);
	if (itor == m_PlayerCIDMap.end()) return NULL;

	return (*itor).second;
}

ZPlayer* ZBasePlayerManager::FindByUID(MUID uidPlayer)
{
	ZPlayerUIDMap::iterator it = m_PlayerUIDMap.find(uidPlayer.Value);
	if (it == m_PlayerUIDMap.end())		return NULL;

	return it->second;
}

ZPlayer* ZBasePlayerManager::FindByName( wstring strName )
{
	for each (ZPlayerAIDMap::value_type val in m_PlayerAIDMap)
	{
		ZPlayer* pPlayer = val.second;
		if (NULL == pPlayer) continue;

		if (strName == pPlayer->GetPlayerName())
		{
			return pPlayer;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

ZPlayerManager::ZPlayerManager() : ZBasePlayerManager()
{

}

ZPlayerManager::~ZPlayerManager()
{
	
}

ZPlayer* ZPlayerManager::NewPlayer(MUID uidPlayer, AID nAID, const wstring& strUserID)
{
	ZPlayer* pNewPlayer = new ZPlayer(uidPlayer, nAID, strUserID);
	return pNewPlayer;
}

void ZPlayerManager::Clear()
{
	m_PlayerUIDMap.clear();

	ZBasePlayerManager::Clear();
}

void ZPlayerManager::UpdateUID(ZPlayer* pPlayer, const MUID& uidPlayer)
{
	if (NULL == Find(pPlayer->GetAID())) return;
	
	if (pPlayer->GetUID().IsValid())
		m_PlayerUIDMap.erase(pPlayer->GetUID().Value);

	m_PlayerUIDMap.insert(ZPlayerUIDMap::value_type(uidPlayer.Value, pPlayer));
	pPlayer->SetUID(uidPlayer);
}

void ZPlayerManager::UpdateCIDUID( ZPlayer* pPlayer, CID nCID, const MUID& uidPlayer )
{
	if (NULL == Find(pPlayer->GetAID())) return;

	m_PlayerCIDMap.erase(nCID);
	m_PlayerCIDMap.insert(ZPlayerCIDMap::value_type(nCID, pPlayer));

	m_PlayerUIDMap.erase(pPlayer->GetUID().Value);
	m_PlayerUIDMap.insert(ZPlayerUIDMap::value_type(uidPlayer.Value, pPlayer));
}

void ZPlayerManager::DelCIDUID(ZPlayer* pPlayer)
{
	if (NULL == Find(pPlayer->GetAID())) return;

	m_PlayerCIDMap.erase(pPlayer->GetCID());
	m_PlayerUIDMap.erase(pPlayer->GetUID().Value);
}