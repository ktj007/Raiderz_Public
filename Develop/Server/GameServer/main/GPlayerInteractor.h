#pragma once

class GEntityPlayer;
class GEntityNPC;

class GPlayerInteractor : public MTestMemPool<GPlayerInteractor>
{
public:
	bool Interaction(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
};
