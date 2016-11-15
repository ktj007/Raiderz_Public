#include "stdafx.h"
#include "GNPCInfoMgr.h"
#include "GNPCInfo.h"
#include "GTalentInfo.h"
#include "GLootInfoMgr.h"
#include "GValidateLogger.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"
#include "GStrings.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GNPCScriptBuilder.h"
#include "GBuffinfo.h"


#define NPC_XML_ATTR_CRITICAL_DMG_AMP		"CriticalDmgAmp"
#define NPC_XML_ATTR_CRITICAL_MELEE			"CriticalMelee"
#define NPC_XML_ATTR_CRITICAL_RANGE			"CriticalRange"
#define NPC_XML_ATTR_CRITICAL_MAGIC			"CriticalMagic"

#define NPC_XML_ATTR_MIN_DAMAGE				"MinDamage"
#define NPC_XML_ATTR_MAX_DAMAGE				"MaxDamage"
#define NPC_XML_ATTR_SPELL_POWER			"SpellPower"

#define NPC_XML_ATTR_CHA					"CHA"
#define NPC_XML_ATTR_FR						"FR"
#define NPC_XML_ATTR_CR						"CR"
#define NPC_XML_ATTR_LR						"LR"
#define NPC_XML_ATTR_PR						"PR"
#define NPC_XML_ATTR_HR						"HR"
#define NPC_XML_ATTR_UR						"UR"
#define NPC_XML_ATTR_EAT_OWNERBUFFID		"EatOwnerBuffID"
#define NPC_XML_ATTR_EAT_TARGETBUFFID		"EatTargetBuffID"
#define NPC_XML_ATTR_EAT_BUFFPERIOD			"EatBuffPeriod"
#define NPC_XML_ATTR_HATE_EXPIREDTIME		"HateExpiredTime"
#define NPC_XML_ATTR_ANI_PREFIX				"AniPrefix"
#define NPC_XML_ATTR_COLT_LINK				"ColtLink"


#define NPC_XML_ATTR_ARMOR_TYPE				"ArmorType"
#define NPC_XML_ATTR_LROTATION_ID			"LRotationID"
#define NPC_XML_ATTR_RROTATION_ID			"RRotationID"
#define NPC_XML_ATTR_ROTATION_ANGLE			"RotationAngle"
#define NPC_XML_ATTR_ROTATION_ACT_ANGLE		"RotationActAngle"

#define NPC_XML_ATTR_LUA_SPAWN_COUNT		"MaxSpawnCount"
#define NPC_XML_ATTR_LUA_KEEPCORPSETIME		"KeepCorpseTime"
#define NPC_XML_ATTR_LUA_HATE_MODIFIER_FOR_NPC	"HateModifierForNPC"


