#include "stdafx.h"
#include "GManagedFieldInfoMgr.h"

SUITE(ManagedFieldInfoMgr)
{
	TEST(TestManagedSharedFieldInfo)
	{
		GManagedSharedFieldInfo managedSharedFieldInfo;

		const int nChannelID = 1;
		managedSharedFieldInfo.AddChannel(nChannelID, MUID(1,1));
		CHECK_EQUAL(true, managedSharedFieldInfo.ExistChannel(nChannelID));
	}

	TEST(TestManagedSharedFieldInfo_DelChannel)
	{
		GManagedSharedFieldInfo managedSharedFieldInfo;

		const int nChannelID = 1;
		const MUID uidChannel = MUID(1,1);
		managedSharedFieldInfo.AddChannel(nChannelID, uidChannel);
		CHECK_EQUAL(1, managedSharedFieldInfo.GetChannelCount());

		managedSharedFieldInfo.DelChannel(uidChannel);
		CHECK_EQUAL(0, managedSharedFieldInfo.GetChannelCount());
		CHECK_EQUAL(false, managedSharedFieldInfo.ExistChannel(nChannelID));
	}
}