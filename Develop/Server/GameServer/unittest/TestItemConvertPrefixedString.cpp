#include "stdafx.h"
#include "GItemConvertPrefixedString.h"


class MockItemConvertPrefixedString : public GItemConvertPrefixedString
{
public :
	using GItemConvertPrefixedString::Reset;

	using GItemConvertPrefixedString::SetStepToSlotType;
	using GItemConvertPrefixedString::SetStepToSlotID;
	using GItemConvertPrefixedString::SetStepToStackAmt;
	using GItemConvertPrefixedString::SetStepToDeltaStackAmt;


	using GItemConvertPrefixedString::CheckIsFirstStep;
	using GItemConvertPrefixedString::CheckPreStepIsSlotType;
	using GItemConvertPrefixedString::CheckPreStepIsSlotID;
	using GItemConvertPrefixedString::CheckPreStepIsStackAmt;


	using GItemConvertPrefixedString::WriteSlotType;
	using GItemConvertPrefixedString::WriteSlotID;
	using GItemConvertPrefixedString::WriteStackAmt;
	using GItemConvertPrefixedString::WriteDeltaStackAmt;

	void SetTestData()
	{
		m_nStep = FIRST_STEP;
		wmemset(m_szString, L'a', STRING_LEN);
	}

	int GetStep()
	{
		return m_nStep;
	}
};


SUITE(GItemConvertPrefixedString_Write_SUCCESS)
{
	TEST(SLOT_ID_SUCCESS)
	{
		MockItemConvertPrefixedString cov;

		cov.Reset();
		cov.SetStepToSlotType();
		CHECK(cov.WriteSlotID(0));

		cov.Reset();
		cov.SetStepToSlotType();
		CHECK(cov.WriteSlotID(10));

		cov.Reset();
		cov.SetStepToSlotType();
		CHECK(cov.WriteSlotID(100));

		cov.Reset();
		cov.SetStepToSlotType();
		CHECK(cov.WriteSlotID(999));
	}

	TEST(STACK_AMT_SUCCESS)
	{
		MockItemConvertPrefixedString cov;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		CHECK(cov.WriteStackAmt(0));

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		CHECK(cov.WriteStackAmt(10));

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		CHECK(cov.WriteStackAmt(100));

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		CHECK(cov.WriteStackAmt(999));
	}

	TEST(DELTA_STACK_AMT_SUCCESS)
	{
		MockItemConvertPrefixedString cov;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();
		CHECK(cov.WriteDeltaStackAmt(0));

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();
		CHECK(cov.WriteDeltaStackAmt(10));

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();
		CHECK(cov.WriteDeltaStackAmt(100));

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();
		CHECK(cov.WriteDeltaStackAmt(999));
	}
}

SUITE(GItemConvertPrefixedString_Write_FAIL)
{
	TEST(SLOT_ID_OVERFLOW)
	{
		MockItemConvertPrefixedString cov;
		cov.SetStepToSlotType();
		CHECK(!cov.WriteSlotID(1000));
	}

	TEST(SLOT_ID_UNDERFLOW)
	{
		MockItemConvertPrefixedString cov;
		cov.SetStepToSlotType();
		CHECK(!cov.WriteSlotID(-1));
	}

	TEST(STACK_AMT_OVERFLOW)
	{
		MockItemConvertPrefixedString cov;
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		CHECK(!cov.WriteStackAmt(1000));
	}

	TEST(STACK_AMT_UNDERFLOW)
	{
		MockItemConvertPrefixedString cov;
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		CHECK(!cov.WriteStackAmt(-1));
	}

	TEST(DELTA_STACK_AMT_OVERFLOW)
	{
		MockItemConvertPrefixedString cov;
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();
		CHECK(!cov.WriteDeltaStackAmt(1000));
	}

	TEST(DELTA_STACK_AMT_UNDERFLOW)
	{
		MockItemConvertPrefixedString cov;
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();
		CHECK(!cov.WriteDeltaStackAmt(-1));
	}
}

