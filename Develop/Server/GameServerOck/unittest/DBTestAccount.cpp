#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"

SUITE(DBTestAccount)
{
	ACCDB_TEST(USP_RZ_USER_GET_INFO)
	{
		const wchar_t szUserID[] = L"Acc1";

		const int64 nGSN= DBTestHelper::InsertAccount(szUserID);
		CHECK(0 != nGSN);
		
		TDBRecordSet recordSet;
		CHECK(UTestDB.Execute(recordSet, L"{CALL USP_RZ_ACCOUNT_GET_INFO (%I64d)}", nGSN));

		CHECK_EQUAL(1, recordSet.GetFetchedRowCount());
		CHECK(szUserID  == recordSet.Field(L"USER_ID").AsWString());
		recordSet.Close();
	}


	ACCDB_TEST(USP_RZ_USER_INSERT)
	{
		const wchar_t USER_ID[]			= L"INSERT USER";
		const wchar_t SITE_CODE[]		= L"SITE CODE";
		const wchar_t SITE_USER_ID[]	= L"SITE USER ID";
		const wchar_t PWD[]				= L"PWD";

		// 새로운 계정을 만든다.
		TDBRecordSet rs1;
		CHECK(UTestDB.Execute(rs1, L"{CALL USP_RZ_ACCOUNT_INSERT (N'%s', N'%s', N'%s', N'%s')}"
			, USER_ID
			, SITE_CODE
			, SITE_USER_ID
			, PWD));

		CHECK_EQUAL(1, rs1.GetFetchedRowCount());

		const int64 GSN = rs1.Field(L"ACCN_ID").AsInt64();
		CHECK(0 != GSN);
		rs1.Close();

		// 새로 만든 계정 정보 검증.
		TDBRecordSet rs3;
		CHECK(UTestDB.Execute(rs3, L"SELECT USER_ID, SITE_CODE, SITE_USER_ID, PWD, CASE WHEN REG_DATE IS NULL THEN 1 ELSE 0 END NEW_ACC \
									FROM dbo.RZ_ACCOUNT \
									WHERE ACCN_ID = %I64d", GSN));
		CHECK_EQUAL(1, rs3.GetFetchedRowCount());
		CHECK(USER_ID == rs3.Field(L"USER_ID").AsWString());
		CHECK(SITE_CODE == rs3.Field(L"SITE_CODE").AsWString());
		CHECK(SITE_USER_ID == rs3.Field(L"SITE_USER_ID").AsWString());
		CHECK(PWD == rs3.Field(L"PWD").AsWString());
		CHECK_EQUAL(1, rs3.Field(L"NEW_ACC").AsByte());
		rs3.Close();
	}

	ACCDB_TEST(USP_RZ_USER_UPDATE_REG_DATE)
	{
		const int64 nGSN = DBTestHelper::InsertAccount(L"test");

		UTestDB.Execute(L"{CALL dbo.USP_RZ_ACCOUNT_UPDATE_REG_DATE (%I64d)}", nGSN);

		TDBRecordSet rsCreateDt;
		UTestDB.Execute(rsCreateDt, L"SELECT REG_DATE FROM dbo.RZ_ACCOUNT WHERE ACCN_ID = %I64d;", nGSN);
		CHECK_EQUAL(1, rsCreateDt.GetFetchedRowCount());
		CHECK(!rsCreateDt.Field(L"REG_DATE").IsNull());
		rsCreateDt.Close();
	}
}