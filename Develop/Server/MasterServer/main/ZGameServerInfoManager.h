#pragma once

#include "ZGameServerInfo.h"
#include <map>
using namespace std;

typedef map<int, ZGameServerInfo*>	ZGameServerInfoMap;

class ZGameServerInfoManager
{
protected:
	ZGameServerInfoMap		m_InfoMap;

	void Clear();

public:
	ZGameServerInfoManager() {}
	virtual ~ZGameServerInfoManager() 
	{
		Clear();
	}
	bool Insert(ZGameServerInfo* pInfo);
	size_t GetSize()
	{
		return m_InfoMap.size();
	}
	ZGameServerInfo* GetInfo(int nID);
	ZGameServerInfoMap& GetServerInfoMap() { return m_InfoMap; }
};