#pragma once

#include "MTypes.h"
#include "CSDef.h"
#include "SDef.h"
#include "STypes.h"

#pragma pack(push, old)
#pragma pack(1)


struct TD_SERVER_LIST_NODE
{
	int nServerID;
	uint8 nServerType;
};

//
// 로그인서버를 통한 로그인 처리용
//

// 입장 필드 정보
struct TD_LOGIN_FIELD_INFO
{
	int		nSharedFieldID;
	int		nDynamicFieldID;
	MUID	uidDynamicFieldGroup;

	TD_LOGIN_FIELD_INFO()
	: nSharedFieldID(0)
	, nDynamicFieldID(0)
	, uidDynamicFieldGroup(MUID::ZERO)
	{}
};

// 입장 유저 정보
struct TD_LOGIN_ACCOUNT_INFO
{
	uint32				nEnterKey;							// Client IP Address
	uint32				nAID;								// User DB index
	int32				nCID;
	wchar_t				szUserID[ACCOUNT_NAME_LEN + 1];

	TD_LOGIN_ACCOUNT_INFO()
	: nAID(0)
	, nCID(0)
	, nEnterKey(0)
	{}
};

// 프록시 플레이어 정보 동기화 구조체
struct TD_PROXY_PLAYER_INFO
{
	AID		nAID;								// User DB index
	int		nCID;
	MUID	UID;
	wchar_t	szName[ACCOUNT_NAME_LEN + 1];
	int		nGameServerID;
	MUID	uidField;
	MUID	uidParty;
	int		nGuildID;
	int		nFieldID;
	int		nChannelID;		///< 공유필드일 경우만 쓰임.
	MUID	uidFieldGroup;	///< 동적필드일 경우만 쓰임.

	TD_PROXY_PLAYER_INFO()
	: nGameServerID(INVALID_ID)	
	, nGuildID(INVALID_ID)
	, nFieldID(INVALID_ID)
	, nChannelID(INVALID_CHANNELID)
	{
		szName[0] = L'\0';
	}
};


// 피망
struct TD_PMANG_USER_DATA
{
	wchar_t szUserSerial[PMANG_USER_SERIAL_LEN + 1];
	wchar_t szUserID[PMANG_USER_ID_LEN + 1];
	int8 nRegionCode;
	int8 nAgeCode;
	int8 nGenderCode;
	int16 nYearOfBirth;

	wchar_t szPCBID[PMANG_PCBID_LEN + 1];
	wchar_t szServiceType[PMANG_SERVICE_TYPE_LEN + 1];
	int16 nUserType;
};

#pragma pack(pop, old)
