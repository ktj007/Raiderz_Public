#pragma once

struct DBTASK_WORLD_INFO
{
	int		nID;
	wstring	strName;
	wstring strIP;
	int		nType;					// 예약. 특수 목적 월드 구분용. 0:일반
	int		nCurrentPlayerCount;
	int		nMaxPlayerCount;
	bool	isServable;				// 서비스 가능 여부
	bool	isExpire;				// 만료 여부

	DBTASK_WORLD_INFO()
		: nID(0), nType(0), nCurrentPlayerCount(0), nMaxPlayerCount(0), isServable(false), isExpire(false)
	{
	}
};
