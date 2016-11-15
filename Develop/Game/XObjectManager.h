#ifndef _XOBJECT_MANAGER_H
#define _XOBJECT_MANAGER_H

#include "MUID.h"
#include "XObject.h"
#include "XEvent.h"
#include <string>
#include <map>

using namespace std;

template <class _T>
class MNamedMap : public map<wstring, _T> {
protected:

public:
	MNamedMap() {}
	void Add(const wchar_t* szName, _T t) 
	{
		insert(value_type(wstring(szName), t));
	}
	void Delete(const wchar_t* szName)
	{
		iterator itor = m_MapList.find(szName);
		if (itor != end())
		{
			erase(itor);
		}
	}
	_T Find(const wchar_t* szName)
	{
		iterator itor = find(szName);
		return itor != end() ? (itor).second : NULL;
	}

	_T operator[] (const wchar_t* szName)
	{
		return Find(szName);
	}
};


class XPlayer;
class XNetPlayer;
class XActor;
class XNonPlayer;
class XMyPlayer;
class XProjectile;
class XMagicArea;
class XCutScenePlayer;

namespace rs3 { class RActor; }

typedef map<MUID, XObject*>				XObjectMap;
typedef map<XObjectID, XObjectMap>		XObjectMapByID;

class XObjectManager
{
private:
	void AddObjectByID(XObject* pObject);
	void EraseObjectByID(XObject* pObject);

protected:
	XObjectMap					m_ObjectMap;
	XObjectMapByID				m_mapObjectList;

	map<UIID, XActor*>			m_PlayerUIIDMap;

	struct _ReservedDelObject
	{
		MUID			uidObject;
		UIID			nUIID;
		float			fDelayTime;
	};
	list<_ReservedDelObject>	m_ReservedDelObjectList;
	
	void RemoveUIID( XObject* pObject );

	void AddPlayerUIID(XActor* pPlayer);
	void RemovePlayerUIID(UIID nPlayerUIID);
	bool IsActor( XObject* pObject );

	void DestroyAndDropObject(XObject* pObject);
	void UpdateReservedDelObjects(float fDeltaTime);

	XObjectMap::iterator _Erase(XObject* pObject, bool bRemoveUIID);

public:
	XObjectManager();
	virtual ~XObjectManager();
	bool Create();
	void Destroy();

	bool Add(XObject* pObject);
	XObjectMap::iterator Erase(XObject* pObject);

	void ReserveDelete(MUID uidObject, float fDelayTime);
	void CancelReserveDelete(MUID uidObject);

	void Update(float fDelta);

	XObjectMap::iterator BeginItor()	{ return m_ObjectMap.begin(); }
	XObjectMap::iterator EndItor()	{ return m_ObjectMap.end(); }

	static XObject*		CreateObject(XObjectID nObjectID, MUID uid, UIID nUIID=0);

	XObject*			Find(const MUID& uidObject);
	XObject*			FindObject(const MUID& uidObject);
	XPlayer*			FindPlayer(const MUID& uidPlayer);
	XPlayer*			FindPlayer(const wchar_t* szName);
	XActor*				FindActor_UIID(const UIID nUIID);
	XPlayer*			FindPlayer_UIID(const UIID nUIID);
	XPlayer*			FindPlayerInPvP(const wchar_t* szName);
	XNetPlayer*			FindNetPlayer(const MUID& uidPlayer);
	XActor*				FindActor(const MUID& uidActor);
	XNonPlayer*			FindNPC(const MUID& uidNPC);
	XNonPlayer*			FindNPC_UIID( const UIID nUIID );
	XNonPlayer*			FindNPC_ID( const int nNPCID);
	XMyPlayer*			FindMyPlayer();
	XProjectile*		FindProjectile(const MUID& uidProjectile);
	XMagicArea*			FindMagicArea(const MUID& uidMagicArea);
	XCutScenePlayer*	FindCutScenePlayer(const MUID& uidCutScenePlayer);
	XBPartNonPlayer*	FindBPartNPC(const MUID& uidBPartNPC);

	void PostEvent(MUID uidObject, XEvent& msg);
	void BroadcastEvent(XEvent& msg);
	void PostDelayEvent(MUID uidObject, XEvent& msg, unsigned int nDelayTime);
	int GetCount()			{ return (int)m_ObjectMap.size(); }
	int GetCount(ENTITY_TYPEID nEntityType);		// 성능 떨어짐. 차후 최적화 필요
	size_t GetReservedDelObjectCount()		{ return m_ReservedDelObjectList.size(); }

	void DestroyAllObject(MUID uidExcept = MUID::Invalid());
	void Render();
	void PostFrameRender();

	void OffScreenDialog();
	void GetEntityTypeList(vector<MUID>& vecUIDNPC, ENTITY_TYPEID nEntityType);
	void GetEntityTypeList(vector<XObject*>& vecObj, ENTITY_TYPEID nEntityType);
	void GetSortDistanceEntityTypeList(vector<XObject*>& vecObj, ENTITY_TYPEID nEntityType);

	void ReloadObjectAll();

	XObjectMap *		GetObjectMap(XObjectID eID);
};



#endif
