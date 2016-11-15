#ifndef _CSCOMMON_PARSER_H
#define _CSCOMMON_PARSER_H

#include "MXml.h"
#include "CSMotionFactorInfo.h"
#include "CSModifier.h"
#include "CSCommonLib.h"

class CSCOMMON_API CSMotionFactorParser
{
public:
	static bool Parse(MXmlElement* pElement, MOTION_FACTOR_GROUP& outMotionFactorGroup);
};




class CSCOMMON_API CSModifierParser
{
public:
	static bool Parse_InstantModifier(MXmlElement* pElement, CSInstantModifier& outModifier);
	static bool Parse_ActorModifier(MXmlElement* pElement, CSActorModifier& outActorModifier);
	static bool Parse_PlayerModifier(MXmlElement* pElement, CSPlayerModifier& outPlayerModifier);
};

class CSCOMMON_API CSCSVParser
{
public:
	void Parse_CSV( MXmlElement* pElement, const char* name, vector<int>& outvecArgs );
	void Parse_CSV( MXmlElement* pElement, const char* name, set<int>& outvecArgs );
	void Parse_CSV( MXmlElement* pElement, const char* name, set<string>& outvecArgs );
	void Parse_CSV( MXmlElement* pElement, const char* name, vector<string>& outvecArgs );
};

#define XML_ATTR_MF_BEATEN				"MF_BEATEN"
#define XML_ATTR_MF_WEIGHT_BEATEN		"MF_WEIGHT_BEATEN"
#define XML_ATTR_MF_KNOCKBACK			"MF_KNOCKBACK"
#define XML_ATTR_MF_WEIGHT_KNOCKBACK	"MF_WEIGHT_KNOCKBACK"
#define XML_ATTR_MF_STUN				"MF_STUN"
#define XML_ATTR_MF_WEIGHT_STUN			"MF_WEIGHT_STUN"
#define XML_ATTR_MF_DEFLECT				"MF_DEFLECT"
#define XML_ATTR_MF_WEIGHT_DEFLECT		"MF_WEIGHT_DEFLECT"
#define XML_ATTR_MF_THROWUP				"MF_THROWUP"
#define XML_ATTR_MF_WEIGHT_THROWUP		"MF_WEIGHT_THROWUP"
#define XML_ATTR_MF_KNOCKDOWN			"MF_KNOCKDOWN"
#define XML_ATTR_MF_WEIGHT_KNOCKDOWN	"MF_WEIGHT_KNOCKDOWN"
#define XML_ATTR_MF_UPPERED				"MF_UPPERED"
#define XML_ATTR_MF_WEIGHT_UPPERED		"MF_WEIGHT_UPPERED"

#define XML_ATTR_MF_WEIGHT_FAKE_BEATEN	"MF_WEIGHT_FAKE_BEATEN"
#define XML_ATTR_MF_WEIGHT_FAKE_KNOCKBACK	"MF_WEIGHT_FAKE_KNOCKBACK"

#define XML_ATTR_MOD_STR				"ModSTR"
#define XML_ATTR_MOD_DEX				"ModDEX"
#define XML_ATTR_MOD_CON				"ModCON"
#define XML_ATTR_MOD_CHA				"ModCHA"
#define XML_ATTR_MOD_INT				"ModINT"

#define XML_ATTR_MOD_STR_RATE			"ModSTRRate"
#define XML_ATTR_MOD_DEX_RATE			"ModDEXRate"
#define XML_ATTR_MOD_CON_RATE			"ModCONRate"
#define XML_ATTR_MOD_CHA_RATE			"ModCHARate"
#define XML_ATTR_MOD_INT_RATE			"ModINTRate"

#define XML_ATTR_MOD_HP_MAX				"ModHPMax"
#define XML_ATTR_MOD_EN_MAX				"ModENMax"
#define XML_ATTR_MOD_STA_MAX			"ModSTAMax"

