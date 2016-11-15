#ifndef _XPOST_CHAR_H
#define _XPOST_CHAR_H

#include "XNetClient.h"
#include "XCommandTable.h"
#include "CTransData.h"
#include "MCommandParameter.h"
#include "CSDef_Rebirth.h"
#include "XCharFrame.h"

inline void XPostChar_RequestInsertCharacter(const wchar_t* szName, const XCHARACTER_CREATEINFO& ci)
{
	TD_INSERT_CHARACTER_INFO tdInsertCharacterInfo;
	wcsncpy_s(tdInsertCharacterInfo.szName, szName, _TRUNCATE);
	tdInsertCharacterInfo.nSex = ci.nSex;
	tdInsertCharacterInfo.nTalentStyle = (TALENT_STYLE)ci.nStyle;
	tdInsertCharacterInfo.nHair = (int8)ci.nHair;
	tdInsertCharacterInfo.nFace = (int8)ci.nFace;
	tdInsertCharacterInfo.nHairColor = (short)ci.nHairColor;
	tdInsertCharacterInfo.nSkinColor = (short)ci.nSkinColor;
	tdInsertCharacterInfo.nEyeColor = (uint8)ci.nEyeColor;
	tdInsertCharacterInfo.nMakeUp = (uint8)ci.nMakeUp;
	tdInsertCharacterInfo.nTattooType = (uint8)ci.nTattoo;
	tdInsertCharacterInfo.nTattooScale = (uint8)(ci.fTattooScale * 50.0f);
	tdInsertCharacterInfo.nTattooPosX = (short)( ci.vTattooPos.x * 100.0f);
	tdInsertCharacterInfo.nTattooPosY = (short)( ci.vTattooPos.y * 100.0f);
	tdInsertCharacterInfo.nEquipmentIndex = (int8)ci.nEquipments;
	tdInsertCharacterInfo.nEquipmentColorIndex = (int8)ci.nEquipmentsCol;
	tdInsertCharacterInfo.nVoice = (int8)ci.nVoice;
	XPOSTCMD1(MC_CHAR_REQUEST_INSERT_CHAR, MCommandParameterSingleBlob(tdInsertCharacterInfo));
}

inline void XPostChar_RequestDeleteCharacter(int8 nCharIndex)
{
	XPOSTCMD1(MC_CHAR_REQUEST_DELETE_CHAR, MCommandParameterChar(nCharIndex));
}

inline void XPostChar_RequestSelMyChar(int nCharIndex)
{
	XPOSTCMD1(MC_CHAR_REQUEST_SEL_MYCHAR, MCommandParameterInt(nCharIndex));
}

inline void XPostChar_Rebirth(REBIRTH_TYPE nREBT)
{
	XPOSTCMD1(MC_CHAR_REBIRTH_REQ, MCommandParameterInt(nREBT));
}

inline void XPostChar_TargetInfo(const UIID uiid)
{
	vector<UIID> vecUIID;
	vecUIID.push_back(uiid);

	XPOSTCMD1(MC_CHAR_TARGET_INFO_REQ, MCommandParameterBlob(vecUIID));
}

inline void XPostChar_Die(void)
{
	XPOSTCMD0(MC_CHAR_DIE_REQ);
}

inline void XPostChar_AFK()
{
	XPOSTCMD0(MC_ACTION_AFK_REQ);
}

#endif