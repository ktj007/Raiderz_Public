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
	m_nDamageType = TDT_MAX;
	m_nStackType = BUFFSTACK_NONE;
	m_nStackSlot = 0;
	m_nStackPower = 0;
	m_nStackMaxCount = 0;

	m_nBuffLine = 0;

	m_nDamageAttrib = DA_NONE;
	m_nMinDamage = 0;
	m_nMaxDamage = 0;
	m_nMinHeal = 0;
	m_nMaxHeal = 0;
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

	m_fDamagePercentHP = 0.0f;
	m_fRestorePercentHP = 0.0f;
	m_fRestorePercentEN = 0.0f;

	m_nIncludePassiveTalent = INVALID_TALENT_ID;
	m_nRideNPCID = INVALID_ID;

	m_bReleaseOnDead = true;
	m_bFreezeFrame = false;
}

void CSBuffInfoMgr::ParseBuffCommon( CSBuffInfo* pBuffInfo, MXmlElement* pElement, MXml* pXml )
{
	CSModifierParser::Parse_InstantModifier(pElement, pBuffInfo->m_InstantModifier);
	CSModifierParser::Parse_ActorModifier(pElement, pBuffInfo->m_ActorModifier);
	CSModifierParser::Parse_PlayerModifier(pElement, pBuffInfo->m_PlayerModifier);

	tstring strValue;
	CSCSVParser csv_parser;

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

	vector<string> vecstrActiveExtraParam1;
	csv_parser.Parse_CSV(pElement, BUFF_XML_ATTR_ACTIVE_EXTRA_PARAM "1", vecstrActiveExtraParam1);
	_ParseActiveExtraParam1(pBuffInfo->m_nActiveExtraAttrib, vecstrActiveExtraParam1, pBuffInfo->m_vecActiveExtraParam1);

	vector<string> vecstrActiveExtraParam2;
	csv_parser.Parse_CSV(pElement, BUFF_XML_ATTR_ACTIVE_EXTRA_PARAM "2", vecstrActiveExtraParam2);
	_ParseActiveExtraParam2(pBuffInfo->m_nActiveExtraAttrib, vecstrActiveExtraParam2, pBuffInfo->m_vecActiveExtraParam2);


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

	vector<string> vecstrDispelType;
	csv_parser.Parse_CSV(pElement, BUFF_XML_ATTR_DISPEL_TYPE, vecstrDispelType);

	for (const string& strDispelType : vecstrDispelType)
	{
		BUFF_DISPEL_TYPE dispelType;

		if (_ParseDispelType(strDispelType, dispelType))
		{
			pBuffInfo->m_setDispelType.insert(dispelType);
		}
		else
		{
			_ASSERT(0);
		}
	}

	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_STACK_TYPE))
	{		
		for (int i = 0; i < BUFFSTACK_MAX; i++)
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
	
	if (_Attribute(&pBuffInfo->m_nEquipItemIDs[ITEMSLOT_LWEAPON],	pElement, BUFF_XML_ATTR_EQUIPITEM_LWEAPON))
	{
		pBuffInfo->m_bUseChangeEquipmentSlot = true;
	}
	if (_Attribute(&pBuffInfo->m_nEquipItemIDs[ITEMSLOT_RWEAPON],	pElement, BUFF_XML_ATTR_EQUIPITEM_RWEAPON))
	{
		pBuffInfo->m_bUseChangeEquipmentSlot = true;
	}
	

	// Include and Exclude: positive value means Buff ID, while negative value means Buff Line (new in 2014).
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

	talentInfoParser.ParseApplyRate(pElement, pBuffInfo->m_WeaponApplyRate);
	talentInfoParser.ParseDamageType(pElement, pBuffInfo->m_nDamageType);
	talentInfoParser.ParseUsableType(pElement, pBuffInfo->m_nUsableType);
	talentInfoParser.ParseFreezeFrame(pElement, pBuffInfo->m_bFreezeFrame);

	_Attribute(&pBuffInfo->m_fDamagePercentHP, pElement, BUFF_XML_ATTR_DAMAGE_PERCENT_HP);
	_Attribute(&pBuffInfo->m_fRestorePercentHP, pElement, BUFF_XML_ATTR_RESTORE_PERCENT_HP);
	_Attribute(&pBuffInfo->m_fRestorePercentEN, pElement, BUFF_XML_ATTR_RESTORE_PERCENT_EN);

	_Attribute(&pBuffInfo->m_nBuffLine, pElement, BUFF_XML_ATTR_BUFF_LINE);
	_Attribute(&pBuffInfo->m_nIncludePassiveTalent, pElement, BUFF_XML_ATTR_INCLUDE_PASSIVE_TALENT);
	_Attribute(&pBuffInfo->m_nRideNPCID, pElement, BUFF_XML_ATTR_RIDE_NPC_ID);
	_Attribute(&pBuffInfo->m_bReleaseOnDead, pElement, BUFF_XML_ATTR_RELEASE_ON_DEAD);
}

