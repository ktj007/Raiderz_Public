#include "stdafx.h"
#include "PBounceMail.h"
#include "PFixtureHelper.h"
#include "PTestWrapper.h"
#include "PPostOfficeSystem.h"
#include "SProxyPlayer.h"
#include "PSystem.h"

SUITE(BounceMail)
{
	struct TestBounceMail: public FBaseMail
	{
		TestBounceMail()
		{
			// do nothing
		}
	};

	TEST_FIXTURE(TestBounceMail, Export)
	{
		// arrange
		CID nCID = m_pProxyPlayer->GetCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);

		uint32 nNowTime = gsys.pSystem->GetNowTime();


		// act
		PBounceMail mail(nNowTime, nCID, &info);

		TD_MAIL_MAILBOX_MAIL_INFO bounceMailInfo;
		mail.Export(&bounceMailInfo);

		
		// assert
		CHECK_EQUAL(nCID, mail.GetCID());
		CHECK(info == bounceMailInfo);
	}

	TEST_FIXTURE(TestBounceMail, IsReady_NormalTime)
	{
		// arrange
		CID nCID = m_pProxyPlayer->GetCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);

		uint32 nNowTime = 0;


		PBounceMail mail(nNowTime, nCID, &info);


		// act, assert
		CHECK(!mail.IsReady(10000));		
		CHECK(!mail.IsReady(20000));		
		CHECK(!mail.IsReady(29999));

		CHECK(mail.IsReady(30000));
		CHECK(mail.IsReady(40000));
	}

	TEST_FIXTURE(TestBounceMail, IsReady_OverflowTime)
	{
		// arrange
		CID nCID = m_pProxyPlayer->GetCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);

		uint32 nNowTime = UINT_MAX;


		PBounceMail mail(nNowTime, nCID, &info);


		// act, assert
		CHECK(!mail.IsReady(UINT_MAX));
		CHECK(!mail.IsReady(0));
		CHECK(!mail.IsReady(29998));

		CHECK(mail.IsReady(29999));		
		CHECK(mail.IsReady(30000));
	}

	TEST_FIXTURE(TestBounceMail, IsReady_OverflowTime_2)
	{
		// arrange
		CID nCID = m_pProxyPlayer->GetCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);

		uint32 nNowTime = UINT_MAX - 30000;


		PBounceMail mail(nNowTime, nCID, &info);


		// act, assert
		CHECK(!mail.IsReady(UINT_MAX - 1));
		CHECK(mail.IsReady(UINT_MAX));
		CHECK(mail.IsReady(0));
	}
}
