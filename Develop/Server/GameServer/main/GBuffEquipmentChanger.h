#pragma once

#include "GBuffObserver.h"

class GBuffEquipmentChanger : public GBuffObserver, public MTestMemPool<GBuffEquipmentChanger>
{
public:
	GBuffEquipmentChanger(void);
	~GBuffEquipmentChanger(void);

private:
	virtual void OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)		override;
	virtual void OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute)	override;

private:
	void RemoveAllChangeEquipmentBuffs(GEntityPlayer* pTargetPlayer, int nExceptBuff);
};
