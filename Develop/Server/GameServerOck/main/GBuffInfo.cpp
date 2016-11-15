#include "stdafx.h"
#include "GBuffInfo.h"
#include "CSCommonParser.h"
#include "CSTalentinfo.h"
#include "CSTalentInfoParser.h"
#include "GDef.h"
#include "GStrings.h"
#include "CSTalentInfoDef.h"
#include "GConst.h"
#include "GGlobal.h"
#include "GValidateLogger.h"
#include "CSBuffInfoDef.h"
#include "GBuffScript.h"

GBuffInfo::GBuffInfo()
: m_fDuration(BUFF_DURATION_INFINITY)
, m_fPeriod(0.0f)
, m_bExistModValue(false)
, m_nHPCost(0)
, m_nENCost(0)
, m_nSTACost(0)
, m_bFocusBuff(false)
, m_nTalentID(INVALID_TALENT_ID)
, m_nHateAdj(0)
, m_fHateFactor(1.0f)
, m_bSaveToDB(true)
, m_bUpdateRoute(true)
{

}

void GBuffInfo::Cooking()
{
	InitSuffix();

	if (IsInvisibility())
	{
		// 투명화 추가속성 강제지정
		//		탤런트 완료 될 때, 공격 받았을 때, 공격 했을 때 버프 해제
		m_Condition.infoRelease.vecAdditionType.push_back(TC_FINISH_TALENT);
		m_Condition.infoRelease.vecAdditionType.push_back(TC_HIT_ME);
		m_Condition.infoRelease.vecAdditionType.push_back(TC_HIT_ENEMY);
		m_Condition.infoRelease.vecAdditionType.push_back(TC_BUFF_HIT);
	}

	if (m_Condition.infoRelease.nType == TC_RESIST)
	{
		// 버프풀기 조건이 저항인 경우에는 이진저항으로 고정
		m_Resist.m_nMethod = TRM_BINARY;
	}

	// 타입별 속성 고정
	switch (m_nType)
	{
	case BUFT_AURA:
		m_TriggeredBuff.nTiming = TC_BUFF_GAIN_N_PERIOD;
		m_fPeriod = 1.0f; // 주기시간 고정
		m_bSaveToDB = false;
		break;
	case BUFT_AURA_EFFECT_BUFF:
	case BUFT_AURA_EFFECT_DEBUFF:
		if (m_fDuration == 0.0f)
		{
			// 지정된 시간이 없을 경우
			m_fDuration = 1.0f; // 유효시간 고정
		}
		m_bSaveToDB = false;
		m_bUpdateRoute = false;
		break;
	case BUFT_ENCHANT:
		m_fDuration = BUFF_DURATION_INFINITY;
		m_bCancelable = false;
		m_bSaveToDB = false;
		m_bUpdateRoute = false;
		m_nStackSlot = INVALID_ID;
		break;
	}

	if (m_bUseChangeEquipmentSlot)
	{
		m_nStackPower = 10; // 고정된 값
		m_nStackSlot = GConst::BPART_BUFFSTACK_ID;
	}

	GBuffScript::MakeTable(m_nID);
}

void GBuffInfo::InitSuffix()
{
	m_bExistModValue = m_InstantModifier.IsModified() || m_ActorModifier.IsModified() || m_PlayerModifier.IsModified();
}

bool GBuffInfo::IsModified_MF() const
{
	return m_MotionFactorGroup.IsModified();
}

bool GBuffInfo::HasHealEffect() const
{
	if (m_nMaxHeal > 0)
		return true;

	if (m_fPercentageHeal > 0.0f)
		return true;

	return false;
}

bool GBuffInfo::IsInvincibility() const
{
	return IsMatchPassiveExtraAttrib(BUFPEA_INVINCIBILITY);
}

bool GBuffInfo::IsInvisibilityToNPC() const
{
	return IsMatchPassiveExtraAttrib(BUFPEA_INVISIBILITYTONPC);
}

bool GBuffInfo::IsInvisibility() const
{
	return IsMatchPassiveExtraAttrib(BUFPEA_INVISIBILITY);
}

