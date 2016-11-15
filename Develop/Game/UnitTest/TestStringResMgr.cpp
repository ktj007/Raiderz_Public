#include "stdafx.h"
#include "XStrings.h"

SUITE(StringResMgr)
{
	struct MockStringResMgr: public XStringResMgr
	{
		MOCK_METHOD1(GetStringIncludedKeyString, const TCHAR*(const TCHAR* szKey));
	};

	struct TestStringResMgr
	{
		MockStringResMgr m_StringResMgr;
	};

	TEST_FIXTURE(TestStringResMgr, TransKeyStrWithParam)
	{
		using testing::StrEq;
		using testing::Return;
		EXPECT_CALL(m_StringResMgr, GetStringIncludedKeyString(StrEq(L"$$KEY")))
			.WillRepeatedly(Return(L"ParamKey {0}, {1}, {2}"));

		wstring strKeyWithParam = L"$$KEY {param=test1|test2|test3}";
		
		wstring strResult = m_StringResMgr.TransKeyStrWithParam(strKeyWithParam);		
		CHECK(L"ParamKey test1, test2, test3" == strResult);
	}

	TEST_FIXTURE(TestStringResMgr, TransKeyStrWithParam_OnlyKey)
	{
		using testing::StrEq;
		using testing::Return;
		EXPECT_CALL(m_StringResMgr, GetStringIncludedKeyString(StrEq(L"$$KEY")))
			.WillRepeatedly(Return(L"ParamKey {0}, {1}, {2}"));

		wstring strKeyWithParam = L"$$KEY";

		wstring strResult = m_StringResMgr.TransKeyStrWithParam(strKeyWithParam);		
		CHECK(L"ParamKey 0, 1, 2" == strResult);
	}

	TEST_FIXTURE(TestStringResMgr, TransKeyStrWithParam_NoKey)
	{
		using testing::_;
		using testing::Return;
		EXPECT_CALL(m_StringResMgr, GetStringIncludedKeyString(_))
			.Times(1);

		wstring strKeyWithParam = L"{param=test1|test2|test3}";

		wstring strResult = m_StringResMgr.TransKeyStrWithParam(strKeyWithParam);		
		CHECK(strResult.empty());
	}
}
