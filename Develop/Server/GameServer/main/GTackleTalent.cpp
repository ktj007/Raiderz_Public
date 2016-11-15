#include "stdafx.h"
#include "GTackleTalent.h"

void GTackleTalent::OnUpdatePhaseAct( float fDelta )
{
	GTalent::OnUpdatePhaseAct(fDelta);

	m_TalentHit.UpdateHit(this, m_fElapsedTime);
}