#include "stdafx.h"
#include "GTestForward.h"
#include "MockField.h"
#include "GBuffEntity.h"
#include "FBaseGame.h"
#include "GActorBuffEntity.h"

SUITE(BuffEntity)
{
	struct FBuffEntity : public FBaseGame
	{
		FBuffEntity()
			: nIncludeBuffDefaultID(777)
			, fBuffDuration(10.0f)
		{
			pBuffEntity = CreateBuffEntity(m_pPlayer, nIncludeBuffDefaultID, vec3(0,0,0), vec3(0,1,0), fBuffDuration);
		}

		~FBuffEntity()
		{
		}

		GBuffEntity* CreateBuffEntity(GEntityActor* pOwner, int nIncludeBuffID, const vec3& pos, const vec3& dir, float fDuration, int nLimitQty=0)
		{
			VALID_RET(pOwner, NULL);

			GBuffInfo* pBuff = test.buff.NewBuffInfo(nIncludeBuffID);
			pBuff->m_fDuration = fDuration;

			GBuffEntity* entity = GBuffEntity::Spawn(pOwner, pos, nIncludeBuffID, nLimitQty);
			entity->SetDir(dir);
			entity->SetFacingDir(dir);
			return entity;
		}

		const int nIncludeBuffDefaultID;
		const float fBuffDuration;
		GBuffEntity*	pBuffEntity;
		DECLWRAPPER_BuffMgr;
	};

	TEST_FIXTURE(FBuffEntity, DeleteEntityWhenExpired)
	{
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_BUFF_ENTITY));
		CHECK_EQUAL(true, pBuffEntity->HasBuff(nIncludeBuffDefaultID));
		CHECK_EQUAL(1, m_pPlayer->GetActorBuffEntity().GetQty(nIncludeBuffDefaultID));
		m_pField->Update(fBuffDuration);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_BUFF_ENTITY));
		CHECK_EQUAL(0, m_pPlayer->GetActorBuffEntity().GetQty(nIncludeBuffDefaultID));
	}

	TEST_FIXTURE(FBuffEntity, DeleteEntityWhenOwnerInvalid)
	{
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_BUFF_ENTITY));
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_PLAYER));
		CHECK_EQUAL(true, pBuffEntity->HasBuff(nIncludeBuffDefaultID));
		m_pPlayer->DeleteMe();
		m_pField->Update(0.0f);
		m_pField->Update(0.0f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_PLAYER));
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_BUFF_ENTITY));
	}

	TEST_FIXTURE(FBuffEntity, LimitQty)
	{
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_BUFF_ENTITY));
		m_pField->Update(fBuffDuration);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_BUFF_ENTITY));
		const int nLimitQty = 5;
		for (int i=0; i<100; i++)
		{
			CreateBuffEntity(m_pPlayer, nIncludeBuffDefaultID, vec3(0,0,0), vec3(0,1,0), fBuffDuration, nLimitQty);
		}
		m_pField->Update(0.0f);
		CHECK_EQUAL(nLimitQty, m_pField->GetEntityQty(ETID_BUFF_ENTITY));
	}
}
