#include "stdafx.h"
#include "XEffectManager.h"
#include "MockEffect.h"
#include "TestGame.h"
#include "XMyPlayer.h"
#include "XCommonData.h"

SUITE(Effect)
{
	class TestEffectMgr : public XEffectManager
	{
	public:
		RSceneNode*		m_pSceneNode;
		RActorNode*		m_pActorNode;

	public:
		TestEffectMgr() 
		{
			m_pSceneNode	= NULL;
			m_pActorNode	= NULL;
		}
		virtual ~TestEffectMgr() {}

		virtual MockEffect*	CreateXEffect(const wstring& szEffectName)
		{
			MockEffect * pEffect = new MockEffect;

			MUID UIDEffect = global.system->GenerateLocalUID();
			pEffect->Create(UIDEffect, szEffectName);

			m_vEffectList.push_back(pEffect);
			return pEffect;
		}

		virtual void	AttachXEffect(XModelEffect* pEffect, RSceneNode* pSceneNode, RActorNode* pActorNode, const RMatrix& _rBaseFrame = RMatrix::IDENTITY, bool bIgnoreDir = false)
		{
			m_pSceneNode = pSceneNode;
			m_pActorNode = pActorNode;
		}

		int GetEffectListCount()
		{
			return m_vEffectList.size();
		}
	};

	TEST_FIXTURE(TestEffectMgr, TestEffectInstance)
	{
		MockEffect * pEffect = CreateXEffect(L"ef_fade");
		
		CHECK_EQUAL(1, GetEffectListCount());

		DeleteXEffect(pEffect->GetMUID());

		CHECK_EQUAL(0, GetEffectListCount());
	}

	TEST_FIXTURE(TestEffectMgr, TestEffectDurationTime)
	{
		MockEffect * pEffect = CreateXEffect(L"ef_fade");

		pEffect->SetDurationTime(1.0f);
		pEffect->m_bLoadingComplete = true;

		OnUpdate(0.5f);

		CHECK_EQUAL(1, GetEffectListCount());

		OnUpdate(0.5f);

		CHECK_EQUAL(0, GetEffectListCount());

		DeleteXEffect(pEffect->GetMUID());
	}

	TEST_FIXTURE(TestEffectMgr, TestEffectAddEffect_FollowActor_Terrain)
	{
		TestGame	m_TestGame;
		XMyPlayer* pMyPlayer = m_TestGame.GetMyPlayer();

		MMatrix matPost;
		matPost.MakeIdentity();
		vec3 vLocalPos = vec3::ZERO;

		EFFECT_CREATE_DATA effectCreateData(L"ef_Terrain", L"", pMyPlayer->GetUID(), matPost, vLocalPos, 0.0f, 1.0f, 0.0f, 0.0f, false, false, false, true, true, true, false, false, false);

		MUID uidEffect = AddXEffect(effectCreateData);

		CHECK(m_pSceneNode == NULL);
		CHECK(m_pActorNode == NULL);

		DeleteXEffect(uidEffect);
	}
}