TALENT_CATEGORY GBuffInfo::IsDisableTalent() const
{
	if (IsMatchPassiveExtraAttrib(BUFPEA_DISABLE_TALENTS))		return TC_MAX;
	if (IsMatchPassiveExtraAttrib(BUFPEA_DISABLE_TALENT_MELEE))	return TC_MELEE;
	if (IsMatchPassiveExtraAttrib(BUFPEA_DISABLE_TALENT_RANGE))	return TC_RANGE;
	if (IsMatchPassiveExtraAttrib(BUFPEA_DISABLE_TALENT_MAGIC))	return TC_MAGIC;

	return TC_INVALID;
}

bool GBuffInfo::IsMesmerize() const
{
	if (IsMatchPassiveExtraAttrib(BUFPEA_ROOT))
		return true;

	if (IsMatchPassiveExtraAttrib(BUFPEA_SLEEP))
		return true;

	return false;
}

bool GBuffInfo::IsAbsoluteGuard() const
{
	return IsMatchPassiveExtraAttrib(BUFPEA_ABSOLUTE_GUARD);
}

bool GBuffInfo::IsCancelable() const
{
	return m_bCancelable;
}

bool GBuffInfo::IsDieBuff() const
{
	return IsMatchActiveExtraAttrib(BUFAEA_DIE);
}

bool GBuffInfo::IsDespawnOnceBuff() const
{
	return IsMatchActiveExtraAttrib(BUFAEA_DESPAWN_ONCE);
}

bool GBuffInfo::IsDespawnEternalBuff() const
{
	return IsMatchActiveExtraAttrib(BUFAEA_DESPAWN_ETERNAL);
}

float GBuffInfo::GetInvisibilityPower()
{
	const float DEFAULT_POWER = 100.0f;

	if (m_nPassiveExtraAttrib == BUFPEA_INVISIBILITY)
		return min(m_fPassiveExtraAttribValue, DEFAULT_POWER);

	if (m_nPassiveExtraAttrib2 == BUFPEA_INVISIBILITY)
		return min(m_fPassiveExtraAttribValue2, DEFAULT_POWER);

	return DEFAULT_POWER;
}

bool GBuffInfo::IsInstantEffect() const
{
	return (m_fDuration == 0.0f);
}

bool GBuffInfo::IsDisableQuestPVPDrop() const
{
	return IsMatchPassiveExtraAttrib(BUFPEA_DISABLE_QUESTPVP_DROP);
}






GBuffInfoMgr::~GBuffInfoMgr()
{
	Clear();
}

void GBuffInfoMgr::Clear()
{
	while(!empty()) 
	{
		GBuffInfo* pTalentInfo = static_cast<GBuffInfo*>(begin()->second);
		delete pTalentInfo;
		erase(begin());
	}
}

GBuffInfo* GBuffInfoMgr::Get(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return NULL;
	return (GBuffInfo*)((*itor).second);
}

bool GBuffInfoMgr::Exist(int nID)
{
	iterator itor = find(nID);
	if (itor == end()) return false;
	return true;
}

bool GBuffInfoMgr::Load(const wchar_t* szFileName)
{
	MXml xml;

	_VLI(szFileName);

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_BUFF))
		{
			ParseBuff(pElement, &xml);
		}
	}

	GBuffScript::RunScript();

	return true;
}

