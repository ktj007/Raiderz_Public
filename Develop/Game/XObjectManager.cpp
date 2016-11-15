#include "stdafx.h"
#include "XObjectManager.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XBPartNonPlayer.h"
#include "XEffect.h"
#include "XProjectile.h"
#include "XSceneManager.h"
#include "XProfiler.h"
#include "XMagicArea.h"
#include "XModuleUI.h"
#include "XCutScenePlayer.h"

XObject* XObjectManager::CreateObject(XObjectID nObjectID, MUID uid, UIID nUIID)
{
	switch (nObjectID)
	{
	case XOBJ_MY_PLAYER:		return new XMyPlayer(uid, nUIID);
	case XOBJ_NET_PLAYER:		return new XNetPlayer(uid, nUIID);
	case XOBJ_NPC:				return new XNonPlayer(uid, nUIID);
	case XOBJ_PROJECTILE:		return new XProjectile(uid);
	case XOBJ_MAGIC_AREA:		return new XMagicArea(uid);
	case XOBJ_CUTSCENE_PLAYER:	return new XCutScenePlayer(uid, L"none");
	case XOBJ_BPART_NPC:		return new XBPartNonPlayer(uid, nUIID);
	}

	_ASSERT(0);
	return NULL;
}

XObjectManager::XObjectManager()
{

}

XObjectManager::~XObjectManager()
{
	Destroy();
}

bool XObjectManager::Create()
{

	return true;
}

void XObjectManager::Destroy()
{
	DestroyAllObject();
}

bool XObjectManager::Add(XObject* pObject)
{
	if (pObject == NULL) return false;

	MUID uidObject = pObject->GetUID();

	if (Find(uidObject) != NULL) return false;


	// 만약에 삭제 예약된 것이 있으면 취소
	CancelReserveDelete(uidObject);

	if (IsActor(pObject))
	{
		XActor* pActor = static_cast<XActor*>(pObject);
		AddPlayerUIID(pActor);
	}

	m_ObjectMap.insert(XObjectMap::value_type(uidObject, pObject));

	// 각 ID 별로 입력
	AddObjectByID(pObject);

	return true;
}

XObjectMap::iterator XObjectManager::_Erase( XObject* pObject, bool bRemoveUIID )
{
	if (bRemoveUIID)
	{
		RemoveUIID(pObject);
	}

	MUID uidObject = pObject->GetUID();

	XObjectMap::iterator itor = m_ObjectMap.find(uidObject);
	if (itor != m_ObjectMap.end())
	{
		// 각 ID 별로 삭제
		EraseObjectByID(pObject);

		return m_ObjectMap.erase(itor);
	}
	return itor;
}

XObjectMap::iterator XObjectManager::Erase(XObject* pObject)
{
	CancelReserveDelete(pObject->GetUID());

	return _Erase(pObject, true);
}

