#include "stdafx.h"
#include "CSCommonParser.h"
#include "MCsvParser.h"

bool CSMotionFactorParser::Parse(MXmlElement* pElement, MOTION_FACTOR_GROUP& outMotionFactorGroup)
{
	_Attribute(&outMotionFactorGroup.var[MF_BEATEN].nValue,		pElement, XML_ATTR_MF_BEATEN);
	_Attribute(&outMotionFactorGroup.var[MF_BEATEN].nWeight,		pElement, XML_ATTR_MF_WEIGHT_BEATEN);
	_Attribute(&outMotionFactorGroup.var[MF_KNOCKBACK].nValue,		pElement, XML_ATTR_MF_KNOCKBACK);
	_Attribute(&outMotionFactorGroup.var[MF_KNOCKBACK].nWeight,	pElement, XML_ATTR_MF_WEIGHT_KNOCKBACK);
	_Attribute(&outMotionFactorGroup.var[MF_STUN].nValue,			pElement, XML_ATTR_MF_STUN);
	_Attribute(&outMotionFactorGroup.var[MF_STUN].nWeight,			pElement, XML_ATTR_MF_WEIGHT_STUN);
	_Attribute(&outMotionFactorGroup.var[MF_DEFLECT].nValue,		pElement, XML_ATTR_MF_DEFLECT);
	_Attribute(&outMotionFactorGroup.var[MF_DEFLECT].nWeight,		pElement, XML_ATTR_MF_WEIGHT_DEFLECT);
	_Attribute(&outMotionFactorGroup.var[MF_THROWUP].nValue,		pElement, XML_ATTR_MF_THROWUP);
	_Attribute(&outMotionFactorGroup.var[MF_THROWUP].nWeight,		pElement, XML_ATTR_MF_WEIGHT_THROWUP);
	_Attribute(&outMotionFactorGroup.var[MF_KNOCKDOWN].nValue,		pElement, XML_ATTR_MF_KNOCKDOWN);
	_Attribute(&outMotionFactorGroup.var[MF_KNOCKDOWN].nWeight,	pElement, XML_ATTR_MF_WEIGHT_KNOCKDOWN);
	_Attribute(&outMotionFactorGroup.var[MF_UPPERED].nValue,		pElement, XML_ATTR_MF_UPPERED);
	_Attribute(&outMotionFactorGroup.var[MF_UPPERED].nWeight,	pElement, XML_ATTR_MF_WEIGHT_UPPERED);

	_Attribute(&outMotionFactorGroup.var[MF_FAKE_BEATEN].nWeight,	pElement, XML_ATTR_MF_WEIGHT_FAKE_BEATEN);
	_Attribute(&outMotionFactorGroup.var[MF_FAKE_KNOCKBACK].nWeight,	pElement, XML_ATTR_MF_WEIGHT_FAKE_KNOCKBACK);

	return true;
}

bool CSModifierParser::Parse_InstantModifier( MXmlElement* pElement, CSInstantModifier& outModifier )
{
	_Attribute(&outModifier.nHP.nMod, pElement, XML_ATTR_MOD_HP);
	_Attribute(&outModifier.nEN.nMod, pElement, XML_ATTR_MOD_EN);
	_Attribute(&outModifier.nSTA.nMod, pElement, XML_ATTR_MOD_STA);

	_Attribute(&outModifier.nHP.fPer, pElement, XML_ATTR_MOD_HP_RATE);
	_Attribute(&outModifier.nEN.fPer, pElement, XML_ATTR_MOD_EN_RATE);
	_Attribute(&outModifier.nSTA.fPer, pElement, XML_ATTR_MOD_STA_RATE);	

	return true;
}