void GNPCInfoMgr::ParseNPC(MXmlElement* pElement, MXml* pXml, int nExpectedID)
{	
	int id = INVALID_ID;
	_Attribute(&id,									pElement, NPC_XML_ATTR_ID);

	if (id <= INVALID_ID)
		return;

	if (nExpectedID > 0 && nExpectedID != id)
		return;


	wstring strName;
	wstring strValue;
	int nValue = 0;

	GNPCInfo* pParentNPCInfo = NULL;
	GNPCInfo* pNewNPCInfo = NULL;	

	pNewNPCInfo = new GNPCInfo();
	pParentNPCInfo = Find(id);

	// parent가 있으면 parent의 NPC에서 복사한다.
	//int parent_id = -1;
	//if (_Attribute(&parent_id, pElement, NPC_XML_ATTR_PARENT))
	//{
	//	GNPCInfo* pParentNPC = Find(parent_id);
	//	if (pParentNPC)
	//	{
	//		*pNewNPCInfo = *pParentNPC;
	//	}
	//	else parent_id = -1;	// 없으면 다시 -1
	//}

	pNewNPCInfo->nID = id;	

	// 같은 ID의 NPC 정보가 있다면 복제 (모드가 다른 경우)
	if (pParentNPCInfo)
	{
		*pNewNPCInfo = *pParentNPCInfo;
		pNewNPCInfo->ColtInfo = GColtGroupInfo();
		pNewNPCInfo->m_InteractionInfo = GNPCInteractionInfo();
		pNewNPCInfo->vecModes.clear();
	}

	if (true == _Attribute(strValue,		pElement, NPC_XML_ATTR_VALIDATION, pXml))
	{
		pNewNPCInfo->bValidation = GStrings::StringToBool(strValue.c_str());
	}

	_Attribute(pNewNPCInfo->strName,					pElement, NPC_XML_ATTR_NAME, pXml);
	_Attribute(pNewNPCInfo->strMeshName,				pElement, NPC_XML_ATTR_MESH_NAME, pXml);
	_Attribute(&pNewNPCInfo->nMaxHP,					pElement, NPC_XML_ATTR_MAX_HP);
	_Attribute(&pNewNPCInfo->nAP,						pElement, NPC_XML_ATTR_AP);
	_Attribute(&pNewNPCInfo->bHuge,						pElement, NPC_XML_ATTR_HUGE);
	_Attribute(&pNewNPCInfo->fScale,					pElement, NPC_XML_ATTR_SCALE);
	_Attribute(&pNewNPCInfo->fSpeed,					pElement, NPC_XML_ATTR_SPEED);
	_Attribute(&pNewNPCInfo->fRunSpeed,					pElement, NPC_XML_ATTR_RUN_SPEED);
	_Attribute(&pNewNPCInfo->fFleeSpeedAmp,				pElement, NPC_XML_ATTR_FLEE_SPEED_AMP);
	_Attribute(&pNewNPCInfo->fRotateSpeed,				pElement, NPC_XML_ATTR_ROTATE_SPEED);
	_Attribute(&pNewNPCInfo->m_nLuaSpawnCount,			pElement, NPC_XML_ATTR_LUA_SPAWN_COUNT);
	_Attribute(&pNewNPCInfo->m_fKeepCorpseTime,			pElement, NPC_XML_ATTR_LUA_KEEPCORPSETIME);
	_Attribute(&pNewNPCInfo->m_fHateModifierForNPC,		pElement, NPC_XML_ATTR_LUA_HATE_MODIFIER_FOR_NPC);
	
	
	_VLE(_T(NPC_XML_TAG_NPC));
	_VLA(_T(NPC_XML_ATTR_ID), id);
	_VLA(_T(NPC_XML_ATTR_NAME), pNewNPCInfo->strName);
	_VLP;

	if (_Attribute(&nValue,									pElement, NPC_XML_ATTR_MODE))
	{
		bool bExistMode = false;
		for (int i = 0; i < NPC_MODE_MAX; i++)
		{
			if (NPC_MODE(i) == nValue)
			{
				bExistMode = true;
				pNewNPCInfo->nMode = NPC_MODE(i);
			}
		}
		_ASSERT(bExistMode == true);
	}

	if (_Attribute(&nValue,									pElement, NPC_XML_ATTR_SPAWNING_MODE))
	{
		pNewNPCInfo->nSpawningMode = NPC_MODE(nValue);
	}


	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_HITTEST_INDEX))
	{
		pNewNPCInfo->nHitTestIndex = (uint8)nValue;
	}

	_Attribute(pNewNPCInfo->nLevel,						pElement, NPC_XML_ATTR_MIN_LEVEL);
	_Attribute(pNewNPCInfo->nMaxLevel,					pElement, NPC_XML_ATTR_MAX_LEVEL);

	if (true == _Attribute(strValue,		pElement, NPC_XML_ATTR_AA, pXml))
	{
		pNewNPCInfo->nAA = static_cast<NPC_AA_TYPE>(GStrings::StringToAAType(strValue.c_str()));
	}

	if (true == _Attribute(strValue,		pElement, NPC_XML_ATTR_ATTACKABLE, pXml))
	{
		pNewNPCInfo->nInitAttackable = static_cast<NPC_ATTACKABLE_TYPE>(GStrings::StringToAttackableType(strValue.c_str()));
	}	

	_Attribute(pNewNPCInfo->bCombatable,				pElement, NPC_XML_ATTR_COMBATABLE);

	_Attribute(&pNewNPCInfo->nSightRange,				pElement, NPC_XML_ATTR_SIGHTRANGE);
	_Attribute(&pNewNPCInfo->m_bAutoAssist,				pElement, NPC_XML_ATTR_AUTOASSIST);
	_Attribute(&pNewNPCInfo->m_fAutoAssistRange,		pElement, NPC_XML_ATTR_AUTOASSIST_RANGE);
	_Attribute(&pNewNPCInfo->m_bAssistAfterCombat,		pElement, NPC_XML_ATTR_ASSIST_AFTER_COMBAT);
	_Attribute(&pNewNPCInfo->bRooted,					pElement, NPC_XML_ATTR_ROOTED);
	_Attribute(&pNewNPCInfo->bCounterAttack,			pElement, NPC_XML_ATTR_COUNTERATTACK);
	_Attribute(&pNewNPCInfo->bTauntable,				pElement, NPC_XML_ATTR_TAUNTABLE);
	_Attribute(&pNewNPCInfo->bGainEXP,					pElement, NPC_XML_ATTR_GAINEXP);
	_Attribute(&pNewNPCInfo->nCHA,						pElement, NPC_XML_ATTR_CHA);
	_Attribute(&pNewNPCInfo->nEatOwnerBuffID,			pElement, NPC_XML_ATTR_EAT_OWNERBUFFID);
	_Attribute(&pNewNPCInfo->nEatTargetBuffID,			pElement, NPC_XML_ATTR_EAT_TARGETBUFFID);
	_Attribute(&pNewNPCInfo->fEatPeriod,				pElement, NPC_XML_ATTR_EAT_BUFFPERIOD);
	_Attribute(&pNewNPCInfo->m_fHateExpiredTime,		pElement, NPC_XML_ATTR_HATE_EXPIREDTIME);
	_Attribute(pNewNPCInfo->strAniPrefix,				pElement, NPC_XML_ATTR_ANI_PREFIX);
	_Attribute(pNewNPCInfo->m_nColtLink,				pElement, NPC_XML_ATTR_COLT_LINK);
		

	if (_Attribute(strValue,								pElement, NPC_XML_ATTR_TYPE))
	{
		for (int i = 0; i < NPC_TYPE_MAX; i++)
		{
			if (!_wcsicmp(strValue.c_str(), NPC_TYPE_STR[i]))
			{
				pNewNPCInfo->nNpcType = NPC_TYPE(i);
				break;
			}
		}
	}

	if (_Attribute(strValue, pElement, NPC_XML_ATTR_GATHER_TYPE))
	{
		pNewNPCInfo->nGatherType = (GATHER_TYPE)CSStrings::StringToGatherType(strValue.c_str());
	}

	_Attribute(&pNewNPCInfo->bShowEnemyInfo, pElement, NPC_XML_ATTR_SHOW_ENEMYINFO, true);

	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_FR))
	{
		pNewNPCInfo->Resist.nValue[TRT_FR] = (uint8)nValue;
	}
	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_CR))
	{
		pNewNPCInfo->Resist.nValue[TRT_CR] = (uint8)nValue;
	}
	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_LR))
	{
		pNewNPCInfo->Resist.nValue[TRT_LR] = (uint8)nValue;
	}
	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_PR))
	{
		pNewNPCInfo->Resist.nValue[TRT_PR] = (uint8)nValue;
	}
	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_HR))
	{
		pNewNPCInfo->Resist.nValue[TRT_HR] = (uint8)nValue;
	}
	if (_Attribute(&nValue,				pElement, NPC_XML_ATTR_UR))
	{
		pNewNPCInfo->Resist.nValue[TRT_UR] = (uint8)nValue;
	}

	_Attribute(&pNewNPCInfo->m_fCriticalDmgAmp,	pElement, NPC_XML_ATTR_CRITICAL_DMG_AMP);
	_Attribute(&pNewNPCInfo->m_fCriticalMelee,	pElement, NPC_XML_ATTR_CRITICAL_MELEE);
	_Attribute(&pNewNPCInfo->m_fCriticalRange,	pElement, NPC_XML_ATTR_CRITICAL_RANGE);
	_Attribute(&pNewNPCInfo->m_nMinDamage,	pElement, NPC_XML_ATTR_MIN_DAMAGE, 0);
	_Attribute(&pNewNPCInfo->m_nMaxDamage,	pElement, NPC_XML_ATTR_MAX_DAMAGE, 0);
	_Attribute(&pNewNPCInfo->m_fSpellPower,	pElement, NPC_XML_ATTR_SPELL_POWER, 1.0f);

	_Attribute(&pNewNPCInfo->MFModifier.Value[MF_BEATEN].fPer,		pElement, NPC_XML_ATTR_BEATEN_MOD_VALUE);
	_Attribute(&pNewNPCInfo->MFModifier.Value[MF_KNOCKBACK].fPer,	pElement, NPC_XML_ATTR_KNOCKBACK_MOD_VALUE);
	_Attribute(&pNewNPCInfo->MFModifier.Value[MF_STUN].fPer,		pElement, NPC_XML_ATTR_STUN_MOD_VALUE);
	_Attribute(&pNewNPCInfo->MFModifier.Value[MF_DEFLECT].fPer,		pElement, NPC_XML_ATTR_DEFLECT_MOD_VALUE);
	_Attribute(&pNewNPCInfo->MFModifier.Value[MF_THROWUP].fPer,		pElement, NPC_XML_ATTR_THROWUP_MOD_VALUE);
	_Attribute(&pNewNPCInfo->MFModifier.Value[MF_KNOCKDOWN].fPer,	pElement, NPC_XML_ATTR_KNOCKDOWN_MOD_VALUE);
	_Attribute(&pNewNPCInfo->MFModifier.Value[MF_UPPERED].fPer,		pElement, NPC_XML_ATTR_UPPERED_MOD_VALUE);

	_Attribute(&pNewNPCInfo->MFModifier.Weight[MF_BEATEN].fPer,		pElement, NPC_XML_ATTR_BEATEN_MOD_WEIGHT);
	_Attribute(&pNewNPCInfo->MFModifier.Weight[MF_KNOCKBACK].fPer,	pElement, NPC_XML_ATTR_KNOCKBACK_MOD_WEIGHT);
	_Attribute(&pNewNPCInfo->MFModifier.Weight[MF_STUN].fPer,		pElement, NPC_XML_ATTR_STUN_MOD_WEIGHT);
	_Attribute(&pNewNPCInfo->MFModifier.Weight[MF_DEFLECT].fPer,		pElement, NPC_XML_ATTR_DEFLECT_MOD_WEIGHT);
	_Attribute(&pNewNPCInfo->MFModifier.Weight[MF_THROWUP].fPer,		pElement, NPC_XML_ATTR_THROWUP_MOD_WEIGHT);
	_Attribute(&pNewNPCInfo->MFModifier.Weight[MF_KNOCKDOWN].fPer,	pElement, NPC_XML_ATTR_KNOCKDOWN_MOD_WEIGHT);
	_Attribute(&pNewNPCInfo->MFModifier.Weight[MF_UPPERED].fPer,		pElement, NPC_XML_ATTR_UPPERED_MOD_WEIGHT);

	// Immune_DA_SLASH
	// Immune_DISABLE_TALENTS
	// Immune_DRAIN

	// 데미지타입 면역정보
	for (int i=0; i<DA_MAX; ++i)
	{
		string strAttr = NPC_XML_ATTR_IMMUNE;
		strAttr += MLocale::ConvUTF16ToAnsi(DAMAGE_ATTRIB_NAME[i]);

		bool bChecked=false;
		_Attribute(&bChecked,	pElement, strAttr.c_str());

		if (bChecked)
		{
			SetBitSet(pNewNPCInfo->ImmuneInfo.nImmuneDamageType, (DAMAGE_ATTRIB)i);
		}
	}

	// 스킬타입 면역정보
	for (int i=0; i<ST_MAX; ++i)
	{
		string strAttr = NPC_XML_ATTR_IMMUNE;
		strAttr += MLocale::ConvUTF16ToAnsi(TALENT_SKILL_TYPE_STR[i]);

		bool bChecked=false;
		_Attribute(&bChecked,	pElement, strAttr.c_str());

		if (bChecked)
		{
			SetBitSet(pNewNPCInfo->ImmuneInfo.nImmuneSkillType, (TALENT_SKILL_TYPE)i);
		}
	}
	
	// 버프 면역정보
	for (int i=0; i<BUFPEA_MAX; ++i)
	{
		string strAttr = NPC_XML_ATTR_IMMUNE;
		strAttr += MLocale::ConvUTF16ToAnsi(BUFF_PASSIVE_EXTRA_ATTRIB_STR[i]);

		bool bChecked=false;
		_Attribute(&bChecked,	pElement, strAttr.c_str());

		if (bChecked)
		{
			SetBitSet(pNewNPCInfo->ImmuneInfo.nImmuneBuff, (BUFF_PASSIVE_EXTRA_ATTRIB_)i);
		}
	}
	
	// 특수 면역정보
	for (int i=0; i<immune::IMMUNE_MAX; ++i)
	{
		string strAttr = NPC_XML_ATTR_IMMUNE;
		strAttr += MLocale::ConvUTF16ToAnsi(immune::CUSTOM_TYPE_STR[i]);

		bool bChecked=false;
		_Attribute(&bChecked,	pElement, strAttr.c_str());

		if (bChecked)
		{
			SetBitSet(pNewNPCInfo->ImmuneInfo.nImmuneCustomType, (immune::CUSTOM_TYPE)i);
		}
	}

	

	int nGrade = 0;
	_Attribute(&nGrade,									pElement, NPC_XML_ATTR_GRADE);
	pNewNPCInfo->nGrade = static_cast<int8>(nGrade);

	string strValueType;
	if (_Attribute(strValueType,									pElement, NPC_XML_ATTR_ALTITUDE))
	{
		if (strValueType == NPC_XML_VALUE_TYPE_NORMAL)						{ pNewNPCInfo->nAltitude = AT_NORMAL; }
		else if (strValueType == NPC_XML_VALUE_TYPE_FLOATING)				{ pNewNPCInfo->nAltitude = AT_FLOATING; }
		else if (strValueType == NPC_XML_VALUE_TYPE_SKY)					{ pNewNPCInfo->nAltitude = AT_SKY; }
		else if (strValueType == NPC_XML_VALUE_TYPE_UNDERGROUND)			{ pNewNPCInfo->nAltitude = AT_UNDERGROUND; }
	}

	if (_Attribute(strValueType,									pElement, NPC_XML_ATTR_GUARDTYPE))
	{
		if (strValueType == NPC_XML_VALUE_TYPE_PERFECT)				{ pNewNPCInfo->nDefaultGuardType = GT_PERFECT; }
		else if (strValueType == NPC_XML_VALUE_TYPE_PARTIAL)		{ pNewNPCInfo->nDefaultGuardType = GT_PARTIAL; }
		else if (strValueType == NPC_XML_VALUE_TYPE_ABSOLUTE)		{ pNewNPCInfo->nDefaultGuardType = GT_ABSOLUTE; }
	}

	if (_Attribute(strValue,									pElement, NPC_XML_ATTR_ARMOR_TYPE))
	for (int i = 0; i < ARMOR_ATT_MAX; i++)
	{
		if (!_wcsicmp(strValue.c_str(), ARMOR_ATTRIB_STR[i]))
		{
			pNewNPCInfo->nArmorType = ARMOR_ATTRIB(i);
			break;
		}
	}

	_Attribute(&pNewNPCInfo->bInteract,				pElement, NPC_XML_ATTR_INTERACT);

	int nLootID;
	if (true == _Attribute(&nLootID,			pElement, NPC_XML_ATTR_ITEM_LOOT))
	{
		pNewNPCInfo->m_pLootInfo = gmgr.pLootInfoMgr->Get(nLootID);
		if (NULL == pNewNPCInfo->m_pLootInfo)
		{
			if (pNewNPCInfo->bValidation)
			{
				_VLOGGER->Log(_T(NPC_XML_ATTR_ITEM_LOOT) IS_NOT_IN_LOOT);
			}
		}		
	}

	_Attribute(pNewNPCInfo->m_strOpeningStatement,	pElement, NPC_XML_ATTR_OPENINING_STATEMENT);

	ParseInteraction(pElement, pNewNPCInfo);
	ParseFaction(pElement, pNewNPCInfo);


	_Attribute(&pNewNPCInfo->fInteractionLootTime,	pElement, NPC_XML_ATTR_INTERACTION_LOOT_TIME);
	_Attribute(&pNewNPCInfo->nInteractionDistance,	pElement, NPC_XML_ATTR_INTERACTION_DISTANCE);

	if (!_Attribute(&pNewNPCInfo->m_nLRotationID,		pElement, NPC_XML_ATTR_LROTATION_ID))
	{
		pNewNPCInfo->m_nLRotationID = INVALID_TALENT_ID;
	}
	if (!_Attribute(&pNewNPCInfo->m_nRRotationID,		pElement, NPC_XML_ATTR_RROTATION_ID))
	{
		pNewNPCInfo->m_nRRotationID = INVALID_TALENT_ID;
	}

	string str = "90";
	_Attribute( str, pElement, NPC_XML_ATTR_ROTATION_ANGLE);
	if ( str == "toward_target")	pNewNPCInfo->m_fRotationAngle = 1000.0f;
	else							pNewNPCInfo->m_fRotationAngle = DegreeToRadian( (float)atof( str.c_str()) );

	_Attribute(&pNewNPCInfo->m_fRotationActAngle,	pElement, NPC_XML_ATTR_ROTATION_ACT_ANGLE);
	pNewNPCInfo->m_fRotationActAngle = DegreeToRadian( pNewNPCInfo->m_fRotationActAngle);

	_Attribute(&pNewNPCInfo->m_bObjectThinkable,	pElement, NPC_XML_ATTR_OBJECT_THINKABLE);

	_Attribute(&pNewNPCInfo->m_isMajorLodNPC,		pElement, NPC_XML_ATTR_IS_MAJOR_LOD_NPC);



	_InitNPCInfo(pNewNPCInfo);
	_BindUsableTalent(pNewNPCInfo);
	_HandleProcessNPCInfo(pParentNPCInfo, pNewNPCInfo);
}

