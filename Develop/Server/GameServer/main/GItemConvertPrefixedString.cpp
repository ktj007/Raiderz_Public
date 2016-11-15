#include "stdafx.h"
#include "GItemConvertPrefixedString.h"

const wstring GItemConvertPrefixedString::m_strFalse(L"f");


GItemConvertPrefixedString::GItemConvertPrefixedString() : m_nStep(FIRST_STEP)
{

}

const wstring GItemConvertPrefixedString::Convert( SH_ITEM_SLOT_TYPE nSlotType, int16 nSlotID )
{
	Reset();

	if (!WriteSlotType(nSlotType))
		return m_strFalse;

	if (!WriteSlotID(nSlotID))
		return m_strFalse;

	return wstring(m_szString);
}

const wstring GItemConvertPrefixedString::Convert( SH_ITEM_SLOT_TYPE nSlotType, int16 nSlotID, int16 nStackAmt )
{
	Reset();

	if (!WriteSlotType(nSlotType))
		return m_strFalse;

	if (!WriteSlotID(nSlotID))
		return m_strFalse;

	if (!WriteStackAmt(nStackAmt))
		return m_strFalse;

	return wstring(m_szString);
}

const wstring GItemConvertPrefixedString::Convert( SH_ITEM_SLOT_TYPE nSlotType, int16 nSlotID, int16 nStackAmt, int16 nDeltaStackAmt )
{
	Reset();

	if (!WriteSlotType(nSlotType))
		return m_strFalse;

	if (!WriteSlotID(nSlotID))
		return m_strFalse;

	if (!WriteStackAmt(nStackAmt))
		return m_strFalse;

	if (!WriteDeltaStackAmt(nDeltaStackAmt))
		return m_strFalse;

	return wstring(m_szString);
}

const wstring GItemConvertPrefixedString::GetString()
{
	return wstring(m_szString);
}

void GItemConvertPrefixedString::Reset()
{
	m_nStep = FIRST_STEP;
	wmemset(m_szString, L'0', STRING_LEN);
	m_szString[STRING_LEN - 1] = L'\0';
}

void GItemConvertPrefixedString::SetStepToSlotType()
{
	if (CheckIsFirstStep())
		m_nStep = SLOT_TYPE_STEP;
}

void GItemConvertPrefixedString::SetStepToSlotID()
{
	if (CheckPreStepIsSlotType())
		m_nStep = SLOT_ID_STEP;
}

void GItemConvertPrefixedString::SetStepToStackAmt()
{
	if (CheckPreStepIsSlotID())
		m_nStep = STACK_AMT_STEP;
}

void GItemConvertPrefixedString::SetStepToDeltaStackAmt()
{
	if (CheckPreStepIsStackAmt())
		m_nStep = DELTA_STACK_AMT_STEP;
}

bool GItemConvertPrefixedString::CheckIsFirstStep()
{
	return FIRST_STEP == m_nStep;
}

bool GItemConvertPrefixedString::CheckPreStepIsSlotType()
{
	return SLOT_TYPE_STEP == m_nStep;
}

bool GItemConvertPrefixedString::CheckPreStepIsSlotID()
{
	return SLOT_ID_STEP == m_nStep;
}

bool GItemConvertPrefixedString::CheckPreStepIsStackAmt()
{
	return STACK_AMT_STEP == m_nStep;
}

bool GItemConvertPrefixedString::WriteSlotType( SH_ITEM_SLOT_TYPE nSlotType )
{
	if (!CheckIsFirstStep())
		return false;

	if (10 > nSlotType)
		_snwprintf_s(m_szString, STRING_LEN, STRING_LEN - 1, L"0%d", nSlotType);			
	else if (10 <= nSlotType && nSlotType < 100)
		_snwprintf_s(m_szString, STRING_LEN, STRING_LEN - 1, L"%d", nSlotType);			
	else
		return false;

	SetStepToSlotType();

	return true;
}

bool GItemConvertPrefixedString::WriteSlotID( int16 nSlotID )
{
	if (!CheckPreStepIsSlotType())
		return false;

	const size_t nBufLen = STRING_LEN - SLOT_TYPE_LEN;

	if (0 <= nSlotID && nSlotID < 10)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN, nBufLen, nBufLen - 1, L"00%d", nSlotID);			
	else if (10 <= nSlotID && nSlotID < 100)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN, nBufLen, nBufLen - 1, L"0%d", nSlotID);			
	else if (100 <= nSlotID && nSlotID < 1000)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN, nBufLen, nBufLen - 1, L"%d", nSlotID);			
	else
		return false;

	SetStepToSlotID();

	return true;
}

bool GItemConvertPrefixedString::WriteStackAmt( int16 nStackAmt )
{
	if (!CheckPreStepIsSlotID())
		return false;

	const size_t nBufLen = STRING_LEN - (SLOT_TYPE_LEN + SLOT_ID_LEN);

	if (0 <= nStackAmt && nStackAmt < 10)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN + SLOT_ID_LEN, nBufLen, nBufLen - 1, L"00%d", nStackAmt);			
	else if (10 <= nStackAmt && nStackAmt < 100)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN + SLOT_ID_LEN, nBufLen, nBufLen - 1, L"0%d", nStackAmt);			
	else if (100 <= nStackAmt && nStackAmt < 1000)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN + SLOT_ID_LEN, nBufLen, nBufLen - 1, L"%d", nStackAmt);			
	else
		return false;

	SetStepToStackAmt();

	return true;
}

bool GItemConvertPrefixedString::WriteDeltaStackAmt( int16 nDeltaStackAmt )
{
	if (!CheckPreStepIsStackAmt())
		return false;

	const size_t nBufLen = STRING_LEN - (SLOT_TYPE_LEN + SLOT_ID_LEN + STACK_AMT_LEN);

	if (0 <= nDeltaStackAmt && nDeltaStackAmt < 10)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN + SLOT_ID_LEN + STACK_AMT_LEN, nBufLen, nBufLen - 1, L"00%d", nDeltaStackAmt);
	else if (10 <= nDeltaStackAmt && nDeltaStackAmt < 100)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN + SLOT_ID_LEN + STACK_AMT_LEN, nBufLen, nBufLen - 1, L"0%d", nDeltaStackAmt);
	else if (100 <= nDeltaStackAmt && nDeltaStackAmt < 1000)
		_snwprintf_s(m_szString + SLOT_TYPE_LEN + SLOT_ID_LEN + STACK_AMT_LEN, nBufLen, nBufLen - 1, L"%d", nDeltaStackAmt);
	else
		return false;

	SetStepToDeltaStackAmt();

	return true;
}
