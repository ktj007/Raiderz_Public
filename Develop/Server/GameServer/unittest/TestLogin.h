#pragma once

#include "GPlayerAcceptor.h"
#include "GPlayerGameStart_State.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "FBaseMockLink.h"
#include "FBaseNetClient.h"
#include "FBasePlayer.h"
#include "GUTHelper.h"
#include "GDynamicFieldMaster.h"
#include "GCommandTable.h"
#include "GConfig.h"
#include "GPlayerObjectManager.h"
#include "MockDBManager.h"
#include "GDBTaskCharSerialize.h"

namespace Login {

class MockLoginDBManager : public MockDBManager
{
private:
	int m_nExpectSharedFieldID;
	int m_nExpectDynamicFieldID;
public:
	MockLoginDBManager() : m_nExpectSharedFieldID(0), m_nExpectDynamicFieldID(0), MockDBManager() {}
	virtual ~MockLoginDBManager() {}
	virtual bool CharSerialize(const GDBT_CHAR_SERIALIZE& data) override;
	
	void ExpectSharedFieldID(int nFieldID)
	{
		m_nExpectSharedFieldID = nFieldID;
	}
	void ExpectDynamicFieldID(int nFieldID)
	{
		m_nExpectDynamicFieldID = nFieldID;
	}
	int GetExpectSharedFieldID() { return m_nExpectSharedFieldID; }
	int GetExpectDynamicFieldID() { return m_nExpectDynamicFieldID; }
};


class FLogin : public FBaseMockLink, public FBaseNetClient, public FBasePlayer, public FBaseFieldInfo, public FBaseFieldMgr
{
public:
	FLogin();
	virtual ~FLogin();

	void InitField();

	TD_LOGIN_ACCOUNT_INFO MakeParam_TD_LOGIN_ACCOUNT_INFO();

	void OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(MUID uidRequestServer, MUID uidRequestID, MUID uidConnectionKey, TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo);
	void OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(MUID uidConnectionKey);
	void OnRecv_MC_CHAR_GAMESTART(int nFieldID=-1);
	void OnRecv_MMC_PLAYER_STATE_IN_WORLD_RES(CID nCID, int nResult);

	int m_nSharedFieldID;
	int m_nSharedFieldID2;
	int m_nDynamicFieldID;
	MUID m_uidDynamicFieldGroup;
	int m_nAID;
	int m_nCID;
	int m_nGID;

	MUID m_uidRequestServer;
	MUID m_uidRequestID;
	MUID m_uidConnectionKey;

	MockLink* m_pLink;

	GTestSysWrapper2<GDBManager, MockLoginDBManager>	MockDBManagerWrapper;
};

}