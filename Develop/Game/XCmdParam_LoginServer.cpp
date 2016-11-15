#include "stdafx.h"
#include "XCmdParam_LoginServer.h"

bool XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST::Parse( MCommand* pCommand )
{
	if (GetBlobParameter<TD_AccountCharInfo>(pCommand, 0, &m_pTDAccountCharInfo, &m_nBlobCount) == false)
	{
		return false;
	}

	return true;
}

void XCmdParam__MC_COMM_RESPONSE_ACCOUNT_CHAR_LIST::AssignTo( vector<XAccountCharInfo>& outvecAccountCharInfo )
{
	outvecAccountCharInfo.clear();

	for (int i=0; i<m_nBlobCount; i++)
	{
		XAccountCharInfo		new_char_info;
		new_char_info.Feature.nSex = m_pTDAccountCharInfo[i].Feature.nSex;
		new_char_info.Feature.nHair = m_pTDAccountCharInfo[i].Feature.nHair;
		new_char_info.Feature.nFace = m_pTDAccountCharInfo[i].Feature.nFace;
		new_char_info.Feature.nHairColor = m_pTDAccountCharInfo[i].Feature.nHairColor;
		new_char_info.Feature.nSkinColor = m_pTDAccountCharInfo[i].Feature.nSkinColor;
		new_char_info.Feature.nEyeColor = m_pTDAccountCharInfo[i].Feature.nEyeColor;
		new_char_info.Feature.nMakeUp = m_pTDAccountCharInfo[i].Feature.nMakeUp;
		new_char_info.Feature.nWeaponSet = m_pTDAccountCharInfo[i].Feature.nWeaponSet;
		new_char_info.Feature.nTattoo = m_pTDAccountCharInfo[i].Tattoo.nTattooType;
		new_char_info.Feature.fTattooScale = (float)m_pTDAccountCharInfo[i].Tattoo.nTattooScale * 0.02f;
		new_char_info.Feature.vTattooPos.x = (float)m_pTDAccountCharInfo[i].Tattoo.nTattooPosX * 0.01f;
		new_char_info.Feature.vTattooPos.y = (float)m_pTDAccountCharInfo[i].Tattoo.nTattooPosY * 0.01f;

		new_char_info.nLevel = m_pTDAccountCharInfo[i].nLevel;
		new_char_info.nFieldID = m_pTDAccountCharInfo[i].nFieldID;
		wcsncpy_s(new_char_info.szName, m_pTDAccountCharInfo[i].szName, _TRUNCATE);

		for (int j = 0; j < FEATURE_ITEMSLOT_MAX; j++)
		{
			new_char_info.Feature.nEquipedItemID[j] = m_pTDAccountCharInfo[i].Feature.nItemID[j];
			new_char_info.Feature.nEquipedItemColor[j] = m_pTDAccountCharInfo[i].Feature.nItemID_DyedColor[j];
			new_char_info.Feature.nEquipedItemEnchantBuff[j] = m_pTDAccountCharInfo[i].Feature.nItemID_EnchantBuff[j];
		}

		outvecAccountCharInfo.push_back(new_char_info);
	}
}