#pragma once


#define TALENT_XML_TAG_MAIET							"maiet"
#define TALENT_XML_TAG_TALENT							"TALENT"
#define TALENT_XML_ATTR_ID								"id"
#define TALENT_XML_ATTR_REFERENCE_ID					"reference_id"
#define TALENT_XML_ATTR_REFERENCE_MODE					"reference_mode"
#define TALENT_XML_VALUE_TYPE							"type"

//////////////////////////////////////////////////////////////////////////


#define TALENT_XML_ATTR_RANGE	"Range"
#define TALENT_XML_ATTR_HP_COST				"HPCost"
#define TALENT_XML_ATTR_EN_COST				"ENCost"
#define TALENT_XML_ATTR_STA_COST				"STACost"
#define TALENT_XML_ATTR_EN_TICKCOST			"ENTickCost"
#define TALENT_XML_ATTR_STA_TICKCOST			"STATickCost"
#define TALENT_XML_ATTR_CASTING_ANI			"CastingAni"
#define TALENT_XML_ATTR_CASTINGLOOP_ANI		"CastingLoopAni"
#define TALENT_XML_ATTR_CASTING_TIME			"CastingTime"
#define TALENT_XML_ATTR_USE_ANI				"UseAni"
#define TALENT_XML_ATTR_EXTRA_ACT_ANI		"ExtraAni"
#define TALENT_XML_ATTR_EXTRA_ACT_ANI_2		"ExtraAni2"
#define TALENT_XML_ATTR_EXTRA_ACT_ANI_3		"ExtraAni3"
#define TALENT_XML_ATTR_ATTACK_DIRECTION		"AttackDirection"
#define TALENT_XML_ATTR_PROJECTILE_TYPE		"ProjectileType"
#define TALENT_XML_ATTR_PROJECTILE_SPEED		"ProjectileSpeed"
#define TALENT_XML_ATTR_NEEDED_TP					"NeededTP"
//#define TALENT_XML_ATTR_TRIGGER_TYPE				"TriggerType"
#define TALENT_XML_ATTR_FREEZE_FRAME		"FreezeFrame"

// test
#define TALENT_XML_ATTR_HIT_SIZE				"Area2Size"
#define TALENT_XML_ATTR_HIT_POSITION_X		"Area2PosX"
#define TALENT_XML_ATTR_HIT_POSITION_Y		"Area2PosY"
#define TALENT_XML_ATTR_HIT_POSITION_Z		"Area2PosZ"
// test


#define TALENT_XML_VALUE_TALENTTYPE_SKILL				"skill"
#define TALENT_XML_VALUE_TALENTTYPE_WEAPON				"weapon"
#define TALENT_XML_VALUE_TALENTTYPE_ENCHANT				"enchant"
#define TALENT_XML_VALUE_TALENTTYPE_ITEM				"item"
#define TALENT_XML_VALUE_TALENTTYPE_GROUP				"group"


#define TALENT_XML_VALUE_ATTACKDIRECTION_FRONT			"front"
#define TALENT_XML_VALUE_ATTACKDIRECTION_BACK			"back"
#define TALENT_XML_VALUE_ATTACKDIRECTION_LEFT			"left"
#define TALENT_XML_VALUE_ATTACKDIRECTION_RIGHT			"right"
#define TALENT_XML_VALUE_PROJECTILETYPE_MISSILE			"missile"
#define TALENT_XML_VALUE_PROJECTILETYPE_GUIDED			"guided"
#define TALENT_XML_VALUE_PROJECTILETYPE_HITCAPSULE_GUIDED			"hitcapsule_guided"
#define TALENT_XML_VALUE_TARGETING_TYPE_NONE			"none"

//#define TALENT_XML_VALUE_TRIGGERTYPE_MANUAL			"manual"
//#define TALENT_XML_VALUE_TRIGGERTYPE_AUTO			"auto"

#define TALENT_XML_VALUE_USE_BLOOD_EFFECT		 "UseBloodEffect"

#define TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_STRAIGHT		"straight"
#define TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_CURVE			"curve"
#define TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_PARABOLA		"parabola"

#define	TALENT_XML_VALUE_GRAPPLED_ANI				"GrappledAni"
#define	TALENT_XML_VALUE_GRAPPLED_BONE				"GrappledBone"

