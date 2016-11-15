#include "stdafx.h"
#include "SDsnFactory.h"
#include "SBaseDsnFactory.h"


SUITE(DsnFactory)
{
	class MockDefaultDsnFactory : public SBaseDsnFactory
	{
	public :
		MockDefaultDsnFactory(const mdb::MDatabaseDesc& AccDesc
			, const mdb::MDatabaseDesc& GameDesc
			, const mdb::MDatabaseDesc& LogDesc)
			: SBaseDsnFactory(SBaseDsnFactory::DEFAULT, AccDesc, GameDesc, LogDesc)
		{}

		mdb::MDatabaseDesc GetAccountDSN()	
		{
			return m_AccDesc;
		}
		mdb::MDatabaseDesc GetGameDSN()
		{
			return m_GameDesc;
		}
		mdb::MDatabaseDesc GetLogDSN()
		{
			return m_LogDesc;
		}
	};

	class MockPmangDsnFactory : public SBaseDsnFactory
	{
	public :
		MockPmangDsnFactory(const mdb::MDatabaseDesc& AccDesc
			, const mdb::MDatabaseDesc& GameDesc
			, const mdb::MDatabaseDesc& LogDesc)
			: SBaseDsnFactory(SBaseDsnFactory::NEOWIZ, AccDesc, GameDesc, LogDesc)
		{}

		mdb::MDatabaseDesc GetAccountDSN()	
		{
			return m_AccDesc;
		}
		mdb::MDatabaseDesc GetGameDSN()
		{
			return m_GameDesc;
		}
		mdb::MDatabaseDesc GetLogDSN()
		{
			return m_LogDesc;
		}
	};

	class FIXTURE
	{
	public :
		FIXTURE() {}
		~FIXTURE() { SDsnFactory::GetInstance().Release(); }

	};

	TEST_FIXTURE(FIXTURE, GetDefaultFactory)
	{
		CHECK(NULL == SDsnFactory::GetInstance().Get());
	}

	TEST_FIXTURE(FIXTURE, SetFactory)
	{
		SDsnFactory::GetInstance().Set(new MockPmangDsnFactory(mdb::MDatabaseDesc("SQL Server", "MockNwzAccServer", "MockNwzAccDB", "MockNwzAccUser", "MockNwzAccPasswd")
															 , mdb::MDatabaseDesc("SQL Server", "MockNwzGameServer", "MockNwzGameDB", "MockNwzGameUser", "MockNwzGamePasswd")
															 , mdb::MDatabaseDesc("SQL Server", "MockNwzLogServer", "MockNwzLogDB", "MockNwzLogUser", "MockNwzLogPasswd")));

		CHECK_EQUAL(SBaseDsnFactory::NEOWIZ, SDsnFactory::GetInstance().Get()->GetDsnID());
		
	}
}