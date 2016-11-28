#include "stdafx.h"
#include "CSStrings.h"
#include "MDebug.h"
#include "CSDef.h"
#include "CSWorldInfo.h"
#include "CSTalentInfo.h"
#include "CSMotionFactorInfo.h"
#include "CSItemData.h"
#include "CSRecipeInfo.h"
#include "CSSoul.h"
#include "CSChrInfo.h"
#include "CSEffectInfo.h"
#include "CSDef_Quest.h"
#include "CSTriggerInfo.h"
#include "CSConditionInfo.h"
#include "CSFactionInfo.h"
#include "CSDef_Interaction.h"

vector<tstring> CSStrings::m_vecStrGameTime;		
vector<tstring> CSStrings::m_vecStrGameWeather;			
vector<tstring> CSStrings::m_vecStrGameWeatherSimple;	
vector<tstring> CSStrings::m_vecStrMotionFactor;			
vector<tstring> CSStrings::m_vecStrDamageAttrib;
vector<tstring> CSStrings::m_vecStrItemTier;
vector<tstring> CSStrings::m_vecStrItemType;
vector<tstring> CSStrings::m_vecStrWeaponType;
vector<tstring> CSStrings::m_vecStrArmorType;
vector<tstring> CSStrings::m_vecStrTalentStyle;
vector<tstring> CSStrings::m_vecStrSoulType;
vector<tstring> CSStrings::m_vecStrRace;
vector<tstring> CSStrings::m_vecStrSex;
vector<tstring> CSStrings::m_vecStrChatSoulType;
vector<tstring> CSStrings::m_vecStrEffectPoint;
vector<tstring> CSStrings::m_vecStrEffectRelation;
vector<tstring> CSStrings::m_vecStrWeaponReference;
vector<tstring> CSStrings::m_vecStrQuestObjectiveType;
vector<tstring> CSStrings::m_vecStrQuestShareType;
vector<tstring> CSStrings::m_vecStrGateType;
vector<tstring> CSStrings::m_vecStrWarpType;
vector<tstring> CSStrings::m_vecStrQuestTrialType;
vector<tstring> CSStrings::m_vecStrUsableType;
vector<tstring> CSStrings::m_vecStrConditionType;
vector<tstring> CSStrings::m_vecStrConditionQuestState;
vector<tstring> CSStrings::m_vecStrAAType;
vector<tstring> CSStrings::m_vecStrGatherType;
vector<tstring> CSStrings::m_vecStrResistType;
vector<tstring> CSStrings::m_vecStrResistMethod;
vector<tstring> CSStrings::m_vecStrQuestRewardType;
vector<tstring>	CSStrings::m_vecStrAttackableType;
vector<tstring>	CSStrings::m_vecStrFactionRelationType;
vector<tstring>	CSStrings::m_vecStrQOPublicProgress;
vector<tstring>	CSStrings::m_vecStrIElementType;


