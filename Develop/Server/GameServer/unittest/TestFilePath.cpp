#include "stdafx.h"
#include "GFilePath.h"

SUITE(FilePath)
{
	struct FFilePathFixture
	{
		FFilePathFixture()
		{
			m_strSystemPath = L"./system/";
		}
		~FFilePathFixture()
		{

		}
		wstring m_strSystemPath;
	};
	TEST_FIXTURE(FFilePathFixture, TestMake_ZonePath)
	{
		wstring strZoneList = GFilePath::MakeZoneList(m_strSystemPath.c_str());

		CHECK(L"./system/zonelist.xml" == strZoneList);
	}

}