XObject* XObjectManager::Find(const MUID& uidObject)
{
	XObjectMap::iterator itor = m_ObjectMap.find(uidObject);
	if (itor != m_ObjectMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

XObject* XObjectManager::FindObject(const MUID& uidObject)
{
	return Find(uidObject);
}

XPlayer* XObjectManager::FindPlayer(const MUID& uidPlayer)
{
	if(uidPlayer == MUID::ZERO)
		return NULL;

	XPlayer* pPlayer = NULL;

	pPlayer = FindNetPlayer(uidPlayer);
	if(pPlayer == NULL && uidPlayer == XGetMyUID())
		pPlayer = FindMyPlayer();
		
	return pPlayer;
}

XPlayer* XObjectManager::FindPlayer(const wchar_t* szName)
{
	if (NULL == szName) return NULL;

	for (map<MUID, XObject*>::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;
		if (NULL == pObject) continue;
		if (ETID_PLAYER != pObject->GetEntityType()) continue;

		XPlayer* pPlayer = static_cast<XPlayer*>(pObject);
		if (0 != wcscmp(szName, pPlayer->GetName())) continue;

		return pPlayer;
	}

	return NULL;
}

XActor* XObjectManager::FindActor_UIID( const UIID nUIID )
{
	map<UIID, XActor*>::iterator itor = m_PlayerUIIDMap.find(nUIID);
	if (itor != m_PlayerUIIDMap.end())
	{
		return ((*itor).second);
	}
	return NULL;
}

XPlayer* XObjectManager::FindPlayer_UIID( const UIID nUIID )
{
	XActor* pActor = FindActor_UIID(nUIID);
	if (pActor)
	{
		if (ETID_PLAYER != pActor->GetEntityType()) return NULL;

		XPlayer* pPlayer = dynamic_cast<XPlayer*>(pActor);
		return pPlayer;
	}
	return NULL;
}

XPlayer* XObjectManager::FindPlayerInPvP(const wchar_t* szName)
{
	if (NULL == szName) return NULL;

	for (map<MUID, XObject*>::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;
		if (NULL == pObject) continue;
		if (ETID_PLAYER != pObject->GetEntityType()) continue;

		XPlayer* pPlayer = static_cast<XPlayer*>(pObject);
		if (0 != wcscmp(szName, pPlayer->GetRealNameInPvP())) continue;

		return pPlayer;
	}

	return NULL;
}

XNetPlayer* XObjectManager::FindNetPlayer(const MUID& uidPlayer)
{
	if(uidPlayer == MUID::ZERO)
		return NULL;

	XObjectMap * pObjectMap = GetObjectMap(XOBJ_NET_PLAYER);
	if(pObjectMap == NULL)
		return NULL;

	if(pObjectMap->empty() == false)
	{
		XObjectMap::iterator itor = pObjectMap->find(uidPlayer);
		if(itor != pObjectMap->end())
		{
			XObject* pObject = (*itor).second;
			return pObject->AsNetPlayer();
		}
	}

	return NULL;
}

XActor* XObjectManager::FindActor(const MUID& uidActor)
{
	// TODO: 일단 이렇게..-_- - birdkr(2007-02-16)
	XObjectMap::iterator itor = m_ObjectMap.find(uidActor);
	if (itor != m_ObjectMap.end())
	{
		XObject* object = (*itor).second;
		if (MIsDerivedFromClass(XActor, object)) return static_cast<XActor*>(object);
	}
	return NULL;
}

XNonPlayer* XObjectManager::FindNPC(const MUID& uidNPC)
{
	if(uidNPC == MUID::ZERO)
		return NULL;

	XObjectMap * pObjectMap = GetObjectMap(XOBJ_NPC);
	if(pObjectMap == NULL)
		return NULL;

	if(pObjectMap->empty() == false)
	{
		XObjectMap::iterator itor = pObjectMap->find(uidNPC);
		if(itor != pObjectMap->end())
		{
			XObject* pObject = (*itor).second;
			return pObject->AsNPC();
		}
	}

	return NULL;
}

XNonPlayer* XObjectManager::FindNPC_UIID( const UIID nUIID )
{
	XActor* pActor = FindActor_UIID(nUIID);
	if (pActor)
	{
		if (ETID_NPC != pActor->GetEntityType()) return NULL;

		XNonPlayer* pPlayer = dynamic_cast<XNonPlayer*>(pActor);
		return pPlayer;
	}
	return NULL;
}

XNonPlayer* XObjectManager::FindNPC_ID( const int nNPCID)
{
	XObjectMap* pObjectMap = GetObjectMap(XOBJ_NPC);
	if(pObjectMap == NULL)	return NULL;

	if(pObjectMap->empty() == false)
	{
		for(XObjectMap::iterator itObj = pObjectMap->begin(); itObj != pObjectMap->end(); ++itObj)
		{
			XObject* pObject = itObj->second;
			XNonPlayer* pPlayer = pObject->AsNPC();
			if(pPlayer->GetNPCID() == nNPCID) return pPlayer;
		}
	}

	return NULL;
}

XMyPlayer* XObjectManager::FindMyPlayer()
{
	XObjectMap * pObjectMap = GetObjectMap(XOBJ_MY_PLAYER);
	if(pObjectMap == NULL)
		return NULL;

	if(pObjectMap->empty() == false)
	{
		// 내 오브젝트는 하나 이므로 첫번째에 있는건 당연하다.
		XObjectMap::iterator itor = pObjectMap->begin();
		if(itor != pObjectMap->end())
		{
			XObject* pObject = (*itor).second;
			return pObject->AsMyPlayer();
		}
	}

	return NULL;
}

void XObjectManager::Update(float fDelta)
{
	PFC_THISFUNC;

	UpdateReservedDelObjects(fDelta);

	vector<XObject*> vecDelObject;

	for(XObjectMapByID::iterator itor = m_mapObjectList.begin(); itor != m_mapObjectList.end(); ++itor)
	{
		XObjectMap& mapObject = itor->second;
		for(XObjectMap::iterator itObj = mapObject.begin(); itObj != mapObject.end(); ++itObj)
		{
			XObject* pObject = itObj->second;

			pObject->Update(fDelta);

			if (pObject->m_bDeleteMe)
			{
				vecDelObject.push_back(pObject);
			}
			else
			{
				// 업데이트 이후에 발동 되는 업데이트
				// 애니메이션 업데이트가 되어 있으므로 
				// bone, actornode 위치 가져오는데... 여기서 처리하는게 좋다.
				pObject->PostUpdate(fDelta);
			}

			//++itObj;
		}
	}

	// 삭제
	// 오브젝트 삭제는 모든 업데이트가 끝나고 하자. 
	// 업데이트 순서에 의해서 앞쪽 오브젝트를 참조를 할때 보장하기 위해서이기도 하고 
	// 현재 업데이트 구조로는 삭제시 문제가 되기 때문이다.
	for(vector<XObject*>::iterator itDel = vecDelObject.begin(); itDel != vecDelObject.end(); ++itDel)
	{
		_Erase(*itDel, true);
		DestroyAndDropObject(*itDel);
	}

	vecDelObject.clear();
}

void XObjectManager::PostEvent(MUID uidObject, XEvent& msg)
{
	XObject* pObj = Find(uidObject);
	if (pObj)
	{
		pObj->Event(msg);
	}
}

void XObjectManager::BroadcastEvent(XEvent& msg)
{
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		(*itor).second->Event(msg);
	}
}

void XObjectManager::PostDelayEvent(MUID uidObject, XEvent& msg, unsigned int nDelayTime)
{
	XObject* pObj = Find(uidObject);
	if (pObj)
	{
		pObj->PostDelayEvent(msg, nDelayTime);
	}
}

void XObjectManager::DestroyAllObject(MUID uidExcept)
{
	// uidExcept가 있으면 일단 빼고
	XObject* pExceptObject = NULL;

	if (uidExcept != MUID::Invalid())
	{
		pExceptObject = Find(uidExcept);

		if (pExceptObject)
		{
			Erase(pExceptObject);
		}
	}

	// 나머진 모두 삭제
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;

		DestroyAndDropObject(pObject);
	}

	m_ObjectMap.clear();
	m_PlayerUIIDMap.clear();
	m_ReservedDelObjectList.clear();
	m_mapObjectList.clear();

	// 그리고 다시 추가
	if (pExceptObject)
	{
		Add(pExceptObject);
	}
}

