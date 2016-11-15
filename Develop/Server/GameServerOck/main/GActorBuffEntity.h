#pragma once

#include "GActorObserver.h"

class GBuffEntity;

class GActorBuffEntity : private GActorObserver, public MTestMemPool<GActorBuffEntity>
{
public:
	GActorBuffEntity(GEntityActor* pOwner);
	~GActorBuffEntity(void);

	int GetQty(int nBuffID) const;
	void DeleteFrontEntity(int nBuffID, int nLimitQty);
private:
	GBuffEntity* GetBuffEntity(MUID uidEntity);
private:
	// BuffEntity°¡ »ý¼º µÆÀ» ¶§
	virtual void OnCreateBuffEntity(int nBuffID, MUID uidEntity) override;
	// BuffEntity°¡ ¼Ò¸ê µÆÀ» ¶§
	virtual void OnDestroyBuffEntity(int nBuffID, MUID uidEntity) override;
private:
	typedef map<int,deque<MUID>> BuffEntityMap; // <BuffEntityID, Qty>

	GEntityActor*	m_pOwner;
	BuffEntityMap m_mapEntity;
};
