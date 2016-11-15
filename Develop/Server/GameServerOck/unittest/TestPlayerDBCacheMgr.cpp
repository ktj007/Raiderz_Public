#include "stdafx.h"
#include "GPlayerDBCacheMgr.h"
#include "GPlayerDBCache.h"


SUITE(TestDBCache_PlayerDBCacheMgr)
{
	TEST(TestPlayerDBCacheMgrInsert)
	{
		const MUID& uidPlayer	= 1;
		const int	nCID		= 2;

		GPlayerDBCache* pcc = new GPlayerDBCache(uidPlayer, nCID);

		GPlayerDBCacheMgr mgr;
		CHECK(mgr.Insert(pcc));

		GPlayerDBCache* fcc = mgr.Find(uidPlayer);
		CHECK(NULL != fcc);
		CHECK_EQUAL(fcc, pcc);
	}

	TEST(TestPlayerDBCacheMgrErase)
	{
		const MUID& uidPlayer	= 1;
		const int	nCID		= 2;

		GPlayerDBCache* pcc = new GPlayerDBCache(uidPlayer, nCID);

		GPlayerDBCacheMgr mgr;
		mgr.Insert(pcc);

		GPlayerDBCache* fcc = mgr.Find(uidPlayer);

		mgr.Erase(uidPlayer);
		CHECK(NULL == mgr.Find(uidPlayer));

		SAFE_DELETE(fcc);
	}
}