void XObjectManager::Render()
{
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); )
	{
		XObject* pObject = (*itor).second;

		pObject->Render();
		++itor;
	}
}

int XObjectManager::GetCount(ENTITY_TYPEID nEntityType)
{
	int cnt = 0;
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;
		if (pObject->GetEntityType() == nEntityType) cnt++;
	}
	return cnt;
}

void XObjectManager::GetEntityTypeList(vector<MUID>& vecUID, ENTITY_TYPEID nEntityType)
{
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;
		if (pObject->GetEntityType() == nEntityType)
		{
			vecUID.push_back(pObject->GetUID());
		}
	}
}

void XObjectManager::GetEntityTypeList(vector<XObject*>& vecObj, ENTITY_TYPEID nEntityType)
{
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;
		if (pObject->GetEntityType() == nEntityType)
		{
			vecObj.push_back(pObject);
		}
	}
}

void XObjectManager::OffScreenDialog()
{
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;
		if (pObject->GetEntityType() == ETID_NPC)
		{
			XModuleUI* pModuleUI = pObject->GetModuleUI();
			if(pModuleUI == NULL) continue;

			pModuleUI->Destroy();
		}
	}
}

XProjectile* XObjectManager::FindProjectile( const MUID& uidProjectile )
{
	if(uidProjectile == MUID::ZERO)
		return NULL;

	XObjectMap * pObjectMap = GetObjectMap(XOBJ_PROJECTILE);
	if(pObjectMap == NULL)
		return NULL;

	if(pObjectMap->empty() == false)
	{
		XObjectMap::iterator itor = pObjectMap->find(uidProjectile);
		if(itor != pObjectMap->end())
		{
			XObject* pObject = (*itor).second;
			return static_cast<XProjectile*>(pObject);
		}
	}

	return NULL;
}