bool CSModifierParser::Parse_ActorModifier( MXmlElement* pElement, CSActorModifier& outActorModifier )
{
	_Attribute(&outActorModifier.nHPMax.nMod,		pElement, XML_ATTR_MOD_HP_MAX);
	_Attribute(&outActorModifier.nENMax.nMod,		pElement, XML_ATTR_MOD_EN_MAX);
	_Attribute(&outActorModifier.nSTAMax.nMod,		pElement, XML_ATTR_MOD_STA_MAX);
	_Attribute(&outActorModifier.nHPMax.fPer,		pElement, XML_ATTR_MOD_HP_MAX_AMP);
	_Attribute(&outActorModifier.nENMax.fPer,		pElement, XML_ATTR_MOD_EN_MAX_AMP);
	_Attribute(&outActorModifier.nSTAMax.fPer,		pElement, XML_ATTR_MOD_STA_MAX_AMP);

	_Attribute(&outActorModifier.fMoveSpeed,		pElement, XML_ATTR_MOD_MOVE_SPEED);
	_Attribute(&outActorModifier.fBattleMoveSpeed,	pElement, XML_ATTR_MOD_BATTLE_MOVE_SPEED);
	_Attribute(&outActorModifier.fAttackSpeed,		pElement, XML_ATTR_MOD_ATTACK_SPEED);
	_Attribute(&outActorModifier.fCastSpeed,		pElement, XML_ATTR_MOD_CAST_SPEED);

	_Attribute(&outActorModifier.nHPRegen.nMod,		pElement, XML_ATTR_MOD_HP_REGEN);
	_Attribute(&outActorModifier.nENRegen.nMod,		pElement, XML_ATTR_MOD_EN_REGEN);
	_Attribute(&outActorModifier.nSTARegen.nMod,	pElement, XML_ATTR_MOD_STA_REGEN);
	_Attribute(&outActorModifier.nHPRegen.fPer,		pElement, XML_ATTR_MOD_HP_REGEN_AMP);
	_Attribute(&outActorModifier.nENRegen.fPer,		pElement, XML_ATTR_MOD_EN_REGEN_AMP);
	_Attribute(&outActorModifier.nSTARegen.fPer,	pElement, XML_ATTR_MOD_STA_REGEN_AMP);

	_Attribute(&outActorModifier.fPhysicDamageAmp,	pElement, XML_ATTR_MOD_PHYSIC_ATK_AMP);
	_Attribute(&outActorModifier.fMagicDamageAmp,	pElement, XML_ATTR_MOD_MAGIC_ATK_AMP);

	_Attribute(&outActorModifier.fDefenceAmp[DA_SLASH],		pElement, XML_ATTR_MOD_DEF_SLASH_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_BLUNT],		pElement, XML_ATTR_MOD_DEF_BLUNT_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_PIERCE],	pElement, XML_ATTR_MOD_DEF_PIERCE_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_HOLY],		pElement, XML_ATTR_MOD_DEF_HOLY_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_UNHOLY],	pElement, XML_ATTR_MOD_DEF_UNHOLY_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_FIRE],		pElement, XML_ATTR_MOD_DEF_FIRE_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_COLD],		pElement, XML_ATTR_MOD_DEF_COLD_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_LIGHTNING],	pElement, XML_ATTR_MOD_DEF_LIGHTNING_AMP);
	_Attribute(&outActorModifier.fDefenceAmp[DA_POISON],	pElement, XML_ATTR_MOD_DEF_POISON_AMP);

	_Attribute(&outActorModifier.nAP.nMod,			pElement, XML_ATTR_MOD_AP);
	_Attribute(&outActorModifier.nABS.nMod,			pElement, XML_ATTR_MOD_ABS);
	_Attribute(&outActorModifier.fGuardRate.nMod,	pElement, XML_ATTR_MOD_GUARDRATE);
	
	_Attribute(&outActorModifier.nAP.fPer,			pElement, XML_ATTR_MOD_AP_RATE);

	_Attribute(&outActorModifier.fPhysicDodgeAmp,		pElement, XML_ATTR_MOD_PHYSIC_DODGE_AMP);
	_Attribute(&outActorModifier.fMagicDodgeAmp,		pElement, XML_ATTR_MOD_MAGIC_DODGE_AMP);
	_Attribute(&outActorModifier.fPhysicHitRateAmp, 	pElement, XML_ATTR_MOD_PHYSIC_HIT_RATE_AMP);
	_Attribute(&outActorModifier.fMagicHitRateAmp, 		pElement, XML_ATTR_MOD_MAGIC_HIT_RATE_AMP);

	_Attribute(&outActorModifier.fCriticalAmp,			pElement, XML_ATTR_MOD_CRITICAL_AMP);
	_Attribute(&outActorModifier.fCriticalPhysicAmp,	pElement, XML_ATTR_MOD_CRITICAL_PHYSIC_AMP);
	_Attribute(&outActorModifier.fCriticalMagicAmp,		pElement, XML_ATTR_MOD_CRITICAL_MAGIC_AMP);

	_Attribute(&outActorModifier.fCriticalPhysicAmpForMe,	pElement, XML_ATTR_MOD_CRITICAL_PHYSIC_AMP_FORME);
	_Attribute(&outActorModifier.fCriticalMagicAmpForMe, pElement, XML_ATTR_MOD_CRITICAL_MAGIC_AMP_FORME);

	_Attribute(&outActorModifier.fCriticalPhysicDamageAmp,		pElement, XML_ATTR_MOD_CRITICAL_PHYSIC_DAMAGE_AMP);
	_Attribute(&outActorModifier.fCriticalPhysicDamageAmpForMe, pElement, XML_ATTR_MOD_CRITICAL_PHYSIC_DAMAGE_AMP_FORME);
	_Attribute(&outActorModifier.fCriticalMagicDamageAmp,		pElement, XML_ATTR_MOD_CRITICAL_MAGIC_DAMAGE_AMP);
	_Attribute(&outActorModifier.fCriticalMagicDamageAmpForMe,	pElement, XML_ATTR_MOD_CRITICAL_MAGIC_DAMAGE_AMP_FORME);

	_Attribute(&outActorModifier.fHealAmp,		pElement, XML_ATTR_MOD_HEAL_AMP);
	_Attribute(&outActorModifier.fHealAmpForMe, pElement, XML_ATTR_MOD_HEAL_AMP_FORME);

	_Attribute(&outActorModifier.MotionFactorValue[MF_BEATEN].nMod,		pElement, NPC_XML_ATTR_BEATEN_MOD_VALUE);
	_Attribute(&outActorModifier.MotionFactorValue[MF_KNOCKBACK].nMod,	pElement, NPC_XML_ATTR_KNOCKBACK_MOD_VALUE);
	_Attribute(&outActorModifier.MotionFactorValue[MF_STUN].nMod,		pElement, NPC_XML_ATTR_STUN_MOD_VALUE);
	_Attribute(&outActorModifier.MotionFactorValue[MF_DEFLECT].nMod,	pElement, NPC_XML_ATTR_DEFLECT_MOD_VALUE);	
	_Attribute(&outActorModifier.MotionFactorValue[MF_THROWUP].nMod,	pElement, NPC_XML_ATTR_THROWUP_MOD_VALUE);
	_Attribute(&outActorModifier.MotionFactorValue[MF_KNOCKDOWN].nMod,	pElement, NPC_XML_ATTR_KNOCKDOWN_MOD_VALUE);
	_Attribute(&outActorModifier.MotionFactorValue[MF_UPPERED].nMod,	pElement, NPC_XML_ATTR_UPPERED_MOD_VALUE);

	_Attribute(&outActorModifier.MotionFactorWeight[MF_BEATEN].nMod,		pElement, NPC_XML_ATTR_BEATEN_MOD_WEIGHT);
	_Attribute(&outActorModifier.MotionFactorWeight[MF_KNOCKBACK].nMod,	pElement, NPC_XML_ATTR_KNOCKBACK_MOD_WEIGHT);
	_Attribute(&outActorModifier.MotionFactorWeight[MF_STUN].nMod,		pElement, NPC_XML_ATTR_STUN_MOD_WEIGHT);
	_Attribute(&outActorModifier.MotionFactorWeight[MF_DEFLECT].nMod,	pElement, NPC_XML_ATTR_DEFLECT_MOD_WEIGHT);	
	_Attribute(&outActorModifier.MotionFactorWeight[MF_THROWUP].nMod,	pElement, NPC_XML_ATTR_THROWUP_MOD_WEIGHT);
	_Attribute(&outActorModifier.MotionFactorWeight[MF_KNOCKDOWN].nMod,	pElement, NPC_XML_ATTR_KNOCKDOWN_MOD_WEIGHT);
	_Attribute(&outActorModifier.MotionFactorWeight[MF_UPPERED].nMod,	pElement, NPC_XML_ATTR_UPPERED_MOD_WEIGHT);

	int nResistValue = 0;

	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_FR))
	{
		outActorModifier.nResistances[TRT_FR].nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_CR))
	{
		outActorModifier.nResistances[TRT_CR].nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_LR))
	{
		outActorModifier.nResistances[TRT_LR].nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_PR))
	{
		outActorModifier.nResistances[TRT_PR].nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_HR))
	{
		outActorModifier.nResistances[TRT_HR].nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_UR))
	{
		outActorModifier.nResistances[TRT_UR].nMod = nResistValue;
	}

	float fResistValue = 0.0f;

	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_FR_RATE))
	{
		outActorModifier.nResistances[TRT_FR].fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_CR_RATE))
	{
		outActorModifier.nResistances[TRT_CR].fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_LR_RATE))
	{
		outActorModifier.nResistances[TRT_LR].fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_PR_RATE))
	{
		outActorModifier.nResistances[TRT_PR].fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_HR_RATE))
	{
		outActorModifier.nResistances[TRT_HR].fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_UR_RATE))
	{
		outActorModifier.nResistances[TRT_UR].fPer = fResistValue;
	}

	// Attribute Damage AP - constant
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_HOLY_AP))
	{
		outActorModifier.nHolyAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_UNHOLY_AP))
	{
		outActorModifier.nUnholyAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_FIRE_AP))
	{
		outActorModifier.nFireAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_COLD_AP))
	{
		outActorModifier.nColdAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_LIGHTNING_AP))
	{
		outActorModifier.nLightningAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_POISON_AP))
	{
		outActorModifier.nPoisonAP.nMod = nResistValue;
	}

	// Attribute Damage AP - percentage
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_HOLY_AP_RATE))
	{
		outActorModifier.nHolyAP.fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_UNHOLY_AP_RATE))
	{
		outActorModifier.nUnholyAP.fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_FIRE_AP_RATE))
	{
		outActorModifier.nFireAP.fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_COLD_AP_RATE))
	{
		outActorModifier.nColdAP.fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_LIGHTNING_AP_RATE))
	{
		outActorModifier.nLightningAP.fPer = fResistValue;
	}
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_POISON_AP_RATE))
	{
		outActorModifier.nPoisonAP.fPer = fResistValue;
	}

	// Attribute Magic Damage AP - constant
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_HOLY_MAGIC_AP))
	{
		outActorModifier.nHolyMagicAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_UNHOLY_MAGIC_AP))
	{
		outActorModifier.nUnholyMagicAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_FIRE_MAGIC_AP))
	{
		outActorModifier.nFireMagicAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_COLD_MAGIC_AP))
	{
		outActorModifier.nColdMagicAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_LIGHTNING_MAGIC_AP))
	{
		outActorModifier.nLightningMagicAP.nMod = nResistValue;
	}
	if (_Attribute(&nResistValue, pElement, XML_ATTR_MOD_POISON_MAGIC_AP))
	{
		outActorModifier.nPoisonMagicAP.nMod = nResistValue;
	}

	_Attribute(&outActorModifier.fModPhysicPene, pElement, XML_ATTR_MOD_PHYSIC_PENE);
	_Attribute(&outActorModifier.fModMagicPene,  pElement, XML_ATTR_MOD_MAGIC_PENE);


	// Ã¤Áý
	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_GATHER_TIME_MINE_AMP))
	{
		outActorModifier.fGatherTimeAmp[GATHER_MINE].fPer = fResistValue;
	}

	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_GATHER_TIME_HERB_AMP))
	{
		outActorModifier.fGatherTimeAmp[GATHER_HERB].fPer = fResistValue;
	}

	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_GATHER_TIME_RELIC_AMP))
	{
		outActorModifier.fGatherTimeAmp[GATHER_RELIC].fPer = fResistValue;
	}

	if (_Attribute(&fResistValue, pElement, XML_ATTR_MOD_GATHER_TIME_LUMBER_AMP))
	{
		outActorModifier.fGatherTimeAmp[GATHER_LUMBER].fPer = fResistValue;
	}


	return true;
}

