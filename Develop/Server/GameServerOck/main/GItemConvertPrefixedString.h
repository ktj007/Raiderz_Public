#ifndef _GITEM_CONVERT_PREFIXED_STRING_H
#define _GITEM_CONVERT_PREFIXED_STRING_H


class GItemConvertPrefixedString
{
public :
	GItemConvertPrefixedString();

	enum LEN
	{
		SLOT_TYPE_LEN			= 2
		, SLOT_ID_LEN			= 3
		, STACK_AMT_LEN			= 3
		, DELTA_STACK_AMT_LEN	= 3

		, STRING_LEN			= 12
	};

	enum STEP
	{
		FIRST_STEP				= 0
		, SLOT_TYPE_STEP		= 1
		, SLOT_ID_STEP			= 2
		, STACK_AMT_STEP		= 3
		, DELTA_STACK_AMT_STEP	= 4
	};

	const wstring Convert(SH_ITEM_SLOT_TYPE nSlotType, int16 nSlotID);
	const wstring Convert(SH_ITEM_SLOT_TYPE nSlotType, int16 nSlotID, int16 nStackAmt);
	const wstring Convert(SH_ITEM_SLOT_TYPE nSlotType, int16 nSlotID, int16 nStackAmt, int16 nDeltaStackAmt);

	const wstring GetString();

protected :
	virtual void Reset();

	virtual void SetStepToSlotType();
	virtual void SetStepToSlotID();
	virtual void SetStepToStackAmt();
	virtual void SetStepToDeltaStackAmt();


	virtual bool CheckIsFirstStep();
	virtual bool CheckPreStepIsSlotType();
	virtual bool CheckPreStepIsSlotID();
	virtual bool CheckPreStepIsStackAmt();


	virtual bool WriteSlotType(SH_ITEM_SLOT_TYPE nSlotType);
	virtual bool WriteSlotID(int16 nSlotID);
	virtual bool WriteStackAmt(int16 nStackAmt);
	virtual bool WriteDeltaStackAmt(int16 nDeltaStackAmt);


public :
	static const wstring m_strFalse;

protected :
	STEP	m_nStep;
	wchar_t	m_szString[STRING_LEN];	
};


#endif