#include "stdafx.h"
#include "XNaming.h"
#include "XConst.h"
#include "XNPCInfo.h"

void XNaming::GetPartsMeshName(wstring& outstrMeshName, const wstring& strMeshPath, const wstring& strThisModelMeshName, const wstring& strPartsMeshName, bool bMeshNameBySex)
{
	std::wstring strPath;

	if( bMeshNameBySex )
	{
		wstring strPureModelName( MLocale::ConvAnsiToUTF16(MGetPureFileName(MLocale::ConvUTF16ToAnsi(strThisModelMeshName.c_str())).c_str()));

		if (strMeshPath.empty())
		{
			wstring strModelPurePath = MLocale::ConvAnsiToUTF16(MGetPurePath(MLocale::ConvUTF16ToAnsi(strThisModelMeshName.c_str())).c_str());

			if (strPureModelName.length() >= 2 && strPureModelName[1] == 'm')
			{
				// 인간 남자 플레이어 경로
				strPath = PATH_PLAYER_MALE + wstring(L"hm_");
			}
			else
			{
				// 인간 여자 플레이어 경로
				strPath = PATH_PLAYER_FEMALE + wstring(L"hf_");
			}
		}
		else
		{
			strPath = strMeshPath;
		}
	}
	else
	{
		wstring strModelPurePath = MLocale::ConvAnsiToUTF16(MGetPurePath(MLocale::ConvUTF16ToAnsi(strThisModelMeshName.c_str())).c_str());

		if (strMeshPath.empty())
		{
			strPath = strModelPurePath + strMeshPath;
		}
		else
		{
			strPath = strMeshPath;
		}
	}

	outstrMeshName = strPath + strPartsMeshName + L".elu";
}

const void XNaming::GetHairModelName( wstring& outstrModelName, SEX nSex, int nIndex )
{
	if (nSex != SEX_MALE && nSex != SEX_FEMALE) return;
	if (nIndex < 0 || nIndex >= (int)(XCONST::PlayerModel.Info[nSex].HAIR_MODEL.size())) return;

	outstrModelName = PlayerModelPath(nSex) + XCONST::PlayerModel.Info[nSex].HAIR_MODEL[nIndex] + wstring(L".elu");
}

const void XNaming::GetFaceModelName( wstring& outstrModelName, SEX nSex, int nIndex )
{
	if (nSex != SEX_MALE && nSex != SEX_FEMALE) return;
	if (nIndex < 0 || nIndex >= (int)(XCONST::PlayerModel.Info[nSex].FACE_MODEL.size())) return;

	outstrModelName = PlayerModelPath(nSex) + XCONST::PlayerModel.Info[nSex].FACE_MODEL[nIndex] + wstring(L".elu");
}

const wstring XNaming::PlayerModelPath(SEX nSex)
{
	if (nSex == SEX_MALE)
		return wstring(PATH_PLAYER_MALE);
	
	return wstring(PATH_PLAYER_FEMALE);
}

wstring XNaming::ModeAnimationNameHeader( int nCurrNPCMode, XNPCInfo* pNPCInfo )
{
	if (pNPCInfo && pNPCInfo->strAniPrefix.empty() == false)
	{
		return pNPCInfo->strAniPrefix;
	}

	switch (nCurrNPCMode)
	{
	case NPC_MODE_1: 
		{
			return L"mode1_";
		}
		break;
	case NPC_MODE_2: 
		{
			return L"mode2_";
		}
		break;
	case NPC_MODE_3: 
		{
			return L"mode3_";
		}
		break;
	case NPC_MODE_4: 
		{
			return L"mode4_";
		}
		break;
	case NPC_MODE_5: 
		{
			return L"mode5_";
		}
		break;
	}
	return L"";
}

wstring XNaming::StunMotionAnimation( NPC_STUN_USE_ANIMATION nStunAnimation )
{
	switch(nStunAnimation)
	{
	case NPC_USE_STUN:
		return wstring(MOTION_NAME_NPC_MF_STUN);
	case NPC_USE_SLEEP:
		return wstring(MOTION_NAME_NPC_MF_STUN2);
	}

	// 위 상태가 아니라면... IEK
	return wstring(MOTION_NAME_NPC_IDLE);
}

const wchar_t* XNaming::GetPartsNodeName( SH_ITEM_SLOT nSlot )
{
	switch (nSlot)
	{
	case ITEMSLOT_HEAD: return L"hat";
	case ITEMSLOT_BODY: return L"chest";
	case ITEMSLOT_HANDS: return L"hands";
	case ITEMSLOT_LEG: return L"legs";
	case ITEMSLOT_FEET: return L"feet";
	case ITEMSLOT_FACE: return L"face";
	}

	return L"";
}

