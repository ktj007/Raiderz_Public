#pragma once

#include "XStrings.h"

class TStrings : public XStrings
{
public:
	static string TalentIDString_WithMode(int nTalentID, int nTalentMode);
	static const char* SexTypeToString(SEX nSex);
	static int StringToSexType(const char* szValue);
	
	static bool stristr(string str1, string str2);

	static string	GetAnimationLinkListText(vector<string>& vecAniList);
	static void		SetAnimationLinkListText(string str, vector<string>& vecOutAniList);
	static string	GetHitCapsuleLinkHitEffectIndexListText(vector<int>& vecIndexList);
	static void		SetHitCapsuleLinkHitEffectIndexListText(string str, vector<string>& vecOutIndexList);
	
	static void	ParseFindString(System::String^ strParse, vector<string>& outList);
};