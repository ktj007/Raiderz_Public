#include "stdafx.h"
#include "MPacket.h"


SUITE(Packet)
{
	class FPacket : public FCommandDesc
	{
	public:
		FPacket()
		{

		}
		virtual ~FPacket()
		{

		}

		class TestPacket
		{
		private:
			char*	m_pBuf;
		public:
			TestPacket(int nSize)
			{
				m_pBuf = new char[nSize];

				MCommandMsg* pPacket = GetPacket();
				pPacket->nMsg = MSG_COMMAND;
				pPacket->nSize = nSize;
				pPacket->nCheckSum = 0;
			}
			~TestPacket()
			{
				delete [] m_pBuf;
			}
			MCommandMsg* GetPacket() 
			{
				return (MCommandMsg*)m_pBuf;
			}
			int GetDataSize()
			{
				return GetPacket()->nSize - sizeof(MPacketHeader);
			}
		};
	};

	TEST(TestPacketHeaderSize)
	{
		CHECK_EQUAL(5, sizeof(MPacketHeader));
	}

	TEST_FIXTURE(FPacket, TestEqualChecksumWithSerialNumber)
	{
		const int BUF_LEN = 32;
		TestPacket testPacket1(BUF_LEN);
		TestPacket testPacket2(BUF_LEN);

		MCommandMsg* pPacket1 = testPacket1.GetPacket();
		MCommandMsg* pPacket2 = testPacket2.GetPacket();

		for (int i = 0; i < testPacket1.GetDataSize(); i++)
		{
			pPacket1->Buffer[i] = i;
			pPacket2->Buffer[i] = i;
		}

		unsigned short nChecksum1 = MBuildCheckSumAndSerialNumber(pPacket1, pPacket1->nSize, 0);
		unsigned short nChecksum2 = MBuildCheckSumAndSerialNumber(pPacket2, pPacket2->nSize, 5);

		// 시리얼 번호때문에 다르다.
		CHECK(nChecksum1 != nChecksum2);

		// IsEqualCheckSum 함수는 시리얼 번호를 체크하지 않는다.
		CHECK_EQUAL(true, IsEqualCheckSum(nChecksum1, pPacket2, pPacket2->nSize));
	}

	TEST_FIXTURE(FPacket, TestGetPacketSerialNumberFromChecksum)
	{
		const int BUF_LEN = 32;
		TestPacket testPacket1(BUF_LEN);
		MCommandMsg* pPacket1 = testPacket1.GetPacket();

		for (int i = 0; i < testPacket1.GetDataSize(); i++)
		{
			pPacket1->Buffer[i] = MMath::RandomNumber(0, i);
		}

		for (int i = 0; i < 10; i++)
		{
			uint8 nSerialNumber = MMath::RandomNumber(0, 0xF);
			unsigned short nChecksum1 = MBuildCheckSumAndSerialNumber(pPacket1, pPacket1->nSize, nSerialNumber);
			CHECK_EQUAL(nSerialNumber, GetPacketSerialNumberFromChecksum(nChecksum1));
		}
	}
}