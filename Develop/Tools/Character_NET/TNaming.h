#pragma once


class TNaming
{
public:
	static void GetPartsMeshName(string& outstrMeshName, const string& strMeshPath, const string& strThisModelMeshName, const string& strPartsMeshName, bool bMeshNameBySex);
	static const char* GetPartsNodeName(SH_ITEM_SLOT nSlot);
	static bool IsSkinNodeName( const char* szNodeName );
	static bool IsHairNodeName( const char* szNodeName );
	static string GetMeshAliasName( RActor* pActor );
	static string PlayerAnimationNameHeader(WEAPON_TYPE nWeaponType, CHAR_STANCE nStance);
};
