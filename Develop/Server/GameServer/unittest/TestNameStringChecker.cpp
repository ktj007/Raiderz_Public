#include "stdafx.h"
#include "GTestForward.h"
#include "CSNameStringChecker.h"
#include "CSNameWordFilter.h"

SUITE(NameStringChecker)
{
	TEST(ShouldBeChararacterLegnthLargerThenDefaultSize)
	{
		struct CheckData
		{
			const wchar_t* pszWord;
			CSNameStringChecker::ERROR_CODE nRetCode;
		} cases[] = {
			// 영문, 너무 짧음
			{L"", CSNameStringChecker::ERROR_LENGTH_TOO_SHORT },
			// 영문, 너무 짧음
			{L"a", CSNameStringChecker::ERROR_LENGTH_TOO_SHORT },
			// 영문, 적당한 길이
			{L"hello", CSNameStringChecker::OK },
			// 영문, 너무 길음
			{L"helloworldhowareyoudoinghere", CSNameStringChecker::ERROR_LENGTH_TOO_LARGE },
			// 한글, 너무 짧음
			{L"아", CSNameStringChecker::ERROR_LENGTH_TOO_SHORT },
			// 한글, 적당한 길이
			{L"아이", CSNameStringChecker::OK },
			// 한글, 너무 길음
			{L"아이스크림맛있습니다", CSNameStringChecker::ERROR_LENGTH_TOO_LARGE },
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			CSNameStringChecker checker(data.pszWord);
			CHECK_EQUAL(data.nRetCode, checker.Check());
		}
	}

	TEST(ShouldNotBeConatinNumberAndSpace)
	{
		struct CheckData
		{
			const wchar_t* pszWord;
			CSNameStringChecker::ERROR_CODE nRetCode;
		} cases[] = {
			// 영문, 숫자가 들어가지 않음
			{L"test", CSNameStringChecker::OK},
			// 영문, 숫자가 들어감
			{L"aa347", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
			// 영문, 숫자가 들어감
			{L"347aa", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
			// 영문, 공백있음
			{L"t est", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
			// 영문, 공백있음
			{L" test", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
			// 영문, 공백있음
			{L"test ", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			CSNameStringChecker checker(data.pszWord);
			CHECK_EQUAL(data.nRetCode, checker.Check());
		}
	}

	TEST(ShouldBeConatinValidHangul)
	{
		tstring strIllegalStr = _T("END");
		tstring strAllowedChar = _T("깄\r\nEND");
		GetNameWordFilter()->LoadFromString((TCHAR*)strIllegalStr.c_str(), (TCHAR*)strAllowedChar.c_str());

		struct CheckData
		{
			const wchar_t* pszWord;
			CSNameStringChecker::ERROR_CODE nRetCode;
		} cases[] = {
			// 영문
			{L"test", CSNameStringChecker::OK},
			// 한글 완성형
			{L"가가가가", CSNameStringChecker::OK},
			// 한글 완성형
			{L"아아아아", CSNameStringChecker::OK},
			// 한글 완성형
			{L"하하하하", CSNameStringChecker::OK},
			// 한글 확장완성형
			{L"뷁궭뒔뒒", CSNameStringChecker::ERROR_CONTAIN_INVALID_CHARACTER},
			// 한글 확장완성형
			{L"갉밞웖뒔", CSNameStringChecker::ERROR_CONTAIN_INVALID_CHARACTER},
			// 특수문자
			{L"☆★○♂♀　㈛", CSNameStringChecker::ERROR_CONTAIN_INVALID_CHARACTER},
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			CSNameStringChecker checker(data.pszWord);
			CHECK_EQUAL(data.nRetCode, checker.Check());
		}

		GetNameWordFilter()->Clear();
	}

	TEST(NumberOnly)
	{
		struct CheckData
		{
			const wchar_t* pszWord;
			CSNameStringChecker::ERROR_CODE nRetCode;
		} cases[] = {
			// 숫자만
			{L"123456", CSNameStringChecker::ERROR_NUMBER_ONLY},
			// 숫자 짧음
			{L"123", CSNameStringChecker::ERROR_NUMBER_ONLY},
			// 숫자 김
			{L"1234567890", CSNameStringChecker::ERROR_NUMBER_ONLY},
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			CSNameStringChecker checker(data.pszWord);
			CHECK_EQUAL(data.nRetCode, checker.Check());
		}
	}

	TEST(CheckHanguleAssemble)
	{
		struct CheckData
		{
			const wchar_t* pszWord;
			CSNameStringChecker::ERROR_CODE nRetCode;
		} cases[] = {
			// 초성만
			{L"ㄱㄴㄷㄹ", CSNameStringChecker::ERROR_CONTAIN_INVALID_CHARACTER},
			// 중성만
			{L"ㅏㅑㅗㅛ", CSNameStringChecker::ERROR_CONTAIN_INVALID_CHARACTER},
			// 종성만
			{L"ㄻㅄㄵㄼ", CSNameStringChecker::ERROR_CONTAIN_INVALID_CHARACTER},
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			CSNameStringChecker checker(data.pszWord);
			CHECK_EQUAL(data.nRetCode, checker.Check());
		}
	}

	TEST(MixingCheck)
	{
		struct CheckData
		{
			const wchar_t* pszWord;
			CSNameStringChecker::ERROR_CODE nRetCode;
		} cases[] = {
			// 한글 + 영문
			{L"abc가나다abc", CSNameStringChecker::ERROR_MIXING_LANGUAGE},
			// 한글 + 숫자
			{L"가나다111", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
			// 영문 + 숫자
			{L"abcDEF111", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
			// 한글 + 특문
			{L"가☆★♀나나", CSNameStringChecker::ERROR_CONTAIN_INVALID_CHARACTER},
			// 영문 + 특문
			{L"aaa☆★♀bbb", CSNameStringChecker::ERROR_MIXING_LANGUAGE},
			// 한글 + 영문 + 특문
			{L"가a☆마마★♀b다", CSNameStringChecker::ERROR_MIXING_LANGUAGE},
			// 확장한글 + 영문
			{L"abc뷁뒔닰abc", CSNameStringChecker::ERROR_MIXING_LANGUAGE},
			// 확장한글 + 공백
			{L"ac 뷁뒔닰 bc", CSNameStringChecker::ERROR_CONTAIN_NUMBER_OR_SPACE},
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			CSNameStringChecker checker(data.pszWord);
			CHECK_EQUAL(data.nRetCode, checker.Check());
		}
	}

	TEST(CheckAlphabetless)
	{
		struct CheckData
		{
			const wchar_t* pszWord;
			CSNameStringChecker::ERROR_CODE nRetCode;
		} cases[] = {
			// a
			{L"aaaaaa", CSNameStringChecker::OK},
			// z
			{L"zzzzzz", CSNameStringChecker::OK},
			// A
			{L"AAAAAA", CSNameStringChecker::OK},
			// Z
			{L"ZZZZZZ", CSNameStringChecker::OK},
			// !
			{L"!!!!!!", CSNameStringChecker::ERROR_CONTAIN_ALPHABETLESS},
			// @
			{L"@@@@@@", CSNameStringChecker::ERROR_CONTAIN_ALPHABETLESS},
			// +
			{L"++++++", CSNameStringChecker::ERROR_CONTAIN_ALPHABETLESS},
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			CSNameStringChecker checker(data.pszWord);
			CHECK_EQUAL(data.nRetCode, checker.Check());
		}
	}
}


