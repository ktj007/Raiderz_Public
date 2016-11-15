#ifndef _GREGEN_UPDATER_H
#define _GREGEN_UPDATER_H

#include "GDef.h"
#include "MTime.h"

class GEntityActor;

class GRegenUpdater
{
private:
	bool					m_bActive;
	MRegulator				m_RegenRegulator;

	float					m_fHPRegenModValue; ///< 회복시키고 남은 나머지 소숫값 
	float					m_fENRegenModValue; ///< 회복시키고 남은 나머지 소숫값 
	float					m_fSTARegenModValue; ///< 회복시키고 남은 나머지 소숫값 

	bool UpdateSTARegen( GEntityActor* pActor );
	bool UpdateENRegen( GEntityActor* pActor );
	bool UpdateHPRegen( GEntityActor* pActor );
	float CalcAmount( GEntityActor* pActor, float fRegenAmount, float fRegenModValue, float fRegenRateInCombat, float fPlayerSittingRate );
public:
	GRegenUpdater();
	void Update(float fDelta, GEntityActor* pPlayer);

	void SetActive(bool bActive) { m_bActive = bActive;	}
	bool IsActive() { return m_bActive; }
};







#endif // _GREGEN_UPDATER_H