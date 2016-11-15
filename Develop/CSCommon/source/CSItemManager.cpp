#include "stdafx.h"
#include "CSItemManager.h"
#include "CSStrings.h"
#include "CSCommonParser.h"
#include "MLocale.h"
#include "CSHelper.h"
#include "CSFormatString.h"


CSItemManager::CSItemManager()
{
}


CSItemManager::~CSItemManager()
{
	Clear();
}

bool CSItemManager::LoadFromFile(const TCHAR* filename)
{
	MXml xml;
	if ( !xml.LoadFile(MLocale::ConvTCHARToAnsi(filename).c_str()))
	{
	//	_ASSERT(0);			// 일단 디버그모드에선 assert 걸어둠
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild("maiet").FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), "ITEM"))
		{
			CSItemData* pNewItemData = NewItemData();
			if (false == ParseItem(pElement, pNewItemData, &xml))
			{
				SAFE_DELETE(pNewItemData);
				continue;
			}
		}
	}
	return true;
}

bool CSItemManager::LoadFromFile_SubMtrl(const TCHAR* filename)
{
	MXml xml;
	if ( !xml.LoadFile(MLocale::ConvTCHARToAnsi(filename).c_str()))
	{
	//	_ASSERT(0);			// 일단 디버그모드에선 assert 걸어둠
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild("maiet").FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), "ITEM"))
		{
			MXmlAttribute* attr = pElement->FirstAttribute();
			
			if (!_stricmp(attr->Name(), "id"))
			{
				int nowID = atoi(attr->Value());
				CSItemData* pNewItemData = GetItemData(nowID);
				ParseItem_SubMtrl(pElement, pNewItemData);
			}
			else
				return false;
		}
	}
	return true;
}