bool CSStrings::Init()
{
	m_vecStrGameTime.clear();
	m_vecStrGameTime.push_back(_T("Dawn"));
	m_vecStrGameTime.push_back(_T("Daytime"));
	m_vecStrGameTime.push_back(_T("Sunset"));
	m_vecStrGameTime.push_back(_T("Night"));
	m_vecStrGameTime.push_back(_T("Custom1"));
	m_vecStrGameTime.push_back(_T("Custom2"));
	if (MAX_TIME_TYPE != m_vecStrGameTime.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrGameWeather.clear();
	m_vecStrGameWeather.reserve(MAX_WEATHER_TYPE);
	m_vecStrGameWeather.push_back(_T("Sunny"));
	m_vecStrGameWeather.push_back(_T("Cloudy"));
	m_vecStrGameWeather.push_back(_T("Rainy"));
	m_vecStrGameWeather.push_back(_T("Heavy Rainy"));
	m_vecStrGameWeather.push_back(_T("Snowy"));
	m_vecStrGameWeather.push_back(_T("Heavy Snowy"));
	m_vecStrGameWeather.push_back(_T("Custom1"));
	m_vecStrGameWeather.push_back(_T("Custom2"));
	if (MAX_WEATHER_TYPE != m_vecStrGameWeather.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrGameWeatherSimple.clear();
	m_vecStrGameWeatherSimple.push_back(_T("SUN"));
	m_vecStrGameWeatherSimple.push_back(_T("CLD"));
	m_vecStrGameWeatherSimple.push_back(_T("RNY"));
	m_vecStrGameWeatherSimple.push_back(_T("HRN"));
	m_vecStrGameWeatherSimple.push_back(_T("SNW"));
	m_vecStrGameWeatherSimple.push_back(_T("HSW"));
	m_vecStrGameWeatherSimple.push_back(_T("CT1"));
	m_vecStrGameWeatherSimple.push_back(_T("CT2"));
	if (MAX_WEATHER_TYPE != m_vecStrGameWeatherSimple.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrMotionFactor.clear();
	m_vecStrMotionFactor.push_back(_T("beaten"));
	m_vecStrMotionFactor.push_back(_T("knockback"));
	m_vecStrMotionFactor.push_back(_T("stun"));
	m_vecStrMotionFactor.push_back(_T("deflect"));	
	m_vecStrMotionFactor.push_back(_T("throwup"));
	m_vecStrMotionFactor.push_back(_T("knockdown"));
	m_vecStrMotionFactor.push_back(_T("uppered"));
	m_vecStrMotionFactor.push_back(_T("fake_beaten"));
	m_vecStrMotionFactor.push_back(_T("fake_knockback"));
	m_vecStrMotionFactor.push_back(_T("grappled"));
	m_vecStrMotionFactor.push_back(_T("swallowed"));
	m_vecStrMotionFactor.push_back(_T("drag"));
	if (MF_SIZE != m_vecStrMotionFactor.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrDamageAttrib.clear();
	m_vecStrDamageAttrib.push_back(_T("none"));
	m_vecStrDamageAttrib.push_back(_T("slash"));
	m_vecStrDamageAttrib.push_back(_T("blunt"));
	m_vecStrDamageAttrib.push_back(_T("pierce"));
	m_vecStrDamageAttrib.push_back(_T("explosion"));
	m_vecStrDamageAttrib.push_back(_T("inertia"));
	m_vecStrDamageAttrib.push_back(_T("holy"));
	m_vecStrDamageAttrib.push_back(_T("unholy"));
	m_vecStrDamageAttrib.push_back(_T("fire"));
	m_vecStrDamageAttrib.push_back(_T("cold"));
	m_vecStrDamageAttrib.push_back(_T("lightning"));
	m_vecStrDamageAttrib.push_back(_T("poison"));
	if (DA_MAX != m_vecStrDamageAttrib.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrItemTier.clear();
	m_vecStrItemTier.push_back(_T("Very Common"));
	m_vecStrItemTier.push_back(_T("Common"));
	m_vecStrItemTier.push_back(_T("Rare"));
	m_vecStrItemTier.push_back(_T("Treasure"));
	m_vecStrItemTier.push_back(_T("Legendary"));
	m_vecStrItemTier.push_back(_T("Epic"));
	if (ITEM_TIER_MAX != m_vecStrItemTier.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrItemType.clear();
	m_vecStrItemType.push_back(_T("none"));
	m_vecStrItemType.push_back(_T("weapon"));
	m_vecStrItemType.push_back(_T("armor"));
	m_vecStrItemType.push_back(_T("housing"));
	m_vecStrItemType.push_back(_T("usable"));
	m_vecStrItemType.push_back(_T("ammunition"));
	m_vecStrItemType.push_back(_T("inventory"));	
	m_vecStrItemType.push_back(_T("gather"));
	m_vecStrItemType.push_back(_T("enchant"));
	m_vecStrItemType.push_back(_T("dye"));
	if (ITEMTYPE_MAX != m_vecStrItemType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrWeaponType.clear();
	m_vecStrWeaponType.push_back(_T("none"));
	m_vecStrWeaponType.push_back(_T("1hs"));
	m_vecStrWeaponType.push_back(_T("1hb"));
	m_vecStrWeaponType.push_back(_T("1hp"));
	m_vecStrWeaponType.push_back(_T("2hd"));
	m_vecStrWeaponType.push_back(_T("sta"));
	m_vecStrWeaponType.push_back(_T("arc"));
	m_vecStrWeaponType.push_back(_T("2hb"));
	m_vecStrWeaponType.push_back(_T("dwd"));
	m_vecStrWeaponType.push_back(_T("dwp"));
	if (WEAPON_MAX != m_vecStrWeaponType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrArmorType.clear();
	m_vecStrArmorType.push_back(_T("none"));
	m_vecStrArmorType.push_back(_T("cloth"));
	m_vecStrArmorType.push_back(_T("light"));
	m_vecStrArmorType.push_back(_T("medium"));
	m_vecStrArmorType.push_back(_T("heavy"));
	m_vecStrArmorType.push_back(_T("shield"));
	m_vecStrArmorType.push_back(_T("accessory"));
	m_vecStrArmorType.push_back(_T("amulet"));
	if (ARMOR_MAX != m_vecStrArmorType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrUsableType.clear();
	m_vecStrUsableType.push_back(_T("none"));
	m_vecStrUsableType.push_back(_T("talent_use"));
	m_vecStrUsableType.push_back(_T("talent_train"));
	m_vecStrUsableType.push_back(_T("talent_untrain"));
	m_vecStrUsableType.push_back(_T("interaction"));
	m_vecStrUsableType.push_back(_T("interaction_dead"));
	m_vecStrUsableType.push_back(_T("quest_add"));
	if (USABLE_MAX != m_vecStrUsableType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrTalentStyle.clear();
	m_vecStrTalentStyle.push_back(_T("none"));
	m_vecStrTalentStyle.push_back(_T("defender"));
	m_vecStrTalentStyle.push_back(_T("berserker"));
	m_vecStrTalentStyle.push_back(_T("assassin"));
	m_vecStrTalentStyle.push_back(_T("ranger"));
	m_vecStrTalentStyle.push_back(_T("sorcerer"));
	m_vecStrTalentStyle.push_back(_T("cleric"));
	m_vecStrTalentStyle.push_back(_T("common"));
	m_vecStrTalentStyle.push_back(_T("license"));
	if (TS_MAX != m_vecStrTalentStyle.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrSoulType.clear();
	m_vecStrSoulType.push_back(_T("none"));
	m_vecStrSoulType.push_back(_T("A"));
	m_vecStrSoulType.push_back(_T("B"));
	m_vecStrSoulType.push_back(_T("C"));
	if (SOULT_MAX != m_vecStrSoulType.size()) 
	{
		_ASSERT(0);
		return false;
	}
	
	m_vecStrRace.clear();
	m_vecStrRace.reserve(RACE_MAX);
	m_vecStrRace.push_back(_T("human"));
	m_vecStrRace.push_back(_T("volo"));
	m_vecStrRace.push_back(_T("nebit"));
	if (RACE_MAX != m_vecStrRace.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrSex.clear();
	m_vecStrSex.reserve(SEX_MAX);
	m_vecStrSex.push_back(_T("male"));
	m_vecStrSex.push_back(_T("female"));
	m_vecStrSex.push_back(_T("sexless"));
	if (SEX_MAX != m_vecStrSex.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrChatSoulType.clear();
	m_vecStrChatSoulType.push_back(_T(" "));
	m_vecStrChatSoulType.push_back(tstring(_T("고급")));
	m_vecStrChatSoulType.push_back(tstring(_T("중급")));
	m_vecStrChatSoulType.push_back(tstring(_T("저급")));
	if (SOULT_MAX != m_vecStrChatSoulType.size()) 
	{
		_ASSERT(0);
		return false;
	}
	
	m_vecStrEffectPoint.clear();
	m_vecStrEffectPoint.push_back(_T("none"));
	m_vecStrEffectPoint.push_back(_T("self"));
	m_vecStrEffectPoint.push_back(_T("target"));
	m_vecStrEffectPoint.push_back(_T("caster"));
	m_vecStrEffectPoint.push_back(_T("hitcapsule"));
	m_vecStrEffectPoint.push_back(_T("areapoint"));
	if (CSEffectInfo::POINT_MAX != m_vecStrEffectPoint.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrEffectRelation.clear();
	m_vecStrEffectRelation.push_back(_T("none"));
	m_vecStrEffectRelation.push_back(_T("all"));
	m_vecStrEffectRelation.push_back(_T("enemy"));
	m_vecStrEffectRelation.push_back(_T("allied"));
	m_vecStrEffectRelation.push_back(_T("party"));
	m_vecStrEffectRelation.push_back(_T("allied_dead"));
	m_vecStrEffectRelation.push_back(_T("self"));
	m_vecStrEffectRelation.push_back(_T("caster_enemy"));
	if (CSEffectInfo::RELATION_MAX != m_vecStrEffectRelation.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrWeaponReference.clear();
	m_vecStrWeaponReference.push_back(_T("none"));
	m_vecStrWeaponReference.push_back(_T("left"));
	m_vecStrWeaponReference.push_back(_T("right"));
	m_vecStrWeaponReference.push_back(_T("both"));
	if (WR_MAX != m_vecStrWeaponReference.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrQuestObjectiveType.clear();
	m_vecStrQuestObjectiveType.push_back(_T("none"));
	m_vecStrQuestObjectiveType.push_back(_T("destroy"));
	m_vecStrQuestObjectiveType.push_back(_T("collect"));
	m_vecStrQuestObjectiveType.push_back(_T("act"));
	m_vecStrQuestObjectiveType.push_back(_T("scout"));
	m_vecStrQuestObjectiveType.push_back(_T("interact"));
	m_vecStrQuestObjectiveType.push_back(_T("escort"));
	if (QOT_MAX != m_vecStrQuestObjectiveType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrQuestShareType.clear();
	m_vecStrQuestShareType.push_back(_T("none"));
	m_vecStrQuestShareType.push_back(_T("manual"));
	m_vecStrQuestShareType.push_back(_T("auto"));
	if (QSHAT_MAX != m_vecStrQuestShareType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrGateType.clear();
	m_vecStrGateType.push_back(_T("auto"));
	m_vecStrGateType.push_back(_T("manual"));	
	if (TGT_MAX != m_vecStrGateType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrWarpType.clear();
	m_vecStrWarpType.push_back(_T("none"));
	m_vecStrWarpType.push_back(_T("field"));
	m_vecStrWarpType.push_back(_T("innroom"));	
	if (WT_MAX != m_vecStrWarpType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrQuestTrialType.clear();
	m_vecStrQuestTrialType.push_back(_T("party"));
	m_vecStrQuestTrialType.push_back(_T("single"));
	if (QTT_MAX != m_vecStrQuestTrialType.size())	
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrConditionType.clear();
	m_vecStrConditionType.reserve(CT_MAX);
	m_vecStrConditionType.push_back(_T("none"));
	m_vecStrConditionType.push_back(_T("level"));
	m_vecStrConditionType.push_back(_T("quest"));	
	m_vecStrConditionType.push_back(_T("quest_var"));
	m_vecStrConditionType.push_back(_T("quest_objective_complete"));
	m_vecStrConditionType.push_back(_T("quest_objective_incomplete"));
	m_vecStrConditionType.push_back(_T("item"));
	m_vecStrConditionType.push_back(_T("noitem"));
	m_vecStrConditionType.push_back(_T("fewitem"));
	m_vecStrConditionType.push_back(_T("manyitem"));
	m_vecStrConditionType.push_back(_T("equip"));
	m_vecStrConditionType.push_back(_T("race"));
	m_vecStrConditionType.push_back(_T("sex"));
	m_vecStrConditionType.push_back(_T("faction"));
	m_vecStrConditionType.push_back(_T("title"));
	m_vecStrConditionType.push_back(_T("date"));
	m_vecStrConditionType.push_back(_T("talent"));
	m_vecStrConditionType.push_back(_T("talent_style_tp"));
	m_vecStrConditionType.push_back(_T("kill"));
	m_vecStrConditionType.push_back(_T("qpvp_team"));
	if (CT_MAX != m_vecStrConditionType.size()) 
	{
		_ASSERT(0);
		return false;
	}
	
	m_vecStrConditionQuestState.clear();
	m_vecStrConditionQuestState.reserve(CQS_MAX);
	m_vecStrConditionQuestState.push_back(_T("none"));
	m_vecStrConditionQuestState.push_back(_T("notexist"));
	m_vecStrConditionQuestState.push_back(_T("exist"));
	m_vecStrConditionQuestState.push_back(_T("notcomplete"));
	m_vecStrConditionQuestState.push_back(_T("complete"));
	m_vecStrConditionQuestState.push_back(_T("notdone"));
	m_vecStrConditionQuestState.push_back(_T("done"));
	m_vecStrConditionQuestState.push_back(_T("active"));
	if (CQS_MAX != m_vecStrConditionQuestState.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrAAType.clear();
	m_vecStrAAType.reserve(NAAT_MAX);
	m_vecStrAAType.push_back(_T("none"));
	m_vecStrAAType.push_back(_T("always"));
	m_vecStrAAType.push_back(_T("faction"));
	if (NAAT_MAX != m_vecStrAAType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	// 채집
	m_vecStrGatherType.clear();
	m_vecStrGatherType.reserve(MAX_GATHER_TYPE);	
	m_vecStrGatherType.push_back(_T("mine"));
	m_vecStrGatherType.push_back(_T("herb"));
	m_vecStrGatherType.push_back(_T("relic"));
	m_vecStrGatherType.push_back(_T("lumber"));	
	if (MAX_GATHER_TYPE != m_vecStrGatherType.size()) 
	{
		_ASSERT(0);
		return false;
	}
	
	// 저항타입
	m_vecStrResistType.clear();
	m_vecStrResistType.reserve(TRT_MAX);
	m_vecStrResistType.push_back(_T("FR"));
	m_vecStrResistType.push_back(_T("CR"));
	m_vecStrResistType.push_back(_T("PR"));
	m_vecStrResistType.push_back(_T("LR"));
	m_vecStrResistType.push_back(_T("HR"));
	m_vecStrResistType.push_back(_T("UR"));
	m_vecStrResistType.push_back(_T("CHA"));
	if (TRT_MAX != m_vecStrResistType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	// 저항메소드
	m_vecStrResistMethod.clear();
	m_vecStrResistMethod.reserve(TRM_MAX);
	m_vecStrResistMethod.push_back(_T("binary"));
	m_vecStrResistMethod.push_back(_T("partial"));
	if (TRM_MAX != m_vecStrResistMethod.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrQuestRewardType.clear();
	m_vecStrQuestRewardType.reserve(QRT_MAX);
	m_vecStrQuestRewardType.push_back(_T("none"));
	m_vecStrQuestRewardType.push_back(_T("SILVER"));
	m_vecStrQuestRewardType.push_back(_T("XP"));
	m_vecStrQuestRewardType.push_back(_T("TITLE"));
	m_vecStrQuestRewardType.push_back(_T("ITEM"));
	m_vecStrQuestRewardType.push_back(_T("FACTION"));	
	if (QRT_MAX != m_vecStrQuestRewardType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrAttackableType.clear();
	m_vecStrAttackableType.reserve(NAT_MAX);
	m_vecStrAttackableType.push_back(_T("none"));
	m_vecStrAttackableType.push_back(_T("always"));
	m_vecStrAttackableType.push_back(_T("faction"));
	if (NAT_MAX != m_vecStrAttackableType.size()) 
	{
		_ASSERT(0);
		return false;
	}


	m_vecStrFactionRelationType.clear();
	m_vecStrFactionRelationType.reserve(FRT_MAX);
	m_vecStrFactionRelationType.push_back(_T("none"));
	m_vecStrFactionRelationType.push_back(_T("WORST"));
	m_vecStrFactionRelationType.push_back(_T("BAD"));
	m_vecStrFactionRelationType.push_back(_T("NORMAL"));
	m_vecStrFactionRelationType.push_back(_T("GOOD"));
	m_vecStrFactionRelationType.push_back(_T("EXCELLENT"));
	m_vecStrFactionRelationType.push_back(_T("TRUST"));
	if (FRT_MAX != m_vecStrFactionRelationType.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrQOPublicProgress.clear();
	m_vecStrQOPublicProgress.reserve(QPP_MAX);
	m_vecStrQOPublicProgress.push_back(_T("none"));
	m_vecStrQOPublicProgress.push_back(_T("self"));
	m_vecStrQOPublicProgress.push_back(_T("party"));
	m_vecStrQOPublicProgress.push_back(_T("sector"));
	if (QPP_MAX != m_vecStrQOPublicProgress.size()) 
	{
		_ASSERT(0);
		return false;
	}

	m_vecStrIElementType.clear();
	m_vecStrIElementType.reserve(IT_MAX);
	m_vecStrIElementType.push_back(_T("none"));	
	m_vecStrIElementType.push_back(_T("auction"));
	m_vecStrIElementType.push_back(_T("mailbox"));
	m_vecStrIElementType.push_back(_T("storage"));
	m_vecStrIElementType.push_back(_T("soldtrade"));
	m_vecStrIElementType.push_back(_T("inn_sleep"));
	m_vecStrIElementType.push_back(_T("inn_save"));
	m_vecStrIElementType.push_back(_T("trade_market"));
	m_vecStrIElementType.push_back(_T("trigger"));
	m_vecStrIElementType.push_back(_T("guild_destroy"));
	m_vecStrIElementType.push_back(_T("guild_create"));
	m_vecStrIElementType.push_back(_T("guild_storage"));
	m_vecStrIElementType.push_back(_T("quest_count"));
	m_vecStrIElementType.push_back(_T("quest_count_despawn"));		
	// m_vecStrIElementType.push_back(_T("soulbinding"));
	m_vecStrIElementType.push_back(_T("challenger_quest"));
	m_vecStrIElementType.push_back(_T("gather_1"));
	m_vecStrIElementType.push_back(_T("gather_2"));
	m_vecStrIElementType.push_back(_T("gather_3"));
	m_vecStrIElementType.push_back(_T("gather_4"));
	m_vecStrIElementType.push_back(_T("gather_5"));
	m_vecStrIElementType.push_back(_T("gather_6"));
	m_vecStrIElementType.push_back(_T("gather_7"));
	m_vecStrIElementType.push_back(_T("gather_8"));
	m_vecStrIElementType.push_back(_T("gather_9"));
	m_vecStrIElementType.push_back(_T("gather_10"));
	m_vecStrIElementType.push_back(_T("loot"));
	m_vecStrIElementType.push_back(_T("loot_despawn"));
	m_vecStrIElementType.push_back(_T("dialog"));
	m_vecStrIElementType.push_back(_T("dialog_for_help"));
	m_vecStrIElementType.push_back(_T("trade"));	
	m_vecStrIElementType.push_back(_T("craft"));	
	m_vecStrIElementType.push_back(_T("quest_begin"));
	m_vecStrIElementType.push_back(_T("quest_end"));	
	m_vecStrIElementType.push_back(_T("enchant"));
	m_vecStrIElementType.push_back(_T("pc_talent"));
	m_vecStrIElementType.push_back(_T("pc_talent_despawn"));
	m_vecStrIElementType.push_back(_T("pc_buff"));
	m_vecStrIElementType.push_back(_T("pc_buff_despawn"));
	m_vecStrIElementType.push_back(_T("script"));
	m_vecStrIElementType.push_back(_T("script_despawn"));
	m_vecStrIElementType.push_back(_T("hellhound"));
	m_vecStrIElementType.push_back(_T("skillset"));
	m_vecStrIElementType.push_back(_T("beautyshop"));
	m_vecStrIElementType.push_back(_T("element_enchant"));
	if (IT_MAX != m_vecStrIElementType.size()) 
	{
		_ASSERT(0);
		return false;	
	}


	return true;
}

bool CSStrings::IsString$$(const TCHAR* szValue)
{
	TCHAR* szValue2 = const_cast<TCHAR*>(szValue);	

	if (NULL ==  _tcsstr(szValue2, _T("$$"))) return false;

	return true;
}

tstring CSStrings::StringToLower(const tstring& strValue)
{
	tstring strLower = strValue;

	for (tstring::size_type i = 0 ; i < strValue.length() ; ++i)
		strLower[i] = tolower(strValue[i]);

	return (strLower);
}

int CSStrings::StringToEnum(const TCHAR* szValue, const vector<tstring>& vecStrEnum)
{
	int nSize = static_cast<int>(vecStrEnum.size());

	tstring strValue = StringToLower(szValue);

	for (int i = 0; i < nSize; i++)
	{
		if	(!_tcsicmp(strValue.c_str(), vecStrEnum[i].c_str()))
		{ 
			return i;
		}
	}

	return -1;
}

const TCHAR* CSStrings::EnumToString(const int nEnum, const vector<tstring>& vecStrEnum)
{
	if (nEnum < 0 || nEnum >= static_cast<int>(vecStrEnum.size()))
	{
		_ASSERT(0);
		return _T("");
	}

	return vecStrEnum[nEnum].c_str();
}

bool CSStrings::StringToBool(const TCHAR* szValue)
{
	tstring strValue = szValue;
	tstring strLower = StringToLower(strValue);

	if (0 == _tcsicmp(_T("true"), strLower.c_str()))
	{
		return true;
	}
	else if (0 == _tcsicmp(_T("false"), strLower.c_str()))
	{
		return false;
	}

	_ASSERT(0);
	return false;
}

void CSStrings::StringToFieldIDAndMarerID(const TCHAR* szValue, int& outnFieldID, DWORD& outdwMakrerID)
{
	TCHAR* szValue2 = const_cast<TCHAR*>(szValue);
	TCHAR* nextToken;

	if (NULL !=  _tcsstr(szValue2, _T(",")))
	{
		TCHAR* szFieldID = _tcstok_s(szValue2, _T(","), &nextToken);
		outnFieldID = _ttoi(szFieldID);
		TCHAR* szMakrerID = _tcstok_s(NULL, _T(","), &nextToken);
		outdwMakrerID = _ttoi(szMakrerID);		
	}
}

void CSStrings::StringToFieldIDAndVec2(const TCHAR* szValue, int& outnFieldID, vec3& vecPos)
{
	// format	[field_id](x,y)

	TCHAR* szValue2 = const_cast<TCHAR*>(szValue);
	TCHAR* nextToken;

	// 약식파싱
	TCHAR* szFieldId = _tcstok_s(szValue2, _T("[](,) "), &nextToken);
	TCHAR* szX = _tcstok_s(NULL, _T("[](,) "), &nextToken);
	TCHAR* szY = _tcstok_s(NULL, _T("[](,) "), &nextToken);

	// 최소 3개의 요소가 확인되어야 적용합니다.
	if (szFieldId != NULL && szX != NULL && szY != NULL)
	{
		outnFieldID = _ttoi(szFieldId);
		vecPos.Set((float)_tstof(szX), (float)_tstof(szY), 0.0f);
	}
}

void CSStrings::StringToFieldIDAndNPCID( const TCHAR* szValue, int& outnFieldID, int& outnNPCID )
{
	TCHAR* szValue2 = const_cast<TCHAR*>(szValue);
	TCHAR* nextToken;

	if (NULL !=  _tcsstr(szValue2, _T(",")))
	{
		TCHAR* szFieldID = _tcstok_s(szValue2, _T(","), &nextToken);
		outnFieldID = _ttoi(szFieldID);
		TCHAR* szMakrerID = _tcstok_s(NULL, _T(","), &nextToken);
		outnNPCID = _ttoi(szMakrerID);
	}
}

void CSStrings::StringsToStringVector(const TCHAR* szValue, vector<tstring>& vecOut)
{
	TCHAR* szValue2 = const_cast<TCHAR*>(szValue);
	TCHAR* nextToken;

	if (NULL !=  _tcsstr(szValue2, _T(",")))
	{
		TCHAR* szItemID = _tcstok_s(szValue2, _T(","), &nextToken);
		while (NULL != szItemID)
		{
			vecOut.push_back(szItemID);
			szItemID = _tcstok_s(NULL, _T(","), &nextToken);
		}
	}
	else
	{
		vecOut.push_back(szValue2);
	}
}

void CSStrings::StringNumbersToIntVector(const tstring& strValue, vector<int>& vecOut)
{
	if (true == strValue.empty()) return;

	TCHAR* szValue = const_cast<TCHAR*>(strValue.c_str());
	TCHAR* nextToken;

	bool bExistTilde = (NULL == _tcsstr(strValue.c_str(), _T("~"))) ? false : true;
	bool bExistComma = (NULL == _tcsstr(strValue.c_str(), _T(","))) ? false : true;

	if (true == bExistTilde)
	{		
		TCHAR* szItemID = _tcstok_s(szValue, _T("~"), &nextToken);
		while (NULL != szItemID)
		{
			vecOut.push_back(_ttoi(szItemID));
			szItemID = _tcstok_s(NULL, _T("~"), &nextToken);
		}

		if (2 != vecOut.size())
		{
			_ASSERT(0);
			return;
		}

		int nBeingNumber = vecOut[0];
		int nEndNumber = vecOut[1];

		vecOut.clear();
		for (int i = nBeingNumber; nEndNumber >= i; i++)
		{
			vecOut.push_back(i);
		}
	}
	else if (true == bExistComma)
	{
		TCHAR* szItemID = _tcstok_s(szValue, _T(","), &nextToken);
		while (NULL != szItemID)
		{
			vecOut.push_back(_ttoi(szItemID));
			szItemID = _tcstok_s(NULL, _T(","), &nextToken);
		}
	}
	else
	{
		vecOut.push_back(_ttoi(szValue));
	}	
}

void CSStrings::IntVectorToStringNumbers(const vector<int>& vecInt, tstring& outstrValue)
{
	outstrValue = _T("");
	if (true == vecInt.empty()) return;

	TCHAR szNumber[512];

	_itot_s(vecInt[0], szNumber, 512, 10);
	outstrValue += szNumber;

	size_t nVecIntSize = vecInt.size();
	for (size_t i = 1; nVecIntSize > i; i++)
	{
		_itot_s(vecInt[i], szNumber, 512, 10);

		outstrValue += _T(",");
		outstrValue += szNumber;
	}
}

const TCHAR* CSStrings::WeaponType(int nWeaponType)
{
	return EnumToString(nWeaponType, m_vecStrWeaponType);
}

const TCHAR* CSStrings::GameTime(int nGameTime)
{
	return EnumToString(nGameTime, m_vecStrGameTime);
}

const TCHAR* CSStrings::Weather(int  nWeather)
{
	return EnumToString(nWeather, m_vecStrGameWeather);
}

const TCHAR* CSStrings::WeatherSimple(int  nWeather)
{
	return EnumToString(nWeather, m_vecStrGameWeatherSimple);
}

const TCHAR* CSStrings::MotionFactor(int  nMF)
{
	return EnumToString(nMF, m_vecStrMotionFactor);
}

const TCHAR* CSStrings::Sex( SEX nSex )
{
	return EnumToString(nSex, m_vecStrSex);
}

int CSStrings::StringToDamageAttrib(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrDamageAttrib);
}

void CSStrings::StringToDamageAttrib(const TCHAR* szValue, tstring& outParamValue)
{
	TCHAR szBuff[512];
	int nValue = StringToDamageAttrib(szValue);	

	if (nValue != -1)
	{
		_itot(nValue, szBuff, 10);
		outParamValue = szBuff;
	}
	else
	{
		outParamValue = _T("");
	}
}

int CSStrings::StringToMotionfactorType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrMotionFactor);
}

void CSStrings::StringToMotionfactorType(const TCHAR* szValue, tstring& outParamValue)
{
	TCHAR szBuff[512];
	int nValue = -1;

	nValue = StringToMotionfactorType(szValue);

	if (nValue != -1)
	{
		_itot(nValue, szBuff, 10);
		outParamValue = szBuff;
	}
	else
	{
		outParamValue = _T("");
	}
}

int CSStrings::StringToItemType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrItemType);
}

int CSStrings::StringToItemTier( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrItemTier);
}

int CSStrings::StringToWeaponType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrWeaponType);
}

int CSStrings::StringToArmorType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrArmorType);
}

int CSStrings::StringToUsableType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrUsableType);
}

int CSStrings::StringToTalentStyle(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrTalentStyle);
}

int CSStrings::StringToSoulType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrSoulType);
}

int CSStrings::StringToRace(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrRace);
}

int CSStrings::StringToSex(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrSex);
}

int CSStrings::StringToEffectPoint( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrEffectPoint);
}

int CSStrings::StringToEffectRelation( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrEffectRelation);
}

int CSStrings::StringToWeapnReference( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrWeaponReference);
}

int CSStrings::StringToQuestObjectiveType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrQuestObjectiveType);
}

int CSStrings::StringToIElementType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrIElementType);
}

int CSStrings::StringToQuestShareType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrQuestShareType);
}

