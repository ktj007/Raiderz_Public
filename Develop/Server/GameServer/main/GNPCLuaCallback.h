#pragma once

#include "GActorObserver.h"

class GEntityNPC;

class GNPCLuaCallback : public GActorObserver, public MTestMemPool<GNPCLuaCallback>
{
public:
	GNPCLuaCallback(GEntityNPC* pOwner);
	~GNPCLuaCallback(void);

private:
	// 전투 시작
	virtual void OnCombatBegin(GEntityActor* pOwner) override;
	// 전투 종료
	virtual void OnCombatEnd(GEntityActor* pOwner) override;
private:
	GEntityNPC* m_pOwner;
};
