#pragma once

#include "ZAppServerInfo.h"

typedef map<int, ZAppServerInfo*>	ZAppServerInfoMap;

class ZAppServerInfoManager
{
private:
	ZAppServerInfoMap		m_InfoMap;

	void Clear();

public:
	ZAppServerInfoManager() {}
	~ZAppServerInfoManager()
	{
		Clear();
	}
	bool Insert(ZAppServerInfo* pInfo);
	ZAppServerInfo* GetInfo(int nID);
	size_t GetSize()
	{
		return m_InfoMap.size();
	}
	ZAppServerInfoMap& GetServerInfoMap() { return m_InfoMap; }
};
