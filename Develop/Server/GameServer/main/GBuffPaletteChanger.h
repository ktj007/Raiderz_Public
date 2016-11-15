#pragma once

#include "GBuffObserver.h"
#include "GPalette.h"

class GBuffPaletteChanger : public GBuffObserver, public MTestMemPool<GBuffPaletteChanger>
{
private:
	virtual void OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)		override;
	virtual void OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute)	override;

private:
	VEC_PALETTE_ITEM m_vecOldPalettes;
};
