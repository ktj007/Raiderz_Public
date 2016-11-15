#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GUTHelper_Field.h"

SUITE(TalentHelper)
{
	struct Fixture
	{
		Fixture()
		{
			GUTHelper_Field::NewMockFieldAndPlayer(m_pField, m_pPlayer);
		}

		~Fixture()
		{
			SAFE_DELETE(m_pField);
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
	};

	TEST_FIXTURE(Fixture, FindTalentInfo)
	{

	}
}