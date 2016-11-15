#include "stdafx.h"
#include "XNPCSerializeStrategy.h"
#include "XNPCInfo.h"
#include "XItemManager.h"
#include "XNaming.h"
#include "RMesh.h"
#include "XChangeNPCColor.h"

#include "XFieldInfo.h"

XNPCSerializeStrategy::XNPCSerializeStrategy( XCharacter* pActor, bool bResourceLoading ) 
: XCharacterSerializeStrategy(pActor, bResourceLoading)
{

}

void XNPCSerializeStrategy::Serialize( XNPCInfo* pNPCInfo, float fScale )
{
	m_Feature.pNPCInfo = pNPCInfo;
	m_Feature.nSkinColor = pNPCInfo->nSkinColor;
	m_Feature.nHairColor = pNPCInfo->nHairColor;
	m_Feature.nEyeColor = pNPCInfo->nEyeColor;

	std::wstring strMeshName = GetMeshName(pNPCInfo);
	_SetMesh(strMeshName.c_str(), fScale, CSST_NPC);

	// 인간형이나 무기 파츠를 바꿔야 하는 경우 파츠 교환
	if (pNPCInfo->IsUseMeshParts())
	{
		SerializeParts(pNPCInfo);

		// PreLoading이면 파츠를 한번 업데이트 해서 파츠 로딩 큐 로직을 한번 타게 한다.
		if ( info.field && info.field->IsFieldPreLoadResource(strMeshName))
		{
			m_pActor->UpdatePartsNodeForNoneBackgroundIfAllMeshLoaded();
			//m_pActor->UpdatePartsNode();
		}
	}
}

void XNPCSerializeStrategy::SerializeParts( XNPCInfo* pNPCInfo )
{
	for (int i = 0; i < NPC_PARTS_SLOT_MAX; i++)
	{
		int nItemID = pNPCInfo->nPartsItemID[i];
		if (nItemID <= 0) continue;

		CSItemData* pItemData = info.item->GetItemData(nItemID);
		if (pItemData)
		{
			EquipNPCItem(XNPCPartsSlotType(i), pItemData);
		}
	}
}

void XNPCSerializeStrategy::EquipNPCItem( XNPCPartsSlotType nLookSlot, CSItemData* pItemData )
{
	XCreatePartsNodeParam param;

	param.strItemMeshName = GetItemMeshName(pItemData);

	param.bWeaponOrShield = false;
	if (pItemData->m_ItemType == ITEMTYPE_WEAPON) param.bWeaponOrShield = true;

	param.nPartsID = nLookSlot;
	param.pItemData = pItemData;

	if(nLookSlot == NPC_PARTS_SLOT_FACE)
		param.bEye = true;

	CreatePartsNodeAsync(param);
}

void XNPCSerializeStrategy::OnLoadingComplete()
{
	if (m_Feature.pNPCInfo) 
	{
		if (m_pActor->GetUsable())
		{
			// 재질 교체
			SetNPCInfoSubMtrl(m_Feature.pNPCInfo->nID);

			// 텍스쳐 색깔 
			if (m_Feature.pNPCInfo->IsUseTexColor())
			{
				m_pActor->SetColor(m_Feature.pNPCInfo->nTexColor);
			}		

			// 인간형 피부색
			if (m_Feature.nSkinColor != 0xFFFFFFFF)
			{
				XChangeNPCColor::SetSkin(m_pActor, m_Feature.nSkinColor);
			}

			// 머리색
			if(m_Feature.nHairColor != 0xFFFFFFFF)
			{
				XChangeNPCColor::SetHair(m_pActor, m_Feature.nHairColor);
			}

			// 눈동자색
			if(m_Feature.nHairColor != 0xFFFFFFFF)
			{
				XChangeNPCColor::SetEye(m_pActor, m_Feature.nEyeColor);
			}
		}

		m_pActor->SetVisible( true );

		RActorNode* pBPartNode = m_pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(m_strBPartNodeName).c_str());
		if(pBPartNode)
			pBPartNode->SetVisibility(1.0f);
	}
}


void XNPCSerializeStrategy::SetNPCInfoSubMtrl( int NPCID )
{
	XNPCInfo* pNPCInfo = info.npc->Get(NPCID);
	if (pNPCInfo == NULL)	return;

	for ( int i = 0; i < (int)pNPCInfo->vecMtrls.size(); ++i )
	{
		for ( int j = 0; j < pNPCInfo->vecMtrls[i]->nSubCount; ++j )
		{
			m_pActor->ChangeMatetial(pNPCInfo->vecMtrls[i]->nID, j, pNPCInfo->vecMtrls[i]->vecSubMtrls[j]);
		}
	}
}

std::wstring XNPCSerializeStrategy::GetMeshName( XNPCInfo* pNPCInfo )
{
	return wstring(PATH_MODEL + pNPCInfo->strMeshPath + pNPCInfo->strMeshName + L".elu");
}

std::wstring XNPCSerializeStrategy::GetItemMeshName( CSItemData* pItemData )
{
	std::wstring strItemMeshName;

	if (pItemData)
	{
		if (m_pActor->m_pMesh)
		{
			XNaming::GetPartsMeshName(strItemMeshName, pItemData->m_strMeshPath, MLocale::ConvAnsiToUTF16(m_pActor->m_pMesh->GetName().c_str()), pItemData->m_strMeshName, pItemData->m_bMeshNameBySex);
		}
		else 
		{
			strItemMeshName = pItemData->m_strMeshName + L".elu";
		}
	}
	return strItemMeshName;
}

#include "XPartsParam.h"
void XNPCSerializeStrategy::BPartSerialize( XNPCInfo* pNPCInfo, std::wstring strMeshNodeName, std::wstring strActorNodeName )
{
	std::wstring strEmpty;
	strEmpty.clear();

	XPartsParam partsParam;
	partsParam.nPartsID = 0;
	partsParam.strMeshName = GetMeshName(pNPCInfo);
	partsParam.AddPart(strMeshNodeName, strEmpty, strActorNodeName);

	m_pActor->CreatePartsNodeAsync( &partsParam );

	m_strBPartNodeName = strActorNodeName;
}