#define XML_ATTR_MOD_MOVE_SPEED			"ModMoveSpeed"
#define XML_ATTR_MOD_RIDE_SPEED			"ModRideSpeed"
#define XML_ATTR_MOD_HP_REGEN			"ModHPRegen"
#define XML_ATTR_MOD_EN_REGEN			"ModENRegen"
#define XML_ATTR_MOD_STA_REGEN			"ModSTARegen"
#define XML_ATTR_MOD_HP_REGEN_AMP		"ModHPRegenAmp"
#define XML_ATTR_MOD_EN_REGEN_AMP		"ModENRegenAmp"
#define XML_ATTR_MOD_STA_REGEN_AMP		"ModSTARegenAmp"

#define XML_ATTR_MOD_MELEE_ATK_AMP		"ModMeleeAtkAmp"
#define XML_ATTR_MOD_RANGE_ATK_AMP		"ModRangeAtkAmp"
#define XML_ATTR_MOD_MAGIC_ATK_AMP		"ModMagicAtkAmp"

#define XML_ATTR_MOD_DEF_SLASH_AMP		"ModDefSlashAmp"
#define XML_ATTR_MOD_DEF_BLUNT_AMP		"ModDefBluntAmp"
#define XML_ATTR_MOD_DEF_PIERCE_AMP		"ModDefPierceAmp"
#define XML_ATTR_MOD_DEF_HOLY_AMP		"ModDefHolyAmp"
#define XML_ATTR_MOD_DEF_UNHOLY_AMP		"ModDefUnholyAmp"
#define XML_ATTR_MOD_DEF_FIRE_AMP		"ModDefFireAmp"
#define XML_ATTR_MOD_DEF_COLD_AMP		"ModDefColdAmp"
#define XML_ATTR_MOD_DEF_LIGHTNING_AMP	"ModDefLightningAmp"
#define XML_ATTR_MOD_DEF_POISON_AMP		"ModDefPoisonAmp"

#define XML_ATTR_MOD_AP					"ModAP"
#define XML_ATTR_MOD_ABS				"ModABS"
#define XML_ATTR_MOD_GUARDRATE			"ModGuardRate"

#define XML_ATTR_MOD_CRITICAL_MELEE_DAMAGE_AMP		"ModCriticalMeleeDamageAmp"
#define XML_ATTR_MOD_CRITICAL_RANGE_DAMAGE_AMP		"ModCriticalRangeDamageAmp"
#define XML_ATTR_MOD_CRITICAL_MAGIC_DAMAGE_AMP		"ModCriticalMagicDamageAmp"

#define XML_ATTR_MOD_AP_RATE			"ModAPRate"

#define XML_ATTR_MOD_FR					"ModFR"
#define XML_ATTR_MOD_CR					"ModCR"
#define XML_ATTR_MOD_LR					"ModLR"
#define XML_ATTR_MOD_PR					"ModPR"
#define XML_ATTR_MOD_HR					"ModHR"
#define XML_ATTR_MOD_UR					"ModUR"
#define XML_ATTR_MOD_FR_RATE			"ModFRRate"        
#define XML_ATTR_MOD_CR_RATE			"ModCRRate"
#define XML_ATTR_MOD_LR_RATE			"ModLRRate"
#define XML_ATTR_MOD_PR_RATE			"ModPRRate"
#define XML_ATTR_MOD_HR_RATE			"ModHRRate"
#define XML_ATTR_MOD_UR_RATE			"ModURRate"

#define XML_ATTR_MOD_MELEE_PENE			"ModMeleePene"	
#define XML_ATTR_MOD_RANGE_PENE			"ModRangePene"
#define XML_ATTR_MOD_MAGIC_PENE			"ModMagicPene"