void GNPCInfoMgr::_InitNPCInfo( GNPCInfo* pNewNPCInfo )
{
	VALID(m_MeshInfoMgr.GetSize() > 0);	// 히트메쉬정보가 하나도 없을 수는 없음 (아마도 로딩 실패)

	/// 360도를 몇초만에 회전하냐를, 초당 몇도 회전하냐로 수정
	pNewNPCInfo->fRotateSpeed = (0 >= pNewNPCInfo->fRotateSpeed) ? 0.0f : 360.0f / pNewNPCInfo->fRotateSpeed;

	wchar_t szBuff[128];
	swprintf_s(szBuff, L"NPC_%d", pNewNPCInfo->nID);
	pNewNPCInfo->strNameInLua = szBuff;

	CSMeshInfo* pMeshInfo = m_MeshInfoMgr.GetInfo(pNewNPCInfo->strMeshName);
	pNewNPCInfo->m_pMeshInfo = new GActorMeshInfo();
	pNewNPCInfo->m_pMeshInfo->Init(pMeshInfo, pNewNPCInfo->fScale);
}

void GNPCInfoMgr::_BindUsableTalent( GNPCInfo* pNewNPCInfo )
{
	if (pNewNPCInfo->nID < 1)			return;
	if (pNewNPCInfo->nID >= INT_MAX)	return;

	for (GTalentInfoMgr::iterator itorTalent = gmgr.pTalentInfoMgr->begin();
		itorTalent != gmgr.pTalentInfoMgr->end(); ++itorTalent)
	{
		GTalentInfo* pTalentInfo = static_cast<GTalentInfo*>((*itorTalent).second);
		if (!pTalentInfo->m_setNPCID.empty() && pTalentInfo->m_setNPCID.find(pNewNPCInfo->nID) != pTalentInfo->m_setNPCID.end())
		{
			int nTalentID = pTalentInfo->m_nID;
			pNewNPCInfo->m_Talents.push_back(nTalentID);
		}
	}
}

