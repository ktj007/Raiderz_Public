#include "stdafx.h"
#include "ZFieldInfo.h"
#include "ZSharedField.h"
#include "SDef.h"
#include "ZTestHelper.h"


SUITE(Channel)
{
	TEST(TestChannel_FirstEnterField)
	{
		const int CHANNEL_PLAYER_LIMIT_FOR_FIRST_FIELD = 70;
		/// Fixture 설치
		ZFieldInfo* pFieldInfo = new ZFieldInfo();
		pFieldInfo->m_nFieldID = FIRST_ENTER_FIELD_ID;
		pFieldInfo->m_nCapacity = CHANNEL_PLAYER_LIMIT_FOR_FIRST_FIELD;

		int nTestChannelID = 1;
		int nTestGameServerID = 1;
		ZSharedField* pSharedField = new ZSharedField(ZTestHelper::NewUID(), pFieldInfo, nTestChannelID, true, nTestGameServerID);

		CHECK_EQUAL(CHANNEL_PLAYER_LIMIT_FOR_FIRST_FIELD, pSharedField->GetPlayerLimit());
		CHECK_EQUAL(0, pSharedField->GetPlayerCount());
		CHECK_EQUAL(true, pSharedField->IsEnterable());
		CHECK_EQUAL(true, pSharedField->IsQuiet());

		/// SUT 실행
		for (int i = 0; i < ZSharedField::LIMIT_PLAYER_RATE_FOR_QUIET_CHANNEL; ++i)
			pSharedField->IncreasePlayerCount();

		/// 검증
		CHECK_EQUAL(25, pSharedField->GetPlayerCount());
		CHECK_EQUAL(true, pSharedField->IsEnterable());
		CHECK_EQUAL(false, pSharedField->IsQuiet());

		/// SUT 실행
		for (int i = 0; i < CHANNEL_PLAYER_LIMIT_FOR_FIRST_FIELD - ZSharedField::LIMIT_PLAYER_RATE_FOR_QUIET_CHANNEL; ++i)
			pSharedField->IncreasePlayerCount();

		/// 검증
		CHECK_EQUAL(CHANNEL_PLAYER_LIMIT_FOR_FIRST_FIELD, pSharedField->GetPlayerCount());
		CHECK_EQUAL(false, pSharedField->IsEnterable());
		CHECK_EQUAL(false, pSharedField->IsQuiet());


		/// 해체
		SAFE_DELETE(pFieldInfo);
		SAFE_DELETE(pSharedField);
	}

}