#define XML_ATTR_MOD_GATHER_TIME_MINE_AMP	"ModGatherTimeMineAmp"
#define XML_ATTR_MOD_GATHER_TIME_HERB_AMP	"ModGatherTimeHerbAmp"
#define XML_ATTR_MOD_GATHER_TIME_RELIC_AMP	"ModGatherTimeRelicAmp"
#define XML_ATTR_MOD_GATHER_TIME_LUMBER_AMP	"ModGatherTimeLumberAmp"

#define NPC_XML_ATTR_BEATEN_MOD_VALUE		"BeatenModValue"
#define NPC_XML_ATTR_KNOCKBACK_MOD_VALUE	"KnockbackModValue"
#define NPC_XML_ATTR_STUN_MOD_VALUE			"StunModValue"
#define NPC_XML_ATTR_DEFLECT_MOD_VALUE		"DeflectModValue"
#define NPC_XML_ATTR_THROWUP_MOD_VALUE		"ThrowupModValue"
#define NPC_XML_ATTR_KNOCKDOWN_MOD_VALUE	"KnockdownModValue"
#define NPC_XML_ATTR_UPPERED_MOD_VALUE		"UpperedModValue"

#define NPC_XML_ATTR_BEATEN_MOD_WEIGHT		"BeatenModWeight"
#define NPC_XML_ATTR_KNOCKBACK_MOD_WEIGHT	"KnockbackModWeight"
#define NPC_XML_ATTR_STUN_MOD_WEIGHT			"StunModWeight"
#define NPC_XML_ATTR_DEFLECT_MOD_WEIGHT		"DeflectModWeight"
#define NPC_XML_ATTR_THROWUP_MOD_WEIGHT		"ThrowupModWeight"
#define NPC_XML_ATTR_KNOCKDOWN_MOD_WEIGHT	"KnockdownModWeight"
#define NPC_XML_ATTR_UPPERED_MOD_WEIGHT		"UpperedModWeight"

#define XML_ATTR_MOD_HP						"ModHP"
#define XML_ATTR_MOD_EN						"ModEN"
#define XML_ATTR_MOD_STA					"ModSTA"

#define XML_ATTR_MOD_HP_RATE				"ModHPRate"
#define XML_ATTR_MOD_EN_RATE				"ModENRate"
#define XML_ATTR_MOD_STA_RATE				"ModSTARate"

#define XML_ATTR_MOD_HP_RATE					"ModHPRate"
#define XML_ATTR_MOD_EN_RATE					"ModENRate"
#define XML_ATTR_MOD_STA_RATE					"ModSTARate"
#define XML_ATTR_MOD_HP_MAX_AMP					"ModHPMaxAmp"
#define XML_ATTR_MOD_EN_MAX_AMP					"ModENMaxAmp"
#define XML_ATTR_MOD_STA_MAX_AMP				"ModSTAMaxAmp"
#define XML_ATTR_MOD_MELEE_DODGE_AMP			"ModMeleeDodgeAmp"
#define XML_ATTR_MOD_RANGE_DODGE_AMP			"ModRangeDodgeAmp"
#define XML_ATTR_MOD_MAGIC_DODGE_AMP			"ModMagicDodgeAmp"
#define XML_ATTR_MOD_MELEE_HIT_RATE_AMP			"ModMeleeHitRateAmp"
#define XML_ATTR_MOD_RANGE_HIT_RATE_AMP			"ModRangeHitRateAmp"
#define XML_ATTR_MOD_MAGIC_HIT_RATE_AMP			"ModMagicHitRateAmp"

#define XML_ATTR_MOD_CRITICAL_AMP				"ModCriticalAmp"
#define XML_ATTR_MOD_CRITICAL_MELEE_AMP			"ModCriticalMeleeAmp"
#define XML_ATTR_MOD_CRITICAL_MELEE_AMP_FORME	"ModCriticalMeleeAmpForMe"
#define XML_ATTR_MOD_CRITICAL_RANGE_AMP			"ModCriticalRangeAmp"
#define XML_ATTR_MOD_CRITICAL_MAGIC_AMP			"ModCriticalMagicAmp"


#endif