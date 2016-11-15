#include "stdafx.h"
#include "MTrafficMonitor.h"

SUITE(testTrafficMonitor)
{
	class MockCommand
	{
	public:
		MockCommand(const int& id,const int& size) : m_id(id), m_size(size) {}

		int GetID() const
		{
			return m_id;
		}

		int GetSize()
		{
			return m_size;
		}

	private:
		int m_size;
		int m_id;
	};

	class FTrafficMonitor
	{
	public:
		FTrafficMonitor()
		{
			trafficMonitor.Reset();
			totalSend = 0;
			totalRecv = 0;
		}

		~FTrafficMonitor()
		{
		}

		MTrafficMonitor trafficMonitor;
		int64			totalSend;
		int64			totalRecv;
	};

	TEST_FIXTURE(FTrafficMonitor, testTrafficMonitorSend)
	{
		// 보내기 측정
		trafficMonitor.RecordSend(100);

		trafficMonitor.GetTotalTraffic(totalRecv, totalSend);
		CHECK_EQUAL(100, totalSend);
		CHECK_EQUAL(0, totalRecv);
	}

	TEST_FIXTURE(FTrafficMonitor, testTrafficMonitorRecv)
	{
		// 받기 측정
		trafficMonitor.RecordRecv(100);

		trafficMonitor.GetTotalTraffic(totalRecv, totalSend);
		CHECK_EQUAL(0, totalSend);
		CHECK_EQUAL(100, totalRecv);
	}

	TEST_FIXTURE(FTrafficMonitor, testTrafficMonitorSendRecv)
	{
		// 보내고 받기 측정
		trafficMonitor.RecordSend(100);
		trafficMonitor.RecordRecv(200);

		trafficMonitor.GetTotalTraffic(totalRecv, totalSend);
		CHECK_EQUAL(100, totalSend);
		CHECK_EQUAL(200, totalRecv);
	}
}