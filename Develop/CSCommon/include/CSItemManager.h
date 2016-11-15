#pragma once

#include "MUID.h"
#include "MXml.h"
#include "CSItemData.h"
#include "CSDef.h"
#include "CSChrInfo.h"
#include "CSCommonLib.h"

#define ITEM_XML_TAG_ITEM					"item"
#define ITEM_XML_ATTR_ID					"id"
#define ITEM_XML_ATTR_NAME					"name"
#define ITEM_XML_ATTR_USE_ANIMATION			"use_ani"
#define ITEM_XML_ATTR_TEXTURE_COLOR			"texcolor"
#define ITEM_XML_ATTR_MAX_DURABILITY		"max_durability"
#define ITEM_XML_ATTR_STACK_AMOUNT			"stack_amount"
#define ITEM_XML_ATTR_QUEST_RELATED			"quest_related"
#define ITEM_XML_ATTR_CLAIM_REQUIRED		"claim_required"
#define ITEM_XML_ATTR_CRAFT_OPTION_RATE		"craft_optionrate"
#define ITEM_XML_ATTR_HOUSING_NPC			"housing_npc"
#define ITEM_XML_ATTR_SOUL_TYPE				"soul_type"
#define ITEM_XML_ATTR_SOUL_MAX_QUANTITY		"soul_maxquantity"
#define ITEM_XML_ATTR_SOUL_LINKED_ITEM		"soul_linkeditem"
#define ITEM_XML_ATTR_SWORD_TRAIL_EFFECT	"SwordTrailEffect"
#define ITEM_XML_ATTR_REPAIR_GRADE			"repair_grade"
#define ITEM_XML_ATTR_DYEABLE				"Dyeable"
#define ITEM_XML_ATTR_REPAIRABLE			"repairable"
#define ITEM_XML_ATTR_CONDITION				"condition"
#define ITEM_XML_ATTR_VALIDATION			"validation"
#define ITEM_XML_ATTR_LINKED_ENCHANT		"LinkedEnchant"
#define ITEM_XML_ATTR_ENCHANT_RATE			"EnchantRate"
#define ITEM_XML_ATTR_ENCHANT_LIMIT_EQUIPMENTS "EnchantLimitEquipments"
#define ITEM_XML_ATTR_ENCHANT_LIMIT_WEAPONS	"EnchantLimitWeapons"
#define ITEM_XML_ATTR_ENCHANT_LIMIT_LEVEL	"EnchantLimitLevel"
#define ITEM_XML_ATTR_ENCHANT_COLOR			"EnchantColor"
#define ITEM_XML_ATTR_EQUIP_ENCHANT_SLOT	"EquipEnchantSlot"


class CSCOMMON_API CSItemManager
{	
protected:
		virtual CSItemData*		NewItemData() = 0;
		virtual void			Clear() {}
		virtual bool			ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml);
		virtual void			ParseItem_SubMtrl(MXmlElement* pElement, CSItemData* pItemData);
		virtual CSItemData*		GetItemData(int id) = 0;
		virtual void			Cooking() = 0;
public:
		CSItemManager();
		virtual ~CSItemManager();
		bool					LoadFromFile(const TCHAR* filename);			// xitem.xml에서 아이템 정보 로드
		bool					LoadFromFile_SubMtrl(const TCHAR* filename);	// xitem_submtrl.xml에서 아이템 Sub Material 재질 정보 로드		
};