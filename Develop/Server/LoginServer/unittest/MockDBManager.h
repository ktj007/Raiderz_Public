#pragma once

#include "SUnitTestUtil.h"
#include "LDBManager.h"
#include "LCommandCenter.h"
#include "CCommandTable_Login.h"
#include "CCommandResultTable.h"

struct DBP_CHAR_INSERT;

class MockDBManager : public LDBManager
{
public:
	MockDBManager()				{}
	virtual ~MockDBManager()	{}	

	//// 서버 정보 관련 -------------------------------------------
	virtual bool GetServerStatusList(const int nWorldID, const std::vector<int>& vecServerID) override { return true; }

	//// 계정 관련 ------------------------------------------------
	virtual bool LoginGetInfo(LDBT_ACC_LOGIN& data) override
	{
		return true;
	}

	virtual bool ConnectLog(LDBT_CONN_LOG& data) override { return true; }

	virtual bool GetAccountCharList(const MUID& uidPlayer, const int64 nAID) override { return true; }
	virtual bool InsertCharacter(const DBP_CHAR_INSERT& rfParam
		) override
	{
		return true;
	}

	virtual void UpdaateCreateDt(const int64 nAID) override {}

};
