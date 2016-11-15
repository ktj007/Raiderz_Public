#pragma once

#include "STypes.h"

class SProxyPlayer;
struct TD_PROXY_PLAYER_INFO;

class SProxyPlayerListener
{
public:
	virtual void OnPreAdd(CID cid) {}
	virtual void OnAddComplate(const SProxyPlayer* pPlayer) {}
	virtual void OnPreRemove(const SProxyPlayer* pPlayer) {}
	virtual void OnRemoveComplate(CID cid) {}
	virtual void OnPreUpdate(const SProxyPlayer* pPlayer, const TD_PROXY_PLAYER_INFO& info) {}
	virtual void OnUpdateComplate(const SProxyPlayer* pPlayer) {}
};
