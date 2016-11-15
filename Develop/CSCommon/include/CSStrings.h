#ifndef _CS_STRINGS_H
#define _CS_STRINGS_H

#include <string>
#include "MTstring.h"
#include <vector>
using namespace std;

#include "MTypes.h"
#include "CSCommonLib.h"
#include "CSChrInfo.h"


// 공용으로 사용하는 스트링 모음
class CSCOMMON_API CSStrings
{
protected:
	static vector<tstring>	m_vecStrGameTime;				///< 시간
	static vector<tstring>	m_vecStrGameWeather;			///< 날씨
	static vector<tstring>	m_vecStrGameWeatherSimple;		///< 날씨
	static vector<tstring>	m_vecStrMotionFactor;			///< 모션팩터
	static vector<tstring>	m_vecStrDamageAttrib;			///< 데미지타입
	static vector<tstring>	m_vecStrItemTier;				///< 아이템 등급
	static vector<tstring>	m_vecStrItemType;				///< 아이템 타입
	static vector<tstring>	m_vecStrWeaponType;				///< 무기 타입
	static vector<tstring>	m_vecStrArmorType;				///< 방어구 타입
	static vector<tstring>	m_vecStrUsableType;				///< 사용성 아이템 타입
	static vector<tstring>	m_vecStrTalentStyle;			///< 탤런트 스타일
	static vector<tstring>	m_vecStrSoulType;				///< 영혼 종류			
	static vector<tstring>	m_vecStrRace;					///< 종족 
	static vector<tstring>	m_vecStrSex;					///< 성별
	static vector<tstring>	m_vecStrEffectPoint;			///< 효과 위치
	static vector<tstring>	m_vecStrEffectRelation;		///< 효과 대상자의 관계
	static vector<tstring>	m_vecStrWeaponReference;		///< 탤런트 사용할때 참조되는 무기
	static vector<tstring>	m_vecStrQuestObjectiveType;		///< 퀘스트 목적 타입
	static vector<tstring>	m_vecStrQuestShareType;			///< 퀘스트 공유 타입
	static vector<tstring>	m_vecStrGateType;				///< 트라이얼 필드 게이트 타입
	static vector<tstring>	m_vecStrWarpType;				///< 워프 타입
	static vector<tstring>	m_vecStrQuestTrialType;			///< 퀘스트 트라이얼 타입
	static vector<tstring>	m_vecStrAAType;
	static vector<tstring>	m_vecStrQuestRewardType;
	static vector<tstring>	m_vecStrFactionRelationType;	///< 팩션 관계 타입
	static vector<tstring>	m_vecStrQOPublicProgress;		///< 퀘스트 목적갱신 적용대상
	static vector<tstring>	m_vecStrIElementType;			///< 인터랙션 타입

	
	
	
	static vector<tstring>	m_vecStrChatSoulType;

	static vector<tstring>	m_vecStrConditionType;
	static vector<tstring>	m_vecStrConditionQuestState;	///< 퀘스트 상태

	static vector<tstring>	m_vecStrGatherType;				///< 채집 타입
	
	static vector<tstring>	m_vecStrResistType;				///< 저항 타입
	static vector<tstring>	m_vecStrResistMethod;			///< 저항 메소드
	static vector<tstring>	m_vecStrAttackableType;

protected:
	static int			StringToEnum(const TCHAR* szValue, const vector<tstring>& vecStrEnum);	
	static const TCHAR*	EnumToString(const int nEnum, const vector<tstring>& vecStrEnum);

public:
	static bool			Init();

	static bool			IsString$$(const TCHAR* szValue);

	static tstring		StringToLower(const tstring& strValue);

	static bool			StringToBool(const TCHAR* szValue);
	static void			StringToFieldIDAndMarerID(const TCHAR* szValue, int& outnFieldID, DWORD& outdwMakrerID);
	static void			StringToFieldIDAndVec2(const TCHAR* szValue, int& outnFieldID, vec3& vecPos);
	static void			StringToFieldIDAndNPCID(const TCHAR* szValue, int& outnFieldID, int& outnNPCID);
	static void			StringsToStringVector(const TCHAR* szValue, vector<tstring>& vecOut);
	static void			StringNumbersToIntVector(const tstring& strValue, vector<int>& vecOut);
	static void			IntVectorToStringNumbers(const vector<int>& vecInt, tstring& outstrValue);	

