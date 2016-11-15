#include "stdafx.h"
#include "SDsnFactory.h"
#include "PmDsnFactory.h"
#include "nwzCrypto.h"
#include "SBaseDsnFactory.h"
#include "MStringUtil.h"


SUITE(PmangDsnFactory)
{
	class FIXTURE
	{
	public :
		FIXTURE()
		{
			SDsnFactory::GetInstance().Set(new PmDsnFactory(mdb::MDatabaseDesc()
								   , mdb::MDatabaseDesc("", "", "RZ_GAMEDB", "PPeQ/Y0tHlM=", "PPeQ/Y0tHlM=")
								   , mdb::MDatabaseDesc("", "", "RZ_LOGDB", "mYWZxXCuf7UtDB+i8pZ1AA==", "mYWZxXCuf7UtDB+i8pZ1AA==")));
		}

		~FIXTURE()
		{
			SDsnFactory::GetInstance().Release();	
		}
	};

	TEST_FIXTURE(FIXTURE, GetPmangGameDSN)
	{
		//mdb::MDatabaseDesc desc = SDsnFactory::GetInstance().Get()->GetGameDSN();
		//CHECK_EQUAL("dev", desc.strUserName);
		//CHECK_EQUAL("dev", desc.strPassword);
	}

	TEST_FIXTURE(FIXTURE, GetPmangLogDSN)
	{
		//mdb::MDatabaseDesc desc = SDsnFactory::GetInstance().Get()->GetLogDSN();
		//CHECK_EQUAL("dev", desc.strUserName);
		//CHECK_EQUAL("dev", desc.strPassword);
	}
}