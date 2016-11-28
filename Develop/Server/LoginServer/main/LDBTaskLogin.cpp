#include "stdafx.h"
#include "LDBTaskLogin.h"
#include "LDBTaskGlobal.h"
#include "CCommandTable_Login.h"
#include "CCommandResultTable.h"
#include "LCommandCenter.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LDBManager.h"
#include "LCmdRouter_Player.h"
#include "LServer.h"
#include "LMasterClient.h"
#include "SCmdRouter_Login.h"
#include "LConfig.h"
#include "MLocale.h"

LDBTaskLogin::LDBTaskLogin(const MUID& uidReqPlayer) 
: LDBAsyncTask(SDBT_DBTYPE_ACCOUNTDB, SDBTID_LOGIN)
{
}

LDBTaskLogin::~LDBTaskLogin()
{

}

void LDBTaskLogin::Input(LDBT_ACC_LOGIN& data)
{
	m_Data = data;
}

void LDBTaskLogin::OnExecute(mdb::MDatabase& rfDB)
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

	m_Result.m_nGSN			= rs.FieldW(L"ACCN_SN").AsInt64();
	m_Result.m_strPWD	= rs.FieldW(L"PWD").AsWString();
	m_Result.m_bNEW_ACC = rs.FieldW(L"NEW_ACC").AsBool();
	m_Result.m_strSTATUS	= rs.FieldW(L"STATUS").AsWString();
	m_Result.m_nCONN_SN		= rs.FieldW(L"CONN_SN").AsInt64();
}

mdb::MDB_THRTASK_RESULT LDBTaskLogin::_OnCompleted()
{
	Completer completer(m_Data, m_Result);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

LDBTaskLogin::Completer::Completer( LDBT_ACC_LOGIN& data, _RESULT& result ) : m_Data(data), m_Result(result) 
{

}

void LDBTaskLogin::Completer::Do()
{
	if (0 < m_Result.m_nGSN)
	{
		CCommandResultTable nErrCode = CheckError();

		if (nErrCode != CR_SUCCESS)
		{
			SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
			cmdRouter.ResponseLogin(m_Data.m_uidPlayer, nErrCode, LConfig::m_nServerMode);
			return;
		}

		InitAccountInfo();

		if (LConfig::m_bStandAlone)
		{
			SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
			cmdRouter.ResponseLogin(m_Data.m_uidPlayer, CR_SUCCESS, LConfig::m_nServerMode);
		}
		else
		{
			LCmdRouter_Player::PostAddPlayerReq(m_Data.m_uidPlayer, m_Result.m_nGSN, m_Data.m_strUSER_ID);
		}

		gsys.pDBManager->ConnectLog(LDBT_CONN_LOG(m_Result.m_nCONN_SN, m_Result.m_nGSN, m_Data.m_strIP));
	}
	else
	{
		DebugCheckExistAccount();
	}
}

CCommandResultTable LDBTaskLogin::Completer::CheckError()
{
	if (m_Data.m_strPWD != m_Result.m_strPWD)
		return CR_FAIL_LOGIN_INVALID_ID_OR_PASSWORD;

	if (L"ACTIVE" != m_Result.m_strSTATUS)
	{
		if (L"INACTIVE" == m_Result.m_strSTATUS || 
			L"UNREGISTERED" == m_Result.m_strSTATUS)
			return CR_FAIL_LOGIN_NOT_EXIST_USER;
		else if (L"BLOCKED" == m_Result.m_strSTATUS)
			return CR_FAIL_LOGIN_PROHIBITED_ACCOUNT;

		return CR_FAIL_LOGIN;
	}

	return CR_SUCCESS;
}

void LDBTaskLogin::Completer::DebugCheckExistAccount()
{
	if (LConfig::m_isAllowInsertNewAccount)
	{
		// 계정이 없으면 만들어준다.
		LDBT_ACC_INSERT data(m_Data.m_uidPlayer
			, m_Data.m_strUSER_ID
			, m_Data.m_strSITE_CODE
			, m_Data.m_strSITE_USER_ID
			, m_Data.m_strPWD);

		// 계정이 없으면 만들어준다.
		gsys.pDBManager->InsertAccount(data);
	}
	else
	{
		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseLogin(m_Data.m_uidPlayer, CR_FAIL_LOGIN_INVALID_ID_OR_PASSWORD);
	}
}

void LDBTaskLogin::Completer::InitAccountInfo()
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.m_uidPlayer);
	if (NULL == pPlayerObject)
	{
		pPlayerObject = (LPlayerObject*)gmgr.pPlayerObjectManager->NewObject(m_Data.m_uidPlayer);
		gmgr.pPlayerObjectManager->AddObject(pPlayerObject);
	}

	// 계정 정보 설정
	ACCOUNT_INFO AccInfo;

	AccInfo.nAID	= m_Result.m_nGSN;
	AccInfo.bNewAcc = m_Result.m_bNEW_ACC;

	_tcsncpy_s(AccInfo.szUserID, MLocale::ConvUTF16ToTCHAR(m_Data.m_strUSER_ID.c_str()).c_str(), m_Data.m_strUSER_ID.length());
	AccInfo.szUserID[m_Data.m_strUSER_ID.length()] = 0;

	pPlayerObject->InitAccountInfo(AccInfo);
}
