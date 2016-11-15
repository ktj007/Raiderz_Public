#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(RZ_MAIL_GET_MONEY)
{
	GAMEDB_TEST(USP_RZ_MAIL_GET_MONEY)
	{
		const int64 GSN = 1;
		const int64 CSN = DBTestHelper::InsertCharacter(GSN, GConfig::m_nMyWorldID);
		const int	CHAR_MONEY = 20;
		DBTestHelper::SetMoney(GSN, CSN, CHAR_MONEY);

		const int MAIL_MONEY = 10;
		const int64 MAIL_UID = DBTestHelper::InsertMail(GConfig::m_nMyWorldID, GSN, CSN);
		UTestDB.Execute(L"UPDATE dbo.RZ_MAIL SET [MONEY] = %d WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND MAIL_ID = %I64d;"
			, MAIL_MONEY, GSN, CSN, MAIL_UID);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_MAIL_GET_MONEY (%d, %I64d, %I64d, %I64d, %d, %d)}", GConfig::m_nMyWorldID, GSN, CSN, MAIL_UID, MAIL_MONEY, CHAR_MONEY + MAIL_MONEY);

		TDBRecordSet rsMail;
		UTestDB.Execute(rsMail, L"SELECT [MONEY] FROM dbo.RZ_MAIL WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND MAIL_ID = %I64d;", GSN, CSN, MAIL_UID);
		CHECK_EQUAL(1, rsMail.GetFetchedRowCount());
		CHECK_EQUAL(0, rsMail.Field(L"MONEY").AsInt());
		rsMail.Close();

		TDBRecordSet rsChar;
		UTestDB.Execute(rsChar, L"SELECT [MONEY] FROM dbo.RZ_CHARACTER WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", GSN, CSN);
		CHECK_EQUAL(1, rsChar.GetFetchedRowCount());
		CHECK_EQUAL(CHAR_MONEY + MAIL_MONEY, rsChar.Field(L"MONEY").AsInt());
		rsChar.Close();
	}
}