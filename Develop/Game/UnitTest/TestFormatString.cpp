#include "stdafx.h"
#include "CSFormatString.h"
#include "CStringResMgr.h"

SUITE(FormatString)
{
	struct MockDictionary: public CStringResMgr
	{
		MOCK_METHOD1(Plural,wstring (wstring strKey));
	};

	struct TestFormatString
	{
		MockDictionary m_dic;

		TestFormatString()
		{
			CSFormatString::SetDictionary(&m_dic);
		}

		~TestFormatString()
		{
			CSFormatString::SetDictionary(NULL);
		}
	};


	// 파라메터 없음
	TEST_FIXTURE(TestFormatString, NoParam)
	{
		wstring strForamt = L"TestNoParam {0}, {1}, {2}";
		wstring strResult = CSFormatString::Format(strForamt);
		CHECK(L"TestNoParam 0, 1, 2" == strResult);
	}

	// 빈문자열 파라메터
	TEST_FIXTURE(TestFormatString, EmptyStringParam)
	{
		wstring strForamt = L"EmptyParam_{0}{1}{2}{3}";
		wstring strResult = CSFormatString::Format(strForamt, FSParam(L"Test", L"", L"Param", L""));
		CHECK(L"EmptyParam_TestParam" == strResult);
	}

	// 인덱스 순서
	TEST_FIXTURE(TestFormatString, Index)
	{		
		wstring strFormat = L"{2}, {0}, {1}";
		wstring strObject[] = {L"오렌지", L"바나나", L"사과"};

		wstring strResult = CSFormatString::Format(strFormat, FSParam(strObject[0], strObject[1], strObject[2]));
		CHECK(L"사과, 오렌지, 바나나" == strResult);
	}

	// 격조사, 보조사
	TEST_FIXTURE(TestFormatString, ParticleCasting)
	{
		wstring strFormat = L"{0,는} {1,가} {2,를}";
		wstring strObject[] = {L"디펜더", L"클레릭", L"분쇄"};

		wstring strResult = CSFormatString::Format(strFormat, FSParam(strObject[0], strObject[1], strObject[2]));
		CHECK(L"디펜더는 클레릭이 분쇄를" == strResult);
	}


	// 격조사/보조사 외 접미사를 {}내용 맨뒤에 붙이도록 했다 - 단어에 꾸밈을 붙일 수 있다.
	TEST_FIXTURE(TestFormatString, PostfixParticleCasting)
	{
		wstring strFormat = L"당신은 {0,을:<<$>>} 마십니다.";
		wstring strObject[] = {L"물약"};

		wstring strResult = CSFormatString::Format(strFormat, FSParam(strObject[0]));
		CHECK(L"당신은 <<물약>>을 마십니다." == strResult);
	}

	// 영어단어 복수형
	TEST_FIXTURE(TestFormatString, PluralCasting)
	{
		using testing::_;
		using testing::Return;
		EXPECT_CALL(m_dic, Plural(_))
			.WillRepeatedly(Return(L"wolves"));


		wstring strFormat = L"{0,s} {0}";
		wstring strObject = L"wolf";

		wstring strResult = CSFormatString::Format(strFormat, FSParam(strObject));
		CHECK(L"wolves wolf" == strResult);
	}

	// 자기참조 출력양식
	TEST_FIXTURE(TestFormatString, SelfReferenceFormat)
	{
		wstring strFormat = L"{0:당신은 $ 입니다.}";
		wstring strObject = L"디펜더";

		wstring strResult = CSFormatString::Format(strFormat, FSParam(strObject));
		CHECK(L"당신은 디펜더 입니다." == strResult);
	}

	// 재귀 출력양식
	TEST_FIXTURE(TestFormatString, RecursiveFormat)
	{
		wstring strFormat = L"{0:당신은 {1,을} 공격합니다.}";
		wstring strObject[] = {L"클레릭", L"디펜더"};

		wstring strResult = CSFormatString::Format(strFormat, FSParam(strObject[0], strObject[1]));
		CHECK(L"당신은 디펜더를 공격합니다." == strResult);
	}

	// 그룹지정 출력양식
	TEST_FIXTURE(TestFormatString, GroupFormat)
	{
		wstring strFormat = L"{0:없음;하나;많음}";

		CHECK(L"없음" == CSFormatString::Format(strFormat, FSParam(0)));
		CHECK(L"하나" == CSFormatString::Format(strFormat, FSParam(1)));
		CHECK(L"많음" == CSFormatString::Format(strFormat, FSParam(2)));
	}
}
