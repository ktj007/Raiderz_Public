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
		nHolyAP.IsModified() ||
		nUnholyAP.IsModified() ||
		nFireAP.IsModified() ||
		nColdAP.IsModified() ||
		nLightningAP.IsModified() ||
		nPoisonAP.IsModified() ||
		nHolyMagicAP.IsModified() ||
		nUnholyMagicAP.IsModified() ||
		nFireMagicAP.IsModified() ||
		nColdMagicAP.IsModified() ||
		nLightningMagicAP.IsModified() ||
		nPoisonMagicAP.IsModified() ||
		fGuardRate.IsModified() ||
		fMoveSpeed != 0.0f ||
		fBattleMoveSpeed != 0.0f ||
		fAttackSpeed != 0.0f ||
		fCastSpeed != 0.0f ||
		nHPRegen.IsModified() ||
		nENRegen.IsModified() ||
		nSTARegen.IsModified() ||
		fPhysicDodgeAmp != 0.0f ||
		fMagicDodgeAmp != 0.0f ||
		fPhysicHitRateAmp != 0.0f ||
		fMagicHitRateAmp != 0.0f ||

		fCriticalAmp != 0.0f ||
		fCriticalPhysicAmp != 0.0f ||
		fCriticalMagicAmp != 0.0f ||

		fCriticalPhysicAmpForMe != 0.0f ||
		fCriticalMagicAmpForMe != 0.0f ||

		fCriticalPhysicDamageAmp != 0.0f ||
		fCriticalPhysicDamageAmpForMe != 0.0f ||
		fCriticalMagicDamageAmp != 0.0f ||
		fCriticalMagicDamageAmpForMe != 0.0f ||

		fPhysicDamageAmp != 0.0f ||
		fMagicDamageAmp != 0.0f ||

		fHealAmp != 0.0f ||
		fHealAmpForMe != 0.0f ||
		
		fModPhysicPene != 0.0f ||
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
	
	fMoveSpeed			+= rhs.fMoveSpeed;
	fBattleMoveSpeed	+= rhs.fBattleMoveSpeed;
	fAttackSpeed		+= rhs.fAttackSpeed;
	fCastSpeed			+= rhs.fCastSpeed;

	nHPRegen	+= rhs.nHPRegen;
	nENRegen	+= rhs.nENRegen;
	nSTARegen	+= rhs.nSTARegen;

	nAP			+= rhs.nAP;
	nABS		+= rhs.nABS;

	nHolyAP				+= rhs.nHolyAP;
	nUnholyAP			+= rhs.nUnholyAP;
	nFireAP				+= rhs.nFireAP;
	nColdAP				+= rhs.nColdAP;
	nLightningAP		+= rhs.nLightningAP;
	nPoisonAP			+= rhs.nPoisonAP;

	nHolyMagicAP		+= rhs.nHolyMagicAP;
	nUnholyMagicAP		+= rhs.nUnholyMagicAP;
	nFireMagicAP		+= rhs.nFireMagicAP;
	nColdMagicAP		+= rhs.nColdMagicAP;
	nLightningMagicAP	+= rhs.nLightningMagicAP;
	nPoisonMagicAP		+= rhs.nPoisonMagicAP;

	fGuardRate	+= rhs.fGuardRate;

	fPhysicDodgeAmp += rhs.fPhysicDodgeAmp;
	fMagicDodgeAmp += rhs.fMagicDodgeAmp;
	fPhysicHitRateAmp += rhs.fPhysicHitRateAmp;
	fMagicHitRateAmp += rhs.fMagicHitRateAmp;

	fCriticalAmp += rhs.fCriticalAmp;
	fCriticalPhysicAmp += rhs.fCriticalPhysicAmp;
	fCriticalMagicAmp += rhs.fCriticalMagicAmp;
	
	fCriticalPhysicAmpForMe += rhs.fCriticalPhysicAmpForMe;
	fCriticalMagicAmpForMe  += rhs.fCriticalMagicAmpForMe;

	fCriticalPhysicDamageAmp		+= rhs.fCriticalPhysicDamageAmp;
	fCriticalPhysicDamageAmpForMe	+= rhs.fCriticalPhysicDamageAmpForMe;
	fCriticalMagicDamageAmp			+= rhs.fCriticalMagicDamageAmp;
	fCriticalMagicDamageAmpForMe	+= rhs.fCriticalMagicDamageAmpForMe;

	fPhysicDamageAmp += rhs.fPhysicDamageAmp;
	fMagicDamageAmp	 += rhs.fMagicDamageAmp;

	fHealAmp		+= rhs.fHealAmp;
	fHealAmpForMe	+= rhs.fHealAmpForMe;

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

	fModPhysicPene += rhs.fModPhysicPene;
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

	fMoveSpeed			-= rhs.fMoveSpeed;
	fBattleMoveSpeed	-= rhs.fBattleMoveSpeed;
	fAttackSpeed		-= rhs.fAttackSpeed;
	fCastSpeed			-= rhs.fCastSpeed;

	nHPRegen	-= rhs.nHPRegen;
	nENRegen	-= rhs.nENRegen;
	nSTARegen	-= rhs.nSTARegen;

	nAP			-= rhs.nAP;
	nABS		-= rhs.nABS;

	nHolyAP				-= rhs.nHolyAP;
	nUnholyAP			-= rhs.nUnholyAP;
	nFireAP				-= rhs.nFireAP;
	nColdAP				-= rhs.nColdAP;
	nLightningAP		-= rhs.nLightningAP;
	nPoisonAP			-= rhs.nPoisonAP;

	nHolyMagicAP		-= rhs.nHolyMagicAP;
	nUnholyMagicAP		-= rhs.nUnholyMagicAP;
	nFireMagicAP		-= rhs.nFireMagicAP;
	nColdMagicAP		-= rhs.nColdMagicAP;
	nLightningMagicAP	-= rhs.nLightningMagicAP;
	nPoisonMagicAP		-= rhs.nPoisonMagicAP;

	fGuardRate	-= rhs.fGuardRate;

	fPhysicDodgeAmp -= rhs.fPhysicDodgeAmp;
	fMagicDodgeAmp -= rhs.fMagicDodgeAmp;
	fPhysicHitRateAmp -= rhs.fPhysicHitRateAmp;
	fMagicHitRateAmp -= rhs.fMagicHitRateAmp;

	fCriticalAmp -= rhs.fCriticalAmp;
	fCriticalPhysicAmp -= rhs.fCriticalPhysicAmp;
	fCriticalMagicAmp -= rhs.fCriticalMagicAmp;

	fCriticalPhysicAmpForMe -= rhs.fCriticalPhysicAmpForMe;
	fCriticalMagicAmpForMe -= rhs.fCriticalMagicAmpForMe;

	fCriticalPhysicDamageAmp		-= rhs.fCriticalPhysicDamageAmp;
	fCriticalPhysicDamageAmpForMe	-= rhs.fCriticalPhysicDamageAmpForMe;
	fCriticalMagicDamageAmp			-= rhs.fCriticalMagicDamageAmp;
	fCriticalMagicDamageAmpForMe	-= rhs.fCriticalMagicDamageAmpForMe;

	fPhysicDamageAmp	-= rhs.fPhysicDamageAmp;
	fMagicDamageAmp	-= rhs.fMagicDamageAmp;

	fHealAmp		-= rhs.fHealAmp;
	fHealAmpForMe	-= rhs.fHealAmpForMe;

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

	fModPhysicPene -= rhs.fModPhysicPene;
	fModMagicPene -= rhs.fModMagicPene;

	return *this;
}

