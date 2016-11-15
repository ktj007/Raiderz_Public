#include "stdafx.h"
#include "TestGame.h"
#include "XSystem.h"
#include "XUnitTestHelper.h"
#include "TestCommonFixture.h"
#include "XModuleEntity.h"
#include "MockNetClient.h"
#include "XMyPlayer.h"
#include "XModuleAction.h"

SUITE(PositionSync)
{
	TEST_FIXTURE(SimpleTestFixture, TestPostPositionSyncOnKnockback)
	{
		m_pNetAgent->IgnoreCmd(MC_FIELD_LOADING_COMPLETE);

		XMyPlayer* pMyPlayer = m_pGame->GetMyPlayer();
		XModuleAction* pModuleAction = pMyPlayer->GetModuleAction();

		pMyPlayer->SetPosition(vec3(10000.0f, 10000.0f, 0.0f));
		pMyPlayer->SetDirection(vec3(1.0f, 0.0f, 0.0f));

		int nWeight = 1000;
		vec3 vLastServerKnockbackDir = vec3(-1.0f, 0.0f, 0.0f);
		UIID nAttackerUIID = UIID_INVALID;

		stMotionFactorResult rtMotionFactor;
		stServerKnockbackVictimInfo knockbackInfo(vLastServerKnockbackDir, vec3(10000.0f, 10000.0f, 0.0f), vec3(10000.0f, 10000.0f, 0.0f) + (vLastServerKnockbackDir * nWeight)) ;
	
		pModuleAction->ActMF(MF_KNOCKBACK, 
							 nWeight, 
							 nAttackerUIID, 
							 knockbackInfo,
							 0,
							 rtMotionFactor);

		for (int i = 0; i < 15; i++)
		{
			m_pGame->Update(0.1f);
		}
		//m_pNetAgent->OutputLog("Knockback");

	}
}