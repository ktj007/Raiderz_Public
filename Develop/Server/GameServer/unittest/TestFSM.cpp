#include "stdafx.h"
#include "GState.h"


class TestStateParent
{

};

enum TEST_STATE_ID
{
	TSI_BASE=0,
	TSI_ONE,
	TSI_TWO,
	TSI_THREE,
	TSI_INNER,
};

enum TEST_STATUS_MESSAGE
{
	TSM_CHANGE_ONE=0,
	TSM_CHANGE_TWO,
	TSM_CHANGE_THREE,
};

class TestState : public GState<TestStateParent>
{
public:	
	TestState(TestStateParent* pOwner, GStateMachine<TestStateParent>* pFSM) : GState<TestStateParent>(pOwner, pFSM), nEnterCounter(0), nExitCounter(0) {}	
	virtual void			Enter()						{ nEnterCounter++; }
	virtual void			Update(float fDelta)		{}
	virtual void			Exit(int nNextStateID)		{ nExitCounter++; }
	virtual GMessageResult	Message(GMessage& msg)		{ if (__super::Message(msg)==MR_TRUE) return MR_TRUE;	 return MR_FALSE; }
	int						GetID()						{ return TSI_BASE; }

	int		nEnterCounter;
	int		nExitCounter;
};

class TestStateOne : public TestState
{
public:
	TestStateOne(TestStateParent* pOwner, GStateMachine<TestStateParent>* pFSM) : TestState(pOwner, pFSM) 
	{
		
	}	

	void					OnRegisterTrainstionRule()
	{
		AddTransitionRule(TSM_CHANGE_TWO, TSI_TWO);
	}

	int						GetID() { return TSI_ONE; }
};

class TestStateTwo : public TestState
{
public:
	TestStateTwo(TestStateParent* pOwner, GStateMachine<TestStateParent>* pFSM) : TestState(pOwner, pFSM) 
	{

	}

	void					OnRegisterTrainstionRule()
	{
	}

	int						GetID() { return TSI_TWO; }
};

class TestStateThree : public TestState
{
public:
	TestStateThree(TestStateParent* pOwner, GStateMachine<TestStateParent>* pFSM) : TestState(pOwner, pFSM) 
	{

	}	

	void					OnRegisterTrainstionRule()
	{
	}

	int						GetID() { return TSI_THREE; }
};

class GTestStateMachine : public GStateMachine<TestStateParent>
{
public:
	GTestStateMachine(TestStateParent* pOwner) : GStateMachine<TestStateParent>(pOwner) {}
	virtual ~GTestStateMachine() {}
};

class GTestStateInnerMachine : public GStateMachine<TestStateParent>
{
public:
	GTestStateInnerMachine(TestStateParent* pOwner) : GStateMachine<TestStateParent>(pOwner) {}
	virtual ~GTestStateInnerMachine() {}
};

class TestStateTwice : public TestState
{
public:
	TestStateTwice(TestStateParent* pOwner, GStateMachine<TestStateParent>* pFSM) : TestState(pOwner, pFSM),  InnerMachine(pOwner)
	{
		StateOne = new TestStateOne(NULL, &InnerMachine);
		StateTwo = new TestStateTwo(NULL, &InnerMachine);
		StateThree = new TestStateThree(NULL, &InnerMachine);
	}	

	GTestStateInnerMachine	InnerMachine;
	TestStateOne*		StateOne;
	TestStateTwo*		StateTwo;
	TestStateThree*		StateThree;

	void					OnRegisterTrainstionRule()
	{
	}

	int						GetID() { return TSI_INNER; }
};



SUITE(FSM)
{
	struct Fixture
	{
		Fixture()
			:FSM(NULL)
		{ 
			StateOne = new TestStateOne(NULL, &FSM);
			StateTwo = new TestStateTwo(NULL, &FSM);
			StateThree = new TestStateThree(NULL, &FSM);
		}

		~Fixture() 
		{
		}

		GTestStateMachine	FSM;
		TestStateOne*		StateOne;
		TestStateTwo*		StateTwo;
		TestStateThree*		StateThree;
	};

	TEST_FIXTURE(Fixture, AddState)
	{
		CHECK_EQUAL(FSM.GetSize(), 0);
		FSM.AddState(StateOne);
		FSM.AddState(StateTwo);
		FSM.AddState(StateThree);
		CHECK_EQUAL(FSM.GetSize(), 3);
	}

	TEST_FIXTURE(Fixture, ChangeStatus)
	{
		FSM.AddState(StateOne);
		FSM.AddState(StateTwo);
		FSM.AddState(StateThree);

		CHECK_EQUAL(FSM.GetCurrentStateID(), -1);
		FSM.ChangeState(TSI_ONE);	// ID로 상태 설정
		CHECK_EQUAL(FSM.GetCurrentStateID(), TSI_ONE);
		FSM.ChangeState(StateTwo);	// 객체로 상태 설정
		CHECK_EQUAL(FSM.GetCurrentStateID(), TSI_TWO);
		FSM.ChangeState(StateThree);	// 객체로 상태 변경
		CHECK_EQUAL(FSM.GetCurrentStateID(), TSI_THREE);
		FSM.ChangeState(TSI_ONE);	// ID로 상태 변경
		CHECK_EQUAL(FSM.GetCurrentStateID(), TSI_ONE);
	}

	TEST_FIXTURE(Fixture, CheckChangeEvent)
	{
		FSM.AddState(StateOne);
		FSM.AddState(StateTwo);
		FSM.AddState(StateThree);

		CHECK_EQUAL(StateOne->nEnterCounter, 0);
		CHECK_EQUAL(StateOne->nExitCounter, 0);
		FSM.ChangeState(TSI_ONE);
		CHECK_EQUAL(StateOne->nEnterCounter, 1);
		CHECK_EQUAL(StateOne->nExitCounter, 0);
		FSM.ChangeState(TSI_TWO);
		CHECK_EQUAL(StateOne->nEnterCounter, 1);
		CHECK_EQUAL(StateOne->nExitCounter, 1);
		CHECK_EQUAL(StateTwo->nEnterCounter, 1);
		CHECK_EQUAL(StateTwo->nExitCounter, 0);
	}

	TEST_FIXTURE(Fixture, TrasitionByMessage)
	{
		FSM.AddState(StateOne);
		FSM.AddState(StateTwo);
		FSM.AddState(StateThree);

		FSM.ChangeState(TSI_ONE);
		CHECK_EQUAL(StateOne->nEnterCounter, 1);
		CHECK_EQUAL(StateOne->nExitCounter, 0);

		GMessage msg;
		msg.m_nID = TSM_CHANGE_TWO;
		FSM.Message(msg);

		CHECK_EQUAL(StateOne->nEnterCounter, 1);
		CHECK_EQUAL(StateOne->nExitCounter, 1);
		CHECK_EQUAL(FSM.GetCurrentStateID(), TSI_TWO);
		CHECK_EQUAL(StateTwo->nEnterCounter, 1);
		CHECK_EQUAL(StateTwo->nExitCounter, 0);

		msg.m_nID = TSM_CHANGE_THREE;		// 전이 규칙에 없는 메세지가 왔을 경우
		FSM.Message(msg);

		CHECK_EQUAL(FSM.GetCurrentStateID(), TSI_TWO);
		CHECK_EQUAL(StateTwo->nEnterCounter, 1);
		CHECK_EQUAL(StateTwo->nExitCounter, 0);
	}
}




