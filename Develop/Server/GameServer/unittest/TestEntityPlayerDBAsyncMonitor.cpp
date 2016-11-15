#include "stdafx.h"
#include "GEntityPlayerDBAsyncMonitor.h"

SUITE(EntityPlayerDBAsyncMonitor)
{
	TEST(EntityPlayerDBAsyncMonitor)
	{
		GEntityPlayerDBAsyncMonitor em;

		CHECK_EQUAL(0, em.GetRefCount());
		
		em.IncreaseRefCount();
		CHECK_EQUAL(1, em.GetRefCount());
		em.IncreaseRefCount();
		CHECK_EQUAL(2, em.GetRefCount());

		em.DecreaseRefCount();
		CHECK_EQUAL(1, em.GetRefCount());
		em.DecreaseRefCount();
		CHECK_EQUAL(0, em.GetRefCount());
	}
}