#include "stdafx.h"
#include "SCommonBodyCharGetLookList.h"
#include "MDBRecordSet.h"
#include "SDBTask.h"
#include "MLocale.h"

SCommonBodyCharGetLookList::SCommonBodyCharGetLookList()
{
	// do nothing
}

SCommonBodyCharGetLookList::~SCommonBodyCharGetLookList()
{
	// do nothing
}

void SCommonBodyCharGetLookList::GetCharSimpleInfo(mdb::MDBRecordSet& rs)
{
	if (0 == rs.GetFetchedCount())
		return;
	
	for (m_Data.nCharCount = 0; !rs.IsEOF(); rs.MoveNext(), ++m_Data.nCharCount)
	{
		// 게임은 진행 할 수 있게 가지고 온것은 보여주자.
		if (MAX_ACCOUNT_CHAR_COUNT <= m_Data.nCharCount)
			return;
		
		SDBTASK_ACCOUNT_CAHR_FEATURE_LIST& Char = m_Data.CharList[m_Data.nCharCount];
		
		Char.nCID													= rs.Field("CHAR_ID").AsInt64();
		Char.strName												= rs.Field("NAME").AsWString();
		Char.nLevel													= rs.Field("LEV").AsByte();
		Char.Race													= static_cast<RACE>(rs.Field("RACE").AsByte());
		Char.Sex													= static_cast<SEX>(rs.Field("SEX").AsInt());
		Char.nPlayTime												= rs.Field("CHAR_PTM").AsInt();
		Char.nFeatureHair											= static_cast<int8>(rs.Field("FEAT_HAIR").AsInt());
		Char.nFeatureFace											= static_cast<int8>(rs.Field("FEAT_FACE").AsInt());
		Char.nFeatureHairColor										= static_cast<int8>(rs.Field("FEAT_HAIR_COLOR").AsInt());
		Char.nFeatureSkinColor										= static_cast<int8>(rs.Field("FEAT_SKIN_COLOR").AsInt());
		Char.nEyeColor												= rs.Field("FEAT_EYE_COLOR").AsByte();
		Char.nMakeUp												= rs.Field("MAKEUP").AsByte();
		Char.nTattooType											= rs.Field("TATOO").AsByte();
		Char.nTattooPosX											= rs.Field("TATOO_POS_X").AsShort();
		Char.nTattooPosY											= rs.Field("TATOO_POS_Y").AsShort();
		Char.nTattooScale											= rs.Field("TATOO_SCALE").AsByte();
		Char.nSharedFieldID											= rs.Field("SHR_FIELD_ID").AsInt();
		Char.uidDynamicFieldGroup									= rs.Field("DYN_FIELD_GROUP_UID").AsInt64();
		Char.nDynamicFieldID										= rs.Field("DYN_FIELD_ID").AsInt();
	}

	return;
}

