#include "stdafx.h"
#include "CSModifier.h"


CSInstantModifier::CSInstantModifier()
{
	Clear();
}

CSInstantModifier& CSInstantModifier::operator+=( const CSInstantModifier& rhs )
{
	if (!rhs.IsModified())
		return *this;

	nHP	+= rhs.nHP;
	nEN += rhs.nEN;
	nSTA += rhs.nSTA;

	return *this;
}

CSInstantModifier& CSInstantModifier::operator-=( const CSInstantModifier& rhs )
{
	if (!rhs.IsModified())
		return *this;

	nHP -= rhs.nHP;
	nEN -= rhs.nEN;
	nSTA -= rhs.nSTA;

	return *this;
}

bool CSInstantModifier::IsModified() const
{
	if (nHP.IsModified() ||
		nEN.IsModified() ||
		nSTA.IsModified())
	{
		return true;
	}

	return false;
}

void CSInstantModifier::Clear()
{
	nHP.Clear();
	nEN.Clear();
	nSTA.Clear();
}

CSActorModifier::CSActorModifier()
{
	Clear();
}

bool CSActorModifier::IsModified() const 
{
	if (nHPMax.IsModified() ||
		nENMax.IsModified() ||
		nSTAMax.IsModified() ||
		nAP.IsModified() ||
		nABS.IsModified() ||
		fGuardRate.IsModified() ||
		fMoveSpeed != 0.0f ||
		nHPRegen.IsModified() ||
		nENRegen.IsModified() ||
		nSTARegen.IsModified() ||
		fMeleeDodgeAmp != 0.0f ||
		fRangeDodgeAmp != 0.0f ||
		fMagicDodgeAmp != 0.0f ||
		fMeleeHitRateAmp != 0.0f ||
		fRangeHitRateAmp != 0.0f ||
		fMagicHitRateAmp != 0.0f ||

		fCriticalAmp != 0.0f ||
		fCriticalMeleeAmp != 0.0f ||
		fCriticalRangeAmp != 0.0f ||
		fCriticalMagicAmp != 0.0f ||

		fCriticalMeleeAmpForMe != 0.0f ||

		fCriticalMeleeDamageAmp != 0.0f ||
		fCriticalRangeDamageAmp != 0.0f ||
		fCriticalMagicDamageAmp != 0.0f ||

		fMeleeDamageAmp != 0.0f ||
		fRangeDamageAmp != 0.0f ||
		fMagicDamageAmp != 0.0f ||
		
		fModMeleePene != 0.0f ||
		fModRangePene != 0.0f ||
		fModMagicPene != 0.0f
		)
	{
		return true;
	}

	for (int i = 0; i < DA_MAX; i++)
	{
		if (fDefenceAmp[i] != 0.0f) return true;
	}

	for (int i=0; i<MF_PRIORITY_SIZE; i++)
	{
		if (MotionFactorValue[i].IsModified()) return true;
		if (MotionFactorWeight[i].IsModified()) return true;
	}

	for (int i=0; i < TRT_MAX; i++)
	{
		if (nResistances[i].IsModified()) return true;
	}

	for (int i = 0; i < MAX_GATHER_TYPE; i++)
	{
		if (fGatherTimeAmp[i].IsModified())	return true;
	}

	return false;
}

bool CSActorModifier::IsMotionfactorModified() const
{
	for (int i=0; i<MF_PRIORITY_SIZE; i++)
	{
		if (MotionFactorValue[i].IsModified()) return true;
		if (MotionFactorWeight[i].IsModified()) return true;
	}

	return false;
}

