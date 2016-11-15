#pragma once

#include "MUID.h"
#include <map>
using namespace std;

#include "ServerCommonLib.h"


class SCommObjectManager;
class PServer;

/// 플레이어 오브젝트
class SCOMMON_API SCommObject
{	
protected:
	friend class SCommObjectManager;

	bool				m_bCreated;
	MUID				m_UID;				///< 플레이어 UID. Link UID와 같은 값이다.
protected:
	SCommObject() {}

public:
	SCommObject(MUID& uid);
	virtual ~SCommObject();
	virtual void Create();							///< GPlayerObjectManager에서 호출
	virtual void Destroy();							///< GPlayerObjectManager에서 호출

	const MUID		GetUID(void) const;
};