bool CSItemManager::ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml)
{
	_Attribute(pItemData->m_nID, pElement, ITEM_XML_ATTR_ID);
	
	pItemData->m_strName = CSFormatString::Format(_T("ITEM_NAME_{0}"), FSParam(pItemData->m_nID));	// 문자열테이블 참조
	pItemData->m_strDesc = CSFormatString::Format(_T("ITEM_DESC_{0}"), FSParam(pItemData->m_nID));	// 문자열테이블 참조
		
	_Attribute(pItemData->m_strMeshName, pElement, "mesh_name");	
	_Attribute(pItemData->m_bMeshNameBySex, pElement, "mesh_name_by_sex");
	
	tstring strNodeName;
	if (_Attribute(strNodeName, pElement, "node_name"))	pItemData->m_strNodeNames.push_back(strNodeName);
	if (_Attribute(strNodeName, pElement, "node_name2")) pItemData->m_strNodeNames.push_back(strNodeName);
	if (_Attribute(strNodeName, pElement, "node_name3")) pItemData->m_strNodeNames.push_back(strNodeName);
	if (_Attribute(strNodeName, pElement, "node_name4")) pItemData->m_strNodeNames.push_back(strNodeName);

	_Attribute(pItemData->m_strParentName, pElement, "parent_name");
	_Attribute(pItemData->m_strParentNameLeft, pElement, "parent_name_left");

	tstring strItemTier;
	if (_Attribute(strItemTier, pElement, "tier"))
	{
		int nItemTier = CSStrings::StringToItemTier(strItemTier.c_str());
		if (nItemTier >= 0)
		{
			pItemData->m_nTier = static_cast<ITEM_TIER>(nItemTier);
		}
	}

	tstring strItemType;
	if (_Attribute(strItemType, pElement, "type"))
	{
		pItemData->m_ItemType = static_cast<SH_ITEM_TYPE>(CSStrings::StringToItemType(strItemType.c_str()));
	}

	tstring strGatherType;
	if (_Attribute(strGatherType, pElement, "gather_type"))
	{
		pItemData->m_nGatherType = (GATHER_TYPE)CSStrings::StringToGatherType(strGatherType.c_str());
	}

	tstring strDamageAttr;
	if (_Attribute(strDamageAttr, pElement, "DamageAttrib"))
	{
		pItemData->m_nDamageAttrib = static_cast<DAMAGE_ATTRIB>(CSStrings::StringToDamageAttrib(strDamageAttr.c_str()));
	}

	tstring strSlot;
	if (_Attribute(strSlot, pElement, "slot"))
	{
		for(int i = 0; i < ITEMSLOT_MAX; i++)
		{
			if (strSlot == CSItemSlotStr[i])
			{
				pItemData->m_nItemSlot = (SH_ITEM_SLOT)i;
				break;
			}
		}

		// aweapon = rweapon + lweapon
		if (strSlot == _T("aweapon"))
		{
			pItemData->m_nItemSlot = ITEMSLOT_RWEAPON;
			pItemData->m_bLeftWeaponEquipable = true;
		}

		if (strSlot == _T("finger"))
		{
			pItemData->m_nItemSlot = ITEMSLOT_RFINGER;
		}
	}

	tstring strSubSlot;
	if (_Attribute(strSubSlot, pElement, "sub_slot"))
	{
		for (int i = 0; i < ITEMSLOT_MAX; i++)
		{
			if (strSubSlot == CSItemSlotStr[i])
			{
				pItemData->m_nSubSlot = (SH_ITEM_SLOT)i;
				break;
			}
		}
	}

	tstring strWeaponType;
	if (_Attribute(strWeaponType, pElement, "weapon_type"))
	{
		pItemData->m_nWeaponType = static_cast<WEAPON_TYPE>(CSStrings::StringToWeaponType(strWeaponType.c_str()));
	}

	tstring strUsableType;
	if (_Attribute(strUsableType, pElement, "usable_type"))
	{
		pItemData->m_nUsableType = static_cast<USABLE_TYPE>(CSStrings::StringToUsableType(strUsableType.c_str()));
	}

	tstring strUsabeParam;
	if (_Attribute(strUsabeParam, pElement, "usable_param"))
	{
		CSStrings::StringNumbersToIntVector(strUsabeParam, pItemData->m_vecUsableParam);
	}

	tstring strDyeColor;
	if (_Attribute(strDyeColor, pElement, "DyeColor"))
	{
		CSStrings::StringNumbersToIntVector(strDyeColor, pItemData->m_vecDyeColor);
	}

	if (ITEMSLOT_HEAD != pItemData->m_nItemSlot && 
		ITEMSLOT_HANDS != pItemData->m_nItemSlot &&
		ITEMSLOT_FEET != pItemData->m_nItemSlot &&
		ITEMSLOT_BODY != pItemData->m_nItemSlot &&
		ITEMSLOT_LEG != pItemData->m_nItemSlot)
	{
		pItemData->m_bDyeable = false;
	}
	else
	{
		_Attribute(pItemData->m_bDyeable, pElement, ITEM_XML_ATTR_DYEABLE);
	}	

	if ( pItemData->m_ItemType == ITEMTYPE_USABLE &&
		pItemData->m_vecUsableParam.empty())
	{
		pItemData->m_vecUsableParam.push_back(0);
	}

	tstring strUsableConsume;
	if (_Attribute(strUsableConsume, pElement, "usable_consume"))
	{
		pItemData->m_bUsableConsume = CSStrings::StringToBool(strUsableConsume.c_str());
	}
	_Attribute(pItemData->m_bTradable, pElement, "tradable");
	_Attribute(pItemData->m_bDiscardable, pElement, "discardable");
	_Attribute(pItemData->m_bSellable, pElement, "sellable");
	_Attribute(pItemData->m_bUnique, pElement, "unique");
	_Attribute(pItemData->m_nBaseBuyingPrice, pElement, "buying_price");
	_Attribute(pItemData->m_nBaseSellingPrice, pElement, "selling_price");
	_Attribute(pItemData->m_nRepairGrade, pElement, ITEM_XML_ATTR_REPAIR_GRADE);
	_Attribute(pItemData->m_bRepairable, pElement, ITEM_XML_ATTR_REPAIRABLE);
	_Attribute(pItemData->m_nStackAmount, pElement, ITEM_XML_ATTR_STACK_AMOUNT);
	_Attribute(pItemData->m_nMaxDurability, pElement, ITEM_XML_ATTR_MAX_DURABILITY);
	_Attribute(pItemData->m_nAP, pElement, "PhysAP");
	_Attribute(pItemData->m_nMagicAP, pElement, "MagicAP");
	_Attribute(pItemData->m_nMinDamage, pElement, "min_damage");
	_Attribute(pItemData->m_fSpellPower, pElement, "spell_power");
	_Attribute(pItemData->m_nMaxDamage, pElement, "max_damage");
	_Attribute(pItemData->m_fCraftOptionRate, pElement, ITEM_XML_ATTR_CRAFT_OPTION_RATE);
	
	tstring strArmorType;
	if (_Attribute(strArmorType, pElement, "armor_type"))
	{
		pItemData->m_nArmorType = static_cast<ARMOR_TYPE>(CSStrings::StringToArmorType(strArmorType.c_str()));
	}

	string strColor;
	if (_Attribute(strColor, pElement, ITEM_XML_ATTR_TEXTURE_COLOR))
	{
		char buff[512] = {0, };		
		int color = strtol(strColor.c_str(), NULL, 16);		// XML에 16진수로 저장되어있다.
		if (color >= 0)
		{
			pItemData->m_bUseTexColor = true;
			pItemData->m_nTexColor = 0xFF000000 | color;
		}
	}

	_Attribute(pItemData->m_bQuestRelated, pElement, ITEM_XML_ATTR_QUEST_RELATED);
	_Attribute(pItemData->m_bClaimRequired, pElement, ITEM_XML_ATTR_CLAIM_REQUIRED);
	_Attribute(pItemData->m_nHousingNPC, pElement, ITEM_XML_ATTR_HOUSING_NPC);
	
	tstring strSoulType;
	if (_Attribute(strSoulType, pElement, ITEM_XML_ATTR_SOUL_TYPE))
	{
		pItemData->m_nSoulType = static_cast<SOUL_TYPE>(CSStrings::StringToSoulType(strSoulType.c_str()));
	}
	
	_Attribute(pItemData->m_nSoulMaxQuantity, pElement, ITEM_XML_ATTR_SOUL_MAX_QUANTITY);
	_Attribute(pItemData->m_nSoulLinkedItem, pElement, ITEM_XML_ATTR_SOUL_LINKED_ITEM);
	_Attribute(pItemData->m_strSwordTrailEffect, pElement, ITEM_XML_ATTR_SWORD_TRAIL_EFFECT);
	_Attribute(pItemData->m_bValidation, pElement, ITEM_XML_ATTR_VALIDATION);

	// 강화 관련 ---------------------
	_Attribute(pItemData->m_EnchantStone.nLinkedBuffID, pElement, ITEM_XML_ATTR_LINKED_ENCHANT);
	_Attribute(pItemData->m_EnchantStone.fSuccessRate, pElement, ITEM_XML_ATTR_ENCHANT_RATE);
	_Attribute(pItemData->m_EnchantStone.nLimitLevel, pElement, ITEM_XML_ATTR_ENCHANT_LIMIT_LEVEL);

	if (_Attribute(strColor, pElement, ITEM_XML_ATTR_ENCHANT_COLOR))
	{
		int nStrSize = arraysize(ENCHANT_COLOR_STR);
		for (int i=0; i<nStrSize; ++i)
		{
			if (strColor.length() <= 0)
				continue;

			if (strColor == MLocale::ConvTCHARToAnsi(ENCHANT_COLOR_STR[i]))
			{
				pItemData->m_EnchantStone.nColor = (ENCHANT_COLOR)i;
				break;
			}
		}
	}

	USES_CONVERSION_EX;
	set<string> setData;
	CSCSVParser csv_parser;
	csv_parser.Parse_CSV(pElement, ITEM_XML_ATTR_ENCHANT_LIMIT_EQUIPMENTS, setData);
	for each (string strText in setData)
	{
		size_t nOldSize = pItemData->m_EnchantStone.vecLimitEquipments.size();

		int nStrSize = arraysize(CSItemSlotStr);
		for (int i=0; i<nStrSize; ++i)
		{
			if (strText == MLocale::ConvTCHARToAnsi(CSItemSlotStr[i]))
			{
				pItemData->m_EnchantStone.vecLimitEquipments.push_back((SH_ITEM_SLOT)i);
				pItemData->m_EnchantStone.bAllowAllEquipments = false;
			}
		}

		if (pItemData->m_EnchantStone.vecLimitEquipments.size() == nOldSize)
		{
			mlog3("xitem.xml : %s, unknown item slot name : %s\n", ITEM_XML_ATTR_ENCHANT_LIMIT_EQUIPMENTS, strText.c_str());
		}
	}
	csv_parser.Parse_CSV(pElement, ITEM_XML_ATTR_ENCHANT_LIMIT_WEAPONS, setData);
	for each (string strText in setData)
	{
		WEAPON_TYPE nWeaponType = 
			(WEAPON_TYPE)CSStrings::StringToWeaponType(MLocale::ConvAnsiToTCHAR(strText.c_str()).c_str());

		if (nWeaponType == -1)
		{
			mlog3("xitem.xml : %s, unknown weapon type name : %s\n", ITEM_XML_ATTR_ENCHANT_LIMIT_WEAPONS, strText.c_str());
			continue;
		}

		pItemData->m_EnchantStone.vecLimitWeapons.push_back(nWeaponType);
		pItemData->m_EnchantStone.bAllowAllWeapons = false;
	}

	vector<string> vecData;
	csv_parser.Parse_CSV(pElement, ITEM_XML_ATTR_EQUIP_ENCHANT_SLOT, vecData);
	for each (string strText in vecData)
	{
		bool bFound = false;

		int nStrSize = arraysize(ENCHANT_COLOR_STR);
		for (int i=0; i<nStrSize; ++i)
		{
			if (strText == MLocale::ConvTCHARToAnsi(ENCHANT_COLOR_STR[i]))
			{
				pItemData->m_vecEquipEnchantColor.push_back((ENCHANT_COLOR)i);
				bFound = true;
			}
		}

		if (!bFound)
		{
			mlog3("xitem.xml : %s, unknown equipslot name : %s\n", ITEM_XML_ATTR_EQUIP_ENCHANT_SLOT, strText.c_str());
		}
	}

	MXmlAttribute* attr;
	for (attr = pElement->FirstAttribute(); attr; attr=attr->Next())
	{
		if(!_stricmp(attr->Name(), ITEM_XML_ATTR_CONDITION))
		{
			pItemData->m_nConditionsInfoID = static_cast<int>(atoi(attr->Value()));
		}		
	}


	CSModifierParser::Parse_ActorModifier(pElement, pItemData->m_ActorModifier);
	CSModifierParser::Parse_PlayerModifier(pElement, pItemData->m_PlayerModifier);

	return true;
}

void CSItemManager::ParseItem_SubMtrl(MXmlElement* pElement, CSItemData* pItemData)
{
	if(!pItemData)
		return;

	string strMtrlName;

	// <NPC id="21" Name="코볼드 샘플" Model="Monster" MeshName="kobold_pawn">
	MXmlElement* pMeshElement = pElement->FirstChildElement();
	for( pMeshElement; pMeshElement != NULL; pMeshElement=pMeshElement->NextSiblingElement() )
	{
		// <MESH index="0" SubCount="1">
		CSItemMaterial* newMtrl = new CSItemMaterial;
		_Attribute(&newMtrl->nID, pMeshElement, "index");
		_Attribute(&newMtrl->nSubCount, pMeshElement, "SubCount");

		MXmlElement* pMtrlElement = pMeshElement->FirstChildElement();
		for( pMtrlElement; pMtrlElement != NULL; pMtrlElement=pMtrlElement->NextSiblingElement() )
		{
			_Attribute(strMtrlName, pMtrlElement, "MtrlName");
			newMtrl->vecSubMtrls.push_back(strMtrlName);
		}

		pItemData->m_vecMtrls.push_back(newMtrl);	
	}	
}
