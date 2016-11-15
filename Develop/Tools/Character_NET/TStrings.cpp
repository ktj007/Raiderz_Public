#include "stdafx.h"
#include "TStrings.h"
#include "MStringUtil.h"

string TStrings::TalentIDString_WithMode( int nTalentID, int nTalentMode )
{
	char buf[64] = "";
	string strWeapon = WeaponType(nTalentMode);

	if (nTalentMode != 0)
	{
		sprintf_s(buf, "%d - %s", nTalentID, strWeapon.c_str());
	}
	else
	{
		sprintf_s(buf, "%d", nTalentID);
	}

	string retStr = buf;
	return retStr;
}

const char* TStrings::SexTypeToString( SEX nSex )
{
	if (nSex == SEX_FEMALE) return "hf";
	return "hm";
}

int TStrings::StringToSexType(const char* szValue)
{
	if	(!_stricmp(szValue, "hm"))
	{
		return SEX_MALE;
	}
	if	(!_stricmp(szValue, "hf"))
	{
		return SEX_FEMALE;
	}

	return -1;
}

bool TStrings::stristr( string str1, string str2 )
{
	_strlwr((char*)str1.c_str());
	_strlwr((char*)str2.c_str());
	if(str1.find(str2) != string::npos)
	{
		return true;
	}

	return false;
}


string TStrings::GetAnimationLinkListText( vector<string>& vecAniList )
{
	bool bFirst = true;
	string strShowText;
	for(vector<string>::iterator it = vecAniList.begin(); it != vecAniList.end(); ++it)
	{
		if(bFirst)
		{
			strShowText = *it;
			bFirst = false;
		}
		else
		{
			strShowText += _T(';') + *it;
		}
	}

	return strShowText;
}

void TStrings::SetAnimationLinkListText( string str, vector<string>& vecOutAniList )
{
	vecOutAniList.clear();

	if(str.empty())
		return;

	while(1)
	{
		int nIndex = str.find(';');
		if(nIndex == string::npos)
		{
			vecOutAniList.push_back(str);
			return;
		}

		string strSub = str.substr(0, nIndex);

		vecOutAniList.push_back(strSub);

		str = str.substr(nIndex + 1, str.size());
	}
}

std::string TStrings::GetHitCapsuleLinkHitEffectIndexListText( vector<int>& vecIndexList )
{
	bool bFirst = true;
	string strShowText;
	for(vector<int>::iterator it = vecIndexList.begin(); it != vecIndexList.end(); ++it)
	{
		char ch[16] = {0,};
		sprintf_s(ch, "%d", *it);

		if(bFirst)
		{
			strShowText = ch;
			bFirst = false;
		}
		else
		{
			strShowText += string(",") + ch;
		}
	}

	return strShowText;
}

void TStrings::SetHitCapsuleLinkHitEffectIndexListText( string str, vector<string>& vecOutIndexList )
{
	vecOutIndexList.clear();

	if(str.empty())
		return;

	str = cml2::TrimBlanks(str);
	while(1)
	{
		int nIndex = str.find(',');
		if(nIndex == string::npos)
		{
			vecOutIndexList.push_back(str);
			return;
		}

		string strSub = str.substr(0, nIndex);
		cml2::TrimBlanks(strSub);

		vecOutIndexList.push_back(strSub);

		str = str.substr(nIndex + 1, str.size());
	}
}

void TStrings::ParseFindString( System::String^ strParse, vector<string>& outList )
{
	while(1)
	{
		int fndpos = strParse->IndexOf(",");
		if (fndpos != -1)
		{
			String^ strSub = strParse->Substring(0, fndpos)->Trim();

			outList.push_back(MStringToCharPointer(strSub));

			strParse = strParse->Substring(fndpos+1);
		}
		else
		{
			String^ strEnd = strParse->Trim();

			outList.push_back(MStringToCharPointer(strEnd));

			break;
		}
	}
}
