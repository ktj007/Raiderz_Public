#pragma once

#include "SCommObjectManager.h"

class ZAppServerInfo;
class ZAppServerObject;

class ZAppServerObjectManager : public SCommObjectManager
{
public:
	typedef SCommObjectMap::const_iterator	client_iterator;

public:
	ZAppServerObjectManager();
	~ZAppServerObjectManager();
	virtual SCommObject* NewObject(MUID& uid);		// 사용하지 않음
	ZAppServerObject* NewAppServerObject(MUID uid, ZAppServerInfo* pAppServerInfo);

	void AddObject(SCommObject* pClient);
	void DeleteObject(MUID uidClient);
	void DeleteAllObject();
	void Clear();

	size_t GetObjectIDCount() { return m_AppServerObjectIDMap.size(); }
	
	ZAppServerObject* FindAppServer(MUID uid);
	ZAppServerObject* FindAppServerFromServerID(int nAppServerID);

	client_iterator GetClientBegin(void);
	client_iterator GetClientEnd(void);

private:
	typedef map<int, ZAppServerObject*>	ZServerObjectIDMap;
	ZServerObjectIDMap					m_AppServerObjectIDMap;
};
