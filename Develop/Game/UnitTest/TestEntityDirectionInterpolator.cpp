#include "stdafx.h"
#include "XEntityDirectionInterpolator.h"
#include "FTestGame.h"
#include "FBaseNPCInfo.h"
#include "XNonPlayer.h"
#include "XModuleEntity.h"

SUITE(EntityDirectionInterpolator)
{
	class FEntityDirectionInterpolator : public FTestGame, public FBaseNPCInfo
	{
	public:
		FEntityDirectionInterpolator()
		{
			XUnitTestHelper::NewNPCInfo(1);
			m_pNPC = this->NewNonPlayer(1);
			m_pModuleEntity = m_pNPC->GetModuleEntity();

			m_pInterpolator = new XEntityDirectionInterpolator(m_pModuleEntity);
		}
		~FEntityDirectionInterpolator()
		{
			delete m_pInterpolator;
		}
		XNonPlayer*		m_pNPC;
		XModuleEntity*	m_pModuleEntity;
		XEntityDirectionInterpolator*	m_pInterpolator;
	};

	TEST_FIXTURE(FEntityDirectionInterpolator, TestConstructor)
	{
		CHECK_EQUAL(false, m_pInterpolator->IsDoing());
	}

	TEST_FIXTURE(FEntityDirectionInterpolator, TestStartInterpolation)
	{
		vec3 vCurrDir = vec3(0.0f, 1.0f, 0.0f);
		vec3 vToDir = vec3(1.0f, 0.0f, 0.0f);
		float fRotateSpeed = 1.0f;		// 한바퀴도는데 1초

		// 두 각도 사이가 90도니까 0.25초면 보간이 끝난다.

		m_pInterpolator->Start(vCurrDir, vToDir, fRotateSpeed);

		CHECK_EQUAL(true, m_pInterpolator->IsDoing());
	}


	TEST_FIXTURE(FEntityDirectionInterpolator, TestStartInterpolation_And_Finish_Immediately)
	{
		vec3 vCurrDir = vec3(0.0f, 1.0f, 0.0f);
		vec3 vToDir = vCurrDir;
		float fRotateSpeed = 1.0f;		// 한바퀴도는데 1초

		m_pInterpolator->Start(vCurrDir, vToDir, fRotateSpeed);


		// 두 각도가 같으므로 바로 보간 끝
		CHECK_EQUAL(false, m_pInterpolator->IsDoing());
	}

	TEST_FIXTURE(FEntityDirectionInterpolator, TestUpdate)
	{
		vec3 vCurrDir = vec3(0.0f, 1.0f, 0.0f);
		vec3 vToDir = vec3(1.0f, 0.0f, 0.0f);
		float fRotateSpeed = 1.0f;		// 한바퀴도는데 1초

		m_pInterpolator->Start(vCurrDir, vToDir, fRotateSpeed);

		m_pInterpolator->Update(0.1f);

		vec3 vEntityDir = m_pModuleEntity->GetDirection();
		CHECK(vCurrDir != vEntityDir);


		// 보간 끝
		m_pInterpolator->Update(0.25f);

		vEntityDir = m_pModuleEntity->GetDirection();
		CHECK(vToDir == vEntityDir);

	}


	TEST_FIXTURE(FEntityDirectionInterpolator, TestStop)
	{
		vec3 vCurrDir = vec3(0.0f, 1.0f, 0.0f);
		vec3 vToDir = vec3(1.0f, 0.0f, 0.0f);

		m_pInterpolator->Start(vCurrDir, vToDir, 1.0f);

		CHECK_EQUAL(true, m_pInterpolator->IsDoing());

		m_pInterpolator->Update(0.01f);
		CHECK_EQUAL(true, m_pInterpolator->IsDoing());

		m_pInterpolator->Stop();
		CHECK_EQUAL(false, m_pInterpolator->IsDoing());


		// 보간이 중간에 끝났으므로 더이상 회전하지 않는다.
		vec3 vDir1 = m_pModuleEntity->GetDirection();
		m_pInterpolator->Update(0.01f);
		vec3 vDir2 = m_pModuleEntity->GetDirection();

		CHECK_CLOSE(vDir1.x, vDir2.x, 0.0001f);
		CHECK_CLOSE(vDir1.y, vDir2.y, 0.0001f);
		CHECK_CLOSE(vDir1.z, vDir2.z, 0.0001f);
	}

}