#include "stdafx.h"
#include "CSNameStringChecker.h"
#include "CSNameWordFilter.h"
#include "CSGameWordsFilter.h"


SUITE(IllegalWordChecker)
{
	struct Fixture
	{
		Fixture()
		{
		}
		~Fixture()
		{
			GetNameWordFilter()->Clear();
			GetGameWordsFilter()->Clear();
		}
	};

	TEST_FIXTURE(Fixture, TestAllowedWordChecker_Success)
	{
		CSNameStringChecker checker(L"Mother");
		CCommandResultTable nResult = (CCommandResultTable)checker.Check();
		CHECK_EQUAL(CR_SUCCESS, nResult);
	}

	TEST_FIXTURE(Fixture, TestAllowedWordChecker_IllegalStr)
	{
		wstring strIllegalStr;
		strIllegalStr = L"mother\r\n";
		strIllegalStr += L"END";

		wstring strAllowChar = L"END";

		GetNameWordFilter()->LoadFromString((TCHAR*)strIllegalStr.c_str(), (TCHAR*)strAllowChar.c_str());

		CSNameStringChecker checker(L"Mother");
		CCommandResultTable nResult = (CCommandResultTable)checker.Check();
		CHECK_EQUAL(CR_FAIL_SYSTEM_NAME_INCLUDE_INVALID_STRING, nResult);
	}

	TEST_FIXTURE(Fixture, TestAllowedWordChecker_IllegalChar)
	{
		wstring strIllegalStr;
		strIllegalStr = L"°²\r\n";
		strIllegalStr += L"END";

		wstring strAllowChar = L"END";

		GetNameWordFilter()->LoadFromString((TCHAR*)strIllegalStr.c_str(), (TCHAR*)strAllowChar.c_str());

		CSNameStringChecker checker(L"¿ì¾Ç°²");
		CCommandResultTable nResult = (CCommandResultTable)checker.Check();
		CHECK_EQUAL(CR_FAIL_SYSTEM_NAME_INCLUDE_INVALID_STRING, nResult);
	}

	TEST_FIXTURE(Fixture, TestAllowedWordChecker_ExtendedHangul_EmptyAllowChar)
	{
		wstring strIllegalStr = L"END";
		wstring strAllowChar = L"END";

		GetNameWordFilter()->LoadFromString((TCHAR*)strIllegalStr.c_str(), (TCHAR*)strAllowChar.c_str());

		CSNameStringChecker checker(L"dd");
		CCommandResultTable nResult = (CCommandResultTable)checker.Check();
		CHECK_EQUAL(CR_SUCCESS, nResult);
	}

	TEST_FIXTURE(Fixture, TestAllowedWordChecker_ExtendedHangul_ExistAllowChar)
	{
		wstring strIllegalStr = L"END";
		wstring strAllowChar = L"ƒ…\r\nEND";

		GetNameWordFilter()->LoadFromString((TCHAR*)strIllegalStr.c_str(), (TCHAR*)strAllowChar.c_str());

		CSNameStringChecker checker(L"dd");
		CCommandResultTable nResult = (CCommandResultTable)checker.Check();
		CHECK_EQUAL(CR_FAIL_SYSTEM_CONTAIN_INVALID_CHARACTER, nResult);
	}

	TEST_FIXTURE(Fixture, TestAllowedWordChecker_ExtendedHangul_AllowedChar)
	{
		wstring strIllegalStr = L"END";
		wstring strAllowChar;
		strAllowChar = L"‚Å\r\n";
		strAllowChar += L"END";

		GetNameWordFilter()->LoadFromString((TCHAR*)strIllegalStr.c_str(), (TCHAR*)strAllowChar.c_str());

		CSNameStringChecker checker(L"‚Å‚Å");
		CCommandResultTable nResult = (CCommandResultTable)checker.Check();
		CHECK_EQUAL(CR_SUCCESS, nResult);
	}

}