void GBuffInfoMgr::ParseBuff(MXmlElement* pElement, MXml* pXml)
{
	int nID = 0;
	_Attribute(&nID,			pElement, BUFF_XML_ATTR_ID);	
	
	_VLE(_T(BUFF_XML_TAG_BUFF));	
	_VLA(_T(BUFF_XML_ATTR_ID), nID);
	_VLP;

	if (Exist(nID))
	{		
		_VLOGGER->Log(_T(BUFF_XML_ATTR_ID) IS_ALREADY_EXIST);
		return;
	}

	string strValue;
	int nValue=0;
	GBuffInfo* pNewInfo = new GBuffInfo;

	// 클라이언트, 서버 공통으로 읽는 값
	ParseBuffCommon(pNewInfo, pElement, pXml);

	pNewInfo->m_nID = nID;

	ParseIsFocusBuff(pNewInfo);

	_Attribute(pNewInfo->m_strName,			pElement, BUFF_XML_ATTR_NAME);
	_Attribute(&pNewInfo->m_fDuration,		pElement, BUFF_XML_ATTR_DURATION);
	_Attribute(&pNewInfo->m_fPeriod,		pElement, BUFF_XML_ATTR_PERIOD);

	_Attribute(&pNewInfo->m_nStackSlot,		pElement, BUFF_XML_ATTR_STACKSLOT);
	
	if (pNewInfo->m_nStackSlot == 0)
	{
		// StackSlot이 없을 경우, ID로 맵핑한다.
		pNewInfo->m_nStackSlot = pNewInfo->m_nID;
	}

	_Attribute(&pNewInfo->m_nStackPower,	pElement, BUFF_XML_ATTR_STACKPOWER);

	int nDamageType=0;
	_Attribute(&pNewInfo->m_nMinDamage,		pElement, BUFF_XML_ATTR_MIN_DAMAGE);
	_Attribute(&pNewInfo->m_nMaxDamage,		pElement, BUFF_XML_ATTR_MAX_DAMAGE);
	_Attribute(&pNewInfo->m_nMinHeal,		pElement, BUFF_XML_ATTR_MIN_HEAL);
	_Attribute(&pNewInfo->m_nMaxHeal,		pElement, BUFF_XML_ATTR_MAX_HEAL);
	_Attribute(&pNewInfo->m_fPercentageHeal,	pElement, BUFF_XML_ATTR_PERCENTAGE_HEAL);
	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_DAMAGE_ATTRIB)) 
	{
		pNewInfo->m_nDamageAttrib = (DAMAGE_ATTRIB)CSStrings::StringToDamageAttrib(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str());
	}

	_Attribute(&pNewInfo->m_nHPCost,		pElement, BUFF_XML_ATTR_TICKCOST_HP);
	_Attribute(&pNewInfo->m_nENCost,		pElement, BUFF_XML_ATTR_TICKCOST_EN);
	_Attribute(&pNewInfo->m_nSTACost,		pElement, BUFF_XML_ATTR_TICKCOST_STA);
	
	CSTalentInfoParser talentInfoParser;
	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_EFFECT_TIMING)) 
	{
		talentInfoParser.ParseEffectTiming(strValue, pNewInfo->m_Condition.nEffectConditionType);
	}

	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_RELEASE_CONDITION)) 
	{
		talentInfoParser.ParseEffectTiming(strValue, pNewInfo->m_Condition.infoRelease.nType);
		_Attribute(&pNewInfo->m_Condition.infoRelease.nParam,	pElement, BUFF_XML_ATTR_RELEASE_PARAM);
	}

	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_RESIST_TYPE)) 
	{
		pNewInfo->m_Resist.m_nType = (TALENT_RESIST_TYPE)CSStrings::StringToResistType(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str());
	}

	if (_Attribute(strValue,				pElement, BUFF_XML_ATTR_RESIST_METHOD)) 
	{
		pNewInfo->m_Resist.m_nMethod = (TALENT_RESIST_METHOD)CSStrings::StringToResistMethod(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str());
	}

	int nResistDIfficulty;
	if (_Attribute(&nResistDIfficulty,			pElement, BUFF_XML_ATTR_RESIST_DIFFICULTY))
	{
		pNewInfo->m_Resist.m_nDifficulty = uint8(nResistDIfficulty);
	}
	
	_Attribute(&pNewInfo->m_nTalentID,			pElement, BUFF_XML_ATTR_USE_TALENT);
	_Attribute(&pNewInfo->m_nHateAdj,			pElement, BUFF_XML_ATTR_HATE_ADJ);
	_Attribute(&pNewInfo->m_fHateFactor,		pElement, BUFF_XML_ATTR_HATE_FACTOR);
	_Attribute(&pNewInfo->m_bSaveToDB,			pElement, BUFF_XML_ATTR_SAVE_TO_DB);
	
	talentInfoParser.ParseTalentEffect(pElement, pNewInfo->m_EffectInfo, "", "");

	CSMotionFactorParser::Parse(pElement, pNewInfo->m_MotionFactorGroup);

	pNewInfo->Cooking();

	Insert(pNewInfo);
}

void GBuffInfoMgr::Insert(GBuffInfo* pBuffInfo )
{
	insert(value_type(pBuffInfo->m_nID, pBuffInfo));
}

void GBuffInfoMgr::ParseIsFocusBuff(GBuffInfo* pBuffInfo)
{
	if (NULL == pBuffInfo) return;

	pBuffInfo->m_bFocusBuff = false;
	for each (int nFocusBuffID in BUFF_FOCUS_ID)
	{
		if (nFocusBuffID == pBuffInfo->m_nID)
		{
			pBuffInfo->m_bFocusBuff = true;
			return;
		}
	}
}
