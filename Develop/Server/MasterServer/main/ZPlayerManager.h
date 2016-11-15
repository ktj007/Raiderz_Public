#pragma once

#include "CSHelper.h"
#include "ZPlayer.h"

class ZBasePlayerManager
{
protected:
	ZPlayerAIDMap		m_PlayerAIDMap;
	ZPlayerCIDMap		m_PlayerCIDMap;
	ZPlayerUIDMap		m_PlayerUIDMap;
	
public:
	ZBasePlayerManager() {}
	virtual ~ZBasePlayerManager();

	virtual void Add(ZPlayer* pPlayer);
	
	virtual void Delete(AID nAID);	

	virtual void Clear();
	ZPlayer* Find(AID nAID);
	ZPlayer* FindByCID(CID nCID);
	ZPlayer* FindByUID(MUID uidPlayer);
	ZPlayer* FindByName(wstring strName);

	ZPlayerAIDMap& GetPlayerMap(void)	{ return m_PlayerAIDMap; }

	size_t GetObjectCount()	{ return m_PlayerAIDMap.size(); }
	size_t GetCIDCount()	{ return m_PlayerCIDMap.size(); }
	size_t GetUIDCount()	{ return m_PlayerUIDMap.size(); }
};

class ZPlayerManager : public ZBasePlayerManager
{
protected:
	MUIDGenerator		m_UIDGenerator;

public:
	ZPlayerManager();
	virtual ~ZPlayerManager();

	ZPlayer* NewPlayer(MUID uidPlayer, AID nAID, const wstring& strUserID);

	virtual void Clear() override;

	void UpdateUID(ZPlayer* pPlayer, const MUID& uidPlayer);

	void UpdateCIDUID(ZPlayer* pPlayer, CID nCID, const MUID& uidPlayer);
	void DelCIDUID(ZPlayer* pPlayer);
};
