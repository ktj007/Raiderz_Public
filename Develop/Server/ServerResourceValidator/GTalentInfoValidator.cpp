#include "stdafx.h"
#include "GTalentInfoValidator.h"
#include "GTalentInfo.h"
#include "GValidateLogger.h"
#include "GBuffInfo.h"
#include "GServerSystems.h"
#include "GTalentInfoMgr.h"
#include "GTalentInfoDef.h"
#include "CSCommonParser.h"
#include "CSStrings.h"
#include "MLocale.h"

bool GTalentInfoValidator::Check()
{
	if (gmgr.pTalentInfoMgr == NULL)	return true;

	// talent.xml
	_VLI(FILENAME_TALENTINFO);
	for (GTalentInfoMgr::iterator itor = gmgr.pTalentInfoMgr->begin(); itor != gmgr.pTalentInfoMgr->end(); ++itor)
	{
		GTalentInfo* pTalentInfo = static_cast<GTalentInfo*>((*itor).second);
		if (NULL == pTalentInfo) continue;
		if (!pTalentInfo->m_bValidation) continue;

		_VLE(_T(TALENT_XML_TAG_TALENT));
		_VLA(_T(TALENT_XML_ATTR_ID), pTalentInfo->m_nID);
		_VLA(_T(TALENT_XML_ATTR_NAME), wstring(pTalentInfo->m_szName));
		_VLP;

		_VLOGGER->ValidateID(pTalentInfo->m_nID);
		
		CheckRank(pTalentInfo);
		CheckBuff(pTalentInfo);
		CheckPointAndRelation(pTalentInfo);

		CheckLineNRank(pTalentInfo);
		CheckMotionFactor(pTalentInfo);
		CheckTNeedLine(pTalentInfo);
		CheckModifier(pTalentInfo);

		CheckExtraActive(pTalentInfo);
		CheckDamageAttrib(pTalentInfo);

		CheckActEvent(pTalentInfo);

		CheckNPC(pTalentInfo);
		CheckAABBSize(pTalentInfo);
	}

	// talent_ext.xml
	_VLI(FILENAME_TALENTINFO_EXT);
	for (GTalentInfoMgr::iterator itor = gmgr.pTalentInfoMgr->begin(); itor != gmgr.pTalentInfoMgr->end(); ++itor)
	{
		GTalentInfo* pTalentInfo = static_cast<GTalentInfo*>((*itor).second);
		if (NULL == pTalentInfo) continue;

		_VLE(_T(TALENT_XML_TAG_TALENT));
		_VLA(_T(TALENT_XML_ATTR_ID), pTalentInfo->m_nID);
		_VLP;

		CheckTalentExtInfo(pTalentInfo);
		CheckActTime(pTalentInfo);
	}

	CheckTalentPosInfo();

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GTalentInfoValidator::CheckTalentExtInfo(GTalentInfo* pTalentInfo)
{
	if (!pTalentInfo->m_setNPCID.empty() &&
		pTalentInfo->m_nTalentType == TT_SKILL && 
		pTalentInfo->m_nSkillType != ST_PASSIVE &&
		pTalentInfo->m_nSkillType != ST_EXTRA_PASSIVE)
	{
		_VLOGGER->ValidateMinus(pTalentInfo->m_nID, _T(TALENT_XML_ATTR_ID));
	}

	if (pTalentInfo->m_nForceMF == MF_GRAPPLED || pTalentInfo->m_nForceMF == MF_SWALLOWED)
	{
		_VLOGGER->ValidateMinusORZero(pTalentInfo->m_fExtraPhaseTime, _T(TALENT_XML_ATTR_EXTRA_TIME));
	}
}

void GTalentInfoValidator::CheckActEvent(GTalentInfo* pTalentInfo)
{
	vector<CSTalentEventInfo>& vecEvents = pTalentInfo->m_Events.m_vecActEvents;
	for each (const CSTalentEventInfo& each in vecEvents)
	{
		switch (each.m_nEvent)
		{
		case TE_LAUNCH_PROJECTILE:
			{
				switch(each.m_ProjectileInfo.m_nType)
				{
				case TPT_MISSILE:
					break;
				case TPT_MISSILE_GUIDED:
					if (CSEffectInfo::POINT_TARGET != pTalentInfo->m_EffectInfo.m_nPoint)
					{
						_VLOGGER->Log(L"Guided Missile 타입의 발사체는 EffectPoint 속성이 \"target\"이어야합니다. (type: %d)\n", pTalentInfo->m_EffectInfo.m_nPoint);
					}
					
					break;
				case TPT_HITCAPSULE_GUIDED:
					break;
				default:
					_VLOGGER->Log(L"정의되지 않은 탤런트 발사체 타입입니다. (type: %d)\n", each.m_ProjectileInfo.m_nType);
				}

				
			}break;
		}
	}
}

void GTalentInfoValidator::CheckGather(GTalentInfo* pTalentInfo)
{
	if (pTalentInfo->m_nTalentType != TT_GATHER)	return;

    if (pTalentInfo->m_nSkillType != ST_PASSIVE)
	{
		_VLOGGER->Log(L"%s은 %s가 아닙니다.\n", _T(TALENT_XML_ATTR_SKILL_TYPE), TALENT_SKILL_TYPE_STR[ST_PASSIVE]);
	}

	if (pTalentInfo->m_nStyle != TS_LICENSE)
	{
		_VLOGGER->Log(L"%s은 %s가 아닙니다.\n", _T(TALENT_XML_ATTR_STYLE)), CSStrings::TalentStyleToString(TS_LICENSE);
	}

	if (pTalentInfo->m_nRank < 1 || 10 < pTalentInfo->m_nRank)
	{
		_VLOGGER->Log(L"%s은 1 ~ 10 의 값만 허용됩니다. (Input=\'%d\')\n", _T(TALENT_XML_ATTR_RANK), pTalentInfo->m_nRank);
	}
}

void GTalentInfoValidator::CheckAABBSize(GTalentInfo* pTalentInfo)
{
	VALID(NULL!=pTalentInfo);
	if (ST_PASSIVE == pTalentInfo->m_nSkillType)
	{
		return;
	}

	if (TS_LICENSE == pTalentInfo->m_nStyle)
	{
		return;
	}

	CSTalentHitInfo& hit_info = pTalentInfo->m_HitInfo;
	if (true == hit_info.m_vSegments.empty())
	{
		return;
	}

	const MBox& aabb(pTalentInfo->GetHitAABB());
	if (static_cast<float>(SECTOR_SIZE * 2) < aabb.GetRadius() / 2.0f)
	{
		_VLOGGER->Log(L"AABB 반경이 너무 큽니다. AABB의 최대 크기는 섹터크기 * 2(%f)입니다. 현재 AABB의 크기 : %f\n", static_cast<float>(SECTOR_SIZE * 2), aabb.GetRadius() / 2.0f);
	}

	if (aabb.minx >= aabb.maxx)
	{
		_VLOGGER->Log(L"AABB x 최소값은 최대값보다 커야 합니다. 현재 AABB의 x 최소값 : %f, x 최대값 : %f\n", aabb.minx, aabb.maxx);
	}

	if (aabb.miny >= aabb.maxy)
	{
		_VLOGGER->Log(L"AABB y 최소값은 최대값보다 커야 합니다. 현재 AABB의 y 최소값 : %f, y 최대값 : %f\n", aabb.miny, aabb.maxy);
	}
}

void GTalentInfoValidator::CheckRank(GTalentInfo* pTalentInfo)
{
	if (pTalentInfo->m_nID != pTalentInfo->m_nTalentLine &&
		pTalentInfo->IsNPCTalent() == false) // NPC가 사용하지 않는 경우에만 유효 (플레이어 탤런트)
	{
		_VLOGGER->ValidateMinusORZero(pTalentInfo->m_nRank, _T(TALENT_XML_ATTR_RANK));
	}
}

void GTalentInfoValidator::CheckLineNRank( GTalentInfo* pTalentInfo )
{
	if (pTalentInfo == NULL) return;

	if (pTalentInfo->m_nRank <= 0) return;

	//100000번 이상은 학습탈렌트가 아니라서 제외
	if (pTalentInfo->m_nID > 100000) return;

	//라인은 있는데 랭크가 없는.
	//포탈 라인이 3333 이면 패스 ?
	if (pTalentInfo->m_nID >= 39001 && pTalentInfo->m_nID<= 39003) return;

	_VLE(_T(TALENT_XML_TAG_TALENT));
	_VLA(_T(TALENT_XML_ATTR_NAME)		, pTalentInfo->m_szName);
	_VLA(_T(TALENT_XML_ATTR_TALENTLINE)	, pTalentInfo->m_nTalentLine);
	_VLA(_T(TALENT_XML_ATTR_RANK)		, pTalentInfo->m_nRank);
	_VLP;

	int checkcount = 0;
	for (GTalentInfoMgr::iterator itor2 = gmgr.pTalentInfoMgr->begin(); itor2 != gmgr.pTalentInfoMgr->end(); ++itor2)
	{
		GTalentInfo* pTalentInfo2 = static_cast<GTalentInfo*>((*itor2).second);
		if (NULL == pTalentInfo2) continue;

		if (pTalentInfo->m_nTalentLine == pTalentInfo2->m_nTalentLine &&
			pTalentInfo->m_nRank == pTalentInfo2->m_nRank)
		{
			checkcount++;
			if (checkcount != 1)
			{
				_VLOGGER->Log(L"탤런트 라인과 랭크가 동일한 다른 탤런트가 있습니다.\n");
				break;
			}
		}
	}
}

void GTalentInfoValidator::CheckMotionFactor( GTalentInfo* pTalentInfo )
{
	for (int i = 0; i < MF_PRIORITY_SIZE; i++)
	{
		if (pTalentInfo->m_MotionFactorGroup.var[i].nValue > 0 &&
			pTalentInfo->m_MotionFactorGroup.var[i].nWeight <= 0)
		{
			_VLOGGER->Log(L"모션팩터 %s의 WEIGHT값이 0입니다.\n", MF_STATE_NAME[i]);
		}
	}

	if (pTalentInfo->m_nForceMF == MF_GRAPPLED || pTalentInfo->m_nForceMF == MF_SWALLOWED)
	{
		_VLOGGER->ValidateMinusORZero(pTalentInfo->m_nForceMFWeight, _T(TALENT_XML_ATTR_FORCE_MF_WEIGHT));
	}
}

void GTalentInfoValidator::CheckBuff( GTalentInfo* pTalentInfo )
{
	GBuffInfo* pBuffInfo1 = gmgr.pBuffInfoMgr->Get(pTalentInfo->m_Buff1.nID);
	GBuffInfo* pBuffInfo2 = gmgr.pBuffInfoMgr->Get(pTalentInfo->m_Buff2.nID);

	if (pTalentInfo->m_Buff1.nID != INVALID_BUFF_ID && 
		!pBuffInfo1)
	{
		_VLOGGER->Log(L"유효하지 않은 BuffID(%d)가 있습니다.\n", pTalentInfo->m_Buff1.nID);
	}

	if (pTalentInfo->m_Buff2.nID != INVALID_BUFF_ID &&
		!pBuffInfo2)
	{
		_VLOGGER->Log(L"유효하지 않은 BuffID(%d)가 있습니다.\n", pTalentInfo->m_Buff2.nID);
	}
}

void GTalentInfoValidator::CheckTNeedLine(GTalentInfo* pTalentInfo)
{
	_VLOGGER->ValidateTNeedTalent(pTalentInfo->m_nTNeedTalent, _T(TALENT_XML_TAG_TNEED_TALENT));
}

void GTalentInfoValidator::CheckModifier( GTalentInfo* pTalentInfo )
{
	_VLOGGER->ValidateMinMax(_T(TALENT_XML_ATTR_WEAPON_APPLY_RATE), pTalentInfo->m_fWeaponApplyRate, -10.0f, 10.0f);
	_VLOGGER->ValidateMinMax(_T(TALENT_XML_ATTR_DRAIN_RATE), pTalentInfo->m_Drain.m_fRate, -10.0f, 10.0f);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_HP_REGEN), pTalentInfo->m_ActorModifier.nHPRegen.nMod, -100, 100);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_EN_REGEN), pTalentInfo->m_ActorModifier.nENRegen.nMod, -100, 100);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_STA_REGEN), pTalentInfo->m_ActorModifier.nSTARegen.nMod, -100, 100);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_HP_REGEN_AMP), pTalentInfo->m_ActorModifier.nHPRegen.fPer, -2.0f, 2.0f);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_EN_REGEN_AMP), pTalentInfo->m_ActorModifier.nENRegen.fPer, -2.0f, 2.0f);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_STA_REGEN_AMP), pTalentInfo->m_ActorModifier.nSTARegen.fPer, -2.0f, 2.0f);

	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_CRITICAL_MELEE_DAMAGE_AMP), pTalentInfo->m_ActorModifier.fCriticalMeleeDamageAmp, -2.0f, 2.0f);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_CRITICAL_RANGE_DAMAGE_AMP), pTalentInfo->m_ActorModifier.fCriticalRangeDamageAmp, -2.0f, 2.0f);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_CRITICAL_MAGIC_DAMAGE_AMP), pTalentInfo->m_ActorModifier.fCriticalMagicDamageAmp, -2.0f, 2.0f);

	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_MELEE_ATK_AMP), pTalentInfo->m_ActorModifier.fMeleeDamageAmp, -2.0f, 2.0f);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_RANGE_ATK_AMP), pTalentInfo->m_ActorModifier.fRangeDamageAmp, -2.0f, 2.0f);
	_VLOGGER->ValidateMinMax(_T(XML_ATTR_MOD_MAGIC_ATK_AMP), pTalentInfo->m_ActorModifier.fMagicDamageAmp, -2.0f, 2.0f);
}

