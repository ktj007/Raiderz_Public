#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "GColtActionInstMgr.h"
#include "GTestForward.h"

SUITE(ColtActionInstMgr)
{
	struct FColt
	{
		FColt()
		{
		}
		~FColt()
		{
		}

		GColtActionInstMgr inst_mgr;
		DECLWRAPPER_FakeSystem;
	};

	TEST_FIXTURE(FColt, Basic)
	{
		GColtAction action;
		action.nRate = 11;
		action.nMaxCount = 22;

		GColtCheck Check;
		Check.nType = AST_HP;
		Check.vecParams[0] = pair<bool,int>(true, 0);
		Check.vecParams[1] = pair<bool,int>(true, 101);
		Check.vecActions.push_back(action);

		vector<GColtCheck> vecChecks;
		vecChecks.push_back(Check);
		
		// 액션만큼의 인스턴스 액션이 생김
		CHECK_EQUAL(Check.vecActions.size(), inst_mgr.LookupActionInst(vecChecks[0]).size());

		// 인스턴스 복사 제대로 됐는지 확인
		TVALID_EQ(1, Check.vecActions.size());
		CHECK_EQUAL(11, Check.vecActions[0].nRate);
		CHECK_EQUAL(22, Check.vecActions[0].nMaxCount);
	}

	TEST_FIXTURE(FColt, CalcPickRate_Single)
	{
		GColtAction action;
		action.nRate = 11;
		GColtCheck Check;
		vector<GColtCheck> vecChecks;
		Check.vecActions.push_back(action);
		vecChecks.push_back(Check);

		inst_mgr.CalcActionRate(vecChecks[0]);
		vector<GColtActionInst>& vecInsts = inst_mgr.LookupActionInst(vecChecks[0]);
		CHECK_EQUAL(100, vecInsts[0].GetRate());
	}


	TEST_FIXTURE(FColt, CalcPickRate_LessMax)
	{
		GColtCheck Check;
		vector<GColtCheck> vecChecks;
		GColtAction action;
		action.nRate = 253;
		Check.vecActions.push_back(action);
		action.nRate = 721;
		Check.vecActions.push_back(action);
		action.nRate = 336;
		Check.vecActions.push_back(action);
		vecChecks.push_back(Check);
		vector<GColtActionInst>& vecInsts = inst_mgr.LookupActionInst(vecChecks[0]);
		inst_mgr.CalcActionRate(vecChecks[0]);
		CHECK_EQUAL(20, vecInsts[0].GetRate());
		CHECK_EQUAL(55, vecInsts[1].GetRate());
		CHECK_EQUAL(25, vecInsts[2].GetRate());
	}

	TEST_FIXTURE(FColt, CalcPickRate_ThanMax)
	{
		GColtCheck Check;
		vector<GColtCheck> vecChecks;
		GColtAction action;
		action.nRate = 11;
		Check.vecActions.push_back(action);
		action.nRate = 24;
		Check.vecActions.push_back(action);
		action.nRate = 47;
		Check.vecActions.push_back(action);
		vecChecks.push_back(Check);
		vector<GColtActionInst>& vecInsts = inst_mgr.LookupActionInst(vecChecks[0]);
		inst_mgr.CalcActionRate(vecChecks[0]);
		CHECK_EQUAL(14, vecInsts[0].GetRate());
		CHECK_EQUAL(29, vecInsts[1].GetRate());
		CHECK_EQUAL(57, vecInsts[2].GetRate());
	}

	TEST_FIXTURE(FColt, CalcPickRate_ExactlyMax)
	{
		GColtCheck Check;
		vector<GColtCheck> vecChecks;
		GColtAction action;
		action.nRate = 45;
		Check.vecActions.push_back(action);
		action.nRate = 20;
		Check.vecActions.push_back(action);
		action.nRate = 35;
		Check.vecActions.push_back(action);
		vecChecks.push_back(Check);
		vector<GColtActionInst>& vecInsts = inst_mgr.LookupActionInst(vecChecks[0]);
		inst_mgr.CalcActionRate(vecChecks[0]);
		CHECK_EQUAL(45, vecInsts[0].GetRate());
		CHECK_EQUAL(20, vecInsts[1].GetRate());
		CHECK_EQUAL(35, vecInsts[2].GetRate());
	}

	// 사용 횟수를 다 쓴 액션은 무시
	TEST_FIXTURE(FColt, CalcPickRate_ConditionCount)
	{
		GColtCheck Check;
		vector<GColtCheck> vecChecks;
		GColtAction action;
		action.nRate = 50;
		action.nMaxCount = 1; // 최대 사용횟수
		Check.vecActions.push_back(action);
		action.nRate = 50;
		Check.vecActions.push_back(action);
		vecChecks.push_back(Check);
		vector<GColtActionInst>& vecInsts = inst_mgr.LookupActionInst(vecChecks[0]);
		vecInsts[0].IncreaseUseCount(); // 사용횟수 최대로 셋팅
		inst_mgr.CalcActionRate(vecChecks[0]);
		CHECK_EQUAL(0, vecInsts[0].GetRate());
		CHECK_EQUAL(100, vecInsts[1].GetRate());
	}

	// 싸이클 대기 시간에 걸린 액션은 무시
	TEST_FIXTURE(FColt, CalcPickRate_ConditionCycle)
	{
		GColtCheck Check;
		vector<GColtCheck> vecChecks;
		GColtAction action;
		action.nRate = 50;
		Check.vecActions.push_back(action);
		action.nRate = 50;
		Check.vecActions.push_back(action);
		vecChecks.push_back(Check);
		vector<GColtActionInst>& vecInsts = inst_mgr.LookupActionInst(vecChecks[0]);
		vecInsts[0].ResetCycleTime(1); // 1초후에 사용가능
		inst_mgr.CalcActionRate(vecChecks[0]);
		CHECK_EQUAL(0, vecInsts[0].GetRate());
		CHECK_EQUAL(100, vecInsts[1].GetRate());
	}
}

