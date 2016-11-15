#include "stdafx.h"
#include "XMMCode.h"

SUITE(MMcode)
{
	class FMMCode
	{
	public:
		mmcode::MControlCodeTransResult Trans(std::wstring strText)
		{
			mmcode::MControlCodeTransResult result = 
				XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(strText.c_str(), XGetMyUID());
			return result;
		}
	};
	TEST_FIXTURE(FMMCode, TestMMCode_Case1)
	{
		USES_CONVERSION_EX;

		std::wstring strQuestDesc = L"{character}닷소{/character}는 시집이 어디에 있는지 아는 것은 하인인 {character}라팔{/character}과 {character}미스텔{/character} 뿐이라고 합니다. {CR}{CR}북쪽의 {place}방목지{/place}로 가서, 우르크들을 데리러 갔다가 돌아오지 않고 있는 라팔과 미스텔의 행방을 확인하여 닷소에게 보고하십시오.{CR}{COLOR r=50 g=200 b=50}닷소와 미스텔의 예상 위치는 다음과 같습니다.{/COLOR}{CR}{BITMAP name=\"Quest0001\" w=250 h=250}{SPACE w=250 h=260}";

		mmcode::MControlCodeTransResult result = Trans(strQuestDesc);
		
		std::wstring strExpected = L"{character}닷소{/character}는 시집이 어디에 있는지 아는 것은 하인인 {character}라팔{/character}과 {character}미스텔{/character} 뿐이라고 합니다. {CR}{CR}북쪽의 {place}방목지{/place}로 가서, 우르크들을 데리러 갔다가 돌아오지 않고 있는 라팔과 미스텔의 행방을 확인하여 닷소에게 보고하십시오.{CR}{COLOR r=50 g=200 b=50}닷소와 미스텔의 예상 위치는 다음과 같습니다.{/COLOR}{CR}{BITMAP name=\"Quest0001\" w=250 h=250}{SPACE w=250 h=260}";

		CHECK(strExpected == A2W_EX(result.m_strOutput.c_str(), 0));
	}

	TEST_FIXTURE(FMMCode, TestMMCode_Case2)
	{
		USES_CONVERSION_EX;

		std::wstring strQuestDesc = L"{ani=bow}무궁화꽃{talent=52}";

		mmcode::MControlCodeTransResult result = Trans(strQuestDesc);

		std::wstring strExpected = L"무궁화꽃";

		CHECK(strExpected == A2W_EX(result.m_strOutput.c_str(), 0));
	}

}