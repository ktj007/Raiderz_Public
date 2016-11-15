#pragma once

#include "ZFieldInfo.h"

class ZGameServerFieldInfo
{
public:
	int m_nFieldID;
	int m_nZoneID;
	ZFieldType	m_nFieldType;
	ZGameServerFieldInfo() : m_nFieldID(0), m_nZoneID(0), m_nFieldType(FT_SHARED) 
	{

	}
};

/// 게임 서버에 할당하는 필드 정보
struct ZManagedSharedField
{
	int				nFieldID;
	vector<int>		vecChannelID;			///< 게임서버 접속 즉시 생성해야할 채널들.
	vector<int>		vecReservedChannelID;	///< 채널을 확장할 때 생성할 채널들.
	ZManagedSharedField() : nFieldID(0) {}
};

/// 게임 서버 정보
class ZGameServerInfo
{
public:
	int								m_nID;							///< 서버 ID
	vector<ZManagedSharedField>		m_vecManagedSharedField;		///< 할당된 정적 필드


	ZGameServerInfo() : m_nID(0) 
	{

	}
};