void CSActorModifier::Clear()
{
	nHPMax.Clear();
	nENMax.Clear();
	nSTAMax.Clear();

	fMoveSpeed = 0.0f;
	fBattleMoveSpeed = 0.0f;
	fAttackSpeed = 0.0f;
	fCastSpeed = 0.0f;

	nHPRegen.Clear();
	nENRegen.Clear();
	nSTARegen.Clear();

	nAP.Clear();
	nABS.Clear();

	nHolyAP.Clear();
	nUnholyAP.Clear();
	nFireAP.Clear();
	nColdAP.Clear();
	nLightningAP.Clear();
	nPoisonAP.Clear();

	nHolyMagicAP.Clear();
	nUnholyMagicAP.Clear();
	nFireMagicAP.Clear();
	nColdMagicAP.Clear();
	nLightningMagicAP.Clear();
	nPoisonMagicAP.Clear();

	fGuardRate.Clear();

	fPhysicDodgeAmp = 0.0f;
	fMagicDodgeAmp = 0.0f;
	fPhysicHitRateAmp = 0.0f;
	fMagicHitRateAmp = 0.0f;

	fCriticalAmp = 0.0f;
	fCriticalPhysicAmp = 0.0f;
	fCriticalMagicAmp = 0.0f;

	fCriticalPhysicAmpForMe = 0.0f;
	fCriticalMagicAmpForMe = 0.0f;

	fCriticalPhysicDamageAmp = 0.0f;
	fCriticalPhysicDamageAmpForMe = 0.0f;
	fCriticalMagicDamageAmp = 0.0f;
	fCriticalMagicDamageAmpForMe = 0.0f;
	fPhysicDamageAmp = 0.0f;
	fMagicDamageAmp = 0.0f;

	fHealAmp = 0.0f;
	fHealAmpForMe = 0.0f;

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

	fModPhysicPene = 0.0f;
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
		nCON.IsModified() ||
		fRideSpeed != 0.0f ||
		fPVPDamage != 0.0f ||
		nPVPAP.IsModified() ||
		fEXPRate != 0.0f ||
		fDropMoneyAmount != 0.0f ||
		fDropItemRate != 0.0f ||
		fDropItemAmount != 0.0f)
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

	fRideSpeed = 0.0f;

	fPVPDamage = 0.0f;
	nPVPAP.Clear();

	fEXPRate = 0.0f;
	fDropMoneyAmount = 0.0f;
	fDropItemRate = 0.0f;
	fDropItemAmount = 0.0f;
}

