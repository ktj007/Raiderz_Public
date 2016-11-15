#include "stdafx.h"
#include "SUnitTest.h"
#include "MDBThread.h"

using namespace mdb;

SUITE(TEST_DB_THREAD)
{
	class MockDBThread : public MDBThread
	{
	public :
		using MDBThread::DecDBReconnectCount;
		using MDBThread::IncDBReconnectCount;
		using MDBThread::GetDBReconnectCount;
	};

	TEST(DB_MAX_INC_RECONNECT_COUNT)
	{
		MockDBThread mt;

		CHECK_EQUAL(0, mt.GetDBReconnectCount());
		mt.IncDBReconnectCount();
		CHECK_EQUAL(1, mt.GetDBReconnectCount());
		mt.IncDBReconnectCount();
		CHECK_EQUAL(2, mt.GetDBReconnectCount());
		mt.IncDBReconnectCount();
		CHECK_EQUAL(3, mt.GetDBReconnectCount());
		mt.IncDBReconnectCount();
		CHECK_EQUAL(3, mt.GetDBReconnectCount());
	}

	TEST(DB_MIN_DEC_RECONNECT_COUNT)
	{
		MockDBThread mt;

		mt.IncDBReconnectCount();
		
		mt.DecDBReconnectCount();
		CHECK_EQUAL(0, mt.GetDBReconnectCount());
		mt.DecDBReconnectCount();
		CHECK_EQUAL(0, mt.GetDBReconnectCount());
	}
}