bool CSModifierParser::Parse_PlayerModifier( MXmlElement* pElement, CSPlayerModifier& outPlayerModifier )
{
	_Attribute(&outPlayerModifier.nSTR.nMod,			pElement, XML_ATTR_MOD_STR);
	_Attribute(&outPlayerModifier.nDEX.nMod,			pElement, XML_ATTR_MOD_DEX);
	_Attribute(&outPlayerModifier.nCON.nMod,			pElement, XML_ATTR_MOD_CON);
	_Attribute(&outPlayerModifier.nCHA.nMod,			pElement, XML_ATTR_MOD_CHA);
	_Attribute(&outPlayerModifier.nINT.nMod,			pElement, XML_ATTR_MOD_INT);

	_Attribute(&outPlayerModifier.nSTR.fPer,			pElement, XML_ATTR_MOD_STR_RATE);
	_Attribute(&outPlayerModifier.nDEX.fPer,			pElement, XML_ATTR_MOD_DEX_RATE);
	_Attribute(&outPlayerModifier.nCON.fPer,			pElement, XML_ATTR_MOD_CON_RATE);
	_Attribute(&outPlayerModifier.nCHA.fPer,			pElement, XML_ATTR_MOD_CHA_RATE);
	_Attribute(&outPlayerModifier.nINT.fPer,			pElement, XML_ATTR_MOD_INT_RATE);

	_Attribute(&outPlayerModifier.fRideSpeed,			pElement, XML_ATTR_MOD_RIDE_SPEED);

	_Attribute(&outPlayerModifier.fPVPDamage,			pElement, XML_ATTR_MOD_PVP_DAMAGE);
	_Attribute(&outPlayerModifier.nPVPAP.nMod,			pElement, XML_ATTR_MOD_PVP_AP);

	_Attribute(&outPlayerModifier.fEXPRate,				pElement, XML_ATTR_MOD_EXP_RATE);
	_Attribute(&outPlayerModifier.fDropMoneyAmount,		pElement, XML_ATTR_MOD_DROP_MONEY_AMOUNT);
	_Attribute(&outPlayerModifier.fDropItemRate,		pElement, XML_ATTR_MOD_DROP_ITEM_RATE);
	_Attribute(&outPlayerModifier.fDropItemAmount,		pElement, XML_ATTR_MOD_DROP_ITEM_AMOUNT);

	return true;
}

