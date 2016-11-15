#include "stdafx.h"
#include "MNetClient.h"
#include "MLink.h"

SUITE(ConnectionKeeper)
{
	TEST(TestClientConnectionKeeper)
	{
		MClientConnectionKeeper keeper;

		CHECK_EQUAL(false, keeper.CheckToSendCommand(10000, 1000));
		
		keeper.SetSendTime(10000);

		CHECK_EQUAL(false, keeper.CheckToSendCommand(10500, 1000));
		CHECK_EQUAL(true, keeper.CheckToSendCommand(11000, 1000));

		CHECK_EQUAL(false, keeper.CheckToSendCommand(11100, 1000));
	}

	TEST(TestKeepingAliveData)
	{
		MKeepingAliveData keepingAliveData;

		CHECK_EQUAL(true, keepingAliveData.CheckAlive(10000, 1000));

		keepingAliveData.SetRecvTime(10500);
		CHECK_EQUAL(true, keepingAliveData.CheckAlive(11000, 1000));
		CHECK_EQUAL(false, keepingAliveData.CheckAlive(11500, 1000));
	}
}