#include "stdafx.h"
#include "GColtPicker.h"
#include "GNPCInfo.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "GEntityNPC.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GColtRunner.h"
#include "GJobRunner.h"
#include "GFieldMgr.h"
#include "GUTHelper_Field.h"
#include "GJobMgr.h"
#include "GJob_Talent.h"
#include "GModuleAI.h"
#include "GHateTable.h"
#include "GTestForward.h"

class MockColt : public GColtPicker
{
public:
	MockColt(): GColtPicker(GColtPicker::SECTION_SINGLE, COLT_COMBAT, NULL, NULL) {}
	using GColtPicker::CalcActionRate;
	using GColtPicker::LookupActionInst;
	using GColtPicker::PickCheck;

	void SetChecks(vector<GColtCheck*>& vecChecks, GColtCheck* pDefaultCheck)
	{
		m_bMultiSection = false;
		m_vecChecks = &vecChecks;
		m_pDefaultCheck = pDefaultCheck;
	}

	void SetMultiChecks(vector<GColtCheck*>* vecMultiChecks, GColtCheck* pDefaultCheck)
	{
		m_bMultiSection = true;
		m_vecMultiChecks = vecMultiChecks;
		m_pMultiDefaultCheck = pDefaultCheck;
	}
};

SUITE(Colt)
{
	const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
	const vec3 vCommonDir = vec3(0,1,0);

	struct FColt
	{
		FColt()
		{
			m_pColtRunner = new GColtRunner(&m_JobRunner);
			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			GNPCInfo* pMode1Info = new GNPCInfo();
			pMode1Info->nID = 1;
			pMode1Info->nMode = NPC_MODE_1;
			pMode1Info->ModeCopyFromParent(m_pNPCInfo);
			m_pNPCInfo->vecModes.push_back(pMode1Info);	// mode1도 있음
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);

			m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		}

		~FColt()
		{
			SAFE_DELETE(m_pColtRunner);
			m_pField->Update(1.0f);
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
			SAFE_DELETE(m_pFieldInfo);
		}

		GColtAction MakeSimpleAction(int nRate, COLT_ACTION_TYPE nType=CAT_NONE, float fCycleTime=0.0f)
		{
			GColtAction action;
			action.nRate = nRate;
			action.nType = nType;
			action.fCycleTime = fCycleTime;
			return action;
		}
			
		MockColt		m_Colt;
		GColtRunner*	m_pColtRunner;
		GJobRunner		m_JobRunner;
		MockField*		m_pField;
		GFieldInfo*		m_pFieldInfo;
		GUTHelper		m_Helper;
		GNPCInfo*		m_pNPCInfo;
		
		DECLWRAPPER_FakeSystem;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_NPCInfoMgr;
		DECLWRAPPER_FieldInfoMgr;
	};

	TEST_FIXTURE(FColt, CalcActionTotalRate)
	{
		const int ActionQty=4;
		struct ActionRateCheck
		{
			const int nRate[ActionQty];
			const int nTotalRate;
		} cases[] = {
				 {{0, 100, 0, 0}, 100}	// rate 총합이 100 (하나의 액션 rate만 딱100)
				,{{30, 30, 40, 0}, 100}	// rate 총합이 100
				,{{44, 13, 0, 0}, 57}	// rate 총합이 100보다 작음
				,{{35, 66, 0, 0}, 101}	// rate 총합이 100보다 큼
				,{{173, 66, 0, 0}, 239}	// rate 총합이 100보다 큼 (하나의 액션 rate가 100보다 큼)
				,{{0, 0, 0, 0}, 0}		// rate 총합이 0
				,{{-20, -70, -30, 0}, -120}		// rate 총합이 0보다 작음
				,{{300, -200, -50, 0}, 50}		// rate 총합이 0보다 작음
		};

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			ActionRateCheck check = cases[i];
			GColtCheck Check;
			int nOriginalActionTotalRate=0;
			for (int j=0; j<ActionQty; ++j)
			{
				nOriginalActionTotalRate += check.nRate[j];
				Check.vecActions.push_back(MakeSimpleAction(check.nRate[j]));	
			}
			
			m_Colt.CalcActionRate(Check);
			const vector<GColtActionInst>& actioninst_list = m_Colt.LookupActionInst(Check);

			CHECK_EQUAL(Check.vecActions.size(), ActionQty);
			CHECK_EQUAL(actioninst_list.size(), ActionQty);
			CHECK_EQUAL(nOriginalActionTotalRate, check.nTotalRate);

			int nModifiedActionTotalRate=0;
			for (size_t j=0; j<actioninst_list.size(); ++j)
			{
				const int nModifiedRate = actioninst_list[j].GetRate();
				CHECK(nModifiedRate >= 0);
				nModifiedActionTotalRate += nModifiedRate;
			}
			CHECK_EQUAL(nModifiedActionTotalRate, 100);
		}
	}

	TEST_FIXTURE(FColt, ActionPickup)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos);
		GColtCheck Check;
		Check.vecActions.push_back(MakeSimpleAction(0, CAT_TALENT));	
		Check.vecActions.push_back(MakeSimpleAction(100, CAT_LUAFUNC));	// 확률이 100인 이것이 뽑힘
		Check.vecActions.push_back(MakeSimpleAction(0, CAT_BREAKPART));	
		Check.vecActions.push_back(MakeSimpleAction(0, CAT_DAMAGE));	

		RUNNING_ACTION_LIST actionList = m_Colt.PickAction(Check);

		if (actionList.empty())		{ CHECK(!"액션이 뽑히지 않았음"); return; }
		const GColtAction* pPickedAction = actionList.front().first;
		CHECK_EQUAL(pPickedAction->nType, CAT_LUAFUNC);
	}

	TEST_FIXTURE(FColt, CheckDecreaseCountInActionInstance)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos);
		GColtCheck Check;
		GColtAction action = MakeSimpleAction(100);
		action.nMaxCount = 3;
		Check.vecActions.push_back(action);	

		for (int i=0; i<=action.nMaxCount; i++)
		{
			RUNNING_ACTION_LIST actionList = m_Colt.PickAction(Check);

			if (i == action.nMaxCount)
			{
				// 최대횟수만큼 뽑힌 경우, 더 뽑히지 않는다.
				CHECK(actionList.empty());
				break;
			}

			if (actionList.empty())		{ CHECK(!"액션이 뽑히지 않았음"); return; }
			const GColtAction* pPickedAction = actionList.front().first;
			GColtActionInst* pPickedActionInst = actionList.front().second;
			CHECK_EQUAL(pPickedActionInst->GetUseCount(), i);
			m_pColtRunner->RunAction(pPickedAction, pPickedActionInst, pNPC, NULL);
			CHECK_EQUAL(pPickedActionInst->GetUseCount(), i+1);
		}
	}

	TEST_FIXTURE(FColt, Action_IntervalTime)
	{
		GColtCheck Check;
		GColtAction action = MakeSimpleAction(100, CAT_TALENT, 0.05f);
		Check.vecActions.push_back(action);

		m_Colt.CalcActionRate(Check);
		const vector<GColtActionInst>& pqActionInstances = m_Colt.LookupActionInst(Check);		
		if (pqActionInstances.size() <= 0)		
		{ 
			CHECK(!"콜트액션 인스턴스 생성실패"); 
			return; 
		}

		// 초기에는 주기시간이 없이 즉시 사용가능한 시간인가?		
		CHECK_EQUAL(pqActionInstances.front().GetCycleTime(), gsys.pSystem->GetNowTime());

		RUNNING_ACTION_LIST action_list = m_Colt.PickAction(Check);
		if (action_list.empty())		{ CHECK(!"액션이 뽑히지 않았음"); return; }
		
		action_list = m_Colt.PickAction(Check);
		CHECK(action_list.empty());		// 액션이 딜레이중이므로 뽑히지 않음

		gsys.pSystem->Sleep(0.1f);

		action_list = m_Colt.PickAction(Check);
		if (action_list.empty())		{ CHECK(!"액션이 뽑히지 않았음"); return; }
	}

	TEST_FIXTURE(FColt, DontPickDuringDelayedAction)
	{
		GColtCheck CheckNormal;
		CheckNormal.nType = AST_HP;
		CheckNormal.vecParams[0] = pair<bool,int>(true, 0);
		CheckNormal.vecParams[1] = pair<bool,int>(true, 101);
		CheckNormal.vecActions.push_back(MakeSimpleAction(0, CAT_MOVE));

		GColtCheck CheckWithDelay;
		CheckWithDelay.nType = AST_HP;
		CheckWithDelay.vecParams[0] = pair<bool,int>(true, 0);
		CheckWithDelay.vecParams[1] = pair<bool,int>(true, 101);
		CheckWithDelay.vecActions.push_back(MakeSimpleAction(0, CAT_MOVE, 0.05f));

		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(&CheckWithDelay);
		vecChecks.push_back(&CheckNormal);
		m_Colt.SetChecks(vecChecks, NULL);
		
		m_Colt.CalcActionRate(CheckWithDelay);
		m_Colt.CalcActionRate(CheckNormal);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		
		GColtCheck* pPickedCheck = NULL;
		pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, (GColtCheck*)&CheckWithDelay);

		const GColtAction* pPickedAction = NULL;
		GColtActionInst* pActionInstance = NULL;
		RUNNING_ACTION_LIST action_list = m_Colt.PickAction(*pPickedCheck);
		if (action_list.empty())		{ CHECK(!"액션이 뽑히지 않았음"); return; }

		pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, (GColtCheck*)&CheckNormal);

		gsys.pSystem->Sleep(0.1f);

		pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, (GColtCheck*)&CheckWithDelay);	// 대기시간이 모두 지나 원래의 액션이 뽑힌다.
	}

	// Check이 아무것도 없는 경우
	TEST_FIXTURE(FColt, CheckEmpty)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		vector<GColtCheck*> vecChecks;	
		m_Colt.SetChecks(vecChecks, NULL);
		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);

		CHECK(NULL == pPickedCheck);
	}

	// Check 체크가 실패한 경우
	TEST_FIXTURE(FColt, CheckFail)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetNPCInfo()->nMaxHP = 100;
		pNPC->SetHP(20);
		GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(pNPC, AST_HP, 0, 10);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(pCheck);
		m_Colt.SetChecks(vecChecks, NULL);

		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);

		CHECK(NULL == pPickedCheck);
	}

	// Check 체크가 성공한 경우
	TEST_FIXTURE(FColt, CheckSuccess)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetNPCInfo()->nMaxHP = 100;
		pNPC->SetHP(20);
		GColtAction Action1;
		Action1.nType = CAT_TALENT;
		GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(pNPC, AST_HP, 0, 100);
		pCheck->vecActions.push_back(Action1);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(pCheck);
		m_Colt.SetChecks(vecChecks, NULL);

		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);

		CHECK_EQUAL(pCheck, pPickedCheck);
	}		

	// Action이 하나도 없을때
	TEST_FIXTURE(FColt, ActionsIsEmpty)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(pNPC, AST_HP, 0, 101);
		pCheck->vecActions.clear();

		CHECK_EQUAL(false, m_Colt.CheckCondition(*pCheck, pNPC));
	}


	TEST_FIXTURE(FColt, InnerCheckFail)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetNPCInfo()->nMaxHP = 100;
		pNPC->SetHP(20);
		GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(pNPC, AST_HP, 0, 30);
		GColtCheck* pInnerCheck = m_Helper.NewCheckInfo(AST_HP, 0, 10);
		pCheck->vecChecks.push_back(pInnerCheck);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(pCheck);
		m_Colt.SetChecks(vecChecks, NULL);

		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK(pCheck == pPickedCheck);
	}

	// Check과 InnerCheck의 체크가 성공한 경우
	TEST_FIXTURE(FColt, InnerCheckSuccess)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		pNPC->GetNPCInfo()->nMaxHP = 100;
		pNPC->SetHP(20);
		GColtAction dummyActionInfo;
		GColtCheck* pInnerCheck = m_Helper.NewCheckInfo(AST_HP, 0, 40);				
		pInnerCheck->vecActions.push_back(dummyActionInfo);				
		GColtCheck* pCheck = m_Helper.GiveNewCheckInfo(pNPC, AST_HP, 0, 30);				
		pCheck->vecChecks.push_back(pInnerCheck);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(pCheck);			
		m_Colt.SetChecks(vecChecks, NULL);

		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);

		CHECK_EQUAL(pInnerCheck, pPickedCheck);
	}

	TEST_FIXTURE(FColt, Check_PickRate)
	{
		GColtCheck CheckSkipable;
		CheckSkipable.nType = AST_HP;
		CheckSkipable.vecParams[0] = pair<bool,int>(true, 0);
		CheckSkipable.vecParams[1] = pair<bool,int>(true, 101);
		CheckSkipable.nPickRate = 0;

		GColtAction ActionTalent;
		ActionTalent.nType = CAT_NOTHING;

		GColtCheck CheckPickable;
		CheckPickable.nType = AST_HP;
		CheckPickable.vecParams[0] = pair<bool,int>(true, 0);
		CheckPickable.vecParams[1] = pair<bool,int>(true, 101);
		CheckPickable.vecActions.push_back(ActionTalent);

		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(&CheckSkipable);
		vecChecks.push_back(&CheckPickable);
		m_Colt.SetChecks(vecChecks, NULL);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);

		CHECK_EQUAL(pPickedCheck, (GColtCheck*)&CheckPickable);
	}

	TEST_FIXTURE(FColt, AutoRunAction)
	{	
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GColtCheck* pCheckInfo = m_Helper.NewCheckInfo(AST_HP);				
		pCheckInfo->vecAutoRunActions.push_back(MakeSimpleAction(0, CAT_TALENT));

		GColtRunner colt_runner;
		RUNNING_ACTION_LIST action_list = m_Colt.PickAction(*pCheckInfo);
		if (action_list.empty())		{ CHECK(!"액션이 뽑히지 않았음"); return; }

		const GColtAction* pPickedAction = action_list.front().first;
		CHECK_EQUAL(action_list.size(), 1);
		CHECK_EQUAL(pPickedAction->nType, CAT_TALENT);
		SAFE_DELETE(pCheckInfo);
	}

	TEST_FIXTURE(FColt, PickSecondAction)
	{	
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GColtCheck* pCheckInfo = m_Helper.NewCheckInfo(AST_HP);				
		pCheckInfo->vecActions.push_back(MakeSimpleAction(0, CAT_NOTHING));
		pCheckInfo->vecActions.push_back(MakeSimpleAction(100, CAT_TALENT));

		GColtRunner colt_runner;
		RUNNING_ACTION_LIST action_list = m_Colt.PickAction(*pCheckInfo);
		if (action_list.empty())		{ CHECK(!"액션이 뽑히지 않았음"); return; }

		const GColtAction* pPickedAction = action_list.front().first;
		CHECK_EQUAL(action_list.size(), 1);
		CHECK_EQUAL(pPickedAction->nType, CAT_TALENT);

		SAFE_DELETE(pCheckInfo);
	}

	// 하위 체크문이 성공했을때 하위 체크문을 선택
	TEST_FIXTURE(FColt, PickUp_DefaultBottomCheck)
	{
		GColtCheck CheckDepth1;
		CheckDepth1.nType = AST_HP;
		CheckDepth1.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth1.vecParams[1] = pair<bool,int>(true, 101);
		GColtAction action;
		action.nType = CAT_NOTHING;
		CheckDepth1.vecActions.push_back(action); // 뎁스1에 액션이 있음

		GColtCheck CheckDepth2;
		CheckDepth2.nType = AST_HP;
		CheckDepth2.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth2.vecParams[1] = pair<bool,int>(true, 101);
		action.nType = CAT_NOTHING;
		CheckDepth2.vecActions.push_back(action); // 뎁스2에 액션이 있음

		CheckDepth1.vecChecks.push_back(&CheckDepth2);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(&CheckDepth1);
		m_Colt.SetChecks(vecChecks, NULL);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, &CheckDepth2); // 뎁스2 선택됨
	}

	// 하위 체크문이 실패했을때 상위 체크문을 선택
	TEST_FIXTURE(FColt, PickUp_UpDepthWhenCheckFailed)
	{
		GColtCheck CheckDepth1;
		CheckDepth1.nType = AST_HP;
		CheckDepth1.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth1.vecParams[1] = pair<bool,int>(true, 101);
		
		GColtAction action;
		action.nType = CAT_TALENT;
		CheckDepth1.vecActions.push_back(action); // 뎁스1에서 액션이 선택됨

		GColtCheck CheckDepth2;
		CheckDepth2.nType = AST_HP;
		CheckDepth2.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth2.vecParams[1] = pair<bool,int>(true, 101);
		
		CheckDepth1.vecChecks.push_back(&CheckDepth2);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(&CheckDepth1);
		m_Colt.SetChecks(vecChecks, NULL);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, &CheckDepth1); // 뎁스1 선택됨
	}


	// 하위 체크문이 실패한 경우, 중간 체크문에는 액션이 없을때, 상위 체크문이 선택됨
	TEST_FIXTURE(FColt, PickUp_SkipCheckWithoutAction)
	{
		GColtCheck CheckDepth1;
		CheckDepth1.nType = AST_HP;
		CheckDepth1.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth1.vecParams[1] = pair<bool,int>(true, 101);

		GColtAction action;
		action.nType = CAT_TALENT;
		CheckDepth1.vecActions.push_back(action); // 뎁스1에서 액션이 선택됨

		GColtCheck CheckDepth2;
		CheckDepth2.nType = AST_HP;
		CheckDepth2.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth2.vecParams[1] = pair<bool,int>(true, 101);

		GColtCheck CheckDepth3;
		CheckDepth3.nType = AST_HP;
		CheckDepth3.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth3.vecParams[1] = pair<bool,int>(true, 0);
		action.nType = CAT_TALENT;
		CheckDepth3.vecActions.push_back(action); // 뎁스1에서 액션이 선택됨

		CheckDepth2.vecChecks.push_back(&CheckDepth3);
		CheckDepth1.vecChecks.push_back(&CheckDepth2);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(&CheckDepth1);
		m_Colt.SetChecks(vecChecks, NULL);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, &CheckDepth1); // 뎁스1 선택됨
	}

	// 최상위 체크문에는 액션이 없을때, 디폴트 체크문이 선택됨
	TEST_FIXTURE(FColt, PickUp_DefaultWhenTopCheckFailed)
	{
		GColtCheck CheckDefault;

		GColtCheck CheckDepth1;
		CheckDepth1.nType = AST_HP;
		CheckDepth1.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth1.vecParams[1] = pair<bool,int>(true, 101);

		GColtCheck CheckDepth2;
		CheckDepth2.nType = AST_HP;
		CheckDepth2.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth2.vecParams[1] = pair<bool,int>(true, 101);

		CheckDepth1.vecChecks.push_back(&CheckDepth2);
		vector<GColtCheck*> vecChecks;
		vecChecks.push_back(&CheckDepth1);
		m_Colt.SetChecks(vecChecks, &CheckDefault);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, &CheckDefault); // 뎁스1 선택됨
	}

	TEST_FIXTURE(FColt, MultiSection)
	{
		GColtCheck CheckDepth1;
		CheckDepth1.nType = AST_HP;
		CheckDepth1.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth1.vecParams[1] = pair<bool,int>(true, 101);

		GColtAction action;
		action.nType = CAT_TALENT;
		CheckDepth1.vecActions.push_back(action); // 뎁스1에 액션이 있음

		GColtCheck CheckDepth2;
		CheckDepth2.nType = AST_HP;
		CheckDepth2.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth2.vecParams[1] = pair<bool,int>(true, 101);

		CheckDepth1.vecChecks.push_back(&CheckDepth2);
		vector<GColtCheck*> vecChecks[NPC_MODE_MAX];
		vecChecks[NPC_MODE_1].push_back(&CheckDepth1);
		m_Colt.SetMultiChecks(vecChecks, NULL);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK(pPickedCheck == NULL); // 모드 디폴트로는 선택안됨
		pNPC->ChangeMode(NPC_MODE_1);
		pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, &CheckDepth1); // 모드1은 선택됨
	}

	TEST_FIXTURE(FColt, MultiSection_DefaultChecks)
	{
		GColtCheck CheckDepth1;
		CheckDepth1.nType = AST_HP;
		CheckDepth1.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth1.vecParams[1] = pair<bool,int>(true, 101);

		GColtCheck CheckDepth2;
		CheckDepth2.nType = AST_HP;
		CheckDepth2.vecParams[0] = pair<bool,int>(true, 0);
		CheckDepth2.vecParams[1] = pair<bool,int>(true, 101);

		CheckDepth1.vecChecks.push_back(&CheckDepth2);
		vector<GColtCheck*> vecChecks[NPC_MODE_MAX];
		vecChecks[NPC_MODE_1].push_back(&CheckDepth1);
		GColtCheck CheckDefault[NPC_MODE_MAX];
		
		m_Colt.SetMultiChecks(vecChecks, CheckDefault);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		GColtCheck* pPickedCheck = m_Colt.PickCheck(pNPC);
		CHECK_EQUAL(pPickedCheck, &CheckDefault[NPC_MODE_DEFAULT]);
		pNPC->ChangeMode(NPC_MODE_1);
		pPickedCheck = m_Colt.PickCheck(pNPC);
		// 모드1 디폴트가 선택됨
		CHECK(pPickedCheck != &CheckDefault[NPC_MODE_DEFAULT]);
		CHECK_EQUAL(pPickedCheck, &CheckDefault[NPC_MODE_1]);
	}

	TEST_FIXTURE(FColt, MultiSection_CombatAttr)
	{
		test.talent.NewTalentInfo(111)->m_setNPCID.insert(m_pNPCInfo->nID);
		test.talent.NewTalentInfo(222)->m_setNPCID.insert(m_pNPCInfo->nID);

		m_pNPCInfo->ColtInfo.attrCombat[NPC_MODE_DEFAULT].nVictoryTalentID = 111;
		m_pNPCInfo->ColtInfo.attrCombat[NPC_MODE_1].nVictoryTalentID = 222;

		// 모드0,  전투 시작 & 종료
		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
			GEntityNPC* pEnemy = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
			pNPC->GetHateTable().EnableTimeout(true);
			
			pNPC->doAttack(pEnemy->GetUID());
			pEnemy->doDie();
			// 헤이트 테이블에서 지워질 틱
			pNPC->Update(GConst::HATETABLE_ENTRY_TIMEOUT);	 
			// Behavior 상태가 바뀔 틱
			pNPC->GetModuleAI()->Update(0.0f);	 

			// 승리 탤런트 검사
			GJob_Talent* pJob = dynamic_cast<GJob_Talent*>(pNPC->GetJobMgr()->GetCurrJob());
			TVALID(pJob);
			CHECK_EQUAL(111, pJob->GetTalentID());
		}

		// 모드1,  전투 시작 & 종료
		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
			GEntityNPC* pEnemy = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
			pNPC->GetHateTable().EnableTimeout(true);
			pNPC->ChangeMode(NPC_MODE_1);

			pNPC->doAttack(pEnemy->GetUID());
			pEnemy->doDie();
			// 헤이트 테이블에서 지워질 틱
			pNPC->Update(GConst::HATETABLE_ENTRY_TIMEOUT);	 
			// Behavior 상태가 바뀔 틱
			pNPC->GetModuleAI()->Update(0.0f);	 

			// 승리 탤런트 검사
			GJob_Talent* pJob = dynamic_cast<GJob_Talent*>(pNPC->GetJobMgr()->GetCurrJob());
			TVALID(pJob);
			CHECK_EQUAL(222, pJob->GetTalentID());
		}
	}

	TEST_FIXTURE(FColt, CheckActionCondition)
	{
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos);
		GColtCheck Check;
		Check.nType = AST_PEACE;
		GColtAction action = MakeSimpleAction(100, CAT_TALENT);
		action.qParams[0] = L"777";
		action.nMaxCount = 1;
		Check.vecActions.push_back(action);

		GTalentInfo* pTestTalent = 
			test.talent.NewTalentInfo(777); 

		GColtPicker picker(GColtPicker::SECTION_SINGLE, (ColtType)0, NULL, NULL);
		vector<GColtActionInst>& inst = 
			picker.GetActionInstMgr().LookupActionInst(Check);
		inst.push_back(GColtActionInst(action));
		inst[0].SetRate(1000);

		CHECK_EQUAL( true, picker.CheckCondition(Check, pNPC) );

		// 탤런트 사용 불가능한 조건
		pTestTalent->m_nHPCost = 888;
		pNPC->SetHP(1); 

		CHECK_EQUAL( false, picker.CheckCondition(Check, pNPC) );

	}
}