	static const TCHAR*	WeaponType(int nWeaponType);
	static const TCHAR*	GameTime(int nGameTime);
	static const TCHAR*	Weather(int nWeather);
	static const TCHAR*	WeatherSimple(int nWeather);
	static const TCHAR*	MotionFactor(int  nMF);
	static const TCHAR*	Stance(int nStance);
	static const TCHAR*	Sex(SEX nSex);

	static int			StringToDamageAttrib(const TCHAR* szValue);
	static void			StringToDamageAttrib(const TCHAR* szValue, tstring& outParamValue);
	static int			StringToMotionfactorType(const TCHAR* szValue);
	static void			StringToMotionfactorType(const TCHAR* szValue, tstring& outParamValue);	

	static int			StringToItemType(const TCHAR* szValue);
	static int			StringToItemTier(const TCHAR* szValue);
	static int			StringToWeaponType(const TCHAR* szValue);
	static int			StringToArmorType(const TCHAR* szValue);
	static int			StringToUsableType(const TCHAR* szValue);
	static int			StringToTalentStyle(const TCHAR* szValue);
	static int			StringToSoulType(const TCHAR* szValue);		
	static int			StringToRace(const TCHAR* szValue);
	static int			StringToSex(const TCHAR* szValue);	
	static int			StringToEffectPoint(const TCHAR* szValue);	
	static int			StringToEffectRelation(const TCHAR* szValue);
	static int			StringToWeapnReference(const TCHAR* szValue);
	static int			StringToQuestObjectiveType(const TCHAR* szValue);
	static int			StringToQuestShareType(const TCHAR* szValue);
	static int			StringToGateType(const TCHAR* szValue);
	static int			StringToWarpType(const TCHAR* szValue);
	static int			StringToQuestTrialType(const TCHAR* szValue);
	static int			StringToAAType(const TCHAR* szValue);
	static int			StringToQuestRewardType(const TCHAR* szValue);
	static int			StringToFactionRelationType(const TCHAR* szValue);
	static int			StringToQOPublicProgress(const TCHAR* szValue);
	static int			StringToIElementType(const TCHAR* szValue);	

	static const TCHAR*	ItemTypeToString(int itemType);
	static const TCHAR*	ItemTierToString(int nItemTier);
	static const TCHAR*	ArmorTypeToString(int nArmorType);
	static const TCHAR*	SoulTypeToString(int nSoulType);	
	static const TCHAR*	TalentStyleToString(int nValue);
	static const TCHAR*  QuestTiralTypeToString(int nTrialType);
	static const TCHAR*	GateTypeToString(int nGateType);
	static const TCHAR*	QuestRewardTypeToString(int nType);
	static const TCHAR*	FacionRelationTypeToString(int nType);	



	static void			StringsToItemTypeVector(const TCHAR* szValues, vector<int>& outvecItemType);	
	static void			StringsToWeaponAndArmorTypeVector(const TCHAR* szValues, vector<int>& outvecWeaponType, vector<int>& outvecEquipmentType);

	static tstring		XmlEscape(tstring const& value);

	static void			ReplaceChar(tstring& str, TCHAR const c, tstring const& replacement);

	static const TCHAR*	SoulTypeToChatString(int nSoulType);

	// condition
	static int			StringToConditionType(const TCHAR* szValue);
	static int			StringToConditionQuestState(const TCHAR* szValue);	

	// 채집
	static int StringToGatherType(const TCHAR* szValue);
	static const TCHAR* GatherTypeToString(int nGatherType);

	// 저항타입
	static int StringToResistType(const TCHAR* szValue);
	static const TCHAR* ResistTypeToString(int nResistType);

	// 저항메소드
	static int StringToResistMethod(const TCHAR* szValue);
	static const TCHAR* ResistMethodToString(int nResistMethod);

	static int			StringToAttackableType(const TCHAR* szValue);

	// 영어단어 복수형
	static wstring		Plural(wstring strSingular);
};


#endif
