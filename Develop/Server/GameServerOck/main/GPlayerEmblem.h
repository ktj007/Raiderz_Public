#pragma once

#include "GActorObserver.h"

class GEntityPlayer;

class GPlayerEmblem : public GActorObserver, public MTestMemPool<GPlayerEmblem>
{
public:
	GPlayerEmblem(GEntityPlayer* pOwner);
	virtual ~GPlayerEmblem(void);

	bool CheckEmblem(int nEmblemID) const;
	bool HasKilled(int nNPCID) const;
	void LoadFromDB(const deque<int>& qEmblems);

private:
	void CompleteEmblem(int nEmblemID);

private:
	// NPC를 죽여서 보상 받을때 이벤트
	virtual void OnRewarded(int nNPCID) override;
private:
	typedef set<int> EmblemSet;
	typedef set<int> KilledSet;

	GEntityPlayer*	m_pOwner;
	EmblemSet		m_setEmblems;
	KilledSet		m_setKilleds;
};
