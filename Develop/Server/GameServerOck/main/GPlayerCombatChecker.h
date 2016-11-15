#pragma once

#include "GActorObserver.h"

class GPlayerCombatChecker : private GActorObserver, public MTestMemPool<GPlayerCombatChecker>
{
public:
	GPlayerCombatChecker(GEntityActor* pOwner);
	~GPlayerCombatChecker(void);

	void Update(float fDelta);

private:
	// 상대가 자신을 헤이트테이블에서 추가하였을때
	virtual void OnAddHateTarget( GEntityActor* pEnemy ) override;
	// 상대가 자신을 헤이트테이블에서 제거하였을때
	virtual void OnLostHateTarget( MUID uidEnemy ) override;

private:
	GEntityActor*	m_pOwner;
	bool			m_bIsNowCombat;
};
