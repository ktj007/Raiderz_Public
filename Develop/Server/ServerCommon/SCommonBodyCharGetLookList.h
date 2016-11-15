#pragma once

#include "ServerCommonLib.h"
#include "SDBTaskData.h"
#include "MDBRecordSet.h"
#include "SAccountInfo.h"

class SDBAsyncTask;

class SCOMMON_API SCommonBodyCharGetLookList
{
public:
	struct _DATA
	{
		MUID								uidPlayer;
		int									nCharCount;

		SDBTASK_ACCOUNT_CAHR_FEATURE_LIST	CharList[MAX_ACCOUNT_CHAR_COUNT];
	};

public:
	SCommonBodyCharGetLookList();
	virtual ~SCommonBodyCharGetLookList();

	void GetCharSimpleInfo(mdb::MDBRecordSet& rs);
	bool GetCharEquipmentInfo(mdb::MDBRecordSet& rs);

	SH_FEATURE_ITEMSLOT ConvertSlotIDToFeatureSlotID(const int16 nSlotID);


	void SetAccountCharInfo(int idx, TD_AccountCharInfo* pCharInfo);
	void SetAccountCharFieldInfo(int idx, SAccountCharFieldInfo* pFieldInfo);

	MUID GetPlayerUID(void);
	int GetCharCount(void);
	int GetSharedFieldID(int nIdx);
	int GetDynamicFieldID(int nIdx);
	MUID GetDynamicFieldGroupUID(int nIdx);

	void SetPlayerUID(MUID uidReqPlayer);
	void SetCharCount(int nCount);

	_DATA& GetData() { return m_Data; }

private:
	_DATA m_Data;
};
