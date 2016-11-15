#pragma once

class GEntityPlayer;
class GEntityNPC;

class GLootBeginner : public MTestMemPool<GLootBeginner>
{
public:
	bool KillNPC(GEntityPlayer* pPlayer, const MUID& uidNPC);
	bool InteractNPC(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nLootID, bool bImmortal);
	bool GatherNPC(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nGatherTier, int nLootID);

private:
	bool KillNPC_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC);	
	void KillNPC_Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC );	

	bool InteractNPC_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, bool bImmortal);
	void InteractNPC_Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC, float fLootTime);

	bool GatherNPC_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nGatherTier);
	void GatherNPC_Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC, float fLootTime, int nLootID);

	void Route(GEntityPlayer* pPlayer, const MUID& uidNPC, float fLootTime);
};
