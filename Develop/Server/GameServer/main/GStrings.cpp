#include "stdafx.h"
#include "GStrings.h"
#include "GState.h"
#include "GAIState.h"
#include "GConfig_Arg.h"
#include "GInnRoomInfo.h"
#include "GConst.h"
#include "CSTalentInfoEnum.h"
#include "CSQuestInfo.h"
#include "CSFactionInfo.h"
#include "CSFieldInfo.h"
#include "GDef.h"
#include "GNPCInfo.h"
#include "CSDef_Quest.h"
#include "CSDef_QuestPVP.h"
#include "CSDef_Interaction.h"


vector<wstring>	GStrings::m_vecStrColtCondition;
vector<wstring>	GStrings::m_vecStrColtAction;
vector<wstring>	GStrings::m_vecStrBehaviorState;
vector<wstring>	GStrings::m_vecStrCombatState;
vector<wstring>	GStrings::m_vecStrTalentSkillType;
vector<wstring>	GStrings::m_vecStrFieldType;
vector<wstring>	GStrings::m_vecStrInnRoomType;
vector<wstring>	GStrings::m_vecStrFactionRelationType;
vector<wstring>	GStrings::m_vecStrITargetConditionType;


bool GStrings::Init()
{
	if (false == CSStrings::Init()) return false;

	m_vecStrColtCondition.clear();
	m_vecStrColtCondition.reserve(AST_SIZE);
	m_vecStrColtCondition.push_back(L"none");
	m_vecStrColtCondition.push_back(L"hp");
	m_vecStrColtCondition.push_back(L"en");
	m_vecStrColtCondition.push_back(L"sp");
	m_vecStrColtCondition.push_back(L"unbreakpart");
	m_vecStrColtCondition.push_back(L"breakpart");
	m_vecStrColtCondition.push_back(L"damage");
	m_vecStrColtCondition.push_back(L"motionfactor");
	m_vecStrColtCondition.push_back(L"distance");
	m_vecStrColtCondition.push_back(L"angle");
	m_vecStrColtCondition.push_back(L"mode");
	m_vecStrColtCondition.push_back(L"e_hp");
	m_vecStrColtCondition.push_back(L"e_motionfactor");
	m_vecStrColtCondition.push_back(L"stress");
	m_vecStrColtCondition.push_back(L"hitcapsule");
	m_vecStrColtCondition.push_back(L"used_talent");
	m_vecStrColtCondition.push_back(L"gained_buff");
	m_vecStrColtCondition.push_back(L"e_gained_buff");
	m_vecStrColtCondition.push_back(L"rear");
	m_vecStrColtCondition.push_back(L"front");
	m_vecStrColtCondition.push_back(L"rage");
	m_vecStrColtCondition.push_back(L"dead");
	m_vecStrColtCondition.push_back(L"combat");
	m_vecStrColtCondition.push_back(L"peace");
	if (AST_SIZE != m_vecStrColtCondition.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrColtAction.clear();
	m_vecStrColtAction.reserve(CAT_SIZE);
	m_vecStrColtAction.push_back(L"none");
	m_vecStrColtAction.push_back(L"nothing");
	m_vecStrColtAction.push_back(L"group");
	m_vecStrColtAction.push_back(L"talent");
	m_vecStrColtAction.push_back(L"luafunc");
	m_vecStrColtAction.push_back(L"breakpart");
	m_vecStrColtAction.push_back(L"damage");
	m_vecStrColtAction.push_back(L"motionfactor");
	m_vecStrColtAction.push_back(L"flee");
	m_vecStrColtAction.push_back(L"yell");
	m_vecStrColtAction.push_back(L"guard");
	m_vecStrColtAction.push_back(L"adjust");
	m_vecStrColtAction.push_back(L"distance");
	m_vecStrColtAction.push_back(L"follow");
	m_vecStrColtAction.push_back(L"move");
	m_vecStrColtAction.push_back(L"change_hitcapsule");
	m_vecStrColtAction.push_back(L"say");
	m_vecStrColtAction.push_back(L"shot");
	m_vecStrColtAction.push_back(L"balloon");
	m_vecStrColtAction.push_back(L"narration");
	m_vecStrColtAction.push_back(L"aggro");
	m_vecStrColtAction.push_back(L"face_to");
	if (CAT_SIZE != m_vecStrColtAction.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrBehaviorState.clear();
	m_vecStrBehaviorState.reserve(AI_BEHAVIOR_SIZE);
	m_vecStrBehaviorState.push_back(L"idle");
	m_vecStrBehaviorState.push_back(L"combat");
	m_vecStrBehaviorState.push_back(L"mission");
	m_vecStrBehaviorState.push_back(L"dead");
	m_vecStrBehaviorState.push_back(L"return");
	m_vecStrBehaviorState.push_back(L"session");
	if (AI_BEHAVIOR_SIZE != m_vecStrBehaviorState.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrCombatState.clear();
	m_vecStrCombatState.reserve(AI_COMBAT_SIZE);
	m_vecStrCombatState.push_back(L"combat");
	m_vecStrCombatState.push_back(L"flee");
	m_vecStrCombatState.push_back(L"victory");
	m_vecStrCombatState.push_back(L"prologue");
	if (AI_COMBAT_SIZE != m_vecStrCombatState.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrTalentSkillType.clear();
	m_vecStrTalentSkillType.reserve(ST_MAX);
	m_vecStrTalentSkillType.push_back(L"none");
	m_vecStrTalentSkillType.push_back(L"melee");
	m_vecStrTalentSkillType.push_back(L"archery");
	m_vecStrTalentSkillType.push_back(L"gun");
	m_vecStrTalentSkillType.push_back(L"magic");
	m_vecStrTalentSkillType.push_back(L"guard");
	m_vecStrTalentSkillType.push_back(L"passive");
	m_vecStrTalentSkillType.push_back(L"buff");
	m_vecStrTalentSkillType.push_back(L"extra_active");
	m_vecStrTalentSkillType.push_back(L"extra_passive");
	m_vecStrTalentSkillType.push_back(L"move");
	m_vecStrTalentSkillType.push_back(L"item");
	if (ST_MAX != m_vecStrTalentSkillType.size()) 
	{
		_ASSERT(0);
		return false;	
	}

	m_vecStrFieldType.clear();
	m_vecStrFieldType.reserve(FILEDTYPE_MAX);
	m_vecStrFieldType.push_back(L"none");
	m_vecStrFieldType.push_back(L"hunting_ground");
	m_vecStrFieldType.push_back(L"town");
	if (FILEDTYPE_MAX != m_vecStrFieldType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrInnRoomType.clear();
	m_vecStrInnRoomType.reserve(IRT_MAX);
	m_vecStrInnRoomType.push_back(L"none");
	m_vecStrInnRoomType.push_back(L"normal");
	m_vecStrInnRoomType.push_back(L"basic");
	m_vecStrInnRoomType.push_back(L"storage");
	m_vecStrInnRoomType.push_back(L"steward");
	m_vecStrInnRoomType.push_back(L"premium");
	if (IRT_MAX != m_vecStrInnRoomType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrFactionRelationType.clear();
	m_vecStrFactionRelationType.reserve(FRT_MAX);
	m_vecStrFactionRelationType.push_back(L"none");
	m_vecStrFactionRelationType.push_back(L"worst");
	m_vecStrFactionRelationType.push_back(L"bad");
	m_vecStrFactionRelationType.push_back(L"normal");
	m_vecStrFactionRelationType.push_back(L"good");
	m_vecStrFactionRelationType.push_back(L"exellent");
	m_vecStrFactionRelationType.push_back(L"trust");
	VALID_RET(FRT_MAX == m_vecStrFactionRelationType.size(), false) ;

	m_vecStrITargetConditionType.clear();
	m_vecStrITargetConditionType.reserve(ITCT_MAX);
	m_vecStrITargetConditionType.push_back(L"none");
	m_vecStrITargetConditionType.push_back(L"live");
	m_vecStrITargetConditionType.push_back(L"dead");
	m_vecStrITargetConditionType.push_back(L"all");
	VALID_RET(ITCT_MAX == m_vecStrITargetConditionType.size(), false) ;
	
	return true;
}

int GStrings::StringToBehaviorState(const wchar_t* szValue)
{
	return StringToEnum(szValue, m_vecStrBehaviorState);
}

int GStrings::StringToCombatState(const wchar_t* szValue)
{
	return StringToEnum(szValue, m_vecStrCombatState);
}

int GStrings::StringToFieldType(const wchar_t* szValue)
{
	return StringToEnum(szValue, m_vecStrFieldType);
}

int GStrings::StringToInnRoomType(const wchar_t* szValue)
{
	return StringToEnum(szValue, m_vecStrInnRoomType);
}

int GStrings::StringToFactionRelationType(const wchar_t* szValue)
{
	return StringToEnum(szValue, m_vecStrFactionRelationType);
}

int GStrings::StringToITargetConditionType( const wchar_t* szValue )
{
	return StringToEnum(szValue, m_vecStrITargetConditionType);
}

int GStrings::StringToColtConditionState(const wchar_t* szValue)
{
	return StringToEnum(szValue, m_vecStrColtCondition);
}

const wchar_t* GStrings::BehaviorStateToString(int nValue)
{
	return EnumToString(nValue, m_vecStrBehaviorState);
}

const wchar_t* GStrings::CombatStateToString(int nValue)
{
	return EnumToString(nValue, m_vecStrCombatState);
}

const wchar_t* GStrings::ColtConditionStateToString(int nValue)
{
	return EnumToString(nValue, m_vecStrColtCondition);
}

const wchar_t* GStrings::ColtActionToString(int nValue)
{
	return EnumToString(nValue, m_vecStrColtAction);
}

const wchar_t* GStrings::TalentSkillTypeToString(int nValue)
{
	return EnumToString(nValue, m_vecStrTalentSkillType);
}

const wchar_t* GStrings::ConditionTypeToString(int nValue)
{
	return EnumToString(nValue, m_vecStrConditionType);
}

float GStrings::CalcSpeakMaintainTime(const wchar_t* szText)
{
	// 지역화 관련하여 고정값 사용하도록 수정했습니다.
	return GConst::SPEAK_MAINTAIN_TIME;
}