XMagicArea* XObjectManager::FindMagicArea( const MUID& uidMagicArea )
{
	XObjectMap::iterator itor = m_ObjectMap.find(uidMagicArea);
	if (itor != m_ObjectMap.end())
	{
		XObject* object = (*itor).second;
		if (MIsExactlyClass(XMagicArea, object)) return static_cast<XMagicArea*>(object);
	}

	return NULL;
}

template<class T>
struct NearObj
{
	bool operator()(const T& Obj1,const T& Obj2) const
	{
		vec3 vPos = gvar.Game.pMyPlayer->GetPosition();
		vec3 vObj1Pos = Obj1->GetPosition();
		vec3 vObj2Pos = Obj2->GetPosition();
		float fObj1Distance = vPos.DistanceTo(vObj1Pos);
		float fObj2Distance = vPos.DistanceTo(vObj2Pos);

		return fObj1Distance < fObj2Distance;
	}
};

void XObjectManager::GetSortDistanceEntityTypeList(vector<XObject*>& vecObj, ENTITY_TYPEID nEntityType)
{
	GetEntityTypeList(vecObj, nEntityType);
	sort(vecObj.begin(), vecObj.end(), NearObj<XObject*>());
}

XCutScenePlayer* XObjectManager::FindCutScenePlayer( const MUID& uidCutScenePlayer )
{
	XObjectMap::iterator itor = m_ObjectMap.find(uidCutScenePlayer);
	if (itor != m_ObjectMap.end())
	{
		XObject* object = (*itor).second;
		if (MIsExactlyClass(XCutScenePlayer, object)) return static_cast<XCutScenePlayer*>(object);
	}

	return NULL;
}

void XObjectManager::AddPlayerUIID( XActor* pActor )
{
	UIID nUIID = pActor->GetUIID();
	if (nUIID != UIID_INVALID)
	{
		XActor* pExistActor = FindActor_UIID(nUIID);
		if (pExistActor)
		{
			RemovePlayerUIID(nUIID);
		}

		m_PlayerUIIDMap.insert(map<UIID, XActor*>::value_type(nUIID, pActor));
	}
}

void XObjectManager::RemovePlayerUIID( UIID nUIID )
{
	map<UIID, XActor*>::iterator itor = m_PlayerUIIDMap.find(nUIID);
	if (itor != m_PlayerUIIDMap.end())
	{
		m_PlayerUIIDMap.erase(itor);
	}
}

bool XObjectManager::IsActor( XObject* pObject )
{
	if (pObject->GetType() == XOBJ_NET_PLAYER ||
		pObject->GetType() == XOBJ_MY_PLAYER ||
		pObject->GetType() == XOBJ_NPC ||
		pObject->GetType() == XOBJ_BPART_NPC)
	{
		return true;
	}
	return false;
}


void XObjectManager::RemoveUIID( XObject* pObject )
{
	if (IsActor(pObject))
	{
		XActor* pActor = static_cast<XActor*>(pObject);
		RemovePlayerUIID(pActor->GetUIID());
	}
}

void XObjectManager::ReserveDelete( MUID uidObject, float fDelayTime )
{
	XObjectMap::iterator itor = m_ObjectMap.find(uidObject);
	if (itor != m_ObjectMap.end())
	{
		XObject* pObject = (*itor).second;

		_ReservedDelObject reservedDelObject;
		reservedDelObject.uidObject = uidObject;
		reservedDelObject.fDelayTime = fDelayTime;

		if (IsActor(pObject))
		{
			XActor* pActor = static_cast<XActor*>(pObject);
			reservedDelObject.nUIID = pActor->GetUIID();
		}
		else
		{
			reservedDelObject.nUIID = UIID_INVALID;
		}

		m_ReservedDelObjectList.push_back(reservedDelObject);

		RemoveUIID(pObject);
	}

}

