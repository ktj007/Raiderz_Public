#include "stdafx.h"

#include "XNPCInfo.h"
#include "XUnitTestList.h"
#include "MockCharacter.h"
#include "XChangeNPCColor.h"

SUITE(ChangeNPCColor)
{
	class XTestChangeNPCColor : public XChangeNPCColor
	{
	public:
		MockCharacter				m_mockCharacter;

	public:
		XTestChangeNPCColor() 
		{
			TestInit();
		}

		virtual ~XTestChangeNPCColor() {}

		void TestInit()
		{
			Init(&m_mockCharacter);
		}
	};

	TEST_FIXTURE(XTestChangeNPCColor, TestChangeNPCColor)
	{
		CHANGE_NPC_COLOR npcColor;
		npcColor.colorTexSource = D3DCOLOR_ARGB(255, 100, 100, 255);
		npcColor.colorTexTarget = D3DCOLOR_ARGB(255, 200, 150, 50);

		Start(npcColor);

		Update(0.2f);

		CHECK_EQUAL(m_mockCharacter.m_colorTex, D3DCOLOR_ARGB(255, 120, 110, 213));

		Update(0.8f);

		CHECK_EQUAL(m_mockCharacter.m_colorTex, D3DCOLOR_ARGB(255, 200, 150, 50));
	}
}

/////////////////////////////////////////////////////////////////////////

