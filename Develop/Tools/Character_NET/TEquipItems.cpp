#include "stdafx.h"
#include "TEquipItems.h"
//#include "TItem.h"
//#include "TTabModel.h"

enum REVIEW_ITEM_NODE_LIST
{
	RINL_BASE,
	RINL_ITEM,
	RINL_CLOTH,
};

TEquipItems::TEquipItems()
{
	m_pActor		= NULL;
	m_pItemManager	= NULL;
	m_pTabModel		= NULL;
	m_pClothMgr		= NULL;

	m_ItemSlotnum	= ITEMSLOT_MAX;
	m_WeaponType	= WEAPON_TYPE_MAX;
}

bool TEquipItems::SaveEquipItems()
{
	return m_TEquipment.SaveEquipment();
}

void TEquipItems::LoadEquipItems(TCharacter* pActor)
{
	if(pActor == NULL ||
		m_pItemManager == NULL)
		return;

	m_pActor = pActor;

	string name = GetPureFileName(m_pActor->m_pMesh->GetName());
	if( name == HUMAN_FEMALE || name == HUMAN_MALE )
	{
		if(m_TEquipment.LoadEquipment())
			m_TEquipment.DefaultItemEquip(m_pTabModel->GetPlayerType());
	}

	if (m_TEquipment.IsEquip(ITEMSLOT_FACE, m_pTabModel->GetPlayerType()) == false)
	{
		DefaultFaceLoad();
	}

	if (m_TEquipment.IsEquip(ITEMSLOT_CHARM, m_pTabModel->GetPlayerType()) == false)
	{
		DefaultHairLoad();
	}
}

void TEquipItems::DefaultFaceLoad()
{
	string name = GetPureFileName(m_pActor->m_pMesh->GetName());
	if( name == HUMAN_FEMALE )
	{
		EquipReviewItems("hf_face_01", "face", ITEMTYPE_ARMOR);
	}
	else if( name == HUMAN_MALE )
	{
		EquipReviewItems("hm_face_01", "face", ITEMTYPE_ARMOR);
	}
}

void TEquipItems::DefaultHairLoad()
{
	string name = GetPureFileName(m_pActor->m_pMesh->GetName());
	if( name == HUMAN_FEMALE )
	{
		EquipReviewItems("hf_hair_01", "hair", ITEMTYPE_ARMOR);
	}
	else if( name == HUMAN_MALE )
	{
		EquipReviewItems("hm_hair_01", "hair", ITEMTYPE_ARMOR);
	}
}

void TEquipItems::EquipReviewItems( System::String^ MeshName, System::String^ NodeName, SH_ITEM_TYPE iType /*= ITEMTYPE_WEAPON*/ )
{
	const char* cMeshName = MStringToCharPointer(MeshName);
	const char* cNodeName = MStringToCharPointer(NodeName);

	EquipReviewItems(cMeshName, cNodeName, iType);

	MFreeCharPointer(cNodeName);
	MFreeCharPointer(cMeshName);
}

void TEquipItems::EquipReviewItems( const char* MeshName, const char* NodeName, SH_ITEM_TYPE iType /*= ITEMTYPE_WEAPON*/ )
{
	string strMeshName = MeshName;
	string strNodeName = NodeName;

	m_ItemSlotnum = g_pMainApp->GetNodetoItemslot(strNodeName);
	m_ItemSlotnum = GetClothItemSlot(strNodeName, m_ItemSlotnum);

	if (m_ItemSlotnum == ITEMSLOT_MAX) return;

	if (m_ItemSlotnum == ITEMSLOT_LWEAPON ||
		m_ItemSlotnum == ITEMSLOT_RWEAPON ||
		m_ItemSlotnum == ITEMSLOT_LWEAPON2 ||
		m_ItemSlotnum == ITEMSLOT_RWEAPON2 )
	{
		iType = ITEMTYPE_WEAPON;
	}

	if( iType == ITEMTYPE_ARMOR)
	{
		SH_ITEM_SLOT islot = m_ItemSlotnum;
		ReviewEquipItem(strMeshName, strNodeName, m_strItemParentName, islot, WEAPON_NONE);
	}
	else if( iType == ITEMTYPE_WEAPON)
		ReviewWeaponItem(strMeshName);
}

void TEquipItems::ReviewWeaponItem( tstring MeshName )
{
	m_TEquipment.EquipItemName(MeshName);
}

