#include "stdafx.h"
#include "CSBuffInfo.h"
#include "CSDef.h"
#include "CSCommonParser.h"
#include "CSBuffInfoDef.h"
#include "CSTalentInfoParser.h"
#include "CSStrings.h"
#include "MLocale.h"

CSBuffInfo::CSBuffInfo()
{
	m_nType = BUFT_NONE;
	m_nStackType = BUFFSTACK_NONE;
	m_nStackSlot = 0;
	m_nStackPower = 0;
	m_nStackMaxCount = 0;
	m_fWeaponApplyRate = 1.0f;

	m_nDamageAttrib = DA_NONE;
	m_nMinDamage = 0;
	m_nMaxDamage = 0;
	m_nMinHeal = 0;
	m_nMaxHeal = 0;
	m_fPercentageHeal = 0.0f;
	m_nPassiveExtraAttrib = BUFPEA_NONE;
	m_nPassiveExtraAttrib2 = BUFPEA_NONE;
	m_fPassiveExtraAttribValue = 0.0f;
	m_fPassiveExtraAttribValue2 = 0.0f;
	m_nActiveExtraAttrib = BUFAEA_NONE;

	m_bCancelable = true;

	m_bUseChangePalette = false;
	for each (int& each in m_arrPalettes)
	{
		each = INVALID_TALENT_ID;
	}

	m_bUseChangeEquipmentSlot = false;
	for each (int& each in m_nEquipItemIDs)
	{
		each = INVALID_ID;
	}
}

void CSBuffInfoMgr::ParseBuffCommon( CSBuffInfo* pBuffInfo, MXmlElement* pElement, MXml* pXml )
{
	CSModifierParser::Parse_InstantModifier(pElement, pBuffInfo->m_InstantModifier);
	CSModifierParser::Parse_ActorModifier(pElement, pBuffInfo->m_ActorModifier);
	CSModifierParser::Parse_PlayerModifier(pElement, pBuffInfo->m_PlayerModifier);

	tstring strValue;

	if (_Attribute(strValue,	pElement, BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB)) 
	{
		for (int i = 0; i < BUFPEA_MAX; i++)
		{
			if (!_tcsicmp(BUFF_PASSIVE_EXTRA_ATTRIB_STR[i], strValue.c_str()))
			{
				pBuffInfo->m_nPassiveExtraAttrib = (BUFF_PASSIVE_EXTRA_ATTRIB_)i;
				break;
			}
		}
	}

	_Attribute(pBuffInfo->m_fPassiveExtraAttribValue,	pElement, BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB_VALUE);

	if (_Attribute(strValue,	pElement, BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB2)) 
	{
		for (int i = 0; i < BUFAEA_MAX; i++)
		{
			if (!_tcsicmp(BUFF_PASSIVE_EXTRA_ATTRIB_STR[i], strValue.c_str()))
			{
				pBuffInfo->m_nPassiveExtraAttrib2 = (BUFF_PASSIVE_EXTRA_ATTRIB_)i;
				break;
			}
		}
	}

	_Attribute(pBuffInfo->m_fPassiveExtraAttribValue2,	pElement, BUFF_XML_ATTR_PASSIVE_EXTRA_ATTRIB_VALUE2);

	if (_Attribute(strValue,	pElement, BUFF_XML_ATTR_ACTIVE_EXTRA_ATTRIB)) 
	{
		for (int i = 0; i < BUFAEA_MAX; i++)
		{
			if (!_tcsicmp(BUFF_ACTIVE_EXTRA_ATTRIB_STR[i], strValue.c_str()))
			{
				pBuffInfo->m_nActiveExtraAttrib = (BUFF_ACTIVE_EXTRA_ATTRIB_)i;
				break;
			}
		}
	}


	if (_Attribute(strValue,	pElement, BUFF_XML_ATTR_RIPOSTE_TYPE))
	{
		for (int i = 0; i < TC_MAX; i++)
		{
			if (!_tcsicmp(TALENT_CATEGORY_STR[i], strValue.c_str()))
			{
				pBuffInfo->m_Riposte.nType = (TALENT_CATEGORY)i;
				break;
			}
		}
	}

	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_TYPE))
	{		
		for (int i = 0; i < BUFT_MAX; i++)
		{
			if (!_tcsicmp(BUFF_TYPE_STR[i], strValue.c_str()))
			{
				pBuffInfo->m_nType = (BUFF_TYPE)i;
				break;
			}
		}
	}

	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_STACK_TYPE))
	{		
		for (int i = 0; i < BUFT_MAX; i++)
		{
			if (!_tcsicmp(BUFF_STACK_TYPE_STR[i], strValue.c_str()))
			{
				pBuffInfo->m_nStackType = (BUFF_STACK_TYPE)i;
				break;
			}
		}
	}

	for (int i=0; i<PALETTESLOT_MAX; ++i)
	{
		char szAttrName[256];
		sprintf_s(szAttrName, "%s%d", BUFF_XML_ATTR_PALETTE_SLOT, i+1);

		if (_Attribute(&pBuffInfo->m_arrPalettes[i],	pElement, szAttrName))
		{
			pBuffInfo->m_bUseChangePalette = true;
		}
	}

	_Attribute(&pBuffInfo->m_nStackMaxCount,	pElement, BUFF_XML_ATTR_STACK_MAX_COUNT);
	_Attribute(&pBuffInfo->m_Riposte.fRate,	pElement, BUFF_XML_ATTR_RIPOSTE_RATE);
	_Attribute(&pBuffInfo->m_bCancelable,	pElement, BUFF_XML_ATTR_CANCELABLE);
	_Attribute(&pBuffInfo->m_RequireBuff.bKeepIncludeBuff,	pElement, BUFF_XML_ATTR_KEEP_INCLUDE);
	_Attribute(&pBuffInfo->m_RequireBuff.bKeepExcludeBuff,	pElement, BUFF_XML_ATTR_KEEP_EXCLUDE);
	_Attribute(&pBuffInfo->m_fWeaponApplyRate,	pElement, BUFF_XML_ATTR_WEAPON_APPLY_RATE);
	
	if (_Attribute(&pBuffInfo->m_nEquipItemIDs[ITEMSLOT_LWEAPON],	pElement, BUFF_XML_ATTR_EQUIPITEM_LWEAPON))
	{
		pBuffInfo->m_bUseChangeEquipmentSlot = true;
	}
	if (_Attribute(&pBuffInfo->m_nEquipItemIDs[ITEMSLOT_RWEAPON],	pElement, BUFF_XML_ATTR_EQUIPITEM_RWEAPON))
	{
		pBuffInfo->m_bUseChangeEquipmentSlot = true;
	}
	

	CSCSVParser csv_parser;

	csv_parser.Parse_CSV(pElement, BUFF_XML_ATTR_INCLUDE, pBuffInfo->m_RequireBuff.vecIncludeBuffID);
	csv_parser.Parse_CSV(pElement, BUFF_XML_ATTR_EXCLUDE, pBuffInfo->m_RequireBuff.vecExcludeBuffID);
	
	// Triggered Buff
	_Attribute(&pBuffInfo->m_TriggeredBuff.nBuffID,	pElement, BUFF_XML_ATTR_TRIGGEREDBUFF_ID);
	_Attribute(&pBuffInfo->m_TriggeredBuff.fRate,	pElement, BUFF_XML_ATTR_TRIGGEREDBUFF_RATE);
	
	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_TRIGGEREDBUFF_TIMING)) 
	{
		CSTalentInfoParser talentInfoParser;
		talentInfoParser.ParseEffectTiming(MLocale::ConvTCHARToAnsi(strValue.c_str()), pBuffInfo->m_TriggeredBuff.nTiming);
	}
	CSTalentInfoParser talentInfoParser;
	talentInfoParser.ParseTalentEffect(pElement, pBuffInfo->m_TriggeredBuff.infoEffect, "TriggeredBuff", "");
}

