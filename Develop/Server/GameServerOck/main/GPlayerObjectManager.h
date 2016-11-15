#pragma once

#include "MUID.h"
#include "GDef.h"
#include "MSync.h"
#include "GIndexedIDMap.h"
#include "STypes.h"

class GPlayerObject;
class GEntityPlayer;
class GPlayerAcceptManager;

/// 플레이어 관리자
class GPlayerObjectManager : public MTestMemPool<GPlayerObjectManager>
{
public:
	friend class GServer;
	typedef tr1::unordered_map<uint64, GPlayerObject*>	GPlayerObjectMap;
	typedef tr1::unordered_map<CID, GPlayerObject*>		GCIDPlayerMap;

private:
	GPlayerObjectMap		m_PlayerObjectMap;	
	GCIDPlayerMap			m_CIDPlayerMap;
	GPlayerAcceptManager*	m_pPlayerAcceptorManager;
	GIndexedPlayerUIDMap	m_IndexedPlayerUIDMap;		///< 플레이어 Index ID 매니저

	void AddUIID(GPlayerObject* pPlayer);
	void RemoveUIID(UIID nUIID);	
	GPlayerObjectMap::iterator EraseAndDeletePlayer( GPlayerObjectMap::iterator itor );

public:
	GPlayerObjectManager();
	virtual ~GPlayerObjectManager();

	void UpdatePlayerAcceptorManager();

	void Update(float fDelta);

	GPlayerObject* NewPlayerObject(const MUID& uid);
	void AddPlayer(GPlayerObject* pPlayer);
	void DeletePlayer(MUID uidPlayer);
	void ErasePlayer( MUID uidPlayer );
	
	void AddCID(GPlayerObject* pPlayer);
	void RemoveCID(int nCID);

	void DeleteAllPlayer();
	void Clear();
	int  GetPlayersCount() { return (int)m_PlayerObjectMap.size(); }

	GPlayerObject* GetPlayer(const MUID& uidPlayer);
	GPlayerObject* GetPlayer(const CID nPlayerCID);
	GEntityPlayer* GetEntity(const MUID& uidPlayer);
	GEntityPlayer* GetEntity(const UIID nPlayerUIID);
	GEntityPlayer* GetEntity(const CID nPlayerCID);
	GEntityPlayer* GetEntityFromPlayerID(wstring& strPlayerID);

	GEntityPlayer* GetEntityInWorld(const MUID& uidPlayer);

	GPlayerObjectMap& GetObjects()	{ return m_PlayerObjectMap; }	

	GPlayerAcceptManager* GetAcceptManager() { return m_pPlayerAcceptorManager; }

	MUID GetPlayerUID(CID nCID, int nGameServerID);

	bool IsExist(const MUID& uidPlayer);
	bool IsExist(int nCID);
	bool IsMultiLoginPlayerID(wstring& strPlayerID);
};