void TEquipItems::ReviewEquipItem( tstring MeshName, tstring NodeName, tstring ParentName, SH_ITEM_SLOT islot, WEAPON_TYPE itype )
{
	if( islot == ITEMSLOT_MAX)
	{
		mlog("에러 : 장착불가능합니다. \r");
		return;
	}
	
	int nSetNodeList = RINL_BASE;

	bool bCloth = CheckClothItem(NodeName);

	tstring MeshName_Make = MeshName;
	tstring NodeName_Make = NodeName;
	SH_ITEM_SLOT islot_Make = islot;
	WEAPON_TYPE itype_Make = itype;

	if(bCloth)
	{
		SetBitSet(nSetNodeList, RINL_CLOTH);
		SetBitSet(nSetNodeList, RINL_ITEM);

		NodeName_Make = GetRemoveUnderBarlast(NodeName) + "_item";
	}
	else 
		SetBitSet(nSetNodeList, RINL_ITEM);
	
	if (m_pActor->GetEquipItems()[islot].pItemData)
	{
		CSItemData* pItemData = (CSItemData*)m_pActor->GetEquipItems()[islot].pItemData;
		if (MeshName == pItemData->m_strMeshName &&
			GetRemoveUnderBarlast(NodeName) == pItemData->GetMainNodeName())
		{
			MeshName_Make = pItemData->m_strMeshName;
			NodeName_Make = pItemData->m_strName;
			islot_Make = pItemData->m_nItemSlot;
			itype_Make = pItemData->m_nWeaponType;

			for(vector<string>::iterator it = pItemData->m_strNodeNames.begin(); it != pItemData->m_strNodeNames.end(); ++it)
			{
				if(*it == string(GetRemoveUnderBarlast(*it) + "_cloth"))
				{
					if(bCloth)
						ClearBitSet(nSetNodeList, RINL_CLOTH);
					else 
						SetBitSet(nSetNodeList, RINL_CLOTH);
				}
				else if(*it == string(GetRemoveUnderBarlast(*it) + "_item"))
				{
					if(bCloth)
						SetBitSet(nSetNodeList, RINL_ITEM);
					else
						ClearBitSet(nSetNodeList, RINL_ITEM);
				}
			}

			UnEquipItem(islot);

			m_pClothMgr->removeClothIfSlotIsSame(islot);

			if(CheckBitSet(nSetNodeList, RINL_CLOTH) == false &&
				CheckBitSet(nSetNodeList, RINL_ITEM) == false)
				return;
		}
	}

	if(bCloth == false &&
		CheckBitSet(nSetNodeList, RINL_CLOTH))
		SetBitSet(nSetNodeList, RINL_ITEM);

	CSItemData* pItem = MakeItem(NodeName_Make, MeshName_Make, islot_Make, itype_Make, nSetNodeList);	
	m_pActor->EquipItem(*pItem, islot);

	m_ItemSlotnum	= ITEMSLOT_MAX;
	m_WeaponType	= WEAPON_NONE;

	m_TEquipment.SetItemEquipInfo(pItem, islot);

	if(CheckBitSet(nSetNodeList, RINL_CLOTH))
		ReviewEquipCloth(MeshName, NodeName, islot);
}

void TEquipItems::ReviewEquipCloth( tstring MeshName, tstring NodeName, SH_ITEM_SLOT islot )
{
	//////////////////////////////////////////////////////////////////////////
	// 여기서 리뷰용 클로스 생성
	if( NULL == m_pActor ||
		m_pClothMgr == NULL)	
		return;

	RClothActorNode *pClothActorNode = NULL;

	m_pActor->UpdatePartsNode();
	vector<RActorNode*>& actorNodes = m_pActor->GetActorNodes();

	m_pClothMgr->removeClothIfSlotIsSame(islot);

	for(size_t i=0;i<actorNodes.size();i++)
	{
		if( false == actorNodes[i]->m_pMeshNode->isClothMesh() ) continue;

		pClothActorNode = static_cast<RClothActorNode*>(actorNodes[i]);
		if( NodeName != pClothActorNode->getNodeName() ) continue;
		m_pClothMgr->createClothMenuForCharacterReview(islot, MeshName,pClothActorNode);
		return ;
	}
}

void TEquipItems::UnEquipItem( string ItemCategory )
{
	SH_ITEM_SLOT index = g_pMainApp->GetNodetoItemslot(ItemCategory);
	UnEquipItem(index);
}

void TEquipItems::UnEquipItem( int nSlot, bool bResetToDefault /*= true*/ )
{
	if( nSlot == ITEMSLOT_MAX ||
		m_pActor == NULL) 
		return;

	m_pActor->UnequipItem(nSlot, bResetToDefault);
	m_pActor->GetEquipItems()[nSlot].pItemData = NULL;
	m_TEquipment.SetUnEquipInfo(nSlot, m_pTabModel->GetPlayerType());
}

