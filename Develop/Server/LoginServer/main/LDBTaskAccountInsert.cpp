#include "stdafx.h"
#include "LDBTaskAccountInsert.h"
#include "MDBRecordSet.h"
#include "SCmdRouter_Login.h"
#include "LGlobal.h"
#include "LCommandCenter.h"
#include "LDBManager.h"
#include "LCmdRouter_Player.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "MLocale.h"

LDBTaskAccountInsert::LDBTaskAccountInsert(const MUID& uidPlayer) 
: LDBAsyncTask(SDBT_DBTYPE_ACCOUNTDB, SDBTID_ACCOUNTINSERT)
{
	
}

LDBTaskAccountInsert::~LDBTaskAccountInsert()
{

}

void LDBTaskAccountInsert::Input( LDBT_ACC_INSERT& data )
{
	m_Data = data;
}

void LDBTaskAccountInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(ACCOUNT_INSERT)))
		return;

	if (0 == rs.GetFetchedRowCount())
	{
		SetTaskFail();
		return;
	}

	m_Result.m_GSN = rs.FieldW(L"ACCN_ID").AsInt64();

	if (0 == m_Result.m_GSN)
	{
		SetTaskFail();
		return;
	}
}

mdb::MDB_THRTASK_RESULT LDBTaskAccountInsert::_OnCompleted()
{
	Completer completer(m_Data, m_Result);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void LDBTaskAccountInsert::Completer::Do()
{
	if (0 < m_Result.m_GSN)
	{
		InitAccountInfo();
		LCmdRouter_Player::PostAddPlayerReq(m_Data.m_uidPlayer, (int)m_Result.m_GSN, m_Data.m_strUSER_ID);
	}
	else
	{
		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseLogin(m_Data.m_uidPlayer, CR_FAIL_LOGIN_NOT_EXIST_USER);
	}
}

void LDBTaskAccountInsert::Completer::InitAccountInfo()
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.m_uidPlayer);
	if (NULL == pPlayerObject)
	{
		pPlayerObject = (LPlayerObject*)gmgr.pPlayerObjectManager->NewObject(m_Data.m_uidPlayer);
		gmgr.pPlayerObjectManager->AddObject(pPlayerObject);
	}

	// 계정 정보 설정
	ACCOUNT_INFO AccInfo;

	AccInfo.nAID	= (int)m_Result.m_GSN;

	_tcsncpy_s(AccInfo.szUserID, MLocale::ConvUTF16ToTCHAR(m_Data.m_strUSER_ID.c_str()).c_str(), m_Data.m_strUSER_ID.length());
	AccInfo.szUserID[m_Data.m_strUSER_ID.length()] = 0;

	pPlayerObject->InitAccountInfo(AccInfo);
}
