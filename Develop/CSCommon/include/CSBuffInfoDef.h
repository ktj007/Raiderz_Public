#pragma once

// Maiet XML
#define BUFF_XML_TAG_MAIET							"maiet"
#define BUFF_XML_TAG_BUFF							"BUFF"

// 기본 구성
#define BUFF_XML_ATTR_ID							"ID"
#define BUFF_XML_ATTR_NAME							"Name"
#define BUFF_XML_ATTR_TYPE							"Type"
#define BUFF_XML_ATTR_STACK_TYPE					"StackType"
#define BUFF_XML_ATTR_STACK_MAX_COUNT				"StackMaxCount"
#define BUFF_XML_ATTR_DESCRIPTION					"Desc"
#define BUFF_XML_ATTR_OVERRIDE						"override"

// 유지 관련
#define BUFF_XML_ATTR_DURATION						"Duration"
#define BUFF_XML_ATTR_PERIOD						"Period"

// 스택 로직 관련
#define BUFF_XML_ATTR_CANCELABLE					"Cancelable"
#define BUFF_XML_ATTR_STACKSLOT						"StackSlot"
#define BUFF_XML_ATTR_STACKPOWER					"StackPower"

// 선행 조건

// 발동/해제 조건
// TALENT 쪽에서 가져올 것.

// 유지비용
#define BUFF_XML_ATTR_TICKCOST_HP					"TickCostHP"
#define BUFF_XML_ATTR_TICKCOST_EN					"TickCostEN"
#define BUFF_XML_ATTR_TICKCOST_STA					"TickCostSTA"

// 흡수 효과

// 효과 트리거(추가 효과)

// 저항 관련
#define BUFF_XML_ATTR_RESIST_METHOD					"ResistMethod"
#define BUFF_XML_ATTR_RESIST_TYPE					"ResistType"
#define BUFF_XML_ATTR_RESIST_DIFFICULTY				"ResistDifficulty"

// 헤이트 관련
#define BUFF_XML_ATTR_HATE_ADJ						"HateAdj"
#define BUFF_XML_ATTR_HATE_FACTOR					"HateFactor"

// 효과내용
#define BUFF_XML_ATTR_MIN_DAMAGE					"MinDamage"
#define BUFF_XML_ATTR_MAX_DAMAGE					"MaxDamage"
#define BUFF_XML_ATTR_MIN_HEAL						"MinHeal"
#define BUFF_XML_ATTR_MAX_HEAL						"MaxHeal"
#define BUFF_XML_ATTR_PERCENTAGE_HEAL				"PecentageHeal"
#define BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB			"PassiveExtraAttrib"
#define BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB_VALUE	"PassiveExtraAttribValue"
#define BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB2			"PassiveExtraAttrib2"
#define BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB_VALUE2	"PassiveExtraAttribValue2"

// 트리거드 버프 관련
#define BUFF_XML_ATTR_TRIGGEREDBUFF_ID		"TriggeredBuff"
#define BUFF_XML_ATTR_TRIGGEREDBUFF_RATE	"TriggeredBuffRate"
#define BUFF_XML_ATTR_TRIGGEREDBUFF_TIMING	"TriggeredBuffTiming"

// Include/Exclude
#define BUFF_XML_ATTR_KEEP_INCLUDE	"KeepInclude"
#define BUFF_XML_ATTR_KEEP_EXCLUDE	"KeepExclude"
#define BUFF_XML_ATTR_INCLUDE	"Include"
#define BUFF_XML_ATTR_EXCLUDE	"Exclude"
#define BUFF_XML_ATTR_WEAPON_APPLY_RATE	"WeaponApplyRate"


// 효과내용2(보정치)
// 보정치는 CSCommandParser에서 별도로 파싱

// 모션팩터 수정치
// 생략

// 기타
#define BUFF_XML_ATTR_USEANI						"UseAni"
#define BUFF_XML_ATTR_ANI_POSTFIX					"AniPostfix"
#define BUFF_XML_ATTR_LOGABLE						"Logable"
#define BUFF_XML_ATTR_ICON							"Icon"
#define BUFF_XML_ATTR_SAVE_TO_DB					"SaveToDB"
#define BUFF_XML_ATTR_PALETTE_SLOT					"PaletteSlot"
#define BUFF_XML_ATTR_EQUIPITEM_LWEAPON				"EquipItem_LWeapon"
#define BUFF_XML_ATTR_EQUIPITEM_RWEAPON				"EquipItem_RWeapon"



#define BUFF_XML_ATTR_DAMAGE_ATTRIB					"DamageAttrib"
#define BUFF_XML_ATTR_ACTIVE_EXTRA_ATTRIB			"ActiveExtraAttrib"
#define BUFF_XML_ATTR_RIPOSTE_TYPE					"RiposteType"
#define BUFF_XML_ATTR_RIPOSTE_RATE					"RiposteRate"
#define BUFF_XML_ATTR_USE_TALENT					"UseTalent"
#define BUFF_XML_ATTR_EFFECTNAME					"EffectName"
#define BUFF_XML_ATTR_BONENAME						"BoneName"

