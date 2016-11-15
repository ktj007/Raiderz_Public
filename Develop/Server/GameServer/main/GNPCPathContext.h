#pragma once

#include "GActorObserver.h"

class GEntityNPC;

class GNPCPathContext : private GActorObserver, public MTestMemPool<GNPCPathContext>
{
public:
	GNPCPathContext(GEntityNPC* pOwner);
	~GNPCPathContext(void);

private:
	// 자신의 헤이트테이블에 타켓이 추가되었을때
	virtual void OnAddHateEntry( GEntityActor* pEnemy ) override;
	// 전투 종료
	virtual void OnCombatEnd(GEntityActor* pOwner) override;

private:
	GEntityNPC* m_pOwner;
};