void XObjectManager::DestroyAndDropObject( XObject* pObject )
{
	pObject->Destroy();
	pObject->Drop();
}

void XObjectManager::UpdateReservedDelObjects(float fDeltaTime)
{
	for (list<_ReservedDelObject>::iterator itor = m_ReservedDelObjectList.begin();
		itor != m_ReservedDelObjectList.end(); )
	{
		_ReservedDelObject& reservedDelObject = (*itor);
		reservedDelObject.fDelayTime -= fDeltaTime;

		if (reservedDelObject.fDelayTime <= 0.0f)
		{
			XObject* pObject = Find(reservedDelObject.uidObject);

			if (pObject)
			{
				_Erase(pObject, false);

				DestroyAndDropObject(pObject);
			}

			itor = m_ReservedDelObjectList.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void XObjectManager::CancelReserveDelete( MUID uidObject )
{
	if (m_ReservedDelObjectList.empty()) return;

	for (list<_ReservedDelObject>::iterator itor = m_ReservedDelObjectList.begin();
		itor != m_ReservedDelObjectList.end(); ++itor)
	{
		_ReservedDelObject& reservedDelObject = (*itor);

		if (uidObject == reservedDelObject.uidObject)
		{
			XObject* pObject = Find(uidObject);

			if (pObject && IsActor(pObject))
			{
				XActor* pActor = static_cast<XActor*>(pObject);
				AddPlayerUIID(pActor);
			}

			m_ReservedDelObjectList.erase(itor);
			return;
		}
	}

}

void XObjectManager::AddObjectByID( XObject* pObject )
{
	if(pObject == NULL)
		return ;

	XObjectMapByID::iterator it = m_mapObjectList.find(pObject->GetType());
	if(it != m_mapObjectList.end())
	{
		// 이미 등록 되어 있다.
		it->second.insert(XObjectMap::value_type(pObject->GetUID(), pObject));
	}
	else 
	{
		// 등록이 되어 있지 않다.
		XObjectMap	mapObject;
		mapObject.insert(XObjectMap::value_type(pObject->GetUID(), pObject));
		m_mapObjectList.insert(XObjectMapByID::value_type(pObject->GetType(), mapObject));
	}
}

void XObjectManager::EraseObjectByID( XObject* pObject )
{
	if(pObject == NULL)
		return ;

	XObjectMapByID::iterator it = m_mapObjectList.find(pObject->GetType());
	if(it != m_mapObjectList.end())
	{
		XObjectMap::iterator itObject = it->second.find(pObject->GetUID());
		if(itObject != it->second.end())
			it->second.erase(itObject);
	}
}

XObjectMap * XObjectManager::GetObjectMap( XObjectID eID )
{
	XObjectMapByID::iterator it = m_mapObjectList.find(eID);
	if(it != m_mapObjectList.end())
		return &(it->second);

	return NULL;
}

void XObjectManager::PostFrameRender()
{
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); )
	{
		XObject* pObject = (*itor).second;

		pObject->OnPostRender();
		++itor;
	}
}

void XObjectManager::ReloadObjectAll()
{
	for (XObjectMap::iterator itor = m_ObjectMap.begin(); itor != m_ObjectMap.end(); ++itor)
	{
		XObject* pObject = (*itor).second;

		pObject->Reload();
	}
}

XBPartNonPlayer* XObjectManager::FindBPartNPC( const MUID& uidBPartNPC )
{
	if(uidBPartNPC == MUID::ZERO)
		return NULL;

	XObjectMap * pObjectMap = GetObjectMap(XOBJ_BPART_NPC);
	if(pObjectMap == NULL)
		return NULL;

	if(pObjectMap->empty() == false)
	{
		XObjectMap::iterator itor = pObjectMap->find(uidBPartNPC);
		if(itor != pObjectMap->end())
		{
			XObject* pObject = (*itor).second;
			return pObject->AsBPartNPC();
		}
	}

	return NULL;
}
