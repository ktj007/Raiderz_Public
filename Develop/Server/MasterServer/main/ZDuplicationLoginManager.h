#pragma once

#include "PmDef.h"

struct ZDUPLICATION_LOGIN_INFO
{
	wstring strUserID;
	MUID uidPlayer;
	bool bExistPmUserData;
	PmUserData pmUserData;

	ZDUPLICATION_LOGIN_INFO() : bExistPmUserData(false) {}
};

class ZDuplicationLoginManager
{
	typedef std::map<AID, ZDUPLICATION_LOGIN_INFO*> ZDuplicationLoginMap;

public:
	ZDuplicationLoginManager();
	~ZDuplicationLoginManager();

	bool IsExist(AID nAID);
	bool Add(AID nAID, const wstring& strUserID, MUID uidPlayer, PmUserData* pmUserData);
	void Del(AID nAID);
	ZDUPLICATION_LOGIN_INFO* Get(AID nAID);

private:
	ZDuplicationLoginMap	m_mapDuplicationLogin;
};
