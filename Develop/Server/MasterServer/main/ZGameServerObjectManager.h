#pragma once

#include "SCommObjectManager.h"
#include <unordered_map>

class ZGameServerObject;
class ZGameServerInfo;
struct TD_LOGIN_FIELD_INFO;

/// Client 관리자
class ZGameServerObjectManager : public SCommObjectManager
{
public:
	typedef tr1::unordered_map<int, ZGameServerObject*>	ZServerObjectIDMap;

protected:
	ZServerObjectIDMap						m_GameServerObjectIDMap;
	
public:
	ZGameServerObjectManager();
	virtual ~ZGameServerObjectManager();
	virtual SCommObject*	NewObject(MUID& uid);		// 사용하지 않음
	ZGameServerObject*		NewGameServerObject(MUID uid, ZGameServerInfo* pGameServerInfo);

	virtual void			AddObject(SCommObject* pClient);
	virtual void			DeleteObject(MUID uidClient);
	virtual void			DeleteAllObject();
	virtual void			Clear();

	ZServerObjectIDMap&		GetGameServerObjectIDMap()	{ return m_GameServerObjectIDMap; }
	size_t					GetObjectIDCount()			{ return m_GameServerObjectIDMap.size(); }

	ZGameServerObject*		GetFirstGameServer(void);
	ZGameServerObject*		FindGameServer(MUID uid);
	ZGameServerObject*		FindGameServer(int nGameServerID);
	ZGameServerObject*		FindAnotherGameServer(int nIgnoreServerID); ///< 서버 이동 테스트용으로 작성된 함수입니다. - aibeast

	MUID					IDToUID(int nID);
	int						UIDToID(MUID nUID);
};
