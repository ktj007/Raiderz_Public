#pragma once

#include "CSEffectInfo.h"

class GEntityPlayer;
class GFactionInfo;
class GEntityActor;
class GNPCInfo;

class GFactionSystem
{
private:
	uint16	CalcIncreasableQuantity(const uint16 nQuantity, const uint16 nCurrentQuantiy);
	uint16	IncreasePlayer(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity);
	void	Increase_Route(GEntityPlayer* pPlayer, const uint8 nID);
	
	uint16	CalcDecreasableQuantity(const uint16 nQuantity, const uint16 nCurrentQuantiy);
	uint16	DecreasePlayer(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity);
	void	Decrease_Route(GEntityPlayer* pPlayer, uint8 nID);
	
public:
	virtual void RewardFaction(const vector<GEntityPlayer*>& vecBeneficiary, GNPCInfo* pNPCInfo);

	bool	Increase(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity);	
	bool	Decrease(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity);
	bool	Set(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity);

	void	Reset(GEntityPlayer* pPlayer);
};