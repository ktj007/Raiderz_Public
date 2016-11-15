#pragma once

class GEntityNPC;

class GQPKillRewarder : public MTestMemPool<GQPKillRewarder>
{
public:
	GQPKillRewarder(GEntityPlayer*	pOwner);
	~GQPKillRewarder(void);

	GEntityNPC* Reward();

private:
	GEntityPlayer*	m_pOwner;
};