bool SCommonBodyCharGetLookList::GetCharEquipmentInfo( mdb::MDBRecordSet& rs )
{
	if (0 == rs.GetFetchedCount())
		return true;

	const static size_t nCIDHash			= rs.MakeHashValue("CHAR_ID");
	const static size_t nSlotTypeHash		= rs.MakeHashValue("SLOT_TYPE");
	const static size_t nSlotIDhash			= rs.MakeHashValue("SLOT_ID");
	const static size_t nItemIDHash			= rs.MakeHashValue("ITEM_ID");
	const static size_t nColorHash			= rs.MakeHashValue("COLOR");
	static const size_t nEnchItemID_1_Hash	= rs.MakeHashValueW(L"ENCH_ITEM_ID_1");	

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		if (rs.FieldHash(nCIDHash).IsNull())
			continue;

		const int64 nCID		= rs.FieldHash(nCIDHash).AsInt64();
		const uint8 nSlottype	= (uint8)rs.FieldHash(nSlotTypeHash).AsInt();
		const int16 nSlotID		= (int16)rs.FieldHash(nSlotIDhash).AsInt();
		const int nItemID		= rs.FieldHash(nItemIDHash).AsInt();
		const int nColor		= rs.FieldHash(nColorHash).AsInt();
		const int nEnchItemID_1	= rs.FieldHash(nEnchItemID_1_Hash).AsInt();		
		
		if (SLOTTYPE_EQUIP != nSlottype) continue;

		int nIndex = -1;
		for (int i = 0; i < MAX_ACCOUNT_CHAR_COUNT; ++i)
		{
			if (nCID == m_Data.CharList[i].nCID)
			{
				nIndex = i;
				break;
			}
		}

		if (nIndex < 0)
			return false;

		const SH_FEATURE_ITEMSLOT nFeatureSlotID = ConvertSlotIDToFeatureSlotID(nSlotID);
		if (FEATURE_ITEMSLOT_MAX == nFeatureSlotID)
		{
			mlog3("Error! SCommonBodyCharGetLookList::GetCharEquipmentInfo(), Invalid FeatureSlot(CID: %I64d, SlotID: %d)!\n", (int64)nCID, nSlotID);
			continue;
		}

		m_Data.CharList[nIndex].FeatureItem[nFeatureSlotID].nItemID			= nItemID;
		m_Data.CharList[nIndex].FeatureItem[nFeatureSlotID].nItemDyedColor	= nColor;
		m_Data.CharList[nIndex].FeatureItem[nFeatureSlotID].nDefEnchItemID	= nEnchItemID_1;		
	}

	return true;
}

SH_FEATURE_ITEMSLOT SCommonBodyCharGetLookList::ConvertSlotIDToFeatureSlotID( const int16 nSlotID )
{
	switch (nSlotID)
	{
	case ITEMSLOT_HEAD :
		return FEATURE_ITEMSLOT_HEAD;		
	case ITEMSLOT_FACE :
		return FEATURE_ITEMSLOT_FACE;
	case ITEMSLOT_HANDS :
		return FEATURE_ITEMSLOT_HANDS;
	case ITEMSLOT_FEET : 
		return FEATURE_ITEMSLOT_FEET;
	case ITEMSLOT_BODY :
		return FEATURE_ITEMSLOT_BODY;
	case ITEMSLOT_LEG :
		return FEATURE_ITEMSLOT_LEG;
	case ITEMSLOT_LWEAPON : 
		return FEATURE_ITEMSLOT_LWEAPON;
	case ITEMSLOT_RWEAPON :
		return FEATURE_ITEMSLOT_RWEAPON;
	case ITEMSLOT_LWEAPON2 :
		return FEATURE_ITEMSLOT_LWEAPON2;
	case ITEMSLOT_RWEAPON2 :
		return FEATURE_ITEMSLOT_RWEAPON2;
	}

	//ITEMSLOT_HEAD == FEATURE_ITEMSLOT_HEAD;
	//ITEMSLOT_FACE == FEATURE_ITEMSLOT_FACE;
	//ITEMSLOT_HANDS == FEATURE_ITEMSLOT_HANDS;
	//ITEMSLOT_FEET == FEATURE_ITEMSLOT_FEET;
	//ITEMSLOT_BODY == FEATURE_ITEMSLOT_BODY;
	//ITEMSLOT_LEG == FEATURE_ITEMSLOT_LEG;

	//ITEMSLOT_LWEAPON == FEATURE_ITEMSLOT_LWEAPON;
	//ITEMSLOT_RWEAPON == FEATURE_ITEMSLOT_RWEAPON;
	//ITEMSLOT_LWEAPON2 == FEATURE_ITEMSLOT_LWEAPON2;
	//ITEMSLOT_RWEAPON2 == FEATURE_ITEMSLOT_RWEAPON2;

	return FEATURE_ITEMSLOT_MAX;
}

