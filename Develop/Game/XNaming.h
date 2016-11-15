#pragma once

#include "CSItemData.h"
#include "XDef.h"
#include "XAnim.h"

class XNPCInfo;

/// 이름 규칙
class XNaming
{
private:
	static const wstring PlayerModelPath(SEX nSex);
public:
	// Model ----------------
	static void GetPartsMeshName(wstring& outstrMeshName, const wstring& strMeshPath, const wstring& strThisModelMeshName, const wstring& strPartsMeshName, bool bMeshNameBySex);
	static const wchar_t* GetPartsNodeName(SH_ITEM_SLOT nSlot);
	static const wchar_t* HairNodeName()	{ return L"hair"; }
	static const wchar_t* FaceNodeName()	{ return L"face"; }
	static const wchar_t* EyeNodeName()		{ return L"eye"; }
	static const void GetHairModelName(wstring& outstrModelName, SEX nSex, int nIndex);
	static const void GetFaceModelName(wstring& outstrModelName, SEX nSex, int nIndex);
	static const wchar_t* GetPlayerModelName(SEX nSex);
	static const wchar_t* GetPlayerModelSimpleName( SEX nSex );

	static const wstring GetMeshName(wstring& strFileName);
	static const wstring GetModelName(wstring& strFileName);
	static bool IsSkinNodeName(const wchar_t* szNodeName);
	static bool IsHairNodeName(const wchar_t* szNodeName);

	// Animation -----------
	static wstring ModeAnimationNameHeader(int nCurrNPCMode, XNPCInfo* pNPCInfo);
	static wstring StunMotionAnimation(NPC_STUN_USE_ANIMATION nStunAnimation);
	static tstring PlayerAnimationNameHeader(WEAPON_TYPE nWeaponType, CHAR_STANCE nStance);

	// 
	static const wchar_t* GetArmorTypeName( ARMOR_TYPE nArmor);
	static const wchar_t* GetSlotTypeName( SH_ITEM_SLOT nSlot);
};