CSActorModifier& CSActorModifier::operator+=( const CSActorModifier& rhs )
{
	if (!rhs.IsModified())
		return *this;

	nHPMax		+= rhs.nHPMax;
	nENMax		+= rhs.nENMax;
	nSTAMax		+= rhs.nSTAMax;
	
	fMoveSpeed	+= rhs.fMoveSpeed;
	nHPRegen	+= rhs.nHPRegen;
	nENRegen	+= rhs.nENRegen;
	nSTARegen	+= rhs.nSTARegen;

	nAP			+= rhs.nAP;
	nABS		+= rhs.nABS;
	fGuardRate	+= rhs.fGuardRate;
	fMeleeDodgeAmp += rhs.fMeleeDodgeAmp;
	fRangeDodgeAmp += rhs.fRangeDodgeAmp;
	fMagicDodgeAmp += rhs.fMagicDodgeAmp;
	fMeleeHitRateAmp += rhs.fMeleeHitRateAmp;
	fRangeHitRateAmp += rhs.fRangeHitRateAmp;
	fMagicHitRateAmp += rhs.fMagicHitRateAmp;

	fCriticalAmp += rhs.fCriticalAmp;
	fCriticalMeleeAmp += rhs.fCriticalMeleeAmp;
	fCriticalRangeAmp += rhs.fCriticalRangeAmp;	
	fCriticalMagicAmp += rhs.fCriticalMagicAmp;
	
	fCriticalMeleeAmpForMe += rhs.fCriticalMeleeAmpForMe;

	fCriticalMeleeDamageAmp	+= rhs.fCriticalMeleeDamageAmp;
	fCriticalRangeDamageAmp	+= rhs.fCriticalRangeDamageAmp;
	fCriticalMagicDamageAmp	+= rhs.fCriticalMagicDamageAmp;

	fMeleeDamageAmp	+= rhs.fMeleeDamageAmp;
	fRangeDamageAmp	+= rhs.fRangeDamageAmp;
	fMagicDamageAmp	+= rhs.fMagicDamageAmp;

	for (int i=0; i < DA_MAX; i++)
	{
		fDefenceAmp[i] += rhs.fDefenceAmp[i];
	}

	for (int i=0; i<MF_PRIORITY_SIZE; i++)
	{
		MotionFactorValue[i] += rhs.MotionFactorValue[i];
		MotionFactorWeight[i] += rhs.MotionFactorWeight[i];
	}

	for (int i=0; i<TRT_MAX; i++)
	{
		nResistances[i] += rhs.nResistances[i];
	}

	for (int i = 0; i < MAX_GATHER_TYPE; i++)
	{
		fGatherTimeAmp[i] += rhs.fGatherTimeAmp[i];
	}

	fModMeleePene += rhs.fModMeleePene;
	fModRangePene += rhs.fModRangePene;
	fModMagicPene += rhs.fModMagicPene;

	return *this;
}

CSActorModifier& CSActorModifier::operator-=( const CSActorModifier& rhs )
{
	if (!rhs.IsModified())
		return *this;

	nHPMax		-= rhs.nHPMax;
	nENMax		-= rhs.nENMax;
	nSTAMax		-= rhs.nSTAMax;

	fMoveSpeed		-= rhs.fMoveSpeed;
	nHPRegen	-= rhs.nHPRegen;
	nENRegen	-= rhs.nENRegen;
	nSTARegen	-= rhs.nSTARegen;

	nAP			-= rhs.nAP;
	nABS		-= rhs.nABS;
	fGuardRate	-= rhs.fGuardRate;
	fMeleeDodgeAmp -= rhs.fMeleeDodgeAmp;
	fRangeDodgeAmp -= rhs.fRangeDodgeAmp;
	fMagicDodgeAmp -= rhs.fMagicDodgeAmp;
	fMeleeHitRateAmp -= rhs.fMeleeHitRateAmp;
	fRangeHitRateAmp -= rhs.fRangeHitRateAmp;
	fMagicHitRateAmp -= rhs.fMagicHitRateAmp;

	fCriticalAmp -= rhs.fCriticalAmp;
	fCriticalMeleeAmp -= rhs.fCriticalMeleeAmp;
	fCriticalRangeAmp -= rhs.fCriticalRangeAmp;
	fCriticalMagicAmp -= rhs.fCriticalMagicAmp;

	fCriticalMeleeAmpForMe -= rhs.fCriticalMeleeAmpForMe;

	fCriticalMeleeDamageAmp	-= rhs.fCriticalMeleeDamageAmp;
	fCriticalRangeDamageAmp	-= rhs.fCriticalRangeDamageAmp;
	fCriticalMagicDamageAmp	-= rhs.fCriticalMagicDamageAmp;
	fMeleeDamageAmp	-= rhs.fMeleeDamageAmp;
	fRangeDamageAmp	-= rhs.fRangeDamageAmp;
	fMagicDamageAmp	-= rhs.fMagicDamageAmp;

	for (int i=0; i < DA_MAX; i++)
	{
		fDefenceAmp[i] -= rhs.fDefenceAmp[i];
	}

	for (int i=0; i<MF_PRIORITY_SIZE; i++)
	{
		MotionFactorValue[i] -= rhs.MotionFactorValue[i];
		MotionFactorWeight[i] -= rhs.MotionFactorWeight[i];
	}

	for (int i=0; i<TRT_MAX; i++)
	{
		if (nResistances[i].nMod >= rhs.nResistances[i].nMod) 
			nResistances[i] -= rhs.nResistances[i];
		else 
			nResistances[i].Clear();
	}

	for (int i = 0; i < MAX_GATHER_TYPE; i++)
	{
		fGatherTimeAmp[i] -= rhs.fGatherTimeAmp[i];
	}

	fModMeleePene -= rhs.fModMeleePene;
	fModRangePene -= rhs.fModRangePene;
	fModMagicPene -= rhs.fModMagicPene;

	return *this;
}