void SCommonBodyCharGetLookList::SetAccountCharInfo(int idx, TD_AccountCharInfo* pCharInfo)
{
	VALID(0 <= idx && idx < MAX_ACCOUNT_CHAR_COUNT);

	SDBTASK_ACCOUNT_CAHR_FEATURE_LIST& dbInfo = m_Data.CharList[idx];

	
	// 기본정보	
	pCharInfo->nLevel = dbInfo.nLevel;
	
	wcsncpy_s(pCharInfo->szName, dbInfo.strName.c_str(), _TRUNCATE);

	// 외형
	pCharInfo->Feature.nSex = dbInfo.Sex;
	pCharInfo->Feature.nHair = dbInfo.nFeatureHair;
	pCharInfo->Feature.nFace = dbInfo.nFeatureFace;
	pCharInfo->Feature.nHairColor = dbInfo.nFeatureHairColor;
	pCharInfo->Feature.nSkinColor = dbInfo.nFeatureSkinColor;
	pCharInfo->Feature.nEyeColor = dbInfo.nEyeColor;

	pCharInfo->Feature.nMakeUp = dbInfo.nMakeUp;
	pCharInfo->Tattoo.nTattooType = dbInfo.nTattooType;
	pCharInfo->Tattoo.nTattooPosX = dbInfo.nTattooPosX;
	pCharInfo->Tattoo.nTattooPosY = dbInfo.nTattooPosY;
	pCharInfo->Tattoo.nTattooScale = dbInfo.nTattooScale;
	pCharInfo->Feature.nWeaponSet = 0;
	

	// 아이템
	for(int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
	{
		pCharInfo->Feature.nItemID[i] = dbInfo.FeatureItem[i].nItemID;
		pCharInfo->Feature.nItemID_DyedColor[i] = dbInfo.FeatureItem[i].nItemDyedColor;
		pCharInfo->Feature.nItemID_EnchantBuff[i] = dbInfo.FeatureItem[i].nDefEnchItemID;
	}

	
	// 마지막 접속 필드
	if (dbInfo.uidDynamicFieldGroup.IsInvalid() || dbInfo.nDynamicFieldID <= 0)
	{
		pCharInfo->nFieldID = dbInfo.nSharedFieldID;
	}
	else
	{
		pCharInfo->nFieldID = dbInfo.nDynamicFieldID ;
	}	
}

void SCommonBodyCharGetLookList::SetAccountCharFieldInfo(int idx, SAccountCharFieldInfo* pFieldInfo)
{
	VALID(0 <= idx && idx < MAX_ACCOUNT_CHAR_COUNT);

	SDBTASK_ACCOUNT_CAHR_FEATURE_LIST& dbInfo = m_Data.CharList[idx];

	pFieldInfo->nCID = (int)dbInfo.nCID;

	pFieldInfo->nExitedSharedFieldID = dbInfo.nSharedFieldID;
	pFieldInfo->nExitedDynamicFieldID = dbInfo.nDynamicFieldID;		
	pFieldInfo->uidExitedDynamicFieldGroup = dbInfo.uidDynamicFieldGroup;	
}

MUID SCommonBodyCharGetLookList::GetPlayerUID(void)
{
	return m_Data.uidPlayer;
}

int SCommonBodyCharGetLookList::GetCharCount(void)
{
	return m_Data.nCharCount;
}

int SCommonBodyCharGetLookList::GetSharedFieldID(int nIdx)
{
	return m_Data.CharList[nIdx].nSharedFieldID;
}

int SCommonBodyCharGetLookList::GetDynamicFieldID(int nIdx)
{
	return m_Data.CharList[nIdx].nDynamicFieldID;
}

MUID SCommonBodyCharGetLookList::GetDynamicFieldGroupUID(int nIdx)
{
	return m_Data.CharList[nIdx].uidDynamicFieldGroup;
}

void SCommonBodyCharGetLookList::SetPlayerUID(MUID uidReqPlayer)
{
	m_Data.uidPlayer = uidReqPlayer;
}

void SCommonBodyCharGetLookList::SetCharCount(int nCount)
{
	m_Data.nCharCount = nCount;
}