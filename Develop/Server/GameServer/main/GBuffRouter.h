#pragma once

#include "GBuffObserver.h"

class GBuff;

class GBuffRouter : public GBuffObserver, public MTestMemPool<GBuffRouter>
{
public:
	GBuffRouter(GBuff* pOwner);
	~GBuffRouter(void);

private:
	void NotifyForParty();
	void RouteGainBuff();
	void RouteLostBuff();
	void RouteStackIncrease();
	void RouteStackDecrease();

private:
	// 버프 얻을때 효과처리
	virtual void OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) override;
	// 버프 잃을때 효과처리
	virtual void OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute) override;
	// 버프가 중복되서 걸린 경우
	virtual void OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo) override;
	
private:
	GBuff* m_pOwner;
};
