#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GFieldMgr.h"
#include "GDuelMgr.h"
#include "FBasePlayer.h"
#include "MockDuel.h"
#include "GTestForward.h"
#include "GUTHelper_Duel.h"

SUITE(EffectTargetSelector)
{
	struct FEffectTargetSelector : public FBasePlayer
	{
		FEffectTargetSelector()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pSelf = NewEntityPlayer(m_pField, vec3(0.0f, 0.0f, 0.0f));			
			m_pTarget = NewEntityPlayer(m_pField, vec3(10000.0f, 10000.0f, 0.0f));			
		}

		virtual ~FEffectTargetSelector() 
		{
			m_pField->Destroy();
			test.network.ClearLinks();
		}

		bool IsEffectTarget(const vector<GEntityActor*>& vecEffectTarget, GEntityActor* pTarget)
		{
			return (find(vecEffectTarget.begin(), vecEffectTarget.end(), pTarget) != vecEffectTarget.end());
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pSelf;		
		GEntityPlayer*	m_pTarget;		
		CSEffectInfo	m_EffectInfo;
		GEffectTargetSelector m_EffectTargetSelector;
	};

	namespace EffectPointAndRadius
	{
		struct FEffectPointAndRadius : public FEffectTargetSelector
		{
			FEffectPointAndRadius()
			{
				m_pSelfNeighbor = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(1.0f, 1.0f, 0.0f));
				m_pTargetNeighbor = NewEntityPlayer(m_pField, m_pTarget->GetPos()+vec3(1.0f, 1.0f, 0.0f));
			}

			virtual ~FEffectPointAndRadius()
			{

			}

			GEntityPlayer*	m_pSelfNeighbor;	// 자신과 가까이 있는자
			GEntityPlayer*	m_pTargetNeighbor;	// 대상과 가까이 있는자
		};

		namespace Self
		{
			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				m_EffectInfo.m_fRadius = 0.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(1, vecEffectTarget.size());
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(false, IsEffectTarget(vecEffectTarget, m_pSelfNeighbor));
			}

			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsNotZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(2, vecEffectTarget.size());
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pSelfNeighbor));
			}
		}

		namespace Target
		{
			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
				m_EffectInfo.m_fRadius = 0.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(1, vecEffectTarget.size());
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pTarget));
				CHECK_EQUAL(false, IsEffectTarget(vecEffectTarget, m_pTargetNeighbor));
			}

			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsNotZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(2, vecEffectTarget.size());
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pTarget));
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pTargetNeighbor));
			}
		}

		namespace Caster
		{
			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_CASTER;
				m_EffectInfo.m_fRadius = 0.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pCaster = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(1, vecEffectTarget.size());
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(false, IsEffectTarget(vecEffectTarget, m_pSelfNeighbor));
			}

			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsNotZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_CASTER;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pCaster = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(2, vecEffectTarget.size());
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pSelfNeighbor));
			}
		}

		namespace HitCapsule
		{
			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_HITCAPSULE;
				m_EffectInfo.m_fRadius = 0.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pTarget));
				CHECK_EQUAL(false, IsEffectTarget(vecEffectTarget, m_pTargetNeighbor));
			}

			TEST_FIXTURE(FEffectPointAndRadius, RadiusIsNotZero)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_HITCAPSULE;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(2, vecEffectTarget.size());
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pTarget));
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pTargetNeighbor));
			}
		}

		namespace AreaPoint
		{
			TEST_FIXTURE(FEffectPointAndRadius, AreaPoint_Basic)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_AREAPOINT;
				m_EffectInfo.m_fRadius = 1000.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
				m_EffectInfo.m_nLimit = INT_MAX;
				m_pTarget->SetPos(vec3(10000,10000,0));
				m_pTargetNeighbor->SetPos(vec3(0,0,0));

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				desc.vCenterPoint = vec3(10000,10000,0);
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);
				CHECK_EQUAL(true, IsEffectTarget(vecEffectTarget, m_pTarget));
				CHECK_EQUAL(false, IsEffectTarget(vecEffectTarget, m_pTargetNeighbor));
			}
		}
	}

	namespace EffectRelation
	{
		struct FEffectRelation : public FEffectTargetSelector
		{
			FEffectRelation()
			{
				m_pSelfNeighborNPC = GUTHelper_NPC::SpawnNPC(m_pField, INVALID_ID, m_pSelf->GetPos()+vec3(1.0f,1.0f,0.0f));
				m_pSelfNeighborPlayer = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(1.0f,1.0f,0.0f));
				m_pSelfNeighborDuelPC = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(1.0f,1.0f,0.0f));
				m_pDuel = GUTHelper_Duel::NewMockDuel_Fighting(m_pSelf, m_pSelfNeighborDuelPC);
			}

			GEntityNPC*		m_pSelfNeighborNPC;		// 자신과 가까이 있는 NPC			
			GEntityPlayer*	m_pSelfNeighborPlayer;	// 자신과 가까이 있는 PC
			GEntityPlayer*	m_pSelfNeighborDuelPC;	// 자신과 가까이 있는 결투 대상 PC			
			GNPCInfo*		m_pDuelFlagNPCInfo;
			MockDuel*			m_pDuel;
		};

		TEST_FIXTURE(FEffectRelation, All)
		{
			m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
			m_EffectInfo.m_fRadius = 100.0f;
			m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
			m_EffectInfo.m_nLimit = INT_MAX;

			m_pSelf->SetDuel(NULL);
			m_pTarget->SetDuel(NULL);

			GEffectTargetSelector::Desc desc(m_EffectInfo);
			desc.pSelf = m_pSelf;
			desc.pTarget = m_pTarget;
			vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

			CHECK_EQUAL(3, vecEffectTarget.size());
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelf));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborNPC));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborPlayer));
			CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborDuelPC));	 // 결투자에게는 영향가지 않음		

			m_pDuel->Cancel();
		}

		TEST_FIXTURE(FEffectRelation, Enemy)
		{
			m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
			m_EffectInfo.m_fRadius = 100.0f;
			m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
			m_EffectInfo.m_nLimit = INT_MAX;

			GEffectTargetSelector::Desc desc(m_EffectInfo);
			desc.pSelf = m_pSelf;
			desc.pTarget = m_pTarget;
			vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

			CHECK_EQUAL(2, vecEffectTarget.size());
			CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelf));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborNPC));
			CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborPlayer));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborDuelPC));

			m_pDuel->Cancel();
		}

		TEST_FIXTURE(FEffectRelation, Allied)
		{
			m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
			m_EffectInfo.m_fRadius = 100.0f;
			m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALLIED;
			m_EffectInfo.m_nLimit = INT_MAX;

			GEffectTargetSelector::Desc desc(m_EffectInfo);
			desc.pSelf = m_pSelf;
			desc.pTarget = m_pTarget;
			vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

			CHECK_EQUAL(2, vecEffectTarget.size());
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelf));
			CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborNPC));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborPlayer));
			CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborDuelPC));

			m_pDuel->Cancel();
		}

		namespace Party
		{
			TEST_FIXTURE(FEffectRelation, SelfIsNPC)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_PARTY;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEntityNPC* pSelfNPC = GUTHelper_NPC::SpawnNPC(m_pField);

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = pSelfNPC;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

				CHECK_EQUAL(1, vecEffectTarget.size());
				CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, pSelfNPC));
			}

			TEST_FIXTURE(FEffectRelation, SelfIsPlayer_NotHaveParty)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_PARTY;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

				CHECK_EQUAL(1, vecEffectTarget.size());
				CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborPlayer));
			}

			TEST_FIXTURE(FEffectRelation, SelfIsPlayer_HaveParty)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_PARTY;
				m_EffectInfo.m_nLimit = INT_MAX;

				test.party.CreateParty(m_pSelf, m_pSelfNeighborPlayer);

				m_pSelf->SetDuel(NULL);
				m_pTarget->SetDuel(NULL);

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

				CHECK_EQUAL(2, vecEffectTarget.size());
				CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborPlayer));
			}
		}

		namespace AlliedDead
		{
			TEST_FIXTURE(FEffectRelation, AllianceIsAlive)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALLIED_DEAD;
				m_EffectInfo.m_nLimit = INT_MAX;

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

				CHECK_EQUAL(0, vecEffectTarget.size());
				CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(false,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborPlayer));
			}

			TEST_FIXTURE(FEffectRelation, AllianceIsDead)
			{
				m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				m_EffectInfo.m_fRadius = 100.0f;
				m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALLIED_DEAD;
				m_EffectInfo.m_nLimit = INT_MAX;

				m_pSelf->doDie();
				m_pSelfNeighborPlayer->doDie();

				GEffectTargetSelector::Desc desc(m_EffectInfo);
				desc.pSelf = m_pSelf;
				desc.pTarget = m_pTarget;
				vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

				CHECK_EQUAL(2, vecEffectTarget.size());
				CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelf));
				CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighborPlayer));
			}
		}
	}

	namespace EffectLimit
	{
		struct FEffectLimit : public FEffectTargetSelector
		{
			FEffectLimit()
			{
				m_pSelfNeighbor1 = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(1.0f, 1.0f, 0.0f));
				m_pSelfNeighbor2 = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(2.0f, 2.0f, 0.0f));
				m_pSelfNeighbor3 = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(3.0f, 3.0f, 0.0f));
				m_pSelfNeighbor4 = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(4.0f, 4.0f, 0.0f));
				m_pSelfNeighbor5 = NewEntityPlayer(m_pField, m_pSelf->GetPos()+vec3(5.0f, 5.0f, 0.0f));				
			}

			virtual ~FEffectLimit()
			{

			}

			GEntityPlayer*	m_pSelfNeighbor1;	// 자신과 가까이 있는자1
			GEntityPlayer*	m_pSelfNeighbor2;	// 자신과 가까이 있는자2
			GEntityPlayer*	m_pSelfNeighbor3;	// 자신과 가까이 있는자3
			GEntityPlayer*	m_pSelfNeighbor4;	// 자신과 가까이 있는자4
			GEntityPlayer*	m_pSelfNeighbor5;	// 자신과 가까이 있는자5
		};

		TEST_FIXTURE(FEffectLimit, LimitOver)
		{
			m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
			m_EffectInfo.m_fRadius = 100.0f;
			m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
			m_EffectInfo.m_nLimit = 3;

			GEffectTargetSelector::Desc desc(m_EffectInfo);
			desc.pSelf = m_pSelf;
			desc.pTarget = m_pTarget;
			vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

			CHECK_EQUAL(3, vecEffectTarget.size());
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelf));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighbor1));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighbor2));
		}

		TEST_FIXTURE(FEffectLimit, LimitNotOver)
		{
			m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
			m_EffectInfo.m_fRadius = 100.0f;
			m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
			m_EffectInfo.m_nLimit = INT_MAX;

			GEffectTargetSelector::Desc desc(m_EffectInfo);
			desc.pSelf = m_pSelf;
			desc.pTarget = m_pTarget;
			vector<GEntityActor*> vecEffectTarget =	m_EffectTargetSelector.Select(desc);

			CHECK_EQUAL(6, vecEffectTarget.size());
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelf));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighbor1));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighbor2));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighbor3));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighbor4));
			CHECK_EQUAL(true,	IsEffectTarget(vecEffectTarget, m_pSelfNeighbor5));
		}
	}
}