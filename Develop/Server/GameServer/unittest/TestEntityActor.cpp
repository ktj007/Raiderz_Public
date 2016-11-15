#include "stdafx.h"
#include "GEntityActor.h"
#include "GEntityNPC.h"
#include "MockField.h"
#include "GCommonFixture.h"

SUITE(EntityActor)
{
	TEST_FIXTURE(CommonFixture_FN, SetDirForwardToActor)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vec3(0.0f, 1000.0f, 0.0f));
		GEntityNPC* pTargetNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vec3(0.0f, 0.0f, 0.0f));


		pNPC->SetDir(vec3(0.0f, 1.0f, 0.0f));


		// 타겟 방향으로 방향 변경됨 --
		pNPC->SetPos(vec3(0.0f, 1000.0f, 0.0f));

		pNPC->SetDirForwardToActor(pTargetNPC);

		CHECK_CLOSE(0.0f, pNPC->GetDir().x, 0.001f);
		CHECK_CLOSE(-1.0f, pNPC->GetDir().y, 0.001f);
		CHECK_CLOSE(0.0f, pNPC->GetDir().z, 0.001f);
		CHECK_CLOSE(0.0f, pNPC->GetFacingDir().x, 0.001f);
		CHECK_CLOSE(-1.0f, pNPC->GetFacingDir().y, 0.001f);
		CHECK_CLOSE(0.0f, pNPC->GetFacingDir().z, 0.001f);


		// 자기 자신일 때에는 현재 방향 그대로 유지한다. --
		pNPC->SetDir(vec3(0.0f, 1.0f, 0.0f));

		pNPC->SetDirForwardToActor(pNPC);

		CHECK_CLOSE(0.0f, pNPC->GetDir().x, 0.001f);
		CHECK_CLOSE(1.0f, pNPC->GetDir().y, 0.001f);
		CHECK_CLOSE(0.0f, pNPC->GetDir().z, 0.001f);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
		pTargetNPC->Destroy();
		SAFE_DELETE(pTargetNPC);
	}

}