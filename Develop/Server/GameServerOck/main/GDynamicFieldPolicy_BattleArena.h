#pragma once

#include "GDynamicFieldPolicy.h"
#include "GBattleArena.h"

class GDynamicFieldPolicy_BattleArena : public GDynamicFieldPolicy, public GBattleArena::Lisener, public MTestMemPool<GDynamicFieldPolicy_BattleArena>
{
private:
	bool m_bEnterable;
protected:
	virtual void OnUpdate(float fDelta);
	virtual bool OnEnterPlayer(GEntityPlayer* pPlayer);
	virtual void OnLeavePlayer(GEntityPlayer* pPlayer);
	virtual void OnDestroy();

	virtual FIELD_ENTER_FAIL_TYPE IsEnterable() override;

	virtual DYNAMICFIELD_POLICY_TYPE GetType() override { return DPT_BATTLEARENA; }

	virtual void OnBattleArenaShowScoreBoard(const BattleArena::TEAM_MAP& mapEntries) override; 
	virtual void OnBattleArenaComplete(const BattleArena::TEAM_MAP& mapEntries)	override;

public:
	GDynamicFieldPolicy_BattleArena(GDynamicFieldMaster* pOwner);
	virtual ~GDynamicFieldPolicy_BattleArena();
};

