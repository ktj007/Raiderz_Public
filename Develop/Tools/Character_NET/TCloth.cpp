#include "StdAfx.h"
#include "resource.h"
#include "TCloth.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;



BOOL CALLBACK MlessDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}

TCloth::TCloth(int slot,int ItemID,std::string ItemName,std::string ItemMeshName,RClothActorNode* pRClothActorNode )
{
	setSlotNumber(slot);
	setIdentifier(ItemID,ItemName,ItemMeshName);
	setRClothActorNode(pRClothActorNode);

	m_ReflectiveRClothInfo.setClothParameter(pRClothActorNode->getClothInfo()->getParameter());
}

TCloth::~TCloth( void )
{
	m_ClothReflectiveProperty->Close();
}

bool TCloth::createClothMenu(std::string menuName)
{
	std::string fileName = getRClothActorNode()->getFileName();
	m_ClothReflectiveProperty = ClothReflectivePropertyForm::create(menuName.c_str(),getRClothActorNode()->getClothInfo(),&m_ReflectiveRClothInfo,fileName.c_str());
	return true;
}	

void TCloth::setRClothActorNode( RClothActorNode* pRClothActorNode )
{
	_ASSERT(NULL != pRClothActorNode);
	m_pRClothActorNode = pRClothActorNode;
}

void TCloth::setIdentifier( int ItemID,std::string ItemName,std::string ItemMeshName )
{
	m_ItemID = ItemID;
	m_ItemName = ItemName;
	m_ItemMeshName = ItemMeshName;
}	

TCloth* TClothMgr::addClothForCharacterItem(int slot,int ItemID,std::string ItemName,std::string ItemMeshName,RClothActorNode* pRClothActorNode )
{
	TCloth *pCloth = NULL;
	pCloth = new TCloth(slot,ItemID,ItemName,ItemMeshName,pRClothActorNode);
	_ASSERT( NULL != pCloth	);
	m_clothesForCharacterItem.push_back(pCloth);

	return pCloth;
}

TCloth* TClothMgr::addClothForStaticMesh( RClothActorNode* pRClothActorNode )
{
	TCloth *pCloth = NULL;
	pCloth = new TCloth(-1,-1,"NoItem","NoItem",pRClothActorNode);
	_ASSERT( NULL != pCloth	);
	m_clothesForStaticMesh.push_back(pCloth);

	return pCloth;
}

int TClothMgr::removeClothIfSlotIsSame( int slot )
{
	int removedClothItemID = -1;
	CLOTH_ITERATOR iter;
	for(iter = m_clothesForCharacterItem.begin(); iter != m_clothesForCharacterItem.end(); iter++)
	{
		TCloth *pCloth = *iter;
		if( pCloth->getSlotNumber() == slot )	//이미 같은 슬롯에 있으면
		{
			removedClothItemID = pCloth->getID();

			removeClothForCharacterItem(iter);
			break; //이거 없으면 for문 iter++ 수정해야함
		}
	}

	return removedClothItemID;
}

void TClothMgr::removeClothForCharacterItem( CLOTH_ITERATOR iter )
{
	TCloth *pCloth = *iter;
	m_clothesForCharacterItem.erase(iter);
	delete pCloth;	
}


bool TClothMgr::createClothMenuForCharacterItem( int slot,int ItemID,std::string ItemName,std::string ItemMeshName,RClothActorNode* pRClothActorNode )
{
	int removedClothItemID = removeClothIfSlotIsSame(slot);

	if( removedClothItemID != ItemID )	// '==' 인경우는 옷을 벗을 경우
	{
		TCloth *pCloth = NULL;
		pCloth = addClothForCharacterItem(slot,ItemID,ItemName,ItemMeshName,pRClothActorNode);
		_ASSERT(NULL != pCloth);
		pCloth->createClothMenu(ItemName);
	}

	return true;
}

bool TClothMgr::createClothMenuForStaticMesh( RClothActorNode* pRClothActorNode )
{

	TCloth *pCloth = NULL;
	pCloth = addClothForStaticMesh(pRClothActorNode);
	_ASSERT(NULL != pCloth);
	pCloth->createClothMenu(pRClothActorNode->getFileName());

	return true;
}

bool TClothMgr::createClothMenuForCharacterReview(int slot, std::string MeshName,RClothActorNode* pRClothActorNode )
{
	TCloth *pCloth = NULL;
	pCloth = addClothForCharacterItem(slot, -11, "NoItem", MeshName, pRClothActorNode);
	_ASSERT(NULL != pCloth);
	pCloth->createClothMenu(pRClothActorNode->getFileName());

	return true;
}

TClothMgr::TClothMgr()
{

}

TClothMgr::~TClothMgr()
{
	releaseClothMenuForCharacterItem();	//캐릭터 장착 옷
	releaseClothMenuForStaticMesh();	//Static 메쉬와 연결된 옷	

	CPropertySystem::Release();
}

void TClothMgr::releaseClothMenuForStaticMesh( void )
{
	//Static 메쉬와 연결된 옷
	CLOTH_ITERATOR iter;
	for(iter = m_clothesForStaticMesh.begin(); iter != m_clothesForStaticMesh.end(); iter++)
	{
		TCloth *pCloth = *iter;
		if( NULL == pCloth) _ASSERT(false);
		delete pCloth;
	}
	m_clothesForStaticMesh.clear();
}

void TClothMgr::releaseClothMenuForCharacterItem( void )
{
	//캐릭터 장착 옷
	CLOTH_ITERATOR iter;
	for(iter = m_clothesForCharacterItem.begin(); iter != m_clothesForCharacterItem.end(); iter++)
	{
		TCloth *pCloth = *iter;
		if( NULL == pCloth) _ASSERT(false);
		delete pCloth;
	}
	m_clothesForCharacterItem.clear();
}
