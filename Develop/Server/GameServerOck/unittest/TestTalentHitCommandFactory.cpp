#include "stdafx.h"
#include "GTalentHitCommandFactory.h"
#include "FBaseGame.h"

SUITE(TalentHitCommandFactory)
{
	class TestTalentHitCommandFactory : public GTalentHitCommandFactory
	{
	public:
		using GTalentHitCommandFactory::CheckNoMotionFactor;
		using GTalentHitCommandFactory::IsKnockBackCommand;
	};

	class FTalentHitCommandFactory : public FBaseGame
	{
	public:
		FTalentHitCommandFactory()
		{
			NewNPCInfo(1);
			m_pTalentInfo = NewTalentInfo(10);

			m_pAttacker = NewEntityPlayer(m_pField);
			m_pVictim = NewNPC(m_pField, 1);
		}
		virtual ~FTalentHitCommandFactory()
		{

		}

		GEntityPlayer*	m_pAttacker;
		GEntityNPC*		m_pVictim;
		GTalentInfo*	m_pTalentInfo;

		TestTalentHitCommandFactory	m_TalentHitCommandFactory;
	};

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_CheckNoMotionFactor)
	{
		// damage, MFWeight, Flags
		CHECK_EQUAL(true, m_TalentHitCommandFactory.CheckNoMotionFactor(0, 0, 0));

		uint32 nFlags = 0;	SetBitSet(nFlags, CTR_ZERO_TALENT_DAMAGE);
		CHECK_EQUAL(true, m_TalentHitCommandFactory.CheckNoMotionFactor(0, 0, nFlags));
	}

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_CheckNoMotionFactor_Negative)
	{
		// damage, MFWeight, Flags
		CHECK_EQUAL(false, m_TalentHitCommandFactory.CheckNoMotionFactor(100, 0, 0));

		CHECK_EQUAL(false, m_TalentHitCommandFactory.CheckNoMotionFactor(0, 1000, 0));

		CHECK_EQUAL(false, m_TalentHitCommandFactory.CheckNoMotionFactor(50, 1, 0));

		uint32 nFlags = 0;	SetBitSet(nFlags, CTR_RESIST_PERFECT);
		CHECK_EQUAL(false, m_TalentHitCommandFactory.CheckNoMotionFactor(0, 1000, nFlags));
	}

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_IsKnockBackCommand)
	{
		CHECK_EQUAL(true, m_TalentHitCommandFactory.IsKnockBackCommand(MF_KNOCKBACK));
		CHECK_EQUAL(true, m_TalentHitCommandFactory.IsKnockBackCommand(MF_THROWUP));
		CHECK_EQUAL(true, m_TalentHitCommandFactory.IsKnockBackCommand(MF_UPPERED));
	}

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_IsKnockBackCommand_Negative)
	{
		CHECK_EQUAL(false, m_TalentHitCommandFactory.IsKnockBackCommand(MF_NONE));
		CHECK_EQUAL(false, m_TalentHitCommandFactory.IsKnockBackCommand(MF_BEATEN));
		CHECK_EQUAL(false, m_TalentHitCommandFactory.IsKnockBackCommand(MF_STUN));
		CHECK_EQUAL(false, m_TalentHitCommandFactory.IsKnockBackCommand(MF_DEFLECT));		
		CHECK_EQUAL(false, m_TalentHitCommandFactory.IsKnockBackCommand(MF_KNOCKDOWN));
	}

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_MakeHit_MC_ACTION_TALENT_HIT)
	{
		unsigned short nDamage = 100;
		MF_STATE nMFState = MF_BEATEN;
		int nMFWeight = 1500;
		uint16 nActHitTime = 50;
		uint32 nFlags = 0;

		MCommand* pCommand = m_TalentHitCommandFactory.MakeHit(m_pAttacker, m_pVictim, m_pTalentInfo, nDamage, nMFState, nMFWeight, nActHitTime, nFlags);

		ASSERT_CHECK(pCommand != NULL);
		ASSERT_EQUAL(MC_ACTION_TALENT_HIT, pCommand->GetID());

		MCommandParameter* pParam = pCommand->GetParameter(0);
		TD_TALENT_HIT* td_talent_hit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

		CHECK_EQUAL(m_pAttacker->GetUIID(), td_talent_hit->nAttackerUIID);
		CHECK_EQUAL(m_pVictim->GetUIID(), td_talent_hit->nVictimUIID);

		CHECK_EQUAL(m_pTalentInfo->m_nID,	td_talent_hit->nTalentID);
		CHECK_EQUAL(nDamage,				td_talent_hit->nDamage);
		CHECK_EQUAL(nMFWeight,				td_talent_hit->nMFWeight);
		CHECK_EQUAL(nMFState,				td_talent_hit->nMFState);
		CHECK_EQUAL(nActHitTime,			td_talent_hit->nHitTime);
		CHECK_EQUAL(nFlags,					td_talent_hit->nFlags);

		delete pCommand;
	}

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_MakeHit_MC_ACTION_TALENT_HIT_NO_MF)
	{
		unsigned short nDamage = 0;
		MF_STATE nMFState = MF_BEATEN;
		int nMFWeight = 0;
		uint16 nActHitTime = 50;
		uint32 nFlags = 0;

		MCommand* pCommand = m_TalentHitCommandFactory.MakeHit(m_pAttacker, m_pVictim, m_pTalentInfo, nDamage, nMFState, nMFWeight, nActHitTime, nFlags);

		ASSERT_CHECK(pCommand != NULL);
		ASSERT_EQUAL(MC_ACTION_TALENT_HIT_NO_MF, pCommand->GetID());

		UIID nVictimUIID;
		UIID nAttackerUIID;
		int nTalentID;

		pCommand->GetParameter(&nVictimUIID,	0, MPT_USHORT);
		pCommand->GetParameter(&nAttackerUIID,	1, MPT_USHORT);
		pCommand->GetParameter(&nTalentID,		2, MPT_INT);

		CHECK_EQUAL(m_pVictim->GetUIID(), nVictimUIID);
		CHECK_EQUAL(m_pAttacker->GetUIID(), nAttackerUIID);
		CHECK_EQUAL(m_pTalentInfo->m_nID, nTalentID);

		delete pCommand;
	}

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_MakeHit_MC_ACTION_TALENT_HIT_KNOCKBACK_MF)
	{
		unsigned short nDamage = 100;
		MF_STATE nMFState = MF_KNOCKBACK;
		int nMFWeight = 1000;
		uint16 nActHitTime = 0;
		uint32 nFlags = 0;

		m_pAttacker->SetPos(vec3(0.0f, 0.0f, 0.0f));
		m_pAttacker->SetDir(vec3(0.0f, 1.0f, 0.0f));
		m_pVictim->SetPos(vec3(0.0f, 100.0f, 0.0f));
		m_pVictim->SetDir(vec3(0.0f, -1.0f, 0.0f));

		MCommand* pCommand = m_TalentHitCommandFactory.MakeHit(m_pAttacker, m_pVictim, m_pTalentInfo, nDamage, nMFState, nMFWeight, nActHitTime, nFlags);

		ASSERT_CHECK(pCommand != NULL);
		ASSERT_EQUAL(MC_ACTION_TALENT_HIT_KNOCKBACK_MF, pCommand->GetID());

		MCommandParameter* pParam = pCommand->GetParameter(0);
		TD_TALENT_HIT* td_talent_hit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());
		MCommandParameter* pParam1 = pCommand->GetParameter(1);
		TD_TALENT_HIT_EX_KNOCKBACK* td_ex_kb = static_cast<TD_TALENT_HIT_EX_KNOCKBACK*>(pParam1->GetPointer());

		CHECK_EQUAL(m_pAttacker->GetUIID(), td_talent_hit->nAttackerUIID);
		CHECK_EQUAL(m_pVictim->GetUIID(),	td_talent_hit->nVictimUIID);
		CHECK_EQUAL(m_pTalentInfo->m_nID,	td_talent_hit->nTalentID);
		CHECK_EQUAL(nDamage,				td_talent_hit->nDamage);
		CHECK_EQUAL(nMFWeight,				td_talent_hit->nMFWeight);
		CHECK_EQUAL(nMFState,				td_talent_hit->nMFState);
		CHECK_EQUAL(nActHitTime,			td_talent_hit->nHitTime);
		CHECK_EQUAL(nFlags,					td_talent_hit->nFlags);

		// y방향으로 날라감
		CHECK_CLOSE(0.0f, td_ex_kb->vKnockbackDir.X(), 0.001f);
		CHECK_CLOSE(1.0f, td_ex_kb->vKnockbackDir.Y(), 0.001f);

		CHECK_CLOSE(0.0f, td_ex_kb->vKnockbackVictimPos.x, 0.001f);
		CHECK_CLOSE(100.0f, td_ex_kb->vKnockbackVictimPos.y, 0.001f);
		CHECK_CLOSE(0.0f, td_ex_kb->vKnockbackVictimPos.z, 0.001f);

		delete pCommand;
	}


	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_MakeImmume)
	{
		MCommand* pCommand = m_TalentHitCommandFactory.MakeImmume(m_pAttacker, m_pVictim, m_pTalentInfo);

		ASSERT_CHECK(pCommand != NULL);
		ASSERT_EQUAL(MC_ACTION_TALENT_HIT, pCommand->GetID());

		MCommandParameter* pParam = pCommand->GetParameter(0);
		TD_TALENT_HIT* td_talent_hit = static_cast<TD_TALENT_HIT*>(pParam->GetPointer());

		CHECK_EQUAL(m_pAttacker->GetUIID(), td_talent_hit->nAttackerUIID);
		CHECK_EQUAL(m_pVictim->GetUIID(), td_talent_hit->nVictimUIID);

		CHECK_EQUAL(m_pTalentInfo->m_nID,	td_talent_hit->nTalentID);
		CHECK_EQUAL(0,						td_talent_hit->nDamage);
		CHECK_EQUAL(0,						td_talent_hit->nMFWeight);
		CHECK_EQUAL(MF_NONE,				td_talent_hit->nMFState);
		CHECK_EQUAL(0,						td_talent_hit->nHitTime);
		CHECK(CheckBitSet(td_talent_hit->nFlags, CTR_IMMUNE) > 0);

		delete pCommand;
	}

	TEST_FIXTURE(FTalentHitCommandFactory, TestTalentHitCommandFactory_MakeMiss)
	{
		MCommand* pCommand = m_TalentHitCommandFactory.MakeMiss(m_pAttacker, m_pVictim, m_pTalentInfo);

		ASSERT_CHECK(pCommand != NULL);
		ASSERT_EQUAL(MC_ACTION_TALENT_HIT_MISS, pCommand->GetID());

		UIID nVictimUIID;
		UIID nAttackerUIID;
		int nTalentID;

		pCommand->GetParameter(&nVictimUIID,	0, MPT_USHORT);
		pCommand->GetParameter(&nAttackerUIID,	1, MPT_USHORT);
		pCommand->GetParameter(&nTalentID,		2, MPT_INT);

		CHECK_EQUAL(m_pVictim->GetUIID(), nVictimUIID);
		CHECK_EQUAL(m_pAttacker->GetUIID(), nAttackerUIID);
		CHECK_EQUAL(m_pTalentInfo->m_nID, nTalentID);

		delete pCommand;
	}

}