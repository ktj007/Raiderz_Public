#pragma once

#include <string>
#include <vcclr.h>
#include <set>
#include "RClothActorNode.h"
#include ".\reflectiveProperty\ReflectiveRClothInfo.h"
#include ".\reflectiveProperty\ClothReflectivePropertyForm.h"

class TCloth
{
public:
	TCloth(int slot,int ItemID,std::string ItemName,std::string ItemMeshName,RClothActorNode* pRClothActorNode);
	~TCloth(void);

public:
	bool	createClothMenu(std::string menuName);

public:
	//-[ 옷의 ActorNode ]
	void					setRClothActorNode(RClothActorNode* pRClothActorNode);
	RClothActorNode*		getRClothActorNode(void)	{	return m_pRClothActorNode;	}

	//-[ 옷의 식별자 용 변수 ]
	void					setIdentifier(int ItemID,std::string ItemName,std::string ItemMeshName);
	int						getID(void)				{	return m_ItemID;		}
	std::string				getItemName(void)		{	return m_ItemName;		}
	std::string				getItemMeshName(void)	{	return m_ItemMeshName;	}

	//-[ 옷의 식별자 용 변수 (from 'MainApp.cpp') ]
	void					setSlotNumber(int slot)	{	m_slot = slot;	}
	int						getSlotNumber(void)		{	return m_slot;	}

public:
	typedef std::set<int> SetOfID;

protected:
	//-[ c# Form ]
	gcroot<Character_NET::ClothReflectivePropertyForm^> m_ClothReflectiveProperty;

	//-[ Reflection 기능을 위한 변수 ]
	ReflectiveRClothInfo	m_ReflectiveRClothInfo;

	//-[ 옷의 ActorNode ]
	RClothActorNode*	m_pRClothActorNode;

	//-[ 옷의 식별자 용 변수 (from 'CSItemData.h') ]  : 아이템 식별할때 쓸려고 저장하기 위한 변수
	int				m_ItemID;			//아이템의 고유한 ID
	string			m_ItemName;			//아이템의 이름
	string			m_ItemMeshName;		//아이템 메시이름

	//-[ 옷의 식별자 용 변수 (from 'MainApp.cpp') ]
	int				m_slot;	//툴 슬롯 번호

};

class TClothMgr
{
public:
	TClothMgr();
	~TClothMgr();
public:
	typedef list<TCloth*>					CLOTH_CONTAINER;
	typedef CLOTH_CONTAINER::iterator		CLOTH_ITERATOR;

public:
	//캐릭터 장착 옷
	bool	createClothMenuForCharacterItem(int slot,int ItemID,std::string ItemName,std::string ItemMeshName,RClothActorNode* pRClothActorNode);
	void	releaseClothMenuForCharacterItem(void);
	int		removeClothIfSlotIsSame(int slot);	//지워진 옷아이템의 ID를 반환

	//Static 메쉬와 연결된 옷
	bool	createClothMenuForStaticMesh(RClothActorNode* pRClothActorNode);
	void	releaseClothMenuForStaticMesh(void);

	// Review를 위한 장착 옷
	bool	createClothMenuForCharacterReview(int slot,std::string MeshName,RClothActorNode* pRClothActorNode);


private:
	//캐릭터 장착 옷
	TCloth* addClothForCharacterItem(int Slot,int ItemID,std::string ItemName,std::string ItemMeshName,RClothActorNode* pRClothActorNode);
	void	removeClothForCharacterItem(CLOTH_ITERATOR iter);

	//Static 메쉬와 연결된 옷
	TCloth* addClothForStaticMesh(RClothActorNode* pRClothActorNode);

private:
	CLOTH_CONTAINER		m_clothesForCharacterItem;	//캐릭터 장착 옷
	CLOTH_CONTAINER		m_clothesForStaticMesh;		//Static 메쉬와 연결된 옷
};	