int CSStrings::StringToGateType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrGateType);
}

int CSStrings::StringToWarpType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrWarpType);
}

int CSStrings::StringToQuestTrialType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrQuestTrialType);
}

const TCHAR* CSStrings::ItemTypeToString( int itemType )
{
	return (m_vecStrItemType[itemType]).c_str();
}

const TCHAR* CSStrings::ItemTierToString( int nItemTier )
{
	return (m_vecStrItemTier[nItemTier]).c_str();
}

const TCHAR* CSStrings::ArmorTypeToString(int nArmorType)
{
	return (m_vecStrArmorType[nArmorType]).c_str();
}

const TCHAR* CSStrings::SoulTypeToString(int nSoulType)
{
	return (m_vecStrSoulType[nSoulType]).c_str();
}

void CSStrings::StringsToItemTypeVector(const TCHAR* szValues, vector<int>& outvecItemType)
{
	vector<tstring> vecString;
	StringsToStringVector(szValues, vecString);

	for each (const tstring& strString in vecString)
	{		
		outvecItemType.push_back(StringToItemType(strString.c_str()));
	}
}

void CSStrings::StringsToWeaponAndArmorTypeVector(const TCHAR* szValues, vector<int>& outvecWeaponType, vector<int>& outvecEquipmentType)
{
	vector<tstring> vecString;
	StringsToStringVector(szValues, vecString);

	for each (const tstring& strString in vecString)
	{
		int nWeaponType = StringToWeaponType(strString.c_str());
		int nEquipmentType = StringToArmorType(strString.c_str());

		if (0 != nWeaponType)
		{
			outvecWeaponType.push_back(nWeaponType);
		}
		else if (0 != nEquipmentType)
		{
			outvecEquipmentType.push_back(nEquipmentType);
		}		
	}
}

