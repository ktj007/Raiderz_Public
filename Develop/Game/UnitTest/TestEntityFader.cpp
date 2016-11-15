#include "stdafx.h"
#include "XEntityFader.h"
#include "XModuleEntity.h"
#include "MockCharacter.h"

SUITE(EntityFader)
{
	class FEntityFader
	{
	public:
		FEntityFader()
		{
			m_pCharacter = new MockCharacter();
		}
		~FEntityFader()
		{
			delete m_pCharacter;
		}
		MockCharacter*		m_pCharacter;
	};

	TEST_FIXTURE(FEntityFader, TestMockCharacterVisibility)
	{
		CHECK_CLOSE(1.0f, m_pCharacter->GetVisibility(), 0.001f);

		m_pCharacter->SetVisibility(0.5f);
		CHECK_CLOSE(0.5f, m_pCharacter->GetVisibility(), 0.001f);
	}

	TEST_FIXTURE(FEntityFader, TestEntityFader_IsDoing)
	{
		XEntityFader fader(m_pCharacter);
		CHECK_EQUAL(false, fader.IsDoing());

		fader.Start(1.0f, 1.0f);
		CHECK_EQUAL(true, fader.IsDoing());

		fader.Update(0.1f);
		CHECK_EQUAL(true, fader.IsDoing());

		fader.Update(1.0f);
		CHECK_EQUAL(false, fader.IsDoing());
	}

	TEST_FIXTURE(FEntityFader, TestEntityFader)
	{
		XEntityFader fader(m_pCharacter);
		fader.Start(0.0f, 1.0f);

		CHECK_CLOSE(1.0f, m_pCharacter->GetVisibility(), 0.001f);

		fader.Update(0.1f);
		CHECK_CLOSE(0.9f, m_pCharacter->GetVisibility(), 0.001f);

		fader.Update(0.1f);
		CHECK_CLOSE(0.8f, m_pCharacter->GetVisibility(), 0.001f);

		fader.Update(0.8f);
		CHECK_CLOSE(0.0f, m_pCharacter->GetVisibility(), 0.001f);

		fader.Update(1.0f);
		CHECK_CLOSE(0.0f, m_pCharacter->GetVisibility(), 0.001f);
	}

	TEST_FIXTURE(FEntityFader, TestEntityFader2)
	{
		XEntityFader fader(m_pCharacter);
		fader.Start(0.0f, 1.0f, 1.0f);

		fader.Update(0.1f);
		CHECK_CLOSE(0.1f, m_pCharacter->GetVisibility(), 0.001f);

		fader.Update(0.1f);
		CHECK_CLOSE(0.2f, m_pCharacter->GetVisibility(), 0.001f);

		fader.Update(0.8f);
		CHECK_CLOSE(1.0f, m_pCharacter->GetVisibility(), 0.001f);

		fader.Update(1.0f);
		CHECK_CLOSE(1.0f, m_pCharacter->GetVisibility(), 0.001f);
	}


	TEST_FIXTURE(FEntityFader, TestEntityFader_Stop)
	{
		XEntityFader fader(m_pCharacter);

		fader.Start(0.0f, 1.0f, 1.0f);
		CHECK_EQUAL(true, fader.IsDoing());

		fader.Stop();
		CHECK_EQUAL(false, fader.IsDoing());
	}

}