//----------------------------------------------------------------------------//
// [12/12/2007 isnara] Talent Effect Base
#define TALENT_XML_TAG_TALENT_BASE					"EffectBase"
#define TALENT_XML_TAG_TALENT_MELEE					"MeleeAttack"
#define TALENT_XML_TAG_TALENT_MELEE_SECONDARY		"MeleeSecondaryAttack"
#define TALENT_XML_TAG_TALENT_MAGIC					"MagicAttack"
#define TALENT_XML_TAG_TALENT_ARCHERY				"ArcheryAttack"
#define TALENT_XML_TAG_TALENT_GUN					"GunAttack"
#define TALENT_XML_TAG_TALENT_DEFENSE				"Defense"
#define TALENT_XML_TAG_TALENT_NOSHIELD_DEFENSE		"NoShieldDefense"
#define TALENT_XML_TAG_TALENT_MONSTER_DEFENSE		"MonsterDefense"
//------------------------------------------------------------------------
// [2007/5/25 isnara] Talent Effect Data
#define TALENT_XML_TAG_TALENT_EFFECT				"EffectData"
#define TALENT_XML_TAG_TALENT_CAST_EFFECT			"CastEffect"
#define TALENT_XML_TAG_TALENT_USE_EFFECT			"UseEffect"
#define TALENT_XML_TAG_TALENT_ATTACK_EFFECT			"AttackEffect"
#define TALENT_XML_TAG_TALENT_ATTACK_SECONDARY_EFFECT "AttackSecondaryEffect"
#define TALENT_XML_TAG_TALENT_MISS_EFFECT			"MissEffect"
#define TALENT_XML_TAG_TALENT_SHOCK_EFFECT			"ShockEffect"
#define TALENT_XML_TAG_TALENT_DURATION_EFFECT		"DurationEffect"
#define TALENT_XML_TAG_TALENT_DEFENSE_EFFECT		"DefenseEffect"
#define TALENT_XML_TAG_TALENT_HIT_EFFECT_POSDIR		"HitEffectPosDir"
#define TALENT_XML_TAG_TALENT_CAST_END_EFFECT		"CastEndEffect"

#define TALENT_XML_TAG_TALENT_DATA					"Data"

#define TALENT_XML_ATTR_TALENT_EFFECTINFONAME		"EffectInfoName"
#define TALENT_XML_ATTR_TALENT_EFFECTNAME			"EffectName"
#define TALENT_XML_ATTR_TALENT_FOLLOW				"Follow"
#define TALENT_XML_ATTR_TALENT_GROUND				"Ground"
#define TALENT_XML_ATTR_TALENT_BONENAME				"BoneName"
#define TALENT_XML_ATTR_TALENT_PARAMETER			"Parameter"
#define TALENT_XML_ATTR_TALENT_IMPACTPOSITION		"ImpactPosition"
#define TALENT_XML_ATTR_TALENT_EFFECTDIR			"EffectDir"
#define TALENT_XML_ATTR_TALENT_SCALE				"Scale"
#define TALENT_XML_ATTR_TALENT_HIT_MESH_NAME		"Mesh"
#define TALENT_XML_ATTR_TALENT_HIT_TIME				"HitTime"
#define TALENT_XML_ATTR_TALENT_HIT_UP_VEC			"Up"
#define TALENT_XML_ATTR_TALENT_HIT_POS				"Pos"

#define TALENT_IMPACTPOSITION_MODEL_POS				"ModelPos"
#define TALENT_IMPACTPOSITION_HIT_POS				"HitPos"

#define TALENT_XML_ATTR_PROJECTILE_START_X			"start_x"
#define TALENT_XML_ATTR_PROJECTILE_START_Y			"start_y"
#define TALENT_XML_ATTR_PROJECTILE_START_Z			"start_z"
#define TALENT_XML_ATTR_PROJECTILE_START_MALE_X		"male_start_x"
#define TALENT_XML_ATTR_PROJECTILE_START_MALE_Y		"male_start_y"
#define TALENT_XML_ATTR_PROJECTILE_START_MALE_Z		"male_start_z"

#define TALENT_XML_ATTR_PROJECTILE_VISUAL_TYPE		"ProjectileVisualType"
#define TALENT_XML_ATTR_PROJECTILE_RANGE			"ProjectileRange"

#define TALENT_XML_ATTR_PROJECTILE_EFFECT_NAME				"ProjectileEffectName"
#define TALENT_XML_ATTR_PROJECTILE_ATTACK_EFFECT_NAME		"AttackEffectName"
#define TALENT_XML_ATTR_PROJECTILE_HITGROUND_EFFECT_NAME	"HitGroundEffectName"

#define TLAENT_XML_ATTR_PROJECTILE_SEGMENT_INDEX	"SegmentIndex"
#define TLAENT_XML_ATTR_PROJECTILE_CAPSULE_INDEX	"CapsuleIndex"

#define TALENT_XML_ATTR_ICONNAME					"icon"

#define TALENT_XML_ATTR_SHOVE_WHILE_MOVING			"ShoveWhileMoving"
#define TALENT_XML_ATTR_PC_SHOVE_TYPE				"PC_ShoveType"
#define TALENT_XML_ATTR_NPC_SHOVE_TYPE				"NPC_ShoveType"

#define TALENT_XML_ATTR_SHOVE_TYPE_STOP				"stop"
#define TALENT_XML_ATTR_SHOVE_TYPE_PASS				"pass"
#define TALENT_XML_ATTR_SHOVE_TYPE_SCRAPE			"scrape"
#define TALENT_XML_ATTR_SHOVE_TYPE_PUSH				"push"

#define TALENT_XML_ATTR_HIT_BLOOD_SCALE				"HitBloodScale"
