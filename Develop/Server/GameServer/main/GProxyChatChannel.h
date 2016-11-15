#pragma once

#include "CSDef.h"
#include "STypes.h"

class GProxyChatChannel;
class GProxyChatPlayer;

typedef map<MUID, GProxyChatChannel*>	channel_map;
typedef map<CID, GProxyChatPlayer*>		player_map;

class GProxyChatPlayer : public MTestMemPool<GProxyChatPlayer>
{
public:
	GProxyChatPlayer(CID nPlayerCID);
	virtual ~GProxyChatPlayer();

	void AddChannel(GProxyChatChannel* pChannel);
	void RemoveChannel(MUID uidChannel);
	void RemoveFromAllChannel(void);

	CID GetCID(void) const;
	int GetGameServerID(void) const;
	void SetGameServerID(int nGameServerID);
	MUID GetUID(void) const;
	void SetUID(MUID uidPlayer);
	bool IsEmptyChannel(void) const;

private:
	CID m_CID;
	MUID m_UID;	///< 현재 게임서버에 속해있는 플레이어의 UID 만 유효함.
	int m_nGameServerID;
	channel_map m_mapChannel;
};



class GProxyChatChannel : public MTestMemPool<GProxyChatChannel>
{
public:
	GProxyChatChannel(MUID uidChannel);
	virtual ~GProxyChatChannel();

	void AddPlayer(GProxyChatPlayer* pPlayer);
	void RemovePlayer(CID nCID);
	GProxyChatPlayer* FindPlayer(CID nCID);
	player_map::iterator BeginPlayer(void);
	player_map::iterator EndPlayer(void);

	MUID GetUID(void) const;
	void SetName(wstring strName);
	wstring GetName(void) const;
	int GetPlayerCount(void) const;	

private:
	MUID m_UID;
	wstring m_strName;
	player_map m_mapPlayer;
};



class GProxyChatChannelManager : public MTestMemPool<GProxyChatChannelManager>
{
public:
	GProxyChatChannelManager();
	virtual ~GProxyChatChannelManager();

	bool AddChannel(MUID uidChannel, wstring strChannelName);
	bool RemoveChannel(MUID uidChannel);
	bool AddPlayer(MUID uidChannel, CID nCID, int nGameServerID, MUID uidPlayer);	
	bool RemovePlayer(MUID uidChannel, CID nCID);
	bool RemovePlayer(CID nCID);
	bool UpdatePlayerInLocal(CID nCID, int nGameServerID, MUID uidPlayerInLocal);
	void Clear(void);

	GProxyChatChannel* FindChannel(MUID uidChannel);
	int GetChannelCount(void) const;
	
	GProxyChatPlayer* FindPlayer(CID nCID);
	int GetPlayerCount(void) const;
	int GetPlayerCount(MUID uidChannel);

	void DisconnectAppServer();

private:
	channel_map m_mapChannel;
	player_map m_mapPlayer;
};
