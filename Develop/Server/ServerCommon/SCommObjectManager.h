#pragma once

#include "MUID.h"
#include "MSync.h"
#include "ServerCommonLib.h"

class SCommObject;

/// Client 관리자
class SCOMMON_API SCommObjectManager
{
public:
	typedef map<MUID, SCommObject*> SCommObjectMap;
protected:
	SCommObjectMap			m_ClientObjectMap;
	MCriticalSection		m_Lock;
public:
	SCommObjectManager();
	virtual ~SCommObjectManager();
	virtual SCommObject* NewObject(MUID& uid);
	virtual void AddObject(SCommObject* pClient);
	virtual void DeleteObject(MUID uidClient);
	virtual void DeleteAllObject();
	virtual void Clear();

	int  GetClientsCount() { return (int)m_ClientObjectMap.size(); }

	// 쓰레드에 안전하지 않은 함수 - Lock, Unlock를 사용해야한다.
	void Lock();
	void Unlock();
	SCommObject* GetClient(const MUID& uidClient);			// 쓰레드에 안전하지 않다.
	SCommObjectMap& GetObjects()	{ return m_ClientObjectMap; }	
};