void CSPlayerModifier::CutInstantMod()
{
	nSTR.Clear();
	nDEX.Clear();
	nINT.Clear();
	nCHA.Clear();
	nCON.Clear();

	fRideSpeed = 0.0f;

	fPVPDamage = 0.0f;
	nPVPAP.Clear();

	fEXPRate = 0.0f;
	fDropMoneyAmount = 0.0f;
	fDropItemRate = 0.0f;
	fDropItemAmount = 0.0f;
}


CSPlayerModifier& CSPlayerModifier::operator+=( const CSPlayerModifier& rhs )
{
	nSTR += rhs.nSTR;
	nDEX += rhs.nDEX;
	nINT += rhs.nINT;
	nCHA += rhs.nCHA;
	nCON += rhs.nCON;

	fRideSpeed += rhs.fRideSpeed;

	fPVPDamage	+= rhs.fPVPDamage;
	nPVPAP		+= rhs.nPVPAP;

	fEXPRate			+= rhs.fEXPRate;
	fDropMoneyAmount	+= rhs.fDropMoneyAmount;
	fDropItemRate		+= rhs.fDropItemRate;
	fDropItemAmount		+= rhs.fDropItemAmount;

	return *this;
}

CSPlayerModifier& CSPlayerModifier::operator-=( const CSPlayerModifier& rhs )
{
	nSTR -= rhs.nSTR;
	nDEX -= rhs.nDEX;
	nINT -= rhs.nINT;
	nCHA -= rhs.nCHA;
	nCON -= rhs.nCON;

	fRideSpeed -= rhs.fRideSpeed;

	fPVPDamage	-= rhs.fPVPDamage;
	nPVPAP		-= rhs.nPVPAP;

	fEXPRate			-= rhs.fEXPRate;
	fDropMoneyAmount	-= rhs.fDropMoneyAmount;
	fDropItemRate		-= rhs.fDropItemRate;
	fDropItemAmount		-= rhs.fDropItemAmount;

	return *this;
}
