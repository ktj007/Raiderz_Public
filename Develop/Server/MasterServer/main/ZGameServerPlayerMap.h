#pragma once

#include "ZPlayerManager.h"

/// 거임 서버 오브젝트가 가지고 있는 플레이어 인스턴스
/// - ZPlayer 인스턴스는 ZPlayerManager가 가지고 있다.
class ZGameServerPlayerMap : public ZBasePlayerManager
{
public:
	ZGameServerPlayerMap() : ZBasePlayerManager() {}
	virtual ~ZGameServerPlayerMap();

	virtual void Add(ZPlayer* pPlayer) override;
	virtual void Clear() override;

};