void GTalentInfoValidator::CheckActTime( GTalentInfo* pTalentInfo )
{
	if (!pTalentInfo->m_bValidation)
		return; // 검사 항목이 아님

	if (pTalentInfo->m_nSkillType != ST_MELEE &&
		pTalentInfo->m_nSkillType != ST_MAGIC)
		return;	// 공격형 액티브 탤런트만 유효

	if (!pTalentInfo->m_setNPCID.empty())
	{
		if (!pTalentInfo->m_szUseAnimation.empty())
		{
			_VLOGGER->ValidateMinusORZero(pTalentInfo->m_ActAnimationTime.GetTime(), _T(TALENT_XML_ATTR_ACT_ANIMATION_TIME));

		}

		return;	// NPC가 사용하는 탤런트는 예외
	}

	if (!pTalentInfo->m_szUseAnimation.empty())
	{
		if (pTalentInfo->m_WeaponAllowedInfo.IsAllowed(TALENT_WEAPON_ALLOWED_1HS))
		{
			_VLOGGER->ValidateMinusORZero(pTalentInfo->m_ActAnimationTime.GetTime(WEAPON_1H_SLASH), (wstring(_T(TALENT_XML_ATTR_ACT_ANIMATION_TIME)) + L"_slash").c_str());
		}

		if (pTalentInfo->m_WeaponAllowedInfo.IsAllowed(TALENT_WEAPON_ALLOWED_1HB))
		{
			_VLOGGER->ValidateMinusORZero(pTalentInfo->m_ActAnimationTime.GetTime(WEAPON_1H_BLUNT), (wstring(_T(TALENT_XML_ATTR_ACT_ANIMATION_TIME)) + L"_blunt").c_str());
		}

		if (pTalentInfo->m_WeaponAllowedInfo.IsAllowed(TALENT_WEAPON_ALLOWED_2HD_H))	// Heavy version
		{
			_VLOGGER->ValidateMinusORZero(pTalentInfo->m_ActAnimationTime.GetTime(WEAPON_TWO_HANDED), (wstring(_T(TALENT_XML_ATTR_ACT_ANIMATION_TIME)) + L"_2hd").c_str());
		}

		if (pTalentInfo->m_WeaponAllowedInfo.IsAllowed(TALENT_WEAPON_ALLOWED_STF))
		{
			_VLOGGER->ValidateMinusORZero(pTalentInfo->m_ActAnimationTime.GetTime(WEAPON_STAFF), (wstring(_T(TALENT_XML_ATTR_ACT_ANIMATION_TIME)) + L"_staff").c_str());
		}
	}
	
	// WEAPON_1H_DAGGER: 추후 추가
}

