#pragma once

#include "MUID.h"
#include "SCommObject.h"
#include "STypes.h"
#include <map>
using namespace std;

class LCommObjectManager;
class LServer;

/// 플레이어 오브젝트
class LCommObject : public SCommObject
{	
protected:
	SServerType				m_nServerType;
	int						m_nServerID;

	LCommObject() : SCommObject(), m_nServerID(0), m_nServerType(SERVER_GAME) {}
public:
	LCommObject(MUID& uidObject);
	virtual ~LCommObject();
};