tstring CSStrings::XmlEscape(tstring const& value)
{
	tstring escaped = value;

	ReplaceChar(escaped, _T('&'), _T("&amp;"));
	ReplaceChar(escaped, _T('<'), _T("&lt;"));
	ReplaceChar(escaped, _T('>'), _T("&gt;"));
	ReplaceChar(escaped, _T('\''), _T("&apos;"));
	ReplaceChar(escaped, _T('\"'), _T("&quot;"));

	return escaped;	
}

void CSStrings::ReplaceChar(tstring& str, TCHAR const c, tstring const& replacement)
{
	for (size_t pos = str.find(c); pos != tstring::npos; pos = str.find(c, pos + 1))
		str.replace(pos, 1, replacement);
}

const TCHAR* CSStrings::Stance( int nStance )
{
	if (nStance == CS_NORMAL) return _T("normal");
	else if (nStance == CS_BATTLE) return _T("battle");
	return _T("");
}

const TCHAR* CSStrings::SoulTypeToChatString(int nSoulType)
{
	return EnumToString(nSoulType, m_vecStrChatSoulType);
}

const TCHAR* CSStrings::TalentStyleToString( int nValue )
{
	return EnumToString(nValue, m_vecStrTalentStyle);
}

const TCHAR* CSStrings::QuestTiralTypeToString(int nTrialType)
{
	return EnumToString(nTrialType, m_vecStrQuestTrialType);
}

