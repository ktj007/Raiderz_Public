#include "stdafx.h"
#include "GEnvManager.h"

SUITE(EnvManager)
{
	/// 테스트를 위해 LocalTime을 오버라이드한 EnvManager 클래스
	class TestEnvManager : public GEnvManager
	{
	protected:
		tm			m_LocalTime;
		virtual tm*			GetLocalTime()
		{
			return &m_LocalTime;
		}
	public:
		TestEnvManager() : GEnvManager() {}
		virtual ~TestEnvManager() {}
		using GEnvManager::CheckGameTimeHour;

		void ExpectLocalTime(int nHour, int nMin, int nSec)
		{
			m_LocalTime.tm_hour = nHour;
			m_LocalTime.tm_min = nMin;
			m_LocalTime.tm_sec = nSec;
		}
	};
	class FEnvManager
	{
	public:
		FEnvManager()
		{

		}
		~FEnvManager()
		{

		}
		void Init(int nSecPerGameTimeOneHour)
		{
			m_EnvManager.ExpectLocalTime(0, 0, 0);

			GEnvParam envParam;
			envParam.nStartHour[TIME_DAWN]		= 4;
			envParam.nStartHour[TIME_DAYTIME]	= 7;
			envParam.nStartHour[TIME_SUNSET]	= 18;
			envParam.nStartHour[TIME_NIGHT]		= 21;
			envParam.nUpdateTickSec = 1;			// 1초마다 틱 체크하도록 함
			envParam.nLocalTimePerGameTimeOneHour = nSecPerGameTimeOneHour;	

			m_EnvManager.Create(envParam);
		}

		TestEnvManager	m_EnvManager;
	};

	TEST_FIXTURE(FEnvManager, TestEnvManager_UpdateGameTime)
	{
		const float ENV_TICK = 1.1f;

		Init(3600);	// 현실 시간과 같게.. (1시간 == 3600초)

		m_EnvManager.ExpectLocalTime(0, 0, 0);

		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_NIGHT, m_EnvManager.GetCurrentTime());

		// 아직 3시라 시간이 바뀌지 않음
		m_EnvManager.ExpectLocalTime(3, 0, 0);

		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_NIGHT, m_EnvManager.GetCurrentTime());

		// 5시라 DAWN으로 바뀜
		m_EnvManager.ExpectLocalTime(5, 0, 1);

		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_DAWN, m_EnvManager.GetCurrentTime());

		// 7시라 DAYTIME으로 바뀜
		m_EnvManager.ExpectLocalTime(7, 0, 1);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_DAYTIME, m_EnvManager.GetCurrentTime());

		// 18시라 TIME_SUNSET으로 바뀜
		m_EnvManager.ExpectLocalTime(18, 0, 1);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_SUNSET, m_EnvManager.GetCurrentTime());

		// 21시라 TIME_NIGHT으로 바뀜
		m_EnvManager.ExpectLocalTime(21, 0, 1);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_NIGHT, m_EnvManager.GetCurrentTime());

	}

	TEST_FIXTURE(FEnvManager, TestEnvManager_CheckGameTimeHour)
	{
		const float ENV_TICK = 1.1f;
		Init(3600);	// 현실 시간과 같게..

		m_EnvManager.ExpectLocalTime(0, 0, 0);
		int nCurrentGameTimeHour = m_EnvManager.CheckGameTimeHour();
		CHECK_EQUAL(0, nCurrentGameTimeHour);

		m_EnvManager.ExpectLocalTime(1, 0, 0);
		nCurrentGameTimeHour = m_EnvManager.CheckGameTimeHour();
		CHECK_EQUAL(1, nCurrentGameTimeHour);

		m_EnvManager.ExpectLocalTime(17, 0, 0);
		nCurrentGameTimeHour = m_EnvManager.CheckGameTimeHour();
		CHECK_EQUAL(17, nCurrentGameTimeHour);

		m_EnvManager.ExpectLocalTime(17, 30, 0);
		nCurrentGameTimeHour = m_EnvManager.CheckGameTimeHour();
		CHECK_EQUAL(17, nCurrentGameTimeHour);

	}

	TEST_FIXTURE(FEnvManager, TestEnvManager_CheckGameTimeHour_ByChangeTimeForced)
	{
		const float ENV_TICK = 1.1f;
		Init(3600);	// 현실 시간과 같게..

		m_EnvManager.ExpectLocalTime(10, 0, 0);
		m_EnvManager.ChangeTimeForced(TIME_DAWN);	// 10시가 TIME_DAWN 시작

		CHECK_EQUAL(4, m_EnvManager.CheckGameTimeHour());		// Dawn은 4:00 am부터

		m_EnvManager.ExpectLocalTime(20, 0, 0);					// 10시간 후니까 10+4
		CHECK_EQUAL(14, m_EnvManager.CheckGameTimeHour());

		m_EnvManager.ExpectLocalTime(3, 0, 0);					// 17시간 후니까 17+4
		CHECK_EQUAL(21, m_EnvManager.CheckGameTimeHour());


		// --
		m_EnvManager.ExpectLocalTime(3, 0, 0);
		m_EnvManager.ChangeTimeForced(TIME_SUNSET);	// 18시가 TIME_SUNSET 시작

		CHECK_EQUAL(18, m_EnvManager.CheckGameTimeHour());	// Sunset는 6:00 pm부터

		m_EnvManager.ExpectLocalTime(4, 0, 0);
		CHECK_EQUAL(19, m_EnvManager.CheckGameTimeHour());

		m_EnvManager.ExpectLocalTime(5, 0, 0);
		CHECK_EQUAL(20, m_EnvManager.CheckGameTimeHour());

		m_EnvManager.ExpectLocalTime(6, 0, 0);
		CHECK_EQUAL(21, m_EnvManager.CheckGameTimeHour());

		m_EnvManager.ExpectLocalTime(7, 0, 0);
		CHECK_EQUAL(22, m_EnvManager.CheckGameTimeHour());

		m_EnvManager.ExpectLocalTime(2, 0, 0);
		CHECK_EQUAL(17, m_EnvManager.CheckGameTimeHour());

	}

	TEST_FIXTURE(FEnvManager, TestEnvManager_ChangeTimeForced)
	{
		const float ENV_TICK = 1.1f;

		Init(3600);	// 현실 시간과 같게..

		m_EnvManager.ExpectLocalTime(5, 0, 0);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_DAWN, m_EnvManager.GetCurrentTime());

		// 강제로 시간 변경 - 5시지만 SUNSET으로 변경
		m_EnvManager.ChangeTimeForced(TIME_SUNSET);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_SUNSET, m_EnvManager.GetCurrentTime());
		CHECK_EQUAL(18, m_EnvManager.GetCurrentTimeHour());

		m_EnvManager.ExpectLocalTime(6, 0, 0);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_SUNSET, m_EnvManager.GetCurrentTime());
		CHECK_EQUAL(19, m_EnvManager.GetCurrentTimeHour());
		

		// night
		m_EnvManager.ExpectLocalTime(5+3, 0, 0);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_NIGHT, m_EnvManager.GetCurrentTime());

		// dawn
		m_EnvManager.ExpectLocalTime(5+3+7, 0, 0);
		m_EnvManager.Update(ENV_TICK);
		CHECK_EQUAL(TIME_DAWN, m_EnvManager.GetCurrentTime());

	}
}
