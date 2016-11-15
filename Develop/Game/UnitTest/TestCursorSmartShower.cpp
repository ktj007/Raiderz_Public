#include "stdafx.h"
#include "XCursorSmartShower.h"

SUITE(CursorSmartShower)
{
	/*

	* Ctrl키를 누르면 마우스 커서가 나오거나 사라진다. (Toggle)
	* Ctrl키를 오래 누르고 있으면 마우스 커서가 계속 나와있는 상태가 된다.
	* 이동키를 누르면 마우스 커서가 자동으로 사라진다.
		그러나 Ctrl키를 누르고 있으면 마우스 커서가 사라지지 않는다.


	*/

	class TestCursorSmartShower : public XCursorSmartShower
	{
	protected:
		bool	m_bMouseCursorVisible;

		virtual void ShowCursor(bool bVisible)
		{
			m_bMouseCursorVisible = bVisible;
		}
	public:
		TestCursorSmartShower() : XCursorSmartShower(), m_bMouseCursorVisible(false) {}
		virtual ~TestCursorSmartShower() {}
		virtual bool IsMouseCursorVisible()
		{
			return m_bMouseCursorVisible;
		}
	};

	class FCursorSmartShower
	{
	public:
		FCursorSmartShower()
		{

		}
		~FCursorSmartShower()
		{

		}

		TestCursorSmartShower	m_Shower;
	};

	TEST_FIXTURE(FCursorSmartShower, TestCursorSmartShower_OnKeyDown)
	{
		CHECK_EQUAL(false, m_Shower.IsMouseCursorVisible());

		m_Shower.OnKeyDown();

		CHECK_EQUAL(true, m_Shower.IsMouseCursorVisible());
	}

	TEST_FIXTURE(FCursorSmartShower, TestCursorSmartShower_OnKeyDown_ReturnValue)
	{
		CHECK_EQUAL(true, m_Shower.OnKeyDown());

		// KeyUp없이 Down만 되면 false로 리턴한다.
		CHECK_EQUAL(false, m_Shower.OnKeyDown());
		CHECK_EQUAL(false, m_Shower.OnKeyDown());

		m_Shower.OnKeyUp();

		CHECK_EQUAL(true, m_Shower.OnKeyDown());
	}

	TEST_FIXTURE(FCursorSmartShower, TestCursorSmartShower_OnKeyDown_Repeatedly)
	{
		CHECK_EQUAL(false, m_Shower.IsMouseCursorVisible());

		// KeyDown이 반복해서 날라와도 KeyUp이 안날라왔으면 한번만 처리한다.
		m_Shower.OnKeyDown();
		m_Shower.OnKeyDown();
		m_Shower.OnKeyDown();
		m_Shower.OnKeyDown();

		CHECK_EQUAL(true, m_Shower.IsMouseCursorVisible());
	}

	TEST_FIXTURE(FCursorSmartShower, TestCursorSmartShower_OnKeyUp)
	{
		CHECK_EQUAL(false, m_Shower.IsMouseCursorVisible());

		m_Shower.OnKeyDown();
		m_Shower.OnKeyUp();

		CHECK_EQUAL(true, m_Shower.IsMouseCursorVisible());


		// 한번 더 누르면 다시 사라짐
		m_Shower.OnKeyDown();
		m_Shower.OnKeyUp();

		CHECK_EQUAL(false, m_Shower.IsMouseCursorVisible());
	}

}