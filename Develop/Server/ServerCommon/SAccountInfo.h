#pragma once

#include "STypes.h"
#include "CTransData.h"
#include "STransData.h"

/// 계정 정보
struct ACCOUNT_INFO
{
	int64				nAID;								///< 계정 AID(DB값과 동일)
	wchar_t				szUserID[ACCOUNT_NAME_LEN+1];		///< UserID
	bool				bNewAcc;

	ACCOUNT_INFO() : nAID(0), bNewAcc(true)
	{
		szUserID[0] = 0;
	}
};

struct SAccountCharFieldInfo
{
	CID	nCID;
	int nExitedSharedFieldID;
	int nExitedDynamicFieldID;
	MUID uidExitedDynamicFieldGroup;

	SAccountCharFieldInfo(): nCID(0), nExitedSharedFieldID(0), nExitedDynamicFieldID(0)	{}
};

//class SAccountCharList
//{
//public:
//	int					m_nCID[MAX_ACCOUNT_CHAR_COUNT];
//	TD_AccountCharInfo	m_TransData[MAX_ACCOUNT_CHAR_COUNT];
//	TD_LOGIN_FIELD_INFO m_LoginFieldInfo[MAX_ACCOUNT_CHAR_COUNT];
//	int					m_nAccountCharCount;
//
//	SAccountCharList() : m_nAccountCharCount(0) 
//	{
//		memset(m_nCID, 0, sizeof(m_nCID));
//	}
//};
