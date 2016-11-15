#include "stdafx.h"
#include "GTestForward.h"
#include "GConditionInfo.h"
#include "GConditionsSystem.h"
#include "GConditionChecker.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GItemManager.h"
#include "GPlayerTalent.h"
#include "GDialogInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "FBaseMockLink.h"
#include "GFactionInfo.h"
#include "GFactionInfoMgr.h"
#include "GPlayerFactions.h"
#include "GPlayerObjectManager.h"
#include "GQuestSystem.h"
#include "GItemHolder.h"
#include "GPlayerEmblem.h"
#include "FBaseNPC.h"
#include "GHateTable.h"
#include "GQuestCanceler.h"
#include "GQuestFailer.h"

SUITE(Conditions)
{
	struct Fixture : public FBaseMockLink
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pItemHodler = m_pPlayer->GetItemHolder();

			m_pConditionLeaf = new GCondition_Leaf();
			
		}

		~Fixture()
		{
			SAFE_DELETE(m_pConditionLeaf);
			m_pField->Destroy();			
		}


		GTestMgrWrapper<GQuestInfoMgr>		m_QuestInfoMgr;
		GTestMgrWrapper<GDialogInfoMgr>		m_DialogInfoMgr;
		GTestMgrWrapper<GNPCInfoMgr>		m_NPCInfoMgr;
		GTestMgrWrapper<GConditionsInfoMgr> m_ConditionInfoMgr;
		GTestMgrWrapper<GItemManager>		m_ItemManager;
		GTestMgrWrapper<GTalentInfoMgr>		m_TalentInfoManager;
		GTestMgrWrapper<GFactionInfoMgr>	m_FactionInfoManager;
		GUTHelper							m_Helper;

		GEntityPlayer*		m_pPlayer;
		GItemHolder*		m_pItemHodler;
		MockField*			m_pField;
		GCondition_Leaf*	m_pConditionLeaf;

		GConditionChecker	m_ConditionChecker;
	};

	namespace Level
	{
		struct FLevel : Fixture
		{
			FLevel()
			{
				m_pConditionLeaf->m_nCT = CT_LEVEL;
				m_pConditionLeaf->m_vecParam1.push_back(1);
				m_pConditionLeaf->m_vecParam2.push_back(100);
			}
		};

		namespace False
		{
			TEST_FIXTURE(FLevel, LevelIsLow)
			{
				m_pPlayer->GetPlayerInfo()->nLevel = m_pConditionLeaf->m_vecParam1[0] - 1;
				CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}

			TEST_FIXTURE(FLevel, LevelIsHigh)
			{
				m_pPlayer->GetPlayerInfo()->nLevel = m_pConditionLeaf->m_vecParam2[0] + 1;
				CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}
		}

		TEST_FIXTURE(FLevel, True)
		{
			m_pPlayer->GetPlayerInfo()->nLevel = m_pConditionLeaf->m_vecParam1[0];
			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}			
	}

	namespace Quest
	{
		struct FQuest : Fixture
		{
			FQuest()
			{
				m_pQuestInfo = new GQuestInfo();
				m_pQuestInfo->nID = 1;
				gmgr.pQuestInfoMgr->Insert(m_pQuestInfo);

				m_pConditionLeaf->m_nCT = CT_QUEST;
				m_pConditionLeaf->m_vecParam1.push_back(m_pQuestInfo->nID);
			}

			GQuestInfo* m_pQuestInfo;				
		};

		namespace NoExist
		{
			struct FNoExist : FQuest
			{
				FNoExist()
				{
					m_pConditionLeaf->m_vecParam2.push_back(CQS_NOTEXIST);
				}
			};

			TEST_FIXTURE(FNoExist, False)
			{
				m_pPlayer->GetQuests().Insert(0, m_pConditionLeaf->m_vecParam1[0], L"", L"");
				CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}

			TEST_FIXTURE(FNoExist, True)
			{
				CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}
		}

		namespace Exist
		{
			struct FExist : FQuest
			{
				FExist()
				{
					m_pConditionLeaf->m_vecParam2.push_back(CQS_EXIST);
				}
			};

			TEST_FIXTURE(FExist, False)
			{					
				CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}

			TEST_FIXTURE(FExist, True)
			{
				m_pPlayer->GetQuests().Insert(0, m_pConditionLeaf->m_vecParam1[0], L"", L"");
				CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}
		}

		namespace Done
		{
			struct FDone : FQuest
			{
				FDone()
				{
					m_pConditionLeaf->m_vecParam2.push_back(CQS_DONE);

					m_pPlayer->GetQuests().Insert(0, m_pConditionLeaf->m_vecParam1[0], L"", L"");
				}
			};

			TEST_FIXTURE(FDone, False)
			{					
				CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}

			TEST_FIXTURE(FDone, True)
			{
				test.quest.DoneQuest(m_pPlayer, m_pConditionLeaf->m_vecParam1[0]);
				CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}
		}

		TEST(ShareActiveQuestInParty)
		{
			GTestMgrWrapper<GQuestInfoMgr>	 QuestInfoMgr;
			GConditionChecker m_ConditionChecker;

			MockPlayerObject* pPlayerObject = new MockPlayerObject(SUnitTestUtil::NewUID());
			pPlayerObject->Create();
			gmgr.pPlayerObjectManager->AddPlayer(pPlayerObject);

			MockField* pField = GUTHelper_Field::DefaultMockField();
			GEntityPlayer* pPlayer1 = pPlayerObject->GetEntity();
			pPlayer1->GetPlayerInfo()->nCID = SUnitTestUtil::NewCID();
			wcsncpy_s(pPlayer1->GetPlayerInfo()->szName, L"Player1", 64);
			pPlayer1->EnterToField(pField, vec3(0.0f, 0.0f, 0.0f));
			GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField);
			wcsncpy_s(pPlayer2->GetPlayerInfo()->szName, L"Player2", 64);

			// Party Share
			GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 0);
			GCondition_Leaf Condition;
			Condition.m_nCT = CT_QUEST;
			Condition.m_vecParam1.push_back(pPlayerQuest->GetInfo()->nID);
			Condition.m_vecParam2.push_back(CQS_ACTIVE);

			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), true);
			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), false);
			
			GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), true);
			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), true);

			GUTHelper_Party::DoOfflineMember(pParty, pPlayer1);

			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), true);
			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), false);

			GUTHelper_Party::DoOnlineMember(pParty, pPlayer1);

			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), true);
			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), true);

			GUTHelper_Party::LeaveMember(pParty, pPlayer1);

			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), true);
			CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), false);

			pField->Destroy();
		}

		TEST(SynchPartyMembersActiveQuestWhenChangedQuest)
		{
			GTestMgrWrapper<GQuestInfoMgr>	 QuestInfoMgr;
			GConditionChecker m_ConditionChecker;

			MockField* pField = GUTHelper_Field::DefaultMockField();
			GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField);
			GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField);
			
			GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

			// Sync Quest Changed-Status (Cancel)
			{
				GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 0);
				GCondition_Leaf Condition;
				Condition.m_nCT = CT_QUEST;
				Condition.m_vecParam1.push_back(pPlayerQuest->GetInfo()->nID);
				Condition.m_vecParam2.push_back(CQS_ACTIVE);

				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), true);
				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), true);

				gsys.pQuestSystem->GetCanceler().Cancel(pPlayer1, pPlayerQuest->GetInfo()->nID);	// Drop Real gave quest player

				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), false);
				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), false);
			}


			// Sync Quest Changed-Status (Fail)
			{
				GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 0);
				GCondition_Leaf Condition;
				Condition.m_nCT = CT_QUEST;
				Condition.m_vecParam1.push_back(pPlayerQuest->GetInfo()->nID);
				Condition.m_vecParam2.push_back(CQS_ACTIVE);

				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), true);
				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), true);

				gsys.pQuestSystem->GetFailer().Fail(pPlayer1, pPlayerQuest->GetInfo()->nID);

				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer1), false);
				CHECK_EQUAL(m_ConditionChecker.Check(&Condition, pPlayer2), false);
			}

			pField->Destroy();
		}
	}

	namespace QuestVar
	{
		struct FQuestVar : Fixture
		{
			FQuestVar()
			{
				m_pQuestInfo = new GQuestInfo();
				m_pQuestInfo->nID = 1;
				gmgr.pQuestInfoMgr->Insert(m_pQuestInfo);

				m_pConditionLeaf->m_nCT = CT_QUEST_VAR;
				m_pConditionLeaf->m_vecParam1.push_back(m_pQuestInfo->nID);
				m_pConditionLeaf->m_vecParam2.push_back(1);

				m_pPlayer->GetQuests().Insert(0, m_pConditionLeaf->m_vecParam1[0], L"", L"");
			}

			GQuestInfo* m_pQuestInfo;				
		};

		TEST_FIXTURE(FQuestVar, False)
		{
			GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pConditionLeaf->m_vecParam1[0]);
			pPlayerQuest->UpdateQuestVar(m_pConditionLeaf->m_vecParam2[0] + 1);
			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FQuestVar, True)
		{
			GPlayerQuest* pPlayerQuest = m_pPlayer->GetQuests().Get(m_pConditionLeaf->m_vecParam1[0]);
			pPlayerQuest->UpdateQuestVar(m_pConditionLeaf->m_vecParam2[0]);
			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}

	namespace Item
	{
		TEST_FIXTURE(Fixture, False)
		{
			GItemData* pItemData = test.item.NewItemData();
			GCondition_Leaf* pConditionLeaf = test.condition.NewCondL();
			pConditionLeaf->m_nCT = CT_ITEM;
			pConditionLeaf->m_vecParam1.push_back(pItemData->m_nID);

			CHECK_EQUAL(false, m_ConditionChecker.Check(pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(Fixture, True)
		{
			GItemData* pItemData = test.item.NewItemData();
			GCondition_Leaf* pConditionLeaf = test.condition.NewCondL();
			pConditionLeaf->m_nCT = CT_ITEM;
			pConditionLeaf->m_vecParam1.push_back(pItemData->m_nID);

			test.item.GiveNewItem(m_pPlayer, pItemData->m_nID);

			CHECK_EQUAL(true, m_ConditionChecker.Check(pConditionLeaf, m_pPlayer));
		}
	}

	namespace NoItem
	{		
		TEST_FIXTURE(Fixture, False)
		{
			GItemData* pItemData = test.item.NewItemData();
			GCondition_Leaf* pConditionLeaf = test.condition.NewCondL();
			pConditionLeaf->m_nCT = CT_NOITEM;
			pConditionLeaf->m_vecParam1.push_back(pItemData->m_nID);

			test.item.GiveNewItem(m_pPlayer, pItemData->m_nID);

			CHECK_EQUAL(false, m_ConditionChecker.Check(pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(Fixture, True)
		{
			GItemData* pItemData = test.item.NewItemData();
			GCondition_Leaf* pConditionLeaf = test.condition.NewCondL();
			pConditionLeaf->m_nCT = CT_NOITEM;
			pConditionLeaf->m_vecParam1.push_back(pItemData->m_nID);

			CHECK_EQUAL(true, m_ConditionChecker.Check(pConditionLeaf, m_pPlayer));
		}
	}


	namespace Equip
	{
		TEST_FIXTURE(Fixture, False)
		{
			GItemData* pItemData = test.item.NewItemData();
			GCondition_Leaf* pConditionLeaf = test.condition.NewCondL();
			pConditionLeaf->m_nCT = CT_EQUIP;
			pConditionLeaf->m_vecParam1.push_back(pItemData->m_nID);

			test.item.GiveNewItem(m_pPlayer, pItemData->m_nID);

			CHECK_EQUAL(false, m_ConditionChecker.Check(pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(Fixture, True)
		{
			GItemData* pItemData = test.item.NewItemData();
			GCondition_Leaf* pConditionLeaf = test.condition.NewCondL();
			pConditionLeaf->m_nCT = CT_EQUIP;
			pConditionLeaf->m_vecParam1.push_back(pItemData->m_nID);

			test.item.EquipNewItem(m_pPlayer, pItemData->m_nID);

			CHECK_EQUAL(true, m_ConditionChecker.Check(pConditionLeaf, m_pPlayer));
		}
	}

	namespace Race
	{
		struct FRace : Fixture
		{
			FRace()
			{
				m_pConditionLeaf->m_nCT = CT_RACE;
				m_pConditionLeaf->m_vecParam1.push_back(RACE_HUMAN);
			}
		};

		TEST_FIXTURE(FRace, Fail)
		{
			m_pPlayer->GetPlayerInfo()->nRace = RACE_NEBIT;
			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FRace, True)
		{
			m_pPlayer->GetPlayerInfo()->nRace = RACE_HUMAN;
			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}


	namespace Sex
	{
		struct FSex : Fixture
		{
			FSex()
			{
				m_pConditionLeaf->m_nCT = CT_SEX;
				m_pConditionLeaf->m_vecParam1.push_back(SEX_MALE);
			}
		};

		TEST_FIXTURE(FSex, Fail)
		{
			m_pPlayer->GetPlayerInfo()->nSex = SEX_FEMALE;
			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FSex, True)
		{
			m_pPlayer->GetPlayerInfo()->nSex = SEX_MALE;
			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}

	namespace Date
	{
		struct FDate : Fixture
		{
			FDate()
			{			
				m_pConditionLeaf->m_nCT = CT_DATE;

				SYSTEMTIME stCurTime;
				GetLocalTime(&stCurTime);

				m_nCurDate = stCurTime.wYear * 10000 + stCurTime.wMonth * 100 + stCurTime.wDay;
			}

			int	m_nCurDate;				
		};

		namespace EndDate
		{
			TEST_FIXTURE(FDate, False)
			{
				m_pConditionLeaf->m_vecParam1.push_back(m_nCurDate + 1);

				CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}

			TEST_FIXTURE(FDate, True)
			{
				m_pConditionLeaf->m_vecParam1.push_back(m_nCurDate - 1);

				CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}
		}

		namespace StartDate
		{
			TEST_FIXTURE(FDate, False)
			{
				m_pConditionLeaf->m_vecParam2.push_back(m_nCurDate - 1);

				CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}

			TEST_FIXTURE(FDate, True)
			{
				m_pConditionLeaf->m_vecParam2.push_back(m_nCurDate + 1);

				CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
			}
		}
	}

	namespace Talent
	{
		struct FTalent : Fixture
		{
			FTalent()
			{
				m_pTalentInfo1 = new GTalentInfo();
				m_pTalentInfo1->m_nID = 1;
				gmgr.pTalentInfoMgr->Insert(m_pTalentInfo1);

				m_pTalentInfo2 = new GTalentInfo();
				m_pTalentInfo2->m_nID = 2;
				gmgr.pTalentInfoMgr->Insert(m_pTalentInfo2);

				m_pConditionLeaf->m_nCT = CT_TALENT;

				m_pPlayer->GetTalent().SetTP(INT_MAX);
				m_pConditionLeaf->m_vecParam1.push_back(m_pTalentInfo1->m_nID);
				m_pConditionLeaf->m_vecParam1.push_back(m_pTalentInfo2->m_nID);
			}

			GTalentInfo* m_pTalentInfo1;
			GTalentInfo* m_pTalentInfo2;
			GTalentInfo* m_pTalentInfo3;
		};

		TEST_FIXTURE(FTalent, Fail)
		{					
			m_pPlayer->GetTalent().Insert(m_pTalentInfo1);

			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FTalent, True)
		{
			m_pPlayer->GetTalent().Insert(m_pTalentInfo1);
			m_pPlayer->GetTalent().Insert(m_pTalentInfo2);

			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}

	namespace Talent_Style_TP
	{
		struct FTalent : Fixture
		{
			FTalent()
			{
				m_pTalentInfo1 = m_Helper.NewTalentInfo();
				m_pTalentInfo1->m_nStyle = TS_DEFENDER;
				m_pTalentInfo1->m_bNeedTraining = true;

				m_pTalentInfo2 = m_Helper.NewTalentInfo();
				m_pTalentInfo2->m_nStyle = TS_DEFENDER;
				m_pTalentInfo2->m_bNeedTraining = true;

				m_pConditionLeaf->m_nCT = CT_TALENT_STYLE_TP;

				m_pPlayer->GetTalent().SetTP(INT_MAX);
				m_pConditionLeaf->m_vecParam1.push_back(TS_DEFENDER);
				m_pConditionLeaf->m_vecParam2.push_back(2);
			}

			GTalentInfo* m_pTalentInfo1;
			GTalentInfo* m_pTalentInfo2;
		};

		TEST_FIXTURE(FTalent, Fail)
		{					
			m_pPlayer->GetTalent().Insert(m_pTalentInfo1);

			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FTalent, True)
		{
			m_pPlayer->GetTalent().Insert(m_pTalentInfo1);
			m_pPlayer->GetTalent().Insert(m_pTalentInfo2);

			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}
	
	namespace Faction
	{
		const int TEST_FACTION_ID = 100;
		const int TEST_FACTION_DEFAULT_QUANTITY = 20000;

		struct FFaction : Fixture
		{
			FFaction()
			{
				m_pConditionLeaf->m_nCT = CT_FACTION;

				pFactionInfo = new GFactionInfo(TEST_FACTION_ID, TEST_FACTION_DEFAULT_QUANTITY);
				gmgr.pFactionInfoMgr->Insert(pFactionInfo);

				m_pPlayer->GetPlayerFactions().Insert(pFactionInfo, pFactionInfo->m_nDefaultQuantity);
			}

			GFactionInfo* pFactionInfo;
		};

		TEST_FIXTURE(FFaction, Fail)
		{
			m_pConditionLeaf->m_vecParam1.push_back(TEST_FACTION_ID);
			m_pConditionLeaf->m_vecParam2.push_back(TEST_FACTION_DEFAULT_QUANTITY + 10000);

			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FFaction, True)
		{
			m_pConditionLeaf->m_vecParam1.push_back(TEST_FACTION_ID);
			m_pConditionLeaf->m_vecParam2.push_back(TEST_FACTION_DEFAULT_QUANTITY - 10000);

			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}

	namespace Kill
	{
		const int TEST_KILL_NPC = 777;

		struct FKill : Fixture, FBaseNPC
		{
			FKill()
			{
				m_pConditionLeaf->m_nCT = CT_KILL;
			}

			GFactionInfo* pFactionInfo;
		};

		TEST_FIXTURE(FKill, Kill_Fail)
		{
			m_pConditionLeaf->m_vecParam1.push_back(TEST_KILL_NPC-1);

			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FKill, Kill_True)
		{
			m_pConditionLeaf->m_vecParam1.push_back(TEST_KILL_NPC);

			GEntityNPC* pNPC = NewNPC(m_pField, TEST_KILL_NPC);
			pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			pNPC->doDie();

			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}

	namespace QPVP_TEAM
	{
		struct FQPVP_TEAM : Fixture
		{
			FQPVP_TEAM()
			{
				m_pConditionLeaf->m_nCT = CT_QPVP_TEAM;
				m_pConditionLeaf->m_vecParam1.push_back(1);
			}

			GFactionInfo* pFactionInfo;
		};

		TEST_FIXTURE(FQPVP_TEAM, NotBelongTeam)
		{
			test.player.SetMockPlayerPVPArea_Locate(m_pPlayer);

			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FQPVP_TEAM, NotEqualTeam)
		{
			test.player.SetMockPlayerPVPArea_LocateAndTeamTwo(m_pPlayer);

			CHECK_EQUAL(false, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}

		TEST_FIXTURE(FQPVP_TEAM, QPVP_TEAM)
		{
			test.player.SetMockPlayerPVPArea_LocateAndTeamOne(m_pPlayer);

			CHECK_EQUAL(true, m_ConditionChecker.Check(m_pConditionLeaf, m_pPlayer));
		}
	}
}