void GTalentInfoValidator::CheckExtraActive( GTalentInfo* pTalentInfo )
{
	switch(pTalentInfo->m_nExtraActive)
	{
	case TEAT_TOGGLE_PRECISION:		
		CheckExtraActiveToggle(pTalentInfo, TFT_PRECISION);
		break;
	case TEAT_TOGGLE_SNEAK:
		CheckExtraActiveToggle(pTalentInfo, TFT_SNEAK);
		break;

	case TEAT_DISPEL:		
	case TEAT_CURE_DISEASE:		
	case TEAT_CURE_POISON:		
	case TEAT_CURE_CURSE:		
	case TEAT_DISPEL_MESMERIZE:
	case TEAT_DISPEL_DEATHPENALTY:
	case TEAT_DISPEL_BUFF:
		_VLOGGER->ValidateMinus(pTalentInfo->m_nExtraActiveParam1, _T(TALENT_XML_ATTR_EXTRA_ACTIVE_PARAM1));
		break;

	case TEAT_BUFF_ENTITY:
		{						
			GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(pTalentInfo->m_nExtraActiveParam1);
			if (pBuffInfo == NULL)
			{
				_VLOGGER->ValidateBuffID(pTalentInfo->m_nExtraActiveParam1, _T(TALENT_XML_ATTR_EXTRA_ACTIVE_PARAM1));
			}

			_VLOGGER->ValidateMinus(pTalentInfo->m_nExtraActiveParam1, _T(TALENT_XML_ATTR_EXTRA_ACTIVE_PARAM2));
		}
		break;
	default:
		break;
	}

	pTalentInfo->m_nExtraActiveParam1;
}

