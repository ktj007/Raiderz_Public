#include "stdafx.h"
#include "GDBCacheSystem.h"
#include "GTestWrapper.h"
#include "GCharacterDBCache.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_DBCacheSystem_Character)
{
	class MockDBCacheSystem_Char : public GDBCacheSystem
	{
	public :
		GPlayerDBCache* CreateTestPlayerCache()
		{
			return CreatePlayerCache(1, 2);
		}

	public :
		GTestMgrWrapper<GPlayerDBCacheMgr>	m_PlayerDBCacheWrapper;
	};

	
}