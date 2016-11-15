#pragma once

#include "SCommObjectManager.h"
#include "Log/PLog.h"

class PGameServerObject;

/// Client °ü¸®ÀÚ
class PGameServerObjectManager : public SCommObjectManager
{
public:
	typedef std::map<int, PGameServerObject*> PGameServerIDMap;
private:
	PGameServerObject*		PickUpServer();

public:
	PGameServerObjectManager();
	virtual ~PGameServerObjectManager();
	virtual SCommObject*	NewObject(MUID& uid, int nServerID);
	PGameServerObject*		Find(MUID& uid);
	PGameServerObject*		Find(int nGameServerID);

	virtual void			AddObject(SCommObject* pClient);
	virtual void			DeleteObject(MUID uidClient);
	virtual void			DeleteAllObject();
	virtual void			Clear();

	int						GetGameServerIDCount() { return (int)m_mapGameServerByID.size(); }
	MUID					GetGameServerUID(int nServerID);
	int						GetGameServerID(MUID uidServer);

	void					PickUpLog(int nQuantity, vector<PLog*>& outvecLog);

private:
	PGameServerIDMap		m_mapGameServerByID;

};


