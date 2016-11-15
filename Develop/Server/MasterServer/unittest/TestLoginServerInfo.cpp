#include "stdafx.h"
#include "ZLoginServerInfo.h"
#include "ZLoginServerInfoLoader.h"
#include "ZLoginServerInfoManager.h"
#include "ZFixtureHelper.h"

SUITE(LoginServerInfo)
{
	struct FLoginServerInfo : public FBaseLoginServer
	{
		FLoginServerInfo()
		{

		}
		~FLoginServerInfo()
		{

		}
	};

	TEST(TestLoginServerInfo_Constructor)
	{
		ZLoginServerInfo info;

		CHECK_EQUAL(0, info.m_nID);
	}

	TEST_FIXTURE(FLoginServerInfo, TestLoginServerInfoLoader_Sample1)
	{
		string strXmlSample1 = "<Maiet>						\
							   <LoginServer id='1'>			\
							   </LoginServer>				\
							   <LoginServer id='2'>			\
							   </LoginServer>				\
							   </Maiet>";

		ZLoginServerInfoManager loginServerInfoManager;
		ZLoginServerInfoLoader loader;

		bool bSuccess = loader.LoadStream(&loginServerInfoManager, strXmlSample1.c_str());
		CHECK_EQUAL(true, bSuccess);


		// 로그인서버와 마스터서버와의 관계가 1:1이 되었음으로, 한개의 로그인서버 정보만 읽도록 수정되었습니다.


		CHECK_EQUAL(1, loginServerInfoManager.GetSize());

		// 1번 서버
		ZLoginServerInfo* pLoginServerInfo = loginServerInfoManager.GetInfo(1);
		CHECK(pLoginServerInfo != NULL);
		CHECK_EQUAL(1, pLoginServerInfo->m_nID);

		// 2번 서버
		ZLoginServerInfo* pLoginServerInfo2 = loginServerInfoManager.GetInfo(2);
		CHECK(pLoginServerInfo2 == NULL);
	}
}
