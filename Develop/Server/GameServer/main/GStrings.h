#ifndef _GSTRINGS_H
#define _GSTRINGS_H

#include "CSStrings.h"

class GStrings : public CSStrings
{
private:
	static vector<wstring>	m_vecStrColtCondition;
	static vector<wstring>	m_vecStrColtAction;
	static vector<wstring>	m_vecStrBehaviorState;
	static vector<wstring>	m_vecStrCombatState;
	static vector<wstring>	m_vecStrTalentSkillType;	
	static vector<wstring>	m_vecStrFieldType;
	static vector<wstring>	m_vecStrInnRoomType;
	static vector<wstring>	m_vecStrColtCheckType;
	static vector<wstring>	m_vecStrColtActionType;
	static vector<wstring>	m_vecStrFactionRelationType;	
	static vector<wstring>	m_vecStrITargetConditionType;
	

public:
	static bool			Init();

	static int			StringToBehaviorState(const wchar_t* szValue);
	static int			StringToCombatState(const wchar_t* szValue);		
	static int			StringToFieldType(const wchar_t* szValue);
	static int			StringToInnRoomType(const wchar_t* szValue);	
	static int			StringToFactionRelationType(const wchar_t* szValue);
	static int			StringToITargetConditionType(const wchar_t* szValue);
	static int			StringToColtConditionState(const wchar_t* szValue);

	static const wchar_t*	BehaviorStateToString(int nValue);
	static const wchar_t*	CombatStateToString(int nValue);
	static const wchar_t*	ColtConditionStateToString(int nValue);
	static const wchar_t*	ColtActionToString(int nValue);
	static const wchar_t*	TalentSkillTypeToString(int nValue);	
	static const wchar_t*	ConditionTypeToString(int nValue);

	static float		CalcSpeakMaintainTime(const wchar_t* szText);
};




#endif // _GSTRINGS_H