CSItemData* TEquipItems::MakeItem( tstring NodeName, tstring MeshName, SH_ITEM_SLOT islot, WEAPON_TYPE itype, int nBitNodeList /*= 0 */ )
{
	// TODO : 아이디를 해쉬스트링으로 하고
	// 같은 아이디로 검색해서 있으면 가져다 쓰도록 바꾸자.
	CSItemData* pItem		= m_pItemManager->MakeItemData(m_pItemManager->GetDataMap().size());
	pItem->m_strName		= NodeName;			
	pItem->m_strDesc		= NodeName;			

	pItem->m_strMeshName	= MeshName;	//확장자는 제외시켜야된다.
	pItem->m_bMeshNameBySex	= 0;		//성별로 다른 아이템을 갖는 

	pItem->m_strNodeNames.push_back(GetRemoveUnderBarlast(NodeName));

	if(CheckBitSet(nBitNodeList, RINL_ITEM))
		pItem->m_strNodeNames.push_back(NodeName);

	if(CheckBitSet(nBitNodeList, RINL_CLOTH))
		pItem->m_strNodeNames.push_back(string(GetRemoveUnderBarlast(NodeName) + "_cloth"));

	if(islot == ITEMSLOT_FACE)
		pItem->m_strNodeNames.push_back("eye");

	pItem->m_nItemSlot		= islot;
	pItem->m_nSubSlot		= pItem->m_nItemSlot;
	pItem->m_ItemType		= ITEMTYPE_ARMOR;			
	pItem->m_nWeaponType	= itype;

	//리뷰용 아이템은 기본으로 이렇게...
	pItem->m_nArmorType = ARMOR_NONE;

	return pItem;
}

bool TEquipItems::EquipItemID( int itemid, int nSlot )
{
	bool bReturn = m_TEquipment.EquipItemID(itemid, nSlot);

	EquipSimulatedCloth(itemid, nSlot);

	return bReturn;
}

bool TEquipItems::EquipItemSlotIndex( int index )
{
	return m_TEquipment.EquipItemSlotIndex(index);
}

void TEquipItems::EquipSimulatedCloth( int itemid, int nSlot )
{
	if( NULL == m_pActor ||
		m_pClothMgr == NULL)	
		return;

	CSItemData* pItem = m_pItemManager->GetItemData(itemid);
	RClothActorNode *pClothActorNode = NULL;

	m_pActor->UpdatePartsNode();
	vector<RActorNode*>& actorNodes = m_pActor->GetActorNodes();

	m_pClothMgr->removeClothIfSlotIsSame(nSlot);

	for(size_t i=0;i<actorNodes.size();i++)
	{
		if( false == actorNodes[i]->m_pMeshNode->isClothMesh() ) continue;

		pClothActorNode = static_cast<RClothActorNode*>(actorNodes[i]);
		for(int j=0;j< pItem->m_strNodeNames.size() ;j++)
		{
			if( pItem->m_strNodeNames[j] != pClothActorNode->getNodeName() ) continue;
			m_pClothMgr->createClothMenuForCharacterItem(nSlot,pItem->m_nID,pItem->m_strName,pItem->m_strMeshName,pClothActorNode);
			return ;
		}
	}
}

void TEquipItems::CreateClothMenuForNPC( void )
{
	if(m_pActor == NULL ||
		m_pClothMgr == NULL)	
		return;

	m_pClothMgr->releaseClothMenuForStaticMesh(); //기존의 메뉴는 지움

	RClothActorNode *pClothActorNode = NULL;
	m_pActor->UpdatePartsNode();
	vector<RActorNode*>& actorNodes = m_pActor->GetActorNodes();

	for(size_t i=0;i<actorNodes.size();i++)
	{
		if( false == actorNodes[i]->m_pMeshNode->isClothMesh() ) continue;
		pClothActorNode = static_cast<RClothActorNode*>(actorNodes[i]);

		//pClothActorNode->m_fileName
		m_pClothMgr->createClothMenuForStaticMesh(pClothActorNode);
	}
}

void TEquipItems::SetEquipItemData( TItemManager* pItemManager, TTabModel* pTabModel, TClothMgr* pClothMgr )
{
	m_pItemManager	= pItemManager;
	m_pTabModel		= pTabModel;
	m_pClothMgr		= pClothMgr;
}

void TEquipItems::AllUnEquipItem()
{
	if(m_pActor == NULL)	
		return;

	for( int i = 0; i < ITEMSLOT_MAX; i++)
	{
		if (m_pActor->GetEquipItems()[i].pItemData)
		{
			CSItemData* pItemData = (CSItemData*)m_pActor->GetEquipItems()[i].pItemData;
			UnEquipItem(i);
		}
	}
}

SH_ITEM_SLOT TEquipItems::GetClothItemSlot( string& strNodeName, SH_ITEM_SLOT islot )
{
	if(CheckClothItem(strNodeName) == false)
		return islot;

	string strTemp = GetRemoveUnderBarlast(strNodeName);
	return g_pMainApp->GetNodetoItemslot(strTemp + "_item");
}

bool TEquipItems::CheckClothItem( string& strNodeName )
{
	if(GetRemoveUnderBarfirst(strNodeName) == "cloth")
		return true;

	return false;
}