#include "stdafx.h"
//#include "XDef.h"
#include "TNaming.h"

void TNaming::GetPartsMeshName(string& outstrMeshName, const string& strMeshPath, const string& strThisModelMeshName, const string& strPartsMeshName, bool bMeshNameBySex)
{
	std::string strPath;

	if( bMeshNameBySex )
	{
		string strPureModelName( MGetPureFileName(strThisModelMeshName) );

		if (strMeshPath.empty())
		{
			string strModelPurePath = MGetPurePath(strThisModelMeshName);

			if (strPureModelName.length() >= 2 && strPureModelName[1] == 'm')
			{
				// 인간 남자 플레이어 경로
				strPath = string("hm/");// /*PATH_PLAYER_MALE*/ + string("hm_");
			}
			else
			{
				// 인간 여자 플레이어 경로
				strPath = string("hf/");// /*PATH_PLAYER_FEMALE*/ + string("hf_");
			}
		}
		else
		{
			strPath = strMeshPath;
		}
	}
	else
	{
		string strModelPurePath = MGetPurePath(strThisModelMeshName);

		if (strMeshPath.empty())
		{
			strPath = strModelPurePath + strMeshPath;
		}
		else
		{
			strPath = strMeshPath;
		}
	}

	outstrMeshName = strPath + strPartsMeshName;// + ".elu";
}

bool TNaming::IsSkinNodeName( const char* szNodeName )
{
	if (!_stricmp(szNodeName, "hat") ||
		!_stricmp(szNodeName, "chest") ||
		!_stricmp(szNodeName, "hands") ||
		!_stricmp(szNodeName, "legs") ||
		!_stricmp(szNodeName, "feet") ||
		!_stricmp(szNodeName, "face"))
	{
		return true;
	}
	return false;
}

bool TNaming::IsHairNodeName( const char* szNodeName )
{
	if (!_stricmp(szNodeName, "hair"))
	{
		return true;
	}
	return false;

}

const char* TNaming::GetPartsNodeName( SH_ITEM_SLOT nSlot )
{
	switch (nSlot)
	{
	case ITEMSLOT_HEAD: return "hat";
	case ITEMSLOT_BODY: return "chest";
	case ITEMSLOT_HANDS: return "hands";
	case ITEMSLOT_LEG: return "legs";
	case ITEMSLOT_FEET: return "feet";
	}

	return "";
}

string TNaming::GetMeshAliasName( RActor* pActor )
{
	if(pActor == NULL)
		return "";

	string n = pActor->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	return nn;
}

std::string TNaming::PlayerAnimationNameHeader( WEAPON_TYPE nWeaponType, CHAR_STANCE nStance )
{
	if (nStance == CS_NORMAL) return string("none_");

	string strRet;
	strRet = CSStrings::WeaponType(nWeaponType);
	strRet += "_";

	return strRet;
}
