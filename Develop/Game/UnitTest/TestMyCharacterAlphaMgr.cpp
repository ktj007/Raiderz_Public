#include "stdafx.h"
#include "XMyCharacterAlphaMgr.h"
#include "MockPlayer.h"
#include "XModuleEntity.h"

SUITE(MyCharacterAlphaMgr)
{
	class FMyCharacterAlphaMgr
	{
	public:
		FMyCharacterAlphaMgr()
		{
			m_pMockPlayer = new MockPlayer(MUID::ZERO, SEX_FEMALE);
			m_pMockPlayer->Create();

			m_pModuleEntity = m_pMockPlayer->GetModuleEntity();
		}
		~FMyCharacterAlphaMgr()
		{
			m_pMockPlayer->Destroy();
			SAFE_DELETE(m_pMockPlayer);
		}
		MockPlayer*			m_pMockPlayer;
		XModuleEntity*		m_pModuleEntity;
	};

	TEST_FIXTURE(FMyCharacterAlphaMgr, TestMyCharacterAlphaMgr_Check_Fading)
	{
		XMyCharacterAlphaMgr alphaMgr;

		// 카메라와의 거리
		const float DIST = 10.0f;

		CHECK_CLOSE(1.0f, m_pModuleEntity->GetVisibility(), 0.001f);
		CHECK_EQUAL(false, m_pModuleEntity->IsFading());

		alphaMgr.Check(DIST, m_pModuleEntity);

		m_pMockPlayer->Update(0.1f);

		CHECK(m_pModuleEntity->GetVisibility() < 1.0f);
		CHECK_EQUAL(true, m_pModuleEntity->IsFading());

		m_pMockPlayer->Update(10.0f);
		CHECK_EQUAL(false, m_pModuleEntity->IsFading());
	}

	TEST_FIXTURE(FMyCharacterAlphaMgr, TestMyCharacterAlphaMgr_Check_Appear)
	{
		XMyCharacterAlphaMgr alphaMgr;

		alphaMgr.Check(1.0f, m_pModuleEntity);
		m_pMockPlayer->Update(10.0f);

		CHECK_CLOSE(alphaMgr.CONST_ALPHA_VALUE(), m_pModuleEntity->GetVisibility(), 0.001f);
		CHECK_EQUAL(false, m_pModuleEntity->IsFading());

		m_pMockPlayer->Update(10.0f);

		// 카메라와의 거리
		const float DIST = 200.0f;

		alphaMgr.Check(DIST, m_pModuleEntity);
		m_pMockPlayer->Update(0.1f);

		CHECK(m_pModuleEntity->GetVisibility() > alphaMgr.CONST_ALPHA_VALUE());
		CHECK_EQUAL(true, m_pModuleEntity->IsFading());

		m_pMockPlayer->Update(10.0f);
		CHECK_EQUAL(false, m_pModuleEntity->IsFading());
	}

}