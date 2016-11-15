#pragma once

#include "STypes.h"

class SProxyPlayerListener;
class SProxyPlayer;
struct TD_PROXY_PLAYER_INFO;

class SProxyPlayerSpeaker
{
public:
	typedef set<SProxyPlayerListener*>	listener_set;

public:
	SProxyPlayerSpeaker();
	virtual ~SProxyPlayerSpeaker();

	void AddListener(SProxyPlayerListener* pListener);
	void RemoveListener(SProxyPlayerListener* pListener);

	void OnPreAdd(CID cid);
	void OnAddComplate(const SProxyPlayer* pPlayer);
	void OnPreRemove(const SProxyPlayer* pPlayer);
	void OnRemoveComplate(CID cid);
	void OnPreUpdate(const SProxyPlayer* pPlayer, const TD_PROXY_PLAYER_INFO& info);
	void OnUpdateComplate(const SProxyPlayer* pPlayer);

private:
	listener_set m_setListener;
};
