#pragma once

#include "SProxyPlayerListener.h"

class PProxyPlayerListener: public SProxyPlayerListener
{
public:
	virtual void OnAddComplate(const SProxyPlayer* pPlayer) override;
	virtual void OnPreRemove(const SProxyPlayer* pPlayer) override;	
	virtual void OnPreUpdate(const SProxyPlayer* pPlayer, const TD_PROXY_PLAYER_INFO& info);
};
