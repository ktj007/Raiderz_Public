#include "stdafx.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"


SUITE(DB_WString)
{
	GAMEDB_TEST(DBFiledValueTypeWString)
	{
		CHECK(UTestDB.Execute(L"CREATE TABLE T1(Val NVARCHAR(12));"));
		CHECK(UTestDB.Execute(L"INSERT INTO dbo.T1(Val) VALUES (N'test');"));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT Val FROM dbo.T1;"));
		CHECK_EQUAL(1, rs.GetFetchedRowCount());

		std::wstring val(L"test");
		std::wstring str = rs.Field(L"Val").AsWString();
		CHECK(val == str);

		rs.Close();
	}
}

