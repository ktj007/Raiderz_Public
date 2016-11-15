#include "stdafx.h"
#include "GColtChecker.h"
#include "GColtPicker.h"
#include "GColtInfo.h"
#include "GEntityNPC.h"
#include "GNPCInfo.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GModuleAI.h"
#include "GTestForward.h"
#include "GNPCStress.h"
#include "GHateTable.h"
#include "GNPCHitcapsuleGroup.h"
#include "GNPCRage.h"
#include "GNPCBParts.h"

SUITE(TestColtChecker)
{
	struct FColtChecker
	{
		FColtChecker()
			: m_ColtBase(GColtPicker::SECTION_SINGLE, COLT_COMBAT, NULL, NULL)
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pNPC = m_Helper.NewEntityNPC(m_pField);

			m_pEnemy = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC->GetHateTable().AddPoint_GainDamage(m_pEnemy, LONG_MAX-1, 1.0f);
		}

		virtual ~FColtChecker() 
		{
			m_pNPC->Destroy();
			SAFE_DELETE(m_pNPC);
		}

		MockField* m_pField;		
		GEntityNPC* m_pNPC;
		GEntityPlayer* m_pEnemy;

		GColtChecker m_ColtConditioner;
		GColtPicker  m_ColtBase;
		GColtGroupInfo m_ColtInfo;
		GUTHelper m_Helper;
		DECLWRAPPER_SimpleCombatCalc;
		
	};

	namespace CheckCondition
	{
		namespace HP
		{
			// 최소 보다 작은 경우
			TEST_FIXTURE(FColtChecker, SmallerThanMin)
			{
				m_pNPC->GetNPCInfo()->nMaxHP = 1000;
				m_pNPC->SetHP(100);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_HP, 20, 101);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대 보다 큰 경우
			TEST_FIXTURE(FColtChecker, BiggerThanMax)
			{
				m_pNPC->GetNPCInfo()->nMaxHP = 1000;
				m_pNPC->SetHP(900);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_HP, 0, 80);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMin)
			{
				m_pNPC->GetNPCInfo()->nMaxHP = 1000;
				m_pNPC->SetHP(0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_HP, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMax)
			{
				m_pNPC->GetNPCInfo()->nMaxHP = 1000;
				m_pNPC->SetHP(1000);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_HP, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 최대 사이인 경우
			TEST_FIXTURE(FColtChecker, Between)
			{
				m_pNPC->GetNPCInfo()->nMaxHP = 1000;
				m_pNPC->SetHP(500);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_HP, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace EnemyHP
		{
			// 최소 보다 작은 경우
			TEST_FIXTURE(FColtChecker, SmallerThanMin)
			{
				m_pEnemy->GetPlayerInfo()->nMaxHP = 1000;
				m_pEnemy->SetHP(100);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_HP, 20, 101);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대 보다 큰 경우
			TEST_FIXTURE(FColtChecker, BiggerThanMax)
			{
				m_pEnemy->GetPlayerInfo()->nMaxHP = 1000;
				m_pEnemy->SetHP(900);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_HP, 0, 80);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMin)
			{
				m_pEnemy->GetPlayerInfo()->nMaxHP = 1000;
				m_pEnemy->SetHP(0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_HP, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMax)
			{
				m_pEnemy->GetPlayerInfo()->nMaxHP = 1000;
				m_pEnemy->SetHP(1000);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_HP, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 최대 사이인 경우
			TEST_FIXTURE(FColtChecker, Between)
			{
				m_pEnemy->GetPlayerInfo()->nMaxHP = 1000;
				m_pEnemy->SetHP(500);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_HP, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace UnbreakPart
		{
			// 파괴된 경우
			TEST_FIXTURE(FColtChecker, Breaked)
			{
				m_pNPC->GetNPCBParts()->SetBreak(1);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_UNBREAKPART, 1);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 파괴되지 않은 경우
			TEST_FIXTURE(FColtChecker, NotBreaked)
			{
				m_pNPC->GetNPCBParts()->SetBreak(2);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_UNBREAKPART, 1);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace BreakPart
		{
			// 파괴된 경우
			TEST_FIXTURE(FColtChecker, Breaked)
			{
				m_pNPC->GetNPCBParts()->SetBreak(1);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_BREAKPART, 1);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 파괴되지 않은 경우
			TEST_FIXTURE(FColtChecker, NotBreaked)
			{
				m_pNPC->GetNPCBParts()->SetBreak(2);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_BREAKPART, 1);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Damage
		{
			namespace Type
			{
				// 같은 경우
				TEST_FIXTURE(FColtChecker, Equal)
				{
					m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 0);
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DAMAGE, DA_SLASH, 0);

					CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 다른 경우
				TEST_FIXTURE(FColtChecker, Unequal)
				{
					m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 0);
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DAMAGE, DA_BLUNT, 0);

					CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}
			}

			namespace Damage
			{
				// 최소 보다 작은 경우
				TEST_FIXTURE(FColtChecker, SmallerThanMin)
				{
					m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 10);
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DAMAGE, DA_SLASH, 20, 101);

					CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 최대 보다 큰 경우
				TEST_FIXTURE(FColtChecker, BiggerThanMax)
				{
					m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 90);
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DAMAGE, DA_SLASH, 0, 80);

					CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 최소와 같은 경우
				TEST_FIXTURE(FColtChecker, EqualMin)
				{
					m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 0);
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DAMAGE, DA_SLASH, 0, 101);

					CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 최대와 같은 경우
				TEST_FIXTURE(FColtChecker, EqualMax)
				{
					m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 100);
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DAMAGE, DA_SLASH, 0, 100);

					CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 최소와 최대 사이인 경우
				TEST_FIXTURE(FColtChecker, Between)
				{
					m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 50);
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DAMAGE, DA_SLASH, 0, 100);

					CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}
			}
		}

		namespace MotionFactor
		{
			// 같은 경우
			TEST_FIXTURE(FColtChecker, Equal)
			{
				m_pNPC->SetCurrentMotionFactor(MF_BEATEN, 0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_MOTIONFACTOR, MF_BEATEN);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 다른 경우
			TEST_FIXTURE(FColtChecker, Unequal)
			{
				m_pNPC->SetCurrentMotionFactor(MF_BEATEN, 0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_MOTIONFACTOR, MF_STUN);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace EnemyMotionFactor
		{
			// 같은 경우
			TEST_FIXTURE(FColtChecker, Equal)
			{
				m_pEnemy->SetCurrentMotionFactor(MF_BEATEN, 0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_MOTIONFACTOR, MF_BEATEN);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 다른 경우
			TEST_FIXTURE(FColtChecker, Unequal)
			{
				m_pEnemy->SetCurrentMotionFactor(MF_BEATEN, 0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_MOTIONFACTOR, MF_STUN);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Mode
		{
			struct FMode : public FColtChecker
			{
				FMode()
				{
					m_pNPC->GetNPCInfo()->vecModes.push_back(NULL);
					m_pNPC->GetNPCInfo()->vecModes.push_back(NULL);
				}
			};

			// 같은 경우
			TEST_FIXTURE(FMode, Equal)
			{
				m_pNPC->ChangeMode(NPC_MODE_DEFAULT);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_MODE, 0);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 다른 경우
			TEST_FIXTURE(FMode, Unequal)
			{
				m_pNPC->ChangeMode(NPC_MODE_DEFAULT);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_MODE, 1);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Distance
		{
			// 최소 보다 작은 경우
			TEST_FIXTURE(FColtChecker, SmallerThanMin)
			{
				m_pEnemy->SetPos(vec3(0, 10, 0));
				m_pNPC->SetPos(vec3(0, 0, 0));
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DISTANCE, 20, 101);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대 보다 큰 경우
			TEST_FIXTURE(FColtChecker, BiggerThanMax)
			{
				m_pEnemy->SetPos(vec3(0, 90, 0));
				m_pNPC->SetPos(vec3(0, 0, 0));
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DISTANCE, 0, 80);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMin)
			{
				m_pEnemy->SetPos(vec3(0, 0, 0));
				m_pNPC->SetPos(vec3(0, 0, 0));
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DISTANCE, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMax)
			{
				m_pEnemy->SetPos(vec3(0, 100, 0));
				m_pNPC->SetPos(vec3(0, 0, 0));
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DISTANCE, 0, 101);
				
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 최대 사이인 경우
			TEST_FIXTURE(FColtChecker, Between)
			{
				m_pEnemy->SetPos(vec3(0, 50, 0));
				m_pNPC->SetPos(vec3(0, 0, 0));
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DISTANCE, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Angle
		{
			namespace Left
			{
				// 각도가 작은 경우
				TEST_FIXTURE(FColtChecker, Smaller)
				{
					m_pEnemy->SetPos(vec3(50, 0, 0));
					m_pNPC->SetPos(vec3(0, 0, 0));
					m_pNPC->SetDir(vec3(0, 1, 0));
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ANGLE, -80);

					CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 각도가 같은 경우
				TEST_FIXTURE(FColtChecker, Equal)
				{
					m_pEnemy->SetPos(vec3(50, 0, 0));
					m_pNPC->SetPos(vec3(0, 0, 0));
					m_pNPC->SetDir(vec3(0, 1, 0));
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ANGLE, -90);

					CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 각도가 큰 경우
				TEST_FIXTURE(FColtChecker, Bigger)
				{
					m_pEnemy->SetPos(vec3(50, 0, 0));
					m_pNPC->SetPos(vec3(0, 0, 0));
					m_pNPC->SetDir(vec3(0, 1, 0));
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ANGLE, -100);

					CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}
			}

			namespace Right
			{
				// 각도가 작은 경우
				TEST_FIXTURE(FColtChecker, Smaller)
				{
					m_pNPC->SetPos(vec3(50, 0, 0));
					m_pEnemy->SetPos(vec3(0, 0, 0));
					m_pEnemy->SetDir(vec3(0, 1, 0));
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ANGLE, 80);

					CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 각도가 같은 경우
				TEST_FIXTURE(FColtChecker, Equal)
				{
					m_pNPC->SetPos(vec3(50, 0, 0));
					m_pEnemy->SetPos(vec3(0, 0, 0));
					m_pEnemy->SetDir(vec3(0, 1, 0));
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ANGLE, 90);

					CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}

				// 각도가 큰 경우
				TEST_FIXTURE(FColtChecker, Bigger)
				{
					m_pNPC->SetPos(vec3(50, 0, 0));
					m_pEnemy->SetPos(vec3(0, 0, 0));
					m_pEnemy->SetDir(vec3(0, 1, 0));
					GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ANGLE, 101);

					CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				}
			}
		}

		namespace Stress
		{
			// 최소 보다 작은 경우
			TEST_FIXTURE(FColtChecker, SmallerThanMin)
			{				
				m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 50);
				m_pNPC->GetNPCStress().SetStress(10);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_STRESS, 20, 101);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				CHECK_EQUAL(10, m_pNPC->GetNPCStress().GetStress());
			}

			// 최대 보다 큰 경우
			TEST_FIXTURE(FColtChecker, BiggerThanMax)
			{
				m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 50);
				m_pNPC->GetNPCStress().SetStress(90);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_STRESS, 0, 80);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				CHECK_EQUAL(90, m_pNPC->GetNPCStress().GetStress());
			}

			// 최소와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMin)
			{
				m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 50);
				m_pNPC->GetNPCStress().SetStress(0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_STRESS, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				CHECK_EQUAL(0, m_pNPC->GetNPCStress().GetStress());
			}

			// 최대와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMax)
			{
				m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 50);
				m_pNPC->GetNPCStress().SetStress(100);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_STRESS, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				CHECK_EQUAL(0, m_pNPC->GetNPCStress().GetStress());
			}

			// 최소와 최대 사이인 경우
			TEST_FIXTURE(FColtChecker, Between)
			{
				m_pNPC->doDamage(m_pEnemy->GetUID(), DA_SLASH, 50);
				m_pNPC->GetNPCStress().SetStress(50);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_STRESS, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
				CHECK_EQUAL(0, m_pNPC->GetNPCStress().GetStress());
			}
		}

		namespace HitCapsule
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				m_pNPC->GetNPCInfo()->m_pMeshInfo = new GActorMeshInfo();
				m_pNPC->GetMeshInfo()->ReserveHitGroupForTest(2);
				m_pNPC->GetNPCHitcapsuleGroup().ChangeCapsuleGroup_ByTrigger(1);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_HIT_CAPSULE, 1);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));

				SAFE_DELETE(m_pNPC->GetNPCInfo()->m_pMeshInfo);
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				m_pNPC->GetNPCInfo()->m_pMeshInfo = new GActorMeshInfo();
				m_pNPC->GetMeshInfo()->ReserveHitGroupForTest(2);
				m_pNPC->GetNPCHitcapsuleGroup().ChangeCapsuleGroup_ByTrigger(1);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_HIT_CAPSULE, 2);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));

				SAFE_DELETE(m_pNPC->GetNPCInfo()->m_pMeshInfo);
			}
		}

		namespace UsedTalent
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				GTalentInfo* pTalentInfo = m_Helper.NewMeleeTalentInfo();
				pTalentInfo->m_fCastingTime = 10.0f;
				pTalentInfo->Cooking();

				m_pNPC->doUseTalent(pTalentInfo, TALENT_TARGET_INFO());
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_USED_TALENT, pTalentInfo->m_nID);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				GTalentInfo* pTalentInfo = m_Helper.NewMeleeTalentInfo();
				pTalentInfo->m_fCastingTime = 10.0f;
				pTalentInfo->Cooking();

				m_pNPC->doUseTalent(pTalentInfo, TALENT_TARGET_INFO());
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_USED_TALENT, pTalentInfo->m_nID+1);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace GainedBuff
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				int nBuffID = 123;
				CHECK_EQUAL(false, m_pNPC->HasBuff(nBuffID));
				GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
				test.buff.GainBuffSelf(m_pNPC, pBuff);
				CHECK_EQUAL(true, m_pNPC->HasBuff(nBuffID));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_GAINED_BUFF, nBuffID);
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				int nBuffID = 123;
				CHECK_EQUAL(false, m_pNPC->HasBuff(nBuffID));
				GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
				test.buff.GainBuffSelf(m_pNPC, pBuff);
				CHECK_EQUAL(true, m_pNPC->HasBuff(nBuffID));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_GAINED_BUFF, nBuffID+1);
				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace EnemyGainedBuff
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				int nBuffID = 123;
				CHECK_EQUAL(false, m_pEnemy->HasBuff(nBuffID));
				GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
				test.buff.GainBuffSelf(m_pEnemy, pBuff);
				CHECK_EQUAL(true, m_pEnemy->HasBuff(nBuffID));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_GAINED_BUFF, nBuffID);
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				int nBuffID = 123;
				CHECK_EQUAL(false, m_pEnemy->HasBuff(nBuffID));
				GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
				test.buff.GainBuffSelf(m_pEnemy, pBuff);
				CHECK_EQUAL(true, m_pEnemy->HasBuff(nBuffID));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_ENEMY_GAINED_BUFF, nBuffID+1);
				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Rear
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				m_pEnemy->SetDir(vec3(0,1,0));
				m_pEnemy->SetPos(vec3(0,100,0));
				m_pNPC->SetPos(vec3(0,0,0));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_REAR);
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				m_pEnemy->SetDir(vec3(0,1,0));
				m_pEnemy->SetPos(vec3(0,100,0));
				m_pNPC->SetPos(vec3(0,200,0));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_REAR);
				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Front
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				m_pEnemy->SetDir(vec3(0,1,0));
				m_pEnemy->SetPos(vec3(0,100,0));
				m_pNPC->SetPos(vec3(0,200,0));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_FRONT);
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				m_pEnemy->SetDir(vec3(0,1,0));
				m_pEnemy->SetPos(vec3(0,100,0));
				m_pNPC->SetPos(vec3(0,0,0));

				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_FRONT);
				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Rage
		{
			// 최소 보다 작은 경우
			TEST_FIXTURE(FColtChecker, SmallerThanMin)
			{
				m_pNPC->GetNPCRage().AddRage(10);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_RAGE, 20, 101);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대 보다 큰 경우
			TEST_FIXTURE(FColtChecker, BiggerThanMax)
			{
				m_pNPC->GetNPCRage().AddRage(90);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_RAGE, 0, 80);

				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMin)
			{
				m_pNPC->GetNPCRage().AddRage(0);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_RAGE, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최대와 같은 경우
			TEST_FIXTURE(FColtChecker, EqualMax)
			{
				m_pNPC->GetNPCRage().AddRage(100);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_RAGE, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			// 최소와 최대 사이인 경우
			TEST_FIXTURE(FColtChecker, Between)
			{
				m_pNPC->GetNPCRage().AddRage(50);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_RAGE, 0, 101);

				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Dead
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				m_pNPC->doDie();
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DEAD);
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_DEAD);
				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Peace
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				test.npc.MakeCombat(m_pNPC);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_PEACE);
				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_PEACE);
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}

		namespace Combat
		{
			TEST_FIXTURE(FColtChecker, Equal)
			{
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_COMBAT);
				CHECK_EQUAL(false, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}

			TEST_FIXTURE(FColtChecker, Unequal)
			{
				test.npc.MakeCombat(m_pNPC);
				GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(m_pNPC, AST_COMBAT);
				CHECK_EQUAL(true, m_ColtConditioner.CheckCondition(*pCheck, m_pNPC));
			}
		}
	}	
}

