#include "stdafx.h"
#include "LDBTaskPmangAccountInsert.h"
#include "MDBRecordSet.h"
#include "SCmdRouter_Login.h"
#include "LGlobal.h"
#include "LCommandCenter.h"
#include "LDBManager.h"
#include "LCmdRouter_Player.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "PmRegionStat.h"
#include "LPMSSystem.h"
#include "MLocale.h"

LDBTaskPmangAccountInsert::LDBTaskPmangAccountInsert(const MUID& uidPlayer) 
: LDBAsyncTask(SDBT_DBTYPE_ACCOUNTDB, SDBTID_ACCOUNTINSERT)
{
	m_Data.uidPlayer	= uidPlayer;
	m_Data.m_nGSN		= 0;
}

LDBTaskPmangAccountInsert::~LDBTaskPmangAccountInsert()
{

}

void LDBTaskPmangAccountInsert::Input( const std::wstring strUserID, const std::wstring strPassword, const PmUserData& pmangUserData)
{
	m_Data.strUserID	= strUserID;
	m_Data.strPassword	= strPassword;

	m_Data.pmangUserData = pmangUserData;
}

void LDBTaskPmangAccountInsert::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(ACCOUNT_INSERT)))
		return;

	if (0 == rs.GetFetchedRowCount())
	{
		SetTaskFail();
		return;
	}

	m_Data.m_nGSN= (int)rs.FieldW(L"ACCN_ID").AsInt64();
	if (0 == m_Data.m_nGSN)
	{
		SetTaskFail();
		return;
	}
}

mdb::MDB_THRTASK_RESULT LDBTaskPmangAccountInsert::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

void LDBTaskPmangAccountInsert::Completer::Do()
{
	if (0 < m_Data.m_nGSN)
	{
		AddNewPlayer();
		LCmdRouter_Player::PostPmangAddPlayerReq(m_Data.uidPlayer, (int)m_Data.m_nGSN, m_Data.strUserID, &m_Data.pmangUserData);
	}
	else
	{
		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseLogin(m_Data.uidPlayer, CR_FAIL_LOGIN_NOT_EXIST_USER);
	}
}

void LDBTaskPmangAccountInsert::Completer::AddNewPlayer()
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
	if (NULL == pPlayerObject)
	{
		pPlayerObject = (LPlayerObject*)gmgr.pPlayerObjectManager->NewObject(m_Data.uidPlayer);
		gmgr.pPlayerObjectManager->AddObject(pPlayerObject);
	}

	// 계정 정보 설정
	ACCOUNT_INFO AccInfo;

	AccInfo.nAID	= (int)m_Data.m_nGSN;

	_tcsncpy_s(AccInfo.szUserID, MLocale::ConvUTF16ToTCHAR(m_Data.strUserID.c_str()).c_str(), m_Data.strUserID.length());
	AccInfo.szUserID[m_Data.strUserID.length()] = 0;

	pPlayerObject->InitAccountInfo(AccInfo);


	int nPCCafeID = _wtoi(m_Data.pmangUserData.strPCBID.c_str());
	pPlayerObject->AddStatIndex(&m_Data.pmangUserData.statIndex, nPCCafeID);
}
