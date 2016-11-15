#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "FBasePlayer.h"
#include "MockLink.h"
#include "LTestHelper.h"
#include "LPlayerLogic.h"
#include "CSChrInfo.h"
#include "SAccountInfo.h"
#include "LPlayerObject.h"
#include "CCommandTable.h"
#include "CCommandResultTable.h"
#include "MockDBManager.h"
#include "LDBParamData.h"
#include "LDBTaskCharInsert.h"
#include "CSNameWordFilter.h"
#include "CSNameStringChecker.h"


SUITE(CreateCharacter)
{
	class MockDBManager_CreateCharacter : public MockDBManager
	{
	public:
		bool m_bCall_GetAccountCharList;
		bool m_bCall_InsertCharacter;

		MockDBManager_CreateCharacter()
		{
			m_bCall_GetAccountCharList = false;
			m_bCall_InsertCharacter = false;
		}

		virtual bool GetAccountCharList(const MUID& uidPlayer, const int64 nAID) override
		{
			m_bCall_GetAccountCharList = true;
			return true;
		}
		virtual bool InsertCharacter(const DBP_CHAR_INSERT& rfParam) override
		{
			m_bCall_InsertCharacter = true;

			LDBTaskCharInsert taskCharInsert;
			taskCharInsert.Input(rfParam.uidPlayer, rfParam.nAID);
			taskCharInsert._OnCompleted();

			return true;
		}
	};

	struct Fixture : public FBasePlayer
	{
		Fixture()
		{
			InitLoginedPlayer();
			InitParam_InsertCharacter();
		}
		virtual ~Fixture()
		{

		}

		void InitLoginedPlayer()
		{
			m_uidTestPlayer = SUnitTestUtil::NewUID();
			m_pTestPlayer = NewTestPlayerObject(m_uidTestPlayer);

			ACCOUNT_INFO info;
			info.nAID = 1;
			_tcscpy_s(info.szUserID, ACCOUNT_NAME_LEN + 1, _T("TEST_ID"));
			m_pTestPlayer->InitAccountInfo(info);

			m_pPlayerLink = LTestHelper::NewLink(m_uidTestPlayer);
		}
		void InitParam_InsertCharacter()
		{			
			wcsncpy_s(m_tdInsertCharacterInfo.szName, L"TestName", _TRUNCATE);
			m_tdInsertCharacterInfo.nSex			= SEX_MALE;
			m_tdInsertCharacterInfo.nHair			= 0;
			m_tdInsertCharacterInfo.nFace			= 0;
			m_tdInsertCharacterInfo.nHairColor		= 0;
			m_tdInsertCharacterInfo.nSkinColor		= 0;
			m_tdInsertCharacterInfo.nEyeColor		= 0;

			m_tdInsertCharacterInfo.nMakeUp			= 0;
			m_tdInsertCharacterInfo.nTattooType		= 0;
			m_tdInsertCharacterInfo.nTattooPosX		= 0;
			m_tdInsertCharacterInfo.nTattooPosY		= 0;
			m_tdInsertCharacterInfo.nTattooScale		= 0;

			m_tdInsertCharacterInfo.nEquipmentIndex	= 0;
			m_tdInsertCharacterInfo.nEquipmentColorIndex	= 0;
		}

		MUID m_uidTestPlayer;
		LPlayerObject* m_pTestPlayer;
		MockLink* m_pPlayerLink;

		/// Param : Insert Character
		TD_INSERT_CHARACTER_INFO m_tdInsertCharacterInfo;
		//////////////////////////////////////////////////////////////////////////

		LPlayerLogic m_PlayerLogic;
		LTestSysWrapper2<LDBManager, MockDBManager_CreateCharacter>	m_wrapperDBManager;
	};

	TEST_FIXTURE(Fixture, TestPlayerLogic_InsertCharacter_Success)
	{
		CHECK_EQUAL(POS_SELECT_CHAR, m_pTestPlayer->GetState());

		bool bRet = false;
		bRet = m_PlayerLogic.InsertCharacter(m_uidTestPlayer, &m_tdInsertCharacterInfo);
		CHECK_EQUAL(true , bRet);

		CHECK_EQUAL(true, m_wrapperDBManager.Get()->m_bCall_InsertCharacter);
		CHECK_EQUAL(true, m_wrapperDBManager.Get()->m_bCall_GetAccountCharList);

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_RESPONSE_INSERT_CHAR, m_pPlayerLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_SUCCESS, m_pPlayerLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_InsertCharacter_Failure_POS_IsNot_POS_SELECT_CHAR)
	{
		MUID uidDummyPlayer = SUnitTestUtil::NewUID();
		LPlayerObject* pDummyPlayer = NewTestPlayerObject(uidDummyPlayer);

		CHECK(POS_SELECT_CHAR != pDummyPlayer->GetState());

		bool bRet = false;
		bRet = m_PlayerLogic.InsertCharacter(uidDummyPlayer, &m_tdInsertCharacterInfo);
		CHECK_EQUAL(false , bRet);

		CHECK_EQUAL(0, m_pPlayerLink->GetCommandCount());
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_InsertCharacter_Failure_NameLenIsMax)
	{
		wstring strName;
		for (int i = 0; i < CSNameStringChecker::CHARACTER_MAX_ENGLISH+1; ++i)
		{
			strName += L"a";
		}

		CHECK_EQUAL(CSNameStringChecker::CHARACTER_MAX_ENGLISH+1, wcslen(strName.c_str()));

		memset(m_tdInsertCharacterInfo.szName, 0, sizeof(m_tdInsertCharacterInfo.szName));
		wcsncpy_s(m_tdInsertCharacterInfo.szName, strName.c_str(), _TRUNCATE);

		bool bRet = false;
		bRet = m_PlayerLogic.InsertCharacter(m_uidTestPlayer, &m_tdInsertCharacterInfo);
		CHECK_EQUAL(false , bRet);

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_RESPONSE_INSERT_CHAR, m_pPlayerLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_SYSTEM_TOO_LONG_NAME_LEN, m_pPlayerLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_InsertCharacter_Failure_LimitCharSlot)
	{		
		SAccountCharFieldInfo dummyInfo;
		for(int i = 0; i < MAX_ACCOUNT_CHAR_COUNT; i++)
		{
			m_pTestPlayer->AddCharFieldInfo(dummyInfo);
		}

		bool bRet = false;
		bRet = m_PlayerLogic.InsertCharacter(m_uidTestPlayer, &m_tdInsertCharacterInfo);
		CHECK_EQUAL(false , bRet);

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_RESPONSE_INSERT_CHAR, m_pPlayerLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_CHAR_LIMIT_CHAR_SLOT_COUNT, m_pPlayerLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_InsertCharacter_Failure_InvalidValue)
	{
		m_tdInsertCharacterInfo.nSex			= SEX_MALE;
		m_tdInsertCharacterInfo.nHair			= -1;
		m_tdInsertCharacterInfo.nFace			= -1;
		m_tdInsertCharacterInfo.nHairColor		= -1;
		m_tdInsertCharacterInfo.nSkinColor		= -1;
		m_tdInsertCharacterInfo.nEyeColor		= -1;
		m_tdInsertCharacterInfo.nMakeUp			= -1;
		m_tdInsertCharacterInfo.nTattooType		= -1;
		m_tdInsertCharacterInfo.nTattooPosX		= -1;
		m_tdInsertCharacterInfo.nTattooPosY		= -1;
		m_tdInsertCharacterInfo.nTattooScale		= -1;
		m_tdInsertCharacterInfo.nEquipmentIndex	= -1;
		m_tdInsertCharacterInfo.nEquipmentColorIndex	= -1;

		bool bRet = false;
		bRet = m_PlayerLogic.InsertCharacter(m_uidTestPlayer, &m_tdInsertCharacterInfo);
		CHECK_EQUAL(false , bRet);

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_RESPONSE_INSERT_CHAR, m_pPlayerLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_CHAR_INVALID_PARAMETER, m_pPlayerLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_InsertCharacter_Failure_NameWithAbuseWord)
	{
		GetNameWordFilter()->Clear();
		GetNameWordFilter()->AddIllegalWord(MLocale::ConvAnsiToTCHAR("aibeast").c_str());

		wcsncpy_s(m_tdInsertCharacterInfo.szName, L"tmpaibeast", _TRUNCATE);

		bool bRet = false;
		bRet = m_PlayerLogic.InsertCharacter(m_uidTestPlayer, &m_tdInsertCharacterInfo);
		CHECK_EQUAL(false , bRet);

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_RESPONSE_INSERT_CHAR, m_pPlayerLink->GetCommand(0).GetID());
		CHECK_EQUAL(CR_FAIL_SYSTEM_NAME_INCLUDE_INVALID_STRING, m_pPlayerLink->GetParam<int>(0, 0));

		GetNameWordFilter()->Clear();
	}

}

