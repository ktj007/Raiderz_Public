#include "stdafx.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GEntityPlayer.h"
#include "GPlayerAcceptManager.h"
#include "GConfig.h"

GPlayerObjectManager::GPlayerObjectManager()
{
	m_pPlayerAcceptorManager = new GPlayerAcceptManager();
}

GPlayerObjectManager::~GPlayerObjectManager()
{
	DeleteAllPlayer();
	SAFE_DELETE(m_pPlayerAcceptorManager);
}

void GPlayerObjectManager::UpdatePlayerAcceptorManager()
{
	m_pPlayerAcceptorManager->Update();
}

GPlayerObject* GPlayerObjectManager::GetPlayer(const MUID& uidPlayer)
{
	GPlayerObjectMap::iterator itor = m_PlayerObjectMap.find(uidPlayer.Value);
	if (itor != m_PlayerObjectMap.end())
	{
		return (*itor).second;
	}

	return NULL;
}

GPlayerObject* GPlayerObjectManager::GetPlayer(const CID nPlayerCID)
{
	GCIDPlayerMap::iterator it = m_CIDPlayerMap.find(nPlayerCID);
	if (it == m_CIDPlayerMap.end())	return NULL;

	return it->second;
}

GEntityPlayer* GPlayerObjectManager::GetEntity(const MUID& uidPlayer)
{
	GPlayerObject* pPlayer = GetPlayer(uidPlayer);
	if (pPlayer) return pPlayer->GetEntity();
	return NULL;
}

GEntityPlayer* GPlayerObjectManager::GetEntity( const UIID nPlayerUIID )
{
	MUID uid = m_IndexedPlayerUIDMap.Find(nPlayerUIID);
	if (uid != MUID::ZERO)
	{
		return GetEntity(uid);
	}
	return NULL;
}

GEntityPlayer* GPlayerObjectManager::GetEntity(const CID nPlayerCID)
{
	GCIDPlayerMap::iterator it = m_CIDPlayerMap.find(nPlayerCID);
	if (it == m_CIDPlayerMap.end())	return NULL;

	GPlayerObject* pObject = it->second;
	return pObject->GetEntity();
}

GEntityPlayer* GPlayerObjectManager::GetEntityInWorld(const MUID& uidPlayer)
{
	GPlayerObject* pPlayer = GetPlayer(uidPlayer);
	if (pPlayer == NULL) return NULL;
	if (pPlayer->GetState() != POS_INWORLD) return NULL;	
	return pPlayer->GetEntity();
}

GPlayerObject* GPlayerObjectManager::NewPlayerObject(const MUID& uid)
{
	PFI_BLOCK_THISFUNC(6502);
	GPlayerObject* pNewPlayerObject = new GPlayerObject_Leaf(uid);	
	pNewPlayerObject->Create();
	return pNewPlayerObject;
}

void GPlayerObjectManager::AddPlayer(GPlayerObject* pPlayer)
{
	MUID uidPlayer = pPlayer->GetUID();

	if (m_PlayerObjectMap.find(uidPlayer.Value) != m_PlayerObjectMap.end())
	{
		_ASSERT(0);
		return;
	}

	AddUIID(pPlayer);
	// AddCID()의 경우 캐릭터 선택 이후에 호출해야한다. 로그인 완료, 서버이동 완료 시 추가.

	m_PlayerObjectMap.insert(GPlayerObjectMap::value_type(pPlayer->GetUID().Value, pPlayer));	
}


GPlayerObjectManager::GPlayerObjectMap::iterator GPlayerObjectManager::EraseAndDeletePlayer( GPlayerObjectMap::iterator itor )
{
	GPlayerObject* pPlayerObject = (*itor).second;
	UIID nUIID = pPlayerObject->GetUIID();
	int nCID = pPlayerObject->GetEntity()->GetCID();	
	pPlayerObject->Destroy();

	GPlayerObjectMap::iterator itorRet = m_PlayerObjectMap.erase(itor);
	RemoveUIID(nUIID);
	RemoveCID(nCID);	
	delete pPlayerObject;
	return itorRet;
}

