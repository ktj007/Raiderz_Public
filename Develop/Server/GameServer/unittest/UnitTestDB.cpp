#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "TestDBTableStatus.h"
#include "DBTestSeedData.h"
#include "DBTestGlobal.h"

SUITE(DB_UnitTest)
{
	GAMEDB_TEST(CrashColumnHashValue)
	{
		map<size_t, wstring> mapHash;

		mdb::MDBRecordSet rset;

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT DISTINCT(CAST(c.name AS VARCHAR(512))) AS name \
								   FROM   sys.columns c \
							       WHERE  object_id IN (SELECT object_id FROM sys.tables);"));


		int i = 0;
		
		for (; !rs.IsEOF(); rs.MoveNext())
		{
			const wstring strColName = rs.Field(L"name").AsWString();
			const size_t nHashValue = rset.MakeHashValueW(strColName);

			CHECK(wstring() != strColName);
			CHECK(0 != nHashValue);
						
			CHECK(mapHash.end() == mapHash.find(nHashValue));

			if (mapHash.end() != mapHash.find(nHashValue))
			{
				CHECK(0 == wcscmp(L"", strColName.c_str())); // Ãâ·Â¿ë.
			}

			mapHash.insert(pair<size_t, wstring>(nHashValue, strColName));
		}

		CHECK(0 < rs.GetFetchedRowCount());

		rs.Close();
	}
}
