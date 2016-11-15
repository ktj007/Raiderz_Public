#pragma once

#include "ZLoginServerInfo.h"

class ZLoginServerInfoManager
{
private:
	typedef map<int, ZLoginServerInfo*>	ZLoginServerInfoMap;
	ZLoginServerInfoMap		m_InfoMap;

	void Clear();

public:
	ZLoginServerInfoManager() {}
	~ZLoginServerInfoManager()
	{
		Clear();
	}
	bool Insert(ZLoginServerInfo* pInfo);
	ZLoginServerInfo* GetInfo(int nID);
	size_t GetSize()
	{
		return m_InfoMap.size();
	}
};
