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
	if (false == _EndFalling(bIsFallenOnWater, fFallingHeight))
	{
		// route zero falling damage.
		RouteFallingDamage(0);
	}
}

bool GPlayerFalling::_EndFalling(bool bIsFallenOnWater, float fFallingHeight)
{
	if (true == m_pOwner->IsNowInvincibility()) return false;
	if (true == m_pOwner->IsDead()) return false;

	if (fFallingHeight <= 0) return false;
	if (m_pOwner->GetHP() <= 0) return false;

	int nFallingDamage = GCalculator::CalcFallingDamage(bIsFallenOnWater, fFallingHeight, m_pOwner->GetMaxHP());
	if (nFallingDamage <= 0) return false;

	bool bIsDie = m_pOwner->doDamage(MUID::ZERO, DA_NONE, nFallingDamage);

	if (false == bIsDie && false == bIsFallenOnWater)
	{
		int nFallingDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		if (nFallingDebuffID != INVALID_BUFF_ID)
		{
			m_pOwner->GainBuff(nFallingDebuffID);
		}
	}

	RouteFallingDamage(nFallingDamage);


	if (bIsDie)
	{
		m_pOwner->doDie();
	}

	return true;
}

void GPlayerFalling::RouteFallingDamage(int nFallingDamage)
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_FALLING_DAMAGE, 2, NEW_USHORT(m_pOwner->GetUIID()), NEW_INT(nFallingDamage));
	m_pOwner->RouteToThisCell(pNewCommand);
}