void CSCSVParser::Parse_CSV( MXmlElement* pElement, const char* name, vector<int>& outvecArgs )
{
	string strValue;
	if (!_Attribute(strValue, pElement, name)) 
		return;

	MCsvParser csv_parser;
	csv_parser.LoadFromStream(strValue.c_str());
	int nColCount = csv_parser.GetColCount(0);

	char szBuff[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (csv_parser.GetData(nCol, 0, szBuff, 1024))
		{
			outvecArgs.push_back(atoi(szBuff));
		}
	}
}

void CSCSVParser::Parse_CSV( MXmlElement* pElement, const char* name, set<int>& outvecArgs )
{
	string strValue;
	if (!_Attribute(strValue,				pElement, name)) 
		return;

	MCsvParser csv_parser;
	csv_parser.LoadFromStream(strValue.c_str());
	int nColCount = csv_parser.GetColCount(0);

	char szBuff[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (csv_parser.GetData(nCol, 0, szBuff, 1024))
		{
			outvecArgs.insert(atoi(szBuff));
		}
	}
}

void CSCSVParser::Parse_CSV( MXmlElement* pElement, const char* name, set<string>& outvecArgs )
{
	outvecArgs.clear();

	string strValue;
	if (!_Attribute(strValue,				pElement, name)) 
		return;

	MCsvParser csv_parser;
	csv_parser.LoadFromStream(strValue.c_str());
	int nColCount = csv_parser.GetColCount(0);

	char szBuff[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (csv_parser.GetData(nCol, 0, szBuff, 1024))
		{
			outvecArgs.insert(szBuff);
		}
	}
}

void CSCSVParser::Parse_CSV( MXmlElement* pElement, const char* name, vector<string>& outvecArgs )
{
	outvecArgs.clear();

	string strValue;
	if (!_Attribute(strValue,				pElement, name)) 
		return;

	MCsvParser csv_parser;
	csv_parser.LoadFromStream(strValue.c_str());
	int nColCount = csv_parser.GetColCount(0);

	char szBuff[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (csv_parser.GetData(nCol, 0, szBuff, 1024))
		{
			outvecArgs.push_back(szBuff);
		}
	}
}