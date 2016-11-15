#include "stdafx.h"
#include "TestCommonFixture.h"
#include "TestModule.h"
#include "XModuleCollision.h"

SUITE(MyActionIdle)
{
	TEST_FIXTURE(SimpleTestFixture, CheckInteractionObject)
	{
		TestModuleContext testContext(this);

		XNonPlayer* pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(1, CHECK_INTERACTION_DISTANCE*0.5f, 0.0f));
		CHECK(pNPC != NULL);
		if (pNPC == NULL) return;		

		XMyPlayer* pMyPlayer = testContext.GetMyPlayer();
		CHECK(pMyPlayer != NULL);
		if (pMyPlayer == NULL) return;		

		pMyPlayer->SetPosition(vec3(1.0f, 1.0f, 0.0f));
		pMyPlayer->SetDirection(vec3(0.0f, 1.0f, 0.0f));

		XModuleCollision* pModuleCollision = testContext.GetModule<XModuleCollision>(pMyPlayer);
		if( pModuleCollision == NULL) return;

		vec3 vPos = vec3(0.f, 0.f, 100.f);
		vec3 vDir = pMyPlayer->GetDirection();

		float fCheckX = 100.f;
		float fBottomZ = -CHECK_INTERACTION_COL_Z*0.5f;
		RBoundingBox CollisionBox;
		CollisionBox.vmax.Set( fCheckX, fCheckX, CHECK_INTERACTION_COL_Z);
		CollisionBox.vmin.Set(-fCheckX, -CHECK_INTERACTION_DISTANCE, fBottomZ);

		float fDist		= FLT_MAX;
		XObject* pObject = pModuleCollision->GetInteractionObjectColBox(FIND_ALL, vPos, vDir, CollisionBox, fDist);
		CHECK(pObject != NULL);

		if (pObject)
		{
			CHECK(pObject->GetName() == wstring(L"sample"));
		}

		pNPC->SetPosition(vec3(1, CHECK_INTERACTION_DISTANCE*1.5f, 0.0f));
		pObject = pModuleCollision->GetInteractionObjectColBox(FIND_ALL, vPos, vDir, CollisionBox, fDist);
		CHECK(pObject == NULL);

		pNPC->SetPosition(vec3(1, CHECK_INTERACTION_DISTANCE*0.5f, 0.0f));
		pMyPlayer->SetDirection(vec3(1.0f, 0.0f, 0.0f));
		vDir = pMyPlayer->GetDirection();
		pObject = pModuleCollision->GetInteractionObjectColBox(FIND_ALL, vPos, vDir, CollisionBox, fDist);
		CHECK(pObject == NULL);
	}
}