bool CSBuffInfoMgr::_ParseDispelType(const string& strDispelType, BUFF_DISPEL_TYPE& out)
{
	const tstring tstrDispelType = MLocale::ConvAnsiToTCHAR(strDispelType.c_str());

	for (int i = 0; i < BDT_MAX; i++)
	{
		if (!_tcsicmp(BUFF_DISPEL_TYPE_STR[i], tstrDispelType.c_str()))
		{
			out = static_cast<BUFF_DISPEL_TYPE>(i);
			return true;
		}
	}

	return false;
}

bool CSBuffInfoMgr::_ParseActiveExtraParam1( BUFF_ACTIVE_EXTRA_ATTRIB_ nAttrib, const vector<string>& vecstrValue, vector<BUFF_ACTIVE_EXTRA_PARAM>& outvecParam )
{
	vector<BUFF_ACTIVE_EXTRA_PARAM> vecParam;
	BUFF_ACTIVE_EXTRA_PARAM param;

	if (nAttrib == BUFAEA_DISPEL_BUFF)
	{
		if (vecstrValue.empty())
		{
			_ASSERT(0);
			return false;
		}

		for (const string& strValue : vecstrValue)
		{
			BUFF_DISPEL_TYPE dispelType;

			if (_ParseDispelType(strValue, dispelType))
			{
				param.nValue[0] = static_cast<int>(dispelType);
				vecParam.push_back(param);
			}
			else
			{
				_ASSERT(0);
			}
		}
	}
	else if (nAttrib == BUFAEA_ADJUST_COOLTIME)
	{
		for (const string& strValue : vecstrValue)
		{
			if (strValue.find(':') == string::npos)
				return false;

			// posivie nTalentID value means Talent ID, while negative value means Talent Line.
			int nTalentID, nAdjustInterval;
			if (sscanf_s(strValue.c_str(), "%d:%d", &nTalentID, &nAdjustInterval) != 2)
				return false;

			param.nValue[0] = nTalentID;
			param.nValue[1] = nAdjustInterval;
			vecParam.push_back(param);
		}
	}

	outvecParam = vecParam;

	return true;
}

bool CSBuffInfoMgr::_ParseActiveExtraParam2( BUFF_ACTIVE_EXTRA_ATTRIB_ nAttrib, const vector<string>& vecstrValue, vector<BUFF_ACTIVE_EXTRA_PARAM>& outvecParam )
{
	vector<BUFF_ACTIVE_EXTRA_PARAM> vecParam;
	BUFF_ACTIVE_EXTRA_PARAM param;

	if (nAttrib == BUFAEA_DISPEL_BUFF)
	{
		if (vecstrValue.empty())
		{
			_ASSERT(0);
			return false;
		}

		// StackPower of target dispel buff
		for (const string& strValue : vecstrValue)
		{
			param.nValue[0] = atoi(strValue.c_str());
			vecParam.push_back(param);
		}
	}

	outvecParam = vecParam;

	return true;
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

bool CSBuffInfo::IsMesmerize() const
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

bool CSBuffInfo::IsDispel(BUFF_DISPEL_TYPE nType) const
{
	return (m_setDispelType.find(nType) != m_setDispelType.end());
}

bool CSBuffInfo::IsEqualLine(int nBuffLine) const
{
	if (m_nBuffLine == 0)			return false;
	if (m_nBuffLine != nBuffLine)	return false;
	return true;
}

bool CSBuffInfo::IsAuraEffectBuff() const
{
	if (m_nType == BUFT_AURA_EFFECT_BUFF)
		return true;

	if (m_nType == BUFT_AURA_EFFECT_DEBUFF)
		return true;

	return false;
}

bool CSBuffInfo::IsBuff() const
{
	if (m_nType == BUFT_BUFF)
		return true;

	if (m_nType == BUFT_AURA_EFFECT_BUFF)
		return true;

	return false;
}

bool CSBuffInfo::IsDebuff() const
{
	if (m_nType == BUFT_DEBUFF)
		return true;

	if (m_nType == BUFT_AURA_EFFECT_DEBUFF)
		return true;

	return false;
}

bool CSBuffInfo::IsQuestBuff() const
{
	return (m_nType == BUFT_QUEST);
}

bool CSBuffInfo::IsStackable() const
{
	return (m_nStackType != BUFFSTACK_NONE && m_nStackMaxCount > 1);
}

bool CSBuffInfo::HasDamage() const
{
	if (m_nMaxDamage > 0)
		return true;

	if (m_nUsableType == TUT_DAMAGE && m_WeaponApplyRate.fApplyRate > 0.f)
		return true;

	return false;
}

bool CSBuffInfo::IsMagicDamage() const
{
	return (m_nDamageType == TDT_MAGIC);
}

bool CSBuffInfo::HasHealEffect() const
{
	if (m_nMaxHeal > 0)
		return true;

	if (m_nUsableType == TUT_HEAL && m_WeaponApplyRate.fApplyRate > 0.f)
		return true;

	if (m_fRestorePercentHP > 0.0f)
		return true;

	return false;
}

bool CSBuffInfo::HasRestoreEffect() const
{
	if (m_fRestorePercentEN > 0.0f)
		return true;

	return false;
}

bool CSBuffInfo::IsRideBuff() const
{
	if (m_nPassiveExtraAttrib == BUFPEA_RIDE)
		return true;

	if (m_nPassiveExtraAttrib2 == BUFPEA_RIDE)
		return true;

	if (m_nRideNPCID != INVALID_ID)
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