void CSActorModifier::Clear()
{
	nHPMax.Clear();
	nENMax.Clear();
	nSTAMax.Clear();

	fMoveSpeed = 0.0f;
	nHPRegen.Clear();
	nENRegen.Clear();
	nSTARegen.Clear();

	nAP.Clear();
	nABS.Clear();
	fGuardRate.Clear();
	fMeleeDodgeAmp = 0.0f;
	fRangeDodgeAmp = 0.0f;
	fMagicDodgeAmp = 0.0f;
	fMeleeHitRateAmp = 0.0f;
	fRangeHitRateAmp = 0.0f;
	fMagicHitRateAmp = 0.0f;

	fCriticalAmp = 0.0f;
	fCriticalMeleeAmp = 0.0f;
	fCriticalRangeAmp = 0.0f;
	fCriticalMagicAmp = 0.0f;

	fCriticalMeleeAmpForMe = 0.0f;

	fCriticalMeleeDamageAmp = 0.0f;
	fCriticalRangeDamageAmp = 0.0f;
	fCriticalMagicDamageAmp = 0.0f;
	fMeleeDamageAmp = 0.0f;
	fRangeDamageAmp = 0.0f;
	fMagicDamageAmp = 0.0f;

	for (int i=0; i < DA_MAX; i++)
	{
		fDefenceAmp[i] = 0.0f;
	}

	for (int i=0; i<MF_PRIORITY_SIZE; i++)
	{
		MotionFactorValue[i].Clear();
		MotionFactorWeight[i].Clear();
	}

	for (int i=0; i<TRT_MAX; i++)
	{
		nResistances[i].Clear();
	}

	for (int i = 0; i < MAX_GATHER_TYPE; i++)
	{
		fGatherTimeAmp[i].Clear();
	}

	fModMeleePene = 0.0f;
	fModRangePene = 0.0f;
	fModMagicPene = 0.0f;
}




CSPlayerModifier::CSPlayerModifier()
{
	Clear();
}

bool CSPlayerModifier::IsModified() const
{
	if (nSTR.IsModified() || 
		nDEX.IsModified() ||
		nINT.IsModified() ||
		nCHA.IsModified() ||
		nCON.IsModified())
	{
		return true;
	}

	return false;
}

void CSPlayerModifier::Clear()
{
	nSTR.Clear();
	nDEX.Clear();
	nINT.Clear();
	nCHA.Clear();
	nCON.Clear();
}

void CSPlayerModifier::CutInstantMod()
{
	nSTR.Clear();
	nDEX.Clear();
	nINT.Clear();
	nCHA.Clear();
	nCON.Clear();
}


CSPlayerModifier& CSPlayerModifier::operator+=( const CSPlayerModifier& rhs )
{
	nSTR += rhs.nSTR;
	nDEX += rhs.nDEX;
	nINT += rhs.nINT;
	nCHA += rhs.nCHA;
	nCON += rhs.nCON;

	return *this;
}

CSPlayerModifier& CSPlayerModifier::operator-=( const CSPlayerModifier& rhs )
{
	nSTR -= rhs.nSTR;
	nDEX -= rhs.nDEX;
	nINT -= rhs.nINT;
	nCHA -= rhs.nCHA;
	nCON -= rhs.nCON;

	return *this;
}