bool CSBuffInfo::IsMatchPassiveExtraAttrib(BUFF_PASSIVE_EXTRA_ATTRIB_ nAttrib) const
{
	if (m_nPassiveExtraAttrib == nAttrib)	return true;
	if (m_nPassiveExtraAttrib2 == nAttrib)	return true;

	return false;
}

bool CSBuffInfo::IsMatchActiveExtraAttrib(BUFF_ACTIVE_EXTRA_ATTRIB_ nAttrib) const
{
	if (m_nActiveExtraAttrib == nAttrib)	return true;

	return false;
}

bool CSBuffInfo::IsMesmerize()
{
	if(m_nPassiveExtraAttrib == BUFPEA_ROOT ||
		m_nPassiveExtraAttrib == BUFPEA_SLEEP ||
		m_nPassiveExtraAttrib2 == BUFPEA_ROOT ||
		m_nPassiveExtraAttrib2 == BUFPEA_SLEEP)
		return true;

	return false;
}

bool CSBuffInfo::IsEnchant() const
{
	return (m_nType == BUFT_ENCHANT);
}

bool CSBuffInfo::HasProcEffect() const
{
	return (m_TriggeredBuff.nBuffID != INVALID_BUFF_ID);
}

bool CSBuffInfo::IsAuraEffectBuff()
{
	if (m_nType == BUFT_AURA_EFFECT_BUFF)
		return true;

	if (m_nType == BUFT_AURA_EFFECT_DEBUFF)
		return true;

	return false;
}

bool CSBuffInfo::IsBuff()
{
	if (m_nType == BUFT_BUFF)
		return true;

	if (m_nType == BUFT_AURA_EFFECT_BUFF)
		return true;

	return false;
}

bool CSBuffInfo::IsDebuff()
{
	if (m_nType == BUFT_DEBUFF)
		return true;

	if (m_nType == BUFT_AURA_EFFECT_DEBUFF)
		return true;

	return false;
}

bool CSBuffInfo::IsQuestBuff()
{
	return (m_nType == BUFT_QUEST);
}

bool CSBuffInfo::HasDamage()
{
	return m_nMaxDamage > 0;
}

bool CSBuffInfo::HasHealEffect()
{
	return m_nMaxHeal > 0;
}

bool CSBuffInfo::IsRideBuff() const
{
	if (m_nPassiveExtraAttrib == BUFPEA_RIDE)
		return true;

	if (m_nPassiveExtraAttrib2 == BUFPEA_RIDE)
		return true;

	return false;
}

CSBuffInfo::TriggeredBuff::TriggeredBuff()
: nBuffID(INVALID_BUFF_ID)
, nTiming(TC_BUFF_GAIN_N_PERIOD)
, fRate(100.0f)
{
	infoEffect.m_nPoint = CSEffectInfo::POINT_SELF; // 자기 중심으로 고정
}