SUITE(GItemConvertPrefixedString_SUCCESS)
{
	TEST(SLOT_TYPE)
	{
		MockItemConvertPrefixedString cov;

		const SH_ITEM_SLOT_TYPE nSlotType		= SLOTTYPE_INVENTORY;		

		cov.Reset();
		cov.WriteSlotType(nSlotType);

		CHECK(wstring(L"01") == cov.GetString());
	}

	TEST(SLOT_ID_LENTH_1)
	{
		MockItemConvertPrefixedString cov;

		const int16				nSlotID			= 1;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.WriteSlotID(nSlotID);

		CHECK(wstring(L"00001") == cov.GetString());
	}

	TEST(SLOT_ID_LENTH_2)
	{
		MockItemConvertPrefixedString cov;

		const int16				nSlotID			= 12;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.WriteSlotID(nSlotID);

		CHECK(wstring(L"00012") == cov.GetString());
	}

	TEST(SLOT_ID_LENTH_3)
	{
		MockItemConvertPrefixedString cov;

		const int16				nSlotID			= 123;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.WriteSlotID(nSlotID);

		CHECK(wstring(L"00123") == cov.GetString());
	}

	TEST(STACK_AMT_LENTH_1)
	{
		MockItemConvertPrefixedString cov;

		const int16				nStackAmt		= 1;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();

		cov.WriteStackAmt(nStackAmt);

		CHECK(wstring(L"00000001") == cov.GetString());
	}

	TEST(STACK_AMT_LENTH_2)
	{
		MockItemConvertPrefixedString cov;

		const int16				nStackAmt		= 12;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();

		cov.WriteStackAmt(nStackAmt);

		CHECK(wstring(L"00000012") == cov.GetString());
	}

	TEST(STACK_AMT_LENTH_3)
	{
		MockItemConvertPrefixedString cov;

		const int16				nStackAmt		= 123;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();

		cov.WriteStackAmt(nStackAmt);

		CHECK(wstring(L"00000123") == cov.GetString());
	}

	TEST(DELTA_STACK_AMT_LENTH_1)
	{
		MockItemConvertPrefixedString cov;

		const int16				nDeltaStackAmt	= 1;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();

		cov.WriteDeltaStackAmt(nDeltaStackAmt);

		CHECK(wstring(L"00000000001") == cov.GetString());
	}

	TEST(DELTA_STACK_AMT_LENTH_2)
	{
		MockItemConvertPrefixedString cov;

		const int16				nDeltaStackAmt	= 12;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();

		cov.WriteDeltaStackAmt(nDeltaStackAmt);

		CHECK(wstring(L"00000000012") == cov.GetString());
	}

	TEST(DELTA_STACK_AMT_LENTH_3)
	{
		MockItemConvertPrefixedString cov;

		const int16				nDeltaStackAmt	= 123;

		cov.Reset();
		cov.SetStepToSlotType();
		cov.SetStepToSlotID();
		cov.SetStepToStackAmt();

		cov.WriteDeltaStackAmt(nDeltaStackAmt);

		CHECK(wstring(L"00000000123") == cov.GetString());
	}
}

SUITE(GItemConvertPrefixedString_Reset)
{
	TEST(Reset_clear_data)
	{
		MockItemConvertPrefixedString cov;

		cov.SetTestData();
		cov.Reset();

		CHECK_EQUAL(GItemConvertPrefixedString::FIRST_STEP, cov.GetStep());
		CHECK(wstring(L"00000000000") == cov.GetString());
	}
}

SUITE(GItemConvertPrefixedString_Convert)
{
	TEST(SLOT_TYPE_AND_SLOT_ID)
	{
		MockItemConvertPrefixedString cov;

		const SH_ITEM_SLOT_TYPE nSlotType		= SLOTTYPE_INVENTORY;
		const int16				nSlotID			= 2;

		wstring str(L"01002");
		CHECK(str == cov.Convert(nSlotType, nSlotID));
	
	}

	TEST(SLOT_TYPE_AND_SLOT_ID_AND_STACK_AMT)
	{
		MockItemConvertPrefixedString cov;

		const SH_ITEM_SLOT_TYPE nSlotType		= SLOTTYPE_INVENTORY;
		const int16				nSlotID			= 2;
		const int16				nStackAmt		= 3;

		wstring str(L"01002003");
		CHECK(str == cov.Convert(nSlotType, nSlotID, nStackAmt));
	}

	TEST(SLOT_TYPE_AND_SLOT_ID_AND_STACK_AMT_DELTA_STACK_AMT)
	{
		MockItemConvertPrefixedString cov;

		const SH_ITEM_SLOT_TYPE nSlotType		= SLOTTYPE_INVENTORY;
		const int16				nSlotID			= 2;
		const int16				nStackAmt		= 3;
		const int16				nDeltaStackAmt	= 4;

		wstring str(L"01002003004");
		CHECK(str == cov.Convert(nSlotType, nSlotID, nStackAmt, nDeltaStackAmt));
	}
}