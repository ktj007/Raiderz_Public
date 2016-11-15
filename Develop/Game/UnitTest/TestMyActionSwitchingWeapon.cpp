#include "stdafx.h"
#include "XMyActionSwitchingWeapon.h"
#include "FMyActionState.h"
#include "XMyPlayer.h"

SUITE(MyActionSwitchingWeapon)
{
	class FMyActionSwitchingWeapon : public FMyActionState
	{
	public:
		FMyActionSwitchingWeapon()
		{
			m_pMyActionSwitchingWeapon = new XMyActionSwitchingWeapon(m_pMyPlayer->GetModuleMyControl());
		}
		~FMyActionSwitchingWeapon()
		{
			delete m_pMyActionSwitchingWeapon;
		}

		XMyActionSwitchingWeapon*		m_pMyActionSwitchingWeapon;
	};

	TEST_FIXTURE(FMyActionSwitchingWeapon, TestmyActionSwitchingWeapon_CheckEnterable_SwitchingWeapon_WhenNormalStance)
	{
		m_pMyActionSwitchingWeapon->CheckEnterable(NULL);

		CHECK_EQUAL(MC_ACTION_SWITCHING_WEAPON_REQ, m_pNetAgent->GetSendCommandID(0));
	}

	TEST_FIXTURE(FMyActionSwitchingWeapon, TestmyActionSwitchingWeapon_CheckEnterable_CheckTime)
	{
		Update(1.0f);
		m_pNetAgent->ResetSendCommands();

		m_pMyActionSwitchingWeapon->CheckEnterable(NULL);

		CHECK_EQUAL(MC_ACTION_SWITCHING_WEAPON_REQ, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

		// 0.2초 지남
		Update(0.2f);

		// 무기 바꿈 커맨드는 0.5초 후에 다시 보낼 수 있다.
		m_pMyActionSwitchingWeapon->CheckEnterable(NULL);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());


		// 1초 지남
		Update(1.0f);

		// 무기 바꿈 커맨드는 0.5초 후에 다시 보낼 수 있다.
		m_pMyActionSwitchingWeapon->CheckEnterable(NULL);

		CHECK_EQUAL(MC_ACTION_SWITCHING_WEAPON_REQ, m_pNetAgent->GetSendCommandID(1));
		CHECK_EQUAL(2, m_pNetAgent->GetSendCommandCount());

	}
}