void GTalentInfoValidator::CheckDamageAttrib( GTalentInfo* pTalentInfo )
{
	_VLOGGER->ValidateDamageAttribType(pTalentInfo->m_nDamageAttrib, _T(TALENT_XML_ATTR_DAMAGE_ATTRIB));
}

void GTalentInfoValidator::CheckTalentPosInfo()
{
	// talent_pos_info.xml
	_VLI(FILENAME_TALENT_POS_INFO);

	map<int, int> vecTalentList;
	if(Load(&vecTalentList, FILENAME_TALENT_POS_INFO) == false)
	{
		_VLOGGER->Log(L"talent_pos_info.xml 이 존재하지 않습니다.\n");
		return;
	}
}

bool GTalentInfoValidator::Load( map<int, int>* vecTalentList, const wchar_t * szFileName )
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) 
	{
		mlog("Failed To Load %s\n", MLocale::ConvUTF16ToAnsi(szFileName).c_str());
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_INFO))
		{
			int id = -1;
			_Attribute(&id, pElement, TALENT_XML_ATTR_ID);

			if (id != -1)
			{
				vecTalentList->insert(map<int, int>::value_type(id, id));
			}
		}
	}

	return true;
}

void GTalentInfoValidator::CheckNPC(GTalentInfo* pTalentInfo)
{
	// NPC_Include
	for each(int nBuffID in pTalentInfo->m_RequireBuff.vecIncludeBuffID)
	{
		_VLOGGER->ValidateBuffID(nBuffID, _T(TALENT_XML_ATTR_NPC_INCLUDE));
	}
	
	// NPCExclude
	for each(int nBuffID in pTalentInfo->m_RequireBuff.vecExcludeBuffID)
	{
		_VLOGGER->ValidateBuffID(nBuffID, _T(TALENT_XML_ATTR_NPC_EXCLUDE));
	}
}

