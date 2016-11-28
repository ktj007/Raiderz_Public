#pragma once

#include "ServerCommonLib.h"
#include "SProxyPlayerSpeaker.h"
#include "MTString.h"

namespace minet 
{
	class MCommandCommunicator;	
}

class SProxyPlayer;
class SCmdHandler_ProxyPlayer;
struct TD_PROXY_PLAYER_INFO;

class SCOMMON_API SProxyPlayerManager: public SProxyPlayerSpeaker, public MTestMemPool<SProxyPlayerManager>
{
private:
	class name_comp {	// for name_map.
	public:
		bool operator() (const std::wstring& lhs, const std::wstring& rhs) const {
			return (_wcsicmp(lhs.c_str(), rhs.c_str()) < 0);
		}
	};
public:
	typedef map<CID, SProxyPlayer*>		player_map;
	typedef map<wstring, SProxyPlayer*, name_comp>	name_map;	

public:
	SProxyPlayerManager(minet::MCommandCommunicator* pCC);
	virtual ~SProxyPlayerManager();
	
	bool AddPlayer(const TD_PROXY_PLAYER_INFO& info);
	bool RemovePlayer(CID cid);
	bool UpdatePlayer(const TD_PROXY_PLAYER_INFO& info);
	void Clear(void);

	SProxyPlayer* FindPlayer(CID cid);
	SProxyPlayer* FindPlayer(wstring strName);
	int	GetPlayerCount(void) const;

private:
	SCmdHandler_ProxyPlayer* m_pCmdHandler;
	player_map	m_mapPlayer;	
	name_map	m_mapName;
};
