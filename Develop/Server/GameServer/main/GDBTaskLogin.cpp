#include "stdafx.h"
#include "GDBTaskLogin.h"
#include "GDBTaskGlobal.h"
#include "CCommandTable_Login.h"
#include "GCommand.h"
#include "CCommandResultTable.h"
#include "GCommandCenter.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GDBManager.h"
#include "SCmdRouter_Login.h"
#include "GConfig.h"


GDBTaskLogin::GDBTaskLogin(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_LOGIN)
{
	
}


GDBTaskLogin::~GDBTaskLogin()
{

}


void GDBTaskLogin::Input(GDBT_ACC_LOGIN& data)
{
	m_Data = data;
}


void GDBTaskLogin::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	
	if (!ExecuteW(rs, GetSQLW(LOGIN)))
		return;

	if (0 == rs.GetFetchedCount() || rs.IsEOF())
	{
		m_Result.m_bNEW_ACC = true;
		// 계정이 없는 거다.
		SetTaskSuccess();
		return;
	}

	m_Result.m_nGSN		= rs.FieldW(L"ACCN_ID").AsInt64();
	m_Result.m_strPWD	= rs.FieldW(L"PWD").AsWString();
	m_Result.m_bNEW_ACC = rs.FieldW(L"NEW_ACC").AsBool();
}


mdb::MDB_THRTASK_RESULT GDBTaskLogin::_OnCompleted()
{
	Completer completer(m_Data, m_Result);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

GDBTaskLogin::Completer::Completer( GDBT_ACC_LOGIN& data, GDBTaskLogin::_RESULT& result ) : m_Data(data), m_Result(result)
{

}

void GDBTaskLogin::Completer::Do()
{
	if (0 < m_Result.m_nGSN)
	{
		InitAccountInfo();
		PostLoginResCommand(m_Data.m_uidPlayer, CR_SUCCESS);
	}
	else
	{
		DebugCheckExistAccount();	

		// 일단 로그인 실패 통보 - 재접속 유도
		PostLoginResCommand(0, CR_FAIL_LOGIN_NOT_EXIST_USER);
	}
}

void GDBTaskLogin::Completer::DebugCheckExistAccount()
{
	// 계정이 없으면 만들어준다.

	GDBT_ACC_INSERT data(m_Data.m_uidPlayer
		, m_Data.m_strUSER_ID
		, m_Data.m_strSITE_CODE
		, m_Data.m_strSITE_USER_ID
		, m_Data.m_strPWD);

	unsigned int nDbgAID;

	gsys.pDBManager->AccInsert(nDbgAID, data);
}

void GDBTaskLogin::Completer::InitAccountInfo()
{
	GPlayerObject* pNewPlayer = gmgr.pPlayerObjectManager->GetPlayer(m_Data.m_uidPlayer);
	if (pNewPlayer)
	{
		pNewPlayer->OnEndLogin();

		// 계정 정보 설정
		pNewPlayer->InitAccountInfo((int)m_Result.m_nGSN, m_Data.m_strUSER_ID, m_Result.m_bNEW_ACC);
	}
}

void GDBTaskLogin::Completer::PostLoginResCommand( const MUID& uidPlayer, const int nRes )
{
	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseLogin(uidPlayer, (CCommandResultTable)nRes, GConfig::m_nServerMode);
}