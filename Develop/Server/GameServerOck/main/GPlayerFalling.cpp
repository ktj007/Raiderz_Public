#include "StdAfx.h"
#include "GPlayerFalling.h"
#include "GEntityPlayer.h"
#include "GCalculator.h"
#include "GCommand.h"
#include "CCommandTable.h"

GPlayerFalling::GPlayerFalling(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
}

GPlayerFalling::~GPlayerFalling(void)
{
}

void GPlayerFalling::EndFalling(bool bIsFallenOnWater, float fFallingHeight)
{
	if (fFallingHeight <= 0) return;
	if (m_pOwner->GetHP() <= 0) return;

	int nFallingDamage = GCalculator::CalcFallingDamage(bIsFallenOnWater, fFallingHeight, m_pOwner->GetMaxHP());
	if (nFallingDamage <= 0) return;

	bool bIsDie = m_pOwner->doDamage(MUID::ZERO, DA_NONE, nFallingDamage);

	if (false == bIsDie && false == bIsFallenOnWater)
	{
		int nFallingDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		if (nFallingDebuffID != INVALID_BUFF_ID)
		{
			m_pOwner->GainBuff(nFallingDebuffID);
		}
	}

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_FALLING_DAMAGE, 2, NEW_USHORT(m_pOwner->GetUIID()), NEW_INT(nFallingDamage));	
	m_pOwner->RouteToThisCell(pNewCommand);


	if (bIsDie)
	{
		m_pOwner->doDie();
	}
}