void GNPCInfoMgr::_HandleProcessNPCInfo( GNPCInfo* pParentNPCInfo, GNPCInfo* pNewNPCInfo )
{
	GNPCScriptBuilder nsb(pNewNPCInfo);
	LoadColt(pNewNPCInfo, nsb);

	// 부모NPC가 없는 경우
	if (pParentNPCInfo == NULL) 
	{
		insert(value_type(pNewNPCInfo->nID, pNewNPCInfo));
		
		nsb.Build();

		// 루아 Init 콜백 호출
		GetScriptNPC()->OnInit(pNewNPCInfo);
			
	}
	// 부모NPC가 있는 경우
	else
	{
		size_t nModeSize = pParentNPCInfo->vecModes.size();
		if (pNewNPCInfo->nMode-1 == nModeSize && pNewNPCInfo->nID == pParentNPCInfo->nID)
		{
			pParentNPCInfo->vecModes.push_back(pNewNPCInfo);
		}
		else
		{
			mlog("NPC의 Mode값이 잘못되었습니다. NPCID=%d , mode=%d\n", pNewNPCInfo->nID, pNewNPCInfo->nMode);
			delete pNewNPCInfo;
		}
	}
}

void GNPCInfoMgr::ParseFaction(MXmlElement* pElement, GNPCInfo* pNPCInfo)
{
	_Attribute(&pNPCInfo->m_nFactionGainID,			pElement, NPC_XML_ATTR_FACTION_GAIN_ID);
	_Attribute(&pNPCInfo->m_nFactionGainQuantity,	pElement, NPC_XML_ATTR_FACTION_GAIN_VAL);
	_Attribute(&pNPCInfo->m_nFactionLossID,			pElement, NPC_XML_ATTR_FACTION_LOSS_ID);
	_Attribute(&pNPCInfo->m_nFactionLossQuantity,	pElement, NPC_XML_ATTR_FACTION_LOSS_VAL);
}
