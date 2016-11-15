#include "stdafx.h"
#include "GPlayerGameStart_State.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "GDynamicFieldMaster.h"
#include "GUTHelper_Field.h"

SUITE(PlayerGameStart_State)
{
	class FPlayerGameStartState : public FBaseFieldMgr, public FBaseFieldInfo
	{
	public:
		FPlayerGameStartState()
		{
			m_nSharedFieldID = 1;
			m_nDynamicFieldID = 2;

			GFieldInfo* pSharedFieldInfo = AddFieldInfo(m_nSharedFieldID);
			pSharedFieldInfo->m_bDynamic = false;

			GFieldInfo* pDynamicFieldInfo = AddFieldInfo(m_nDynamicFieldID);
			pDynamicFieldInfo->m_bDynamic = true;

			FIELD_GROUP_INFO* pFieldGroupInfo = GUTHelper_Field::NewFieldGroupInfo(pDynamicFieldInfo);
			GDynamicFieldMaster* pDynamicFieldMaster = GUTHelper_Field::NewTrialFieldMaster(pFieldGroupInfo);

			m_uidDynamicFieldGroup = pDynamicFieldMaster->GetUID();
		}
		virtual ~FPlayerGameStartState()
		{

		}

		int m_nSharedFieldID;
		int m_nDynamicFieldID;
		MUID m_uidDynamicFieldGroup;
	};
	TEST_FIXTURE(FPlayerGameStartState, TestPlayerGameStart_State_FirstLogin)
	{
		GPlayerGameStart_State::InputParam inputParam;
		inputParam.nExitFieldID = 0;

		GPlayerGameStart_State state(inputParam);

		CHECK_EQUAL(true, state.IsFirstLogin());
		CHECK_EQUAL(false, state.IsShareField());
		CHECK_EQUAL(false, state.IsDynamicField());
		CHECK_EQUAL(false, state.IsBattleArena());
		CHECK_EQUAL(false, state.IsFieldExist());
		CHECK_EQUAL(false, state.IsDead());
		CHECK_EQUAL(false, state.IsAutoRebirthTimeOver());
	}

	TEST_FIXTURE(FPlayerGameStartState, TestPlayerGameStart_State_ShareField)
	{
		GPlayerGameStart_State::InputParam inputParam;
		inputParam.nExitFieldID = m_nSharedFieldID;

		GPlayerGameStart_State state(inputParam);

		CHECK_EQUAL(false, state.IsFirstLogin());
		CHECK_EQUAL(true, state.IsShareField());
		CHECK_EQUAL(false, state.IsDynamicField());
		CHECK_EQUAL(false, state.IsBattleArena());
		CHECK_EQUAL(true, state.IsFieldExist());
		CHECK_EQUAL(false, state.IsDead());
		CHECK_EQUAL(false, state.IsAutoRebirthTimeOver());
	}

	TEST_FIXTURE(FPlayerGameStartState, TestPlayerGameStart_State_DynamicField)
	{
		GPlayerGameStart_State::InputParam inputParam;
		inputParam.nExitFieldID = m_nDynamicFieldID;

		GPlayerGameStart_State state(inputParam);

		CHECK_EQUAL(false, state.IsFirstLogin());
		CHECK_EQUAL(false, state.IsShareField());
		CHECK_EQUAL(true, state.IsDynamicField());
		CHECK_EQUAL(false, state.IsBattleArena());
		CHECK_EQUAL(false, state.IsDead());
		CHECK_EQUAL(false, state.IsAutoRebirthTimeOver());

		// 필드 인스턴스가 없으므로 false
		CHECK_EQUAL(false, state.IsFieldExist());


		// --
		GPlayerGameStart_State::InputParam inputParam2;
		inputParam2.nExitFieldID = m_nDynamicFieldID;
		inputParam2.uidDynamicFieldGroup = m_uidDynamicFieldGroup;

		GPlayerGameStart_State state2(inputParam2);

		CHECK_EQUAL(false, state2.IsFirstLogin());
		CHECK_EQUAL(false, state2.IsShareField());
		CHECK_EQUAL(true, state2.IsDynamicField());
		CHECK_EQUAL(false, state2.IsBattleArena());
		CHECK_EQUAL(false, state2.IsDead());
		CHECK_EQUAL(false, state2.IsAutoRebirthTimeOver());

		// 필드 인스턴스가 있으므로 true
		CHECK_EQUAL(true, state2.IsFieldExist());

	}

	TEST_FIXTURE(FPlayerGameStartState, TestPlayerGameStart_State_Dead)
	{
		GPlayerGameStart_State::InputParam inputParam;
		inputParam.bIsDead = true;

		GPlayerGameStart_State state(inputParam);

		CHECK_EQUAL(true, state.IsFirstLogin());
		CHECK_EQUAL(false, state.IsShareField());
		CHECK_EQUAL(false, state.IsDynamicField());
		CHECK_EQUAL(false, state.IsBattleArena());
		CHECK_EQUAL(false, state.IsFieldExist());
		CHECK_EQUAL(true, state.IsDead());
		CHECK_EQUAL(false, state.IsAutoRebirthTimeOver());

	}
}