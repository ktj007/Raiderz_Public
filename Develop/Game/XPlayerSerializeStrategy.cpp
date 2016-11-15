#include "stdafx.h"
#include "XPlayerSerializeStrategy.h"
#include "XNaming.h"
#include "XColorTable.h"
#include "RMesh.h"

void XPlayerSerializeStrategy::_Serialize( SerializeParam* pParam )
{
	m_bChangeSkinColor = false;

	m_Feature.nSex = pParam->nSex;
	if (info.colorTable)
	{
		m_Feature.nHairColor = info.colorTable->GetColor(L"hair", pParam->nHairColor).GetARGB();
		m_Feature.nEyeColor = info.colorTable->GetColor(L"eye", pParam->nEyeColor).GetARGB();

		unsigned int nSkinColor = info.colorTable->GetColor(L"skin", pParam->nSkinColor).GetARGB();
		if(nSkinColor != m_Feature.nSkinColor)
		{
			m_bChangeSkinColor = true;
			m_Feature.nSkinColor = nSkinColor;
		}
	}

	// 메쉬
	_SetMesh(pParam->strMeshName, 1.0f, CSST_MY_PLAYER);

	// 머리
	_SetPlayerHairMesh(pParam->nSex, pParam->nHair);

	// 얼굴
	_SetPlayerFaceMesh(pParam->nSex, pParam->nFace);

	SetEquipmentItemParts(ITEMSLOT_HEAD);
	SetEquipmentItemParts(ITEMSLOT_BODY);
	SetEquipmentItemParts(ITEMSLOT_HANDS);
	SetEquipmentItemParts(ITEMSLOT_LEG);
	SetEquipmentItemParts(ITEMSLOT_FEET);

	SetEquipmentItemParts(ITEMSLOT_LWEAPON, pParam->nWeaponSet);
	SetEquipmentItemParts(ITEMSLOT_RWEAPON, pParam->nWeaponSet);
}

void XPlayerSerializeStrategy::SetEquipmentItemParts( SH_ITEM_SLOT nItemSlot, int8 nWeaponSet )
{
	stEquipmentItem stItemData = GetEquipmentItem(nItemSlot, nWeaponSet);

	if (stItemData.pItemData)
	{
		_EquipItemMesh(stItemData, nItemSlot);

		if (stItemData.pItemData->m_nSubSlot != ITEMSLOT_NONE && stItemData.pItemData->m_nSubSlot != ITEMSLOT_MAX)
		{
			_UnEquipItemMesh( stItemData.pItemData->m_nSubSlot );
		}
	}
	else
	{
		_UnEquipItemMesh( nItemSlot );
	}
}

void XMyPlayerSerializeStrategy::Serialize(PLAYER_INFO& playerInfo, XMyEquipmentSlot* pEquipmentSlot)
{
	SerializeParam param;

	param.nSex = SEX(playerInfo.nSex);
	param.nHairColor = playerInfo.nFeatureHairColor;
	param.nSkinColor = playerInfo.nFeatureSkinColor;
	param.nHair = playerInfo.nFeatureHair;
	param.nFace = playerInfo.nFeatureFace;
	param.nWeaponSet = pEquipmentSlot->GetWeaponSet();
	param.strMeshName = XNaming::GetPlayerModelName(param.nSex);

	m_pMyEquipmentSlot = pEquipmentSlot;

	_Serialize(&param);
}

