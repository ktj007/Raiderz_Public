#include "stdafx.h"
#include "LDBTaskPmangLogin.h"
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
#include "PmRegionStat.h"
#include "LPMSSystem.h"
#include "SCmdRouter_Login.h"
#include "LConfig.h"
#include "MLocale.h"
#include "LDBTaskDataAccount.h"


LDBTaskPmangLogin::LDBTaskPmangLogin(const MUID& uidReqPlayer) 
: LDBAsyncTask(SDBT_DBTYPE_ACCOUNTDB, SDBTID_LOGIN)
{
	m_Data.uidPlayer		= uidReqPlayer;
	m_Data.nDBAID			= 0;	
}

LDBTaskPmangLogin::~LDBTaskPmangLogin()
{

}

void LDBTaskPmangLogin::Input(const wstring& strUserID, const PmUserData& pmangUserData)
{
	m_Data.strUserID		= strUserID;
	m_Data.pmangUserData	= pmangUserData;
}


void LDBTaskPmangLogin::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	
	if (!ExecuteW(rs, GetSQLW(LOGIN)))
		return;
	
	if (0 == rs.GetFetchedCount() || rs.IsEOF())
	{
		// 계정이 없는 거다.
		m_Data.bNewAcc = true;
		SetTaskSuccess();
		return;
	}

	m_Data.nDBAID			= rs.FieldW(L"ACCN_ID").AsInt64();
	m_Data.strDBPassword	= rs.FieldW(L"PWD").AsWString();
	m_Data.bNewAcc			= rs.FieldW(L"NEW_ACC").AsBool();

	SetTaskSuccess();
}


mdb::MDB_THRTASK_RESULT LDBTaskPmangLogin::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

LDBTaskPmangLogin::Completer::Completer( _DATA& Data ) : m_Data(Data)
{

}

void LDBTaskPmangLogin::Completer::Do()
{
	if (0 < m_Data.nDBAID)
	{
		AddNewPlayer();
		LCmdRouter_Player::PostPmangAddPlayerReq(m_Data.uidPlayer, m_Data.nDBAID, m_Data.strUserID, &m_Data.pmangUserData);
	}
	else
	{
		DebugCheckExistAccount();
	}
}

void LDBTaskPmangLogin::Completer::DebugCheckExistAccount()
{	
	if (LConfig::m_isAllowInsertNewAccount)
	{
		// 계정이 없으면 만들어준다.
		gsys.pDBManager->PmangInsertAccount(m_Data.uidPlayer, m_Data.pmangUserData);

		gsys.pDBManager->ConnectLog(LDBT_CONN_LOG(m_Data.nDBAID, L"test"));
	}
	else
	{
		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseLogin(m_Data.uidPlayer, CR_FAIL_LOGIN_NOT_EXIST_USER);
	}
}

void LDBTaskPmangLogin::Completer::AddNewPlayer()
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
	if (NULL == pPlayerObject)
	{
		pPlayerObject = (LPlayerObject*)gmgr.pPlayerObjectManager->NewObject(m_Data.uidPlayer);
		gmgr.pPlayerObjectManager->AddObject(pPlayerObject);
	}

	// 계정 정보 설정
	ACCOUNT_INFO AccInfo;

	AccInfo.nAID	= m_Data.nDBAID;
	AccInfo.bNewAcc	= m_Data.bNewAcc;

	_tcsncpy_s(AccInfo.szUserID, MLocale::ConvUTF16ToTCHAR(m_Data.strUserID.c_str()).c_str(), m_Data.strUserID.length());
	AccInfo.szUserID[m_Data.strUserID.length()] = 0;

	pPlayerObject->InitAccountInfo(AccInfo);


	int nPCCafeID = _wtoi(m_Data.pmangUserData.strPCBID.c_str());
	pPlayerObject->AddStatIndex(&m_Data.pmangUserData.statIndex, nPCCafeID);

	PmRegionCode nRegionCode = m_Data.pmangUserData.statIndex.nRegionCode;
	PmAgeCode nAgeCode = m_Data.pmangUserData.statIndex.nAgeCode;
	PmGenderCode nGenderCode = m_Data.pmangUserData.statIndex.nGenderCode;
}