const TCHAR*	CSStrings::GateTypeToString(int nGateType)
{
	return EnumToString(nGateType, m_vecStrGateType);
}

const TCHAR* CSStrings::QuestRewardTypeToString( int nType )
{
	return EnumToString(nType, m_vecStrQuestRewardType);
}

const TCHAR* CSStrings::FacionRelationTypeToString( int nType )
{
	return EnumToString(nType, m_vecStrFactionRelationType);
}

int CSStrings::StringToConditionType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrConditionType);
}

int CSStrings::StringToConditionQuestState(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrConditionQuestState);
}

int CSStrings::StringToAAType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrAAType);
}

int CSStrings::StringToGatherType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrGatherType);
}

const TCHAR* CSStrings::GatherTypeToString(int nGatherType)
{
	return EnumToString(nGatherType, m_vecStrGatherType);
}

int CSStrings::StringToResistType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrResistType);
}

const TCHAR* CSStrings::ResistTypeToString(int nResistType)
{
	return EnumToString(nResistType, m_vecStrResistType);
}

int CSStrings::StringToResistMethod(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrResistMethod);
}

int CSStrings::StringToQuestRewardType( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrQuestRewardType);
}

int CSStrings::StringToFactionRelationType( const TCHAR* szValue )
{
	int nResult = StringToEnum(szValue, m_vecStrFactionRelationType);
	if (-1 != nResult) return nResult;

	return _tstoi(szValue);
}

int CSStrings::StringToQOPublicProgress( const TCHAR* szValue )
{
	return StringToEnum(szValue, m_vecStrQOPublicProgress);
}

const TCHAR* CSStrings::ResistMethodToString(int nResistMethod)
{
	return EnumToString(nResistMethod, m_vecStrResistMethod);
}

int CSStrings::StringToAttackableType(const TCHAR* szValue)
{
	return StringToEnum(szValue, m_vecStrAttackableType);
}

wstring CSStrings::Plural(wstring strSingular)
{
	// TODO: 단어사전 추가해야합니다.  [3/18/2011 praptor]
	map<wstring, wstring> mapDictionary;
	mapDictionary.insert(make_pair(L"wolf", L"wolves"));


	map<wstring, wstring>::iterator it = mapDictionary.find(strSingular);
	if (it == mapDictionary.end())
	{
		return wstring();
	}


	return it->second;
}