////
void XNetPlayerSerializeStrategy::Serialize( XNetPlayer* pNetPlayer, XPlayerInfoFeature& feature )
{
	m_pOwner = pNetPlayer;

	// 장비 색깔
	for(int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
	{
		m_nEquipedItemColor[i] = feature.nEquipedItemColor[i];
	}

	SerializeParam param;

	param.nSex = feature.nSex;
	param.nHairColor = feature.nHairColor;
	param.nSkinColor = feature.nSkinColor;
	param.nEyeColor = feature.nEyeColor;
	param.nHair = feature.nHair;
	param.nFace = feature.nFace;
	param.nWeaponSet = feature.nWeaponSet;
	param.strMeshName = XNaming::GetPlayerModelName(param.nSex);

	_Serialize(&param);
}

XPlayerSerializeStrategy::stEquipmentItem XMyPlayerSerializeStrategy::GetEquipmentItem( SH_ITEM_SLOT nItemSlot, int8 nWeaponSet )
{
	stEquipmentItem stItemData;
	SH_ITEM_SLOT nLookSlot = nItemSlot;
	switch (nItemSlot)
	{
	case ITEMSLOT_HEAD:		nLookSlot = ITEMSLOT_LOOK_HEAD; break;
	case ITEMSLOT_BODY:		nLookSlot = ITEMSLOT_LOOK_BODY; break;
	case ITEMSLOT_HANDS:	nLookSlot = ITEMSLOT_LOOK_HANDS; break;
	case ITEMSLOT_LEG:		nLookSlot = ITEMSLOT_LOOK_LEG; break;
	case ITEMSLOT_FEET:		nLookSlot = ITEMSLOT_LOOK_FEET; break;
	}

	if (nWeaponSet != 0)
	{
		switch (nItemSlot)
		{
		case ITEMSLOT_LWEAPON: 
			{
				nItemSlot = ITEMSLOT_LWEAPON2;
				nLookSlot = ITEMSLOT_LWEAPON2;
			}
			break;
		case ITEMSLOT_RWEAPON: 
			{
				nItemSlot = ITEMSLOT_RWEAPON2;
				nLookSlot = ITEMSLOT_RWEAPON2; 
			}
			break;
		}
	}

	if (m_pMyEquipmentSlot)
	{
		XItem* pItem = m_pMyEquipmentSlot->GetItem(nLookSlot);

		if (pItem == NULL && nItemSlot != nLookSlot)
		{
			pItem = m_pMyEquipmentSlot->GetItem(nItemSlot);
		}

		if (pItem)
		{
			stItemData.pItemData = pItem->m_pItemData;

			// 장비 색깔
			stItemData.nDyedColor = info.colorTable->GetColor(ITEM_DYED_COLOR_NAME, pItem->m_nDyedColor).GetARGB();

			return stItemData;
		}
	}

	return stItemData;
}

XPlayerSerializeStrategy::stEquipmentItem XNetPlayerSerializeStrategy::GetEquipmentItem( SH_ITEM_SLOT nItemSlot, int8 nWeaponSet )
{
	stEquipmentItem stItemData;
	SH_FEATURE_ITEMSLOT nFeatureItemSlot = FEATURE_ITEMSLOT_HEAD;
	switch (nItemSlot)
	{
	case ITEMSLOT_HEAD:		nFeatureItemSlot = FEATURE_ITEMSLOT_HEAD; break;
	case ITEMSLOT_BODY:		nFeatureItemSlot = FEATURE_ITEMSLOT_BODY; break;
	case ITEMSLOT_HANDS:	nFeatureItemSlot = FEATURE_ITEMSLOT_HANDS; break;
	case ITEMSLOT_LEG:		nFeatureItemSlot = FEATURE_ITEMSLOT_LEG; break;
	case ITEMSLOT_FEET:		nFeatureItemSlot = FEATURE_ITEMSLOT_FEET; break;
	case ITEMSLOT_LWEAPON: nFeatureItemSlot = (nWeaponSet==0) ? FEATURE_ITEMSLOT_LWEAPON : FEATURE_ITEMSLOT_LWEAPON2; break;
	case ITEMSLOT_RWEAPON: nFeatureItemSlot = (nWeaponSet==0) ? FEATURE_ITEMSLOT_RWEAPON : FEATURE_ITEMSLOT_RWEAPON2; break;
	}

	// 장비 색깔
	stItemData.nDyedColor = info.colorTable->GetColor(ITEM_DYED_COLOR_NAME, m_nEquipedItemColor[nFeatureItemSlot]).GetARGB();
	stItemData.pItemData = m_pOwner->GetEquipedItemData(nFeatureItemSlot);

	return stItemData;
}

XNetPlayerSerializeStrategy::XNetPlayerSerializeStrategy( XCharacter* pActor, bool bResourceLoading ) 
: XPlayerSerializeStrategy(pActor, bResourceLoading), m_pOwner(NULL)
{

}

void XPlayerSerializeStrategy::_SetPlayerHairMesh( SEX nSex, int nIndex )
{
	wstring strResourcePath;
	XNaming::GetHairModelName(strResourcePath, nSex, nIndex);

	if (m_bResourceLoading)
	{
		XPlayerPartsParam partsParam;
		partsParam.MakeForHair(strResourcePath.c_str(), XNaming::HairNodeName(), m_Feature.nHairColor);

		m_pActor->CreatePartsNodeAsync( &partsParam );
	}

	m_Feature.strHairMeshName = strResourcePath;
}

void XPlayerSerializeStrategy::_SetPlayerFaceMesh( SEX nSex, int nIndex )
{
	wstring strResourcePath;
	XNaming::GetFaceModelName(strResourcePath, nSex, nIndex);

	if (m_bResourceLoading)
	{
		XPlayerPartsParam partsParam;
		partsParam.MakeForFace(strResourcePath.c_str(), XNaming::FaceNodeName(), m_Feature.nSkinColor);
		partsParam.AddPart(wstring(XNaming::EyeNodeName()), wstring(L""), wstring(L""), m_Feature.nEyeColor);

		m_pActor->CreatePartsNodeAsync( &partsParam );
	}

	m_Feature.strFaceMeshName = strResourcePath;
}

void XPlayerSerializeStrategy::_EquipItemMesh( stEquipmentItem& stItemData, SH_ITEM_SLOT nEquipedSlot )
{
	if (m_Feature.m_equipments[nEquipedSlot].pItemData == stItemData.pItemData &&
		m_Feature.m_equipments[nEquipedSlot].nColor == stItemData.nDyedColor &&
		m_bChangeSkinColor == false) return;

	XCreatePartsNodeParam param;

	param.nDyedColor = stItemData.nDyedColor;

	BuildCreatePartsNodeParam(param, stItemData.pItemData, nEquipedSlot);

	CreatePartsNodeAsync(param);

	// item quiver 설정
	if(stItemData.pItemData->m_nWeaponType==WEAPON_ARCHERY)
		AddQuiver();

	// 새로운 아이템 설정
	m_Feature.m_equipments[nEquipedSlot].pItemData = stItemData.pItemData;
	m_Feature.m_equipments[nEquipedSlot].nColor = stItemData.nDyedColor;
}

void XPlayerSerializeStrategy::_UnEquipItemMesh( int nSlot )
{
	if (nSlot < 0 || nSlot >= ITEMSLOT_MAX) return;

	if (m_Feature.m_equipments[nSlot].pItemData)
	{
		if(m_Feature.m_equipments[nSlot].pItemData->m_nWeaponType==WEAPON_ARCHERY)
			RemoveQuiver();

		XPlayerPartsSlotType nLookSlot = XPartsSlotType::Slot2LookSlot((SH_ITEM_SLOT)nSlot);
		m_pActor->DestroyPartsNode(nLookSlot);
	}

	// 기본 메시가 있는경우는 복구
	SetPartsMeshToDefault(nSlot);

	m_Feature.m_equipments[nSlot].pItemData = NULL;
}

void XPlayerSerializeStrategy::SetPartsMeshToDefault( int nSlot )
{
	XPlayerPartsSlotType nLookSlot = XPartsSlotType::Slot2LookSlot((SH_ITEM_SLOT)nSlot);

	vector<XPartsParam::Part> vecParts;

	wstring strPartsNodeName = XNaming::GetPartsNodeName(SH_ITEM_SLOT(nSlot));
	if (strPartsNodeName.size() > 0)
	{
		XPartsParam::Part part;
		part.strNodeName = strPartsNodeName;
		part.dwPartsColor = m_Feature.nSkinColor;
		vecParts.push_back(part);

		XPartsParam::Part part_item;
		part_item.strNodeName = strPartsNodeName + wstring(EQUIPMENT_ITEM_NODE_NAME_ADDITIVE_SUFFIX);
		vecParts.push_back(part_item);
	}
	else
	{
		return;
	}
	
	wstring strDefaultPartsName = GetDefaultPartsName(m_Feature.nSex, SH_ITEM_SLOT(nSlot));

	wstring strRealMeshName;

	// default part mesh 이름을 지정한경우
	if (!strDefaultPartsName.empty())
	{
		XNaming::GetPartsMeshName(strRealMeshName, L"", MLocale::ConvAnsiToUTF16(m_pActor->m_pMesh->GetName().c_str()), strDefaultPartsName, true);
	}
	else
	{
		// 아니라면 원래 모델의 parts 를 지정한다
		if (m_pActor->m_pMesh)
		{
			strRealMeshName = MLocale::ConvAnsiToUTF16(m_pActor->m_pMesh->GetName().c_str());
		}
	}

	XPlayerPartsParam partsParam;
	partsParam.MakeForDefault(nLookSlot, strRealMeshName.c_str(), vecParts);
	m_pActor->CreatePartsNodeAsync( &partsParam );
}


wstring XPlayerSerializeStrategy::GetDefaultPartsName( SEX nSex, SH_ITEM_SLOT nItemSlot )
{
	// 기본 모델 메쉬
	if (nSex == SEX_MALE || nSex == SEX_FEMALE)
	{
		switch (nItemSlot)
		{
		case ITEMSLOT_HEAD: 
			{
				return XCONST::PlayerModel.Info[nSex].DEFAULT_MESHNAME_HAT;
			}
			break;
		case ITEMSLOT_BODY: 
			{
				return XCONST::PlayerModel.Info[nSex].DEFAULT_MESHNAME_CHEST;
			}
			break;
		case ITEMSLOT_HANDS: 
			{
				return XCONST::PlayerModel.Info[nSex].DEFAULT_MESHNAME_HANDS;
			}
			break;
		case ITEMSLOT_LEG: 
			{
				return XCONST::PlayerModel.Info[nSex].DEFAULT_MESHNAME_LEGS;
			}
			break;
		case ITEMSLOT_FEET: 
			{
				return XCONST::PlayerModel.Info[nSex].DEFAULT_MESHNAME_FEET;
			}
			break;

		}
	}
	return L"";
}

void XPlayerSerializeStrategy::AddQuiver()
{
	const wchar_t* szQuiverName = L"quiver_01.elu";
	const wchar_t* szQuiverNodeName = L"quiver_01";
	const wchar_t* szQuiverParentNodeName = L"dummy_quiver";

	if (m_bResourceLoading)
	{
		XPlayerPartsParam partsParam;
		partsParam.MakeForQuiver(szQuiverName, szQuiverNodeName, szQuiverParentNodeName);

		m_pActor->CreatePartsNodeAsync( &partsParam );
	}
}

void XPlayerSerializeStrategy::RemoveQuiver()
{
	m_pActor->DestroyPartsNode(PLAYER_PARTS_SLOT_QUIVER);
}

void XPlayerSerializeStrategy::BuildCreatePartsNodeParam( XCreatePartsNodeParam& outParam, CSItemData* pItemData, SH_ITEM_SLOT nEquipedSlot )
{
	if (pItemData && m_pActor->m_pMesh)
		XNaming::GetPartsMeshName(outParam.strItemMeshName, pItemData->m_strMeshPath, MLocale::ConvAnsiToUTF16(m_pActor->m_pMesh->GetName().c_str()), pItemData->m_strMeshName, pItemData->m_bMeshNameBySex);

	XPlayerPartsSlotType nLookSlot = XPartsSlotType::Slot2LookSlot(nEquipedSlot);

	outParam.bWeaponOrShield = false;
	if (nLookSlot == PLAYER_PARTS_SLOT_LWEAPON || nLookSlot == PLAYER_PARTS_SLOT_RWEAPON)
	{
		outParam.bWeaponOrShield = true;
	}

	outParam.bLeftWeapon = false;
	if (pItemData && 
		pItemData->m_ItemType == ITEMTYPE_WEAPON && 
		(nEquipedSlot == ITEMSLOT_LWEAPON || nEquipedSlot == ITEMSLOT_LWEAPON2))
	{
		outParam.bLeftWeapon = true;
	}

	outParam.pItemData = pItemData;
	outParam.nPartsID = nLookSlot;
}