void GTalentInfoValidator::CheckExtraActiveToggle(GTalentInfo* pTalentInfo, TALENT_FOCUS_TYPE nFocusType)
{
	int nFocusBuffID = BUFF_FOCUS_ID[nFocusType];
	int nBuffID = nFocusBuffID + (pTalentInfo->m_nExtraActiveParam1 - 1);

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (pBuffInfo == NULL)
	{
		_VLOGGER->ValidateBuffID(nBuffID, _T(TALENT_XML_ATTR_EXTRA_ACTIVE_PARAM1));
	}
	else if (nFocusBuffID != nBuffID)
	{			
		GBuffInfo* pFocusBuffInfo = gmgr.pBuffInfoMgr->Get(nFocusBuffID);
		if (pFocusBuffInfo == NULL)
		{
			_VLOGGER->Log(L"%s 기본 포커스버프가 없습니다.\n", _T(TALENT_XML_ATTR_EXTRA_ACTIVE_TYPE));
		}
		else if (pFocusBuffInfo->m_strName != pBuffInfo->m_strName)
		{
			_VLOGGER->Log(L"%s 기본 포커스버프와 %s 버프의 이름이 다릅니다.\n", TALENT_XML_ATTR_EXTRA_ACTIVE_TYPE, _T(TALENT_XML_ATTR_EXTRA_ACTIVE_PARAM1));
		}
	}
}

void GTalentInfoValidator::CheckPointAndRelation( GTalentInfo* pTalentInfo )
{
	if (pTalentInfo->m_EffectInfo.m_nPoint == CSEffectInfo::POINT_SELF &&
		pTalentInfo->m_EffectInfo.m_nRelation == CSEffectInfo::RELATION_ENEMY &&
		pTalentInfo->m_EffectInfo.m_fRadius == 0.0f)
	{
		_VLOGGER->Log(L"EffectPoint가 self고 EffectTarget이 enemy면서 EffectRange가 0이면 타겟을 지정할 수 없습니다.\n");
	}
}
