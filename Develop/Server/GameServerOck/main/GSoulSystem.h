#ifndef _G_SOUL_SYSTEM_H_
#define _G_SOUL_SYSTEM_H_

class GEntityPlayer;
class GEntityNPC;

class GSoulSystem : public MTestMemPool<GSoulSystem>
{
private:
	void doDrainSoul(GEntityPlayer* pPlayer, GEntityNPC* pVictimNPC);

public:
	GSoulSystem();
	~GSoulSystem();

	void DrainSoul(GEntityPlayer* pPlayer, GEntityNPC* pVictimNPC);	
};

#endif
