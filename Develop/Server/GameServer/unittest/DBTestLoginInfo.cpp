#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"

SUITE(DBTestLoginInfo)
{
	ACCDB_TEST(USP_RZ_LOGIN_GET_INFO)
	{
		const wchar_t* szUserID	= L"test_user";
		const wchar_t* szUSC	= L"test_usc";
		const wchar_t* szPasswd	= L"test_passwd";

		const int64 nAID = DBTestHelper::InsertAccount(szUserID, szUSC, szPasswd);
		
		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_LOGIN_GET_INFO (N'', N'%s')}", szUSC);
		CHECK_EQUAL(nAID, rs.Field(L"ACCN_ID").AsInt());
		CHECK(szPasswd == rs.Field(L"PWD").AsWString());
		rs.Close();	
	}

}