const wchar_t* XNaming::GetPlayerModelName( SEX nSex )
{
	if (nSex == SEX_NA || nSex >= SEX_MAX) return L"";

	return XCONST::PlayerModel.Info[nSex].MODEL_NAME.c_str();
}

bool XNaming::IsSkinNodeName( const wchar_t* szNodeName )
{
	if (!_wcsicmp(szNodeName, L"hat") ||
		!_wcsicmp(szNodeName, L"chest") ||
		!_wcsicmp(szNodeName, L"hands") ||
		!_wcsicmp(szNodeName, L"legs") ||
		!_wcsicmp(szNodeName, L"feet") ||
		!_wcsicmp(szNodeName, L"face"))
	{
		return true;
	}
	return false;
}

bool XNaming::IsHairNodeName( const wchar_t* szNodeName )
{
	if (!_wcsicmp(szNodeName, L"hair"))
	{
		return true;
	}
	return false;

}

const wstring XNaming::GetMeshName( wstring& strFileName )
{
	int token_path = strFileName.rfind(L"/") + 1;
	return strFileName.substr(token_path, strFileName.length() - token_path);
}

const wstring XNaming::GetModelName( wstring& strFileName )
{
	wstring strMeshName = GetMeshName(strFileName);
	
	int token_extensions = strMeshName.rfind(L".");
	return strMeshName.substr(0, token_extensions);
}

const wchar_t* XNaming::GetArmorTypeName( ARMOR_TYPE nArmor)
{
	switch (nArmor)
	{
	case ARMOR_NONE		:	return L"none";
	case ARMOR_CLOTH	:	return L"천옷";
	case ARMOR_LIGHT	:	return L"가죽갑옷";
	case ARMOR_MEDIUM	:	return L"사슬갑옷";
	case ARMOR_HEAVY	:	return L"판금갑옷";
	case ARMOR_SHIELD	:	return L"방패";
	case ARMOR_ACCESSORY:	return L"장신구";
	case ARMOR_AMULET	:	return L"에뮬렛";
	}

	return L"";
}

const wchar_t* XNaming::GetSlotTypeName( SH_ITEM_SLOT  nSlot)
{
	switch (nSlot)
	{
	case ITEMSLOT_NONE		:	return L"none";
	case ITEMSLOT_HEAD		:	return L"머리";
	case ITEMSLOT_FACE		:	return L"얼굴";
	case ITEMSLOT_HANDS		:	return L"손";
	case ITEMSLOT_FEET		:	return L"발";
	case ITEMSLOT_BODY		:	return L"몸통";
	case ITEMSLOT_LEG		:	return L"다리";
	case ITEMSLOT_LFINGER	:
	case ITEMSLOT_RFINGER	:	return L"반지";
	case ITEMSLOT_NECK		:	return L"목걸이";
	case ITEMSLOT_CHARM		:	return L"charm";
	case ITEMSLOT_LWEAPON	:
	case ITEMSLOT_RWEAPON	:
	case ITEMSLOT_LWEAPON2	:
	case ITEMSLOT_RWEAPON2	:	return L"무기";
	//case ITEMSLOT_LOOK_HEAD	:	return "";
	//case ITEMSLOT_LOOK_HANDS:	return "";
	//case ITEMSLOT_LOOK_FEET	:	return "";
	//case ITEMSLOT_LOOK_BODY	:	return "";
	//case ITEMSLOT_LOOK_LEG	:	return "";
	//case ITEMSLOT_INN_CRAFTING:	return "";
	//case ITEMSLOT_INN_BED	:	return "";
	//case ITEMSLOT_INN_STEWARD:	return "";
	//case ITEMSLOT_INN_CARPET:	return "";
	//case ITEMSLOT_INN_FIREPLACE:return "";
	//case ITEMSLOT_INN_MURAL	:	return "";
	//case ITEMSLOT_INN_TABLE	:	return "";
	//case ITEMSLOT_INN_FUNITURE:	return "";
	}

	return CSItemSlotStr[nSlot];
}

const wchar_t* XNaming::GetPlayerModelSimpleName( SEX nSex )
{
	if (nSex == SEX_NA || nSex >= SEX_MAX) return L"";

	return XCONST::PlayerModel.Info[nSex].MODEL_NAME_SIMPLE.c_str();
}

tstring XNaming::PlayerAnimationNameHeader( WEAPON_TYPE nWeaponType, CHAR_STANCE nStance )
{
	if (nStance == CS_NORMAL) return tstring(L"none_");

	tstring strRet;
	strRet = CSStrings::WeaponType(nWeaponType);
	strRet += L"_";

	return strRet;
}