void GPlayerObjectManager::ErasePlayer( MUID uidPlayer )
{
	GPlayerObjectMap::iterator itor = m_PlayerObjectMap.find(uidPlayer.Value);
	if (itor != m_PlayerObjectMap.end())
	{
		GPlayerObject* pPlayerObject = (*itor).second;
		RemoveUIID(pPlayerObject->GetUIID());
		RemoveCID(pPlayerObject->GetEntity()->GetCID());

		m_PlayerObjectMap.erase(itor);
	}
}

void GPlayerObjectManager::DeletePlayer(MUID uidPlayer)
{
	GPlayerObjectMap::iterator itor = m_PlayerObjectMap.find(uidPlayer.Value);
	if (itor != m_PlayerObjectMap.end())
	{
		EraseAndDeletePlayer(itor);
	}
}

void GPlayerObjectManager::DeleteAllPlayer()
{
	for (GPlayerObjectMap::iterator itor = m_PlayerObjectMap.begin(); m_PlayerObjectMap.end() != itor; )
	{
		itor = EraseAndDeletePlayer(itor);
	}
	Clear();
}

void GPlayerObjectManager::Clear()
{
	m_PlayerObjectMap.clear();
	m_IndexedPlayerUIDMap.Clear();
	m_CIDPlayerMap.clear();
}


GEntityPlayer* GPlayerObjectManager::GetEntityFromPlayerID( wstring& strPlayerID )
{
	for (GPlayerObjectMap::iterator itor = m_PlayerObjectMap.begin(); itor != m_PlayerObjectMap.end(); ++itor)
	{
		GPlayerObject* pPlayerObject = (*itor).second;
		GEntityPlayer* pEntityPlayer = pPlayerObject->GetEntity();
		if (pEntityPlayer && pEntityPlayer->GetPlayerInfo())
		{
			if (!_wcsicmp(pEntityPlayer->GetPlayerInfo()->szName, strPlayerID.c_str()))
			{
				return pEntityPlayer;
			}
		}
	}

	return NULL;
}

void GPlayerObjectManager::AddUIID( GPlayerObject* pPlayerObject )
{
	MUID uidPlayer = pPlayerObject->GetUID();
	UIID nNewUIID = m_IndexedPlayerUIDMap.Add(uidPlayer);
	pPlayerObject->RegUIID(nNewUIID);
}

void GPlayerObjectManager::RemoveUIID(UIID nUIID)
{
	m_IndexedPlayerUIDMap.Del(nUIID);
}

void GPlayerObjectManager::AddCID(GPlayerObject* pPlayer)
{
	GEntityPlayer* pEntity = pPlayer->GetEntity();
	VALID(pEntity != NULL);

	int nCID = pEntity->GetCID();
	m_CIDPlayerMap.insert(GCIDPlayerMap::value_type(nCID, pPlayer));
}

void GPlayerObjectManager::RemoveCID(int nCID)
{
	m_CIDPlayerMap.erase(nCID);
}

MUID GPlayerObjectManager::GetPlayerUID(CID nCID, int nGameServerID)
{
	if (nGameServerID == GConfig::m_nMyServerID)
	{
		GEntityPlayer* pPlayer = GetEntity(nCID);
		if (pPlayer != NULL)
		{
			return pPlayer->GetUID();
		}
	}

	return MUID::Invalid();
}

void GPlayerObjectManager::Update( float fDelta )
{
	for each (pair<uint64, GPlayerObject*> each in m_PlayerObjectMap)
	{
		each.second->Update(fDelta);
	}
}

bool GPlayerObjectManager::IsExist( const MUID& uidPlayer )
{
	return (NULL != GetEntity(uidPlayer));
}

bool GPlayerObjectManager::IsExist( int nCID )
{
	return (NULL != GetEntity(nCID));
}

bool GPlayerObjectManager::IsMultiLoginPlayerID( wstring& strPlayerID )
{
	int nCount = 0;

	for each (GPlayerObjectMap::value_type val in m_PlayerObjectMap)
	{
		GPlayerObject* pPlayerObject = val.second;
		GEntityPlayer* pEntityPlayer = pPlayerObject->GetEntity();
		if (pEntityPlayer && pEntityPlayer->GetPlayerInfo())
		{
			if (!_wcsicmp(pEntityPlayer->GetPlayerInfo()->szName, strPlayerID.c_str()))
			{
				nCount++;
			}
		}
	}

